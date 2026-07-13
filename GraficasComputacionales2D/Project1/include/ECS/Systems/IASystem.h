#pragma once
#include "ECS/System.h"
#include "ECS/Registry.h"
#include "ECS/Components/Transform.h"
#include "ECS/Components/Physics.h"
#include "ECS/Components/SteeringTarget.h"
#include "ECS/Components/Obstacle.h"
#include <cmath>
#include <cstdlib>

namespace ECS {

 /**
 * @class IASystem
 * @brief Sistema de IA basado en steering behaviors
 * Este sistema recorre todas las entidades que poseen los componentes Transform, Physics y
 * SteeringTarget, y calcula en cada frame una fuerza de dirección (steering force) según el
 * comportamiento configurado
 *
 * El flujo general por entidad es:
 *  1. Si la entidad sigue a otra (followAnEntity), se actualiza la posición y velocidad objetivo
 *     tomándolas del Transform/Physics de la entidad seguida
 *  2. Se calcula la velocidad deseada (desired velocity) según el comportamiento activo.
 *  3. Se obtiene el steering = desired - velocidad actual, limitado por la fuerza máxima (maxForce).
 *  4. Si el comportamiento incluye evasión de obstáculos, se suma una fuerza adicional de evasión
 *     sin limitar por maxForce, para que la evasión tenga prioridad sobre el resto de fuerzas.
 *  5. Se integra la física (aceleración -> velocidad -> posición) y se reinicia la aceleración.
 */
    class IASystem final : public System {
    public:
       
    /**
    * @brief Actualiza el comportamiento de steering de todas las entidades con IA en este frame.
    * @param registry Registro ECS con las entidades y componentes.
    * @param dt Delta time del frame actual.
    */ 
        void OnUpdate(Registry& registry, float dt) override {
            registry.GetView<Transform, Physics, SteeringTarget>().Each(
                [&registry, dt](EntityID id, Transform& t, Physics& ph, SteeringTarget& st) {
                    if (!st.enabled) return;

                    sf::Vector2f targetVelocity{ 0.f, 0.f };
                    if (st.followAnEntity && registry.IsAlive(st.followEntity)) {
                        if (auto* targetT = registry.TryGetComponent<Transform>(st.followEntity))
                            st.targetPosition = targetT->position;
                        if (auto* targetPh = registry.TryGetComponent<Physics>(st.followEntity))
                            targetVelocity = targetPh->velocity;
                    }

                    sf::Vector2f desired = computeDesired(t.position, ph, st, targetVelocity, dt);

                    sf::Vector2f steer = desired - ph.velocity;
                    steer = limit(steer, ph.maxForce);
                    ph.acceleration += steer / ph.mass;

                    if (st.behavior == SteeringBehavior::ObstacleAvoidance) {
                        sf::Vector2f avoid = computeAvoidance(registry, id, t.position, ph, st);
                        ph.acceleration += avoid / ph.mass; // sin límite de maxForce, evasión siempre "gana"
                    }
                    ph.velocity += ph.acceleration * dt;
                    ph.velocity = limit(ph.velocity, ph.maxSpeed);
                    t.position += ph.velocity * dt;
                    ph.acceleration = { 0.f, 0.f };
                });
        }
    private:
        /**
        * @brief Calcula la velocidad deseada según el comportamiento de steering activo.
        * @param pos Posición actual de la entidad.
        * @param ph Componente de física de la entidad.
        * @param targetVelocity Velocidad del objetivo seguido (usada en Pursuit).
        * @param dt Delta time, usado para el jitter de Wander.
        * @return Vector de velocidad deseada.
        */
        static sf::Vector2f computeDesired(const sf::Vector2f& pos,
            const Physics& ph,
            SteeringTarget& st,
            const sf::Vector2f& targetVelocity,
            float dt) {
            switch (st.behavior) {
            case SteeringBehavior::Seek: { ///< Dirección directa hacia el objetivo
                sf::Vector2f diff = st.targetPosition - pos;
                if (length(diff) == 0.f) return { 0.f, 0.f };
                return normalize(diff) * ph.maxSpeed;
            }
            case SteeringBehavior::Flee: {  ///< Dirección opuesta al objetivo
                sf::Vector2f diff = st.targetPosition - pos;
                if (length(diff) == 0.f) return { 0.f, 0.f };
                return normalize(-diff) * ph.maxSpeed;
            }
            case SteeringBehavior::Arrive: {  ///< Como Seek, pero desacelera dentro de slowRadius
                sf::Vector2f diff = st.targetPosition - pos;
                float distance = length(diff);
                if (distance == 0.f) return { 0.f, 0.f };
                float speed = (distance < st.slowRadius)
                    ? ph.maxSpeed * (distance / st.slowRadius)
                    : ph.maxSpeed;
                return normalize(diff) * speed;
            }
            case SteeringBehavior::Pursuit: { ///< Persigue la posición futura predicha del objetivo
                sf::Vector2f predicted = st.targetPosition + targetVelocity * st.predictionTime;
                sf::Vector2f diff = predicted - pos;
                if (length(diff) == 0.f) return { 0.f, 0.f };
                return normalize(diff) * ph.maxSpeed;
            }
            case SteeringBehavior::Wander: { ///< Movimiento errático mediante un círculo proyectado + jitter
                st.wanderAngle += randomJitter() * st.wanderJitter * dt;
                sf::Vector2f heading = length(ph.velocity) > 0.f ? normalize(ph.velocity) : sf::Vector2f{ 1.f, 0.f };
                sf::Vector2f circleCenter = heading * st.wanderDistance;
                sf::Vector2f displacement = { std::cos(st.wanderAngle), std::sin(st.wanderAngle) };
                displacement *= st.wanderRadius;
                sf::Vector2f wanderForce = circleCenter + displacement;
                if (length(wanderForce) == 0.f) return { 0.f, 0.f };
                return normalize(wanderForce) * ph.maxSpeed;
            }
            case SteeringBehavior::ObstacleAvoidance: {///< Fuerza base hacia el objetivo (la evasión real se suma aparte)
                sf::Vector2f diff = st.targetPosition - pos;
                if (length(diff) == 0.f) return { 0.f, 0.f };
                return normalize(diff) * ph.maxSpeed;
            }
            default: return { 0.f, 0.f };
            }
        }

        /**
        * @brief Calcula la fuerza de evasión de obstáculos cercanos a la entidad.
        * @param registry Registro ECS, usado para consultar entidades con componente Obstacle
        * @return Fuerza de evasión resultante
        */
        static sf::Vector2f computeAvoidance(Registry& registry, EntityID self,
            const sf::Vector2f& pos, const Physics& ph, const SteeringTarget& st) {
            sf::Vector2f avoidForce{ 0.f, 0.f };
            registry.GetView<Transform, Obstacle>().Each(
                [&](EntityID obsId, Transform& obsT, Obstacle&) {
                    if (obsId == self) return;
                    sf::Vector2f diff = pos - obsT.position;
                    float distance = length(diff);
                    if (distance > 0.f && distance < st.obstacleDetectionRadius) {
                        float strength = st.obstacleAvoidForce * (1.f - distance / st.obstacleDetectionRadius);
                        avoidForce += normalize(diff) * strength;
                    }
                });
            return avoidForce;
        }

        /// @brief Genera un valor aleatorio en el rango [-1, 1], usado para el jitter de Wander
        static float randomJitter() {
            return (static_cast<float>(std::rand()) / RAND_MAX) * 2.f - 1.f; // -1 a 1
        }

        /// @brief Calcula la magnitud (longitud) de un vector 2D.
        static float length(sf::Vector2f v) {
            return std::sqrt(v.x * v.x + v.y * v.y);
        }

        /// @brief Normaliza un vector 2D; devuelve vector nulo si la longitud es 0
        static sf::Vector2f normalize(sf::Vector2f v) {
            float l = length(v);
            return l > 0.f ? v / l : sf::Vector2f{ 0.f, 0.f };
        }

        /// @brief Limita la magnitud de un vector a un valor máximo, preservando su dirección
        static sf::Vector2f limit(sf::Vector2f v, float max) {
            return length(v) > max ? normalize(v) * max : v;
        }
    };
}