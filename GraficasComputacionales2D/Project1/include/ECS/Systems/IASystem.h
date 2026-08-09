#pragma once
#include "ECS/System.h"
#include "ECS/Registry.h"
#include "ECS/Components/Transform.h"
#include "ECS/Components/Physics.h"
#include "ECS/Components/SteeringTarget.h"
#include "ECS/Components/Obstacle.h"
#include "ECS/Components/Path.h"
#include "ECS/Components/LapCounter.h"

namespace ECS {
    /**
     * @brief Sistema de inteligencia artificial que aplica comportamientos de dirección (steering behaviors)
     *        a las entidades con Transform, Physics y SteeringTarget.
     * @details Soporta Seek, Flee, Arrive, Wander, Pursuit, ObstacleAvoidance y PathFollowing, además
     *          de llevar el conteo de vueltas de las entidades con LapCounter.
     */
    class IASystem final : public System {
    public:
        void OnUpdate(Registry& registry, float dt) override;

    private:
        /** @brief Fuerza de dirección para acercarse directamente al objetivo. */
        static sf::Vector2f SeekForce(const sf::Vector2f& pos, const Physics& ph, const SteeringTarget& st);

        /** @brief Fuerza de dirección para alejarse del objetivo. */
        static sf::Vector2f FleeForce(const sf::Vector2f& pos, const Physics& ph, const SteeringTarget& st);

        /** @brief Fuerza de dirección para llegar al objetivo desacelerando dentro de un radio. */
        static sf::Vector2f ArriveForce(const sf::Vector2f& pos, const Physics& ph, const SteeringTarget& st);

        /** @brief Fuerza de dirección para un movimiento errático (wander) alrededor del objetivo. */
        static sf::Vector2f WanderForce(const sf::Vector2f& pos, const Physics& ph, SteeringTarget& st, float dt);

        /** @brief Fuerza de dirección para interceptar un objetivo en movimiento, prediciendo su posición futura. */
        static sf::Vector2f PursuitForce(const sf::Vector2f& pos, const Physics& ph, const SteeringTarget& st, const sf::Vector2f& targetVelocity);

        /** @brief Fuerza de repulsión acumulada respecto a obstáculos cercanos a la entidad self. */
        static sf::Vector2f ObstacleAvoidanceForce(Registry& registry, EntityID self, const sf::Vector2f& pos, const SteeringTarget& st);

        /** @brief Fuerza de dirección para seguir el Path de la entidad referenciada en la SteeringTarget. */
        static sf::Vector2f PathFollowingForce(Registry& registry, const Transform& t, const Physics& ph, const SteeringTarget& st);

        /** @brief Proyecta el punto p sobre el segmento de línea a-b y devuelve el punto proyectado. */
        static sf::Vector2f getNormalPoint(const sf::Vector2f& p, const sf::Vector2f& a, const sf::Vector2f& b);

        /** @brief Devuelve un valor aleatorio uniforme entre -1 y 1. */
        static float randomJitter();

        /** @brief Devuelve la magnitud (longitud) de un vector 2D. */
        static float length(sf::Vector2f v);

        /** @brief Devuelve el vector normalizado (longitud 1), o vector cero si la entrada es nula. */
        static sf::Vector2f normalize(sf::Vector2f v);

        /** @brief Limita la magnitud de un vector a un valor máximo, preservando su dirección. */
        static sf::Vector2f limit(sf::Vector2f v, float max);

        /** @brief Actualiza el conteo de vueltas de todas las entidades con LapCounter según su posición en el Path. */
        static void UpdateLapCounters(Registry& registry);
    };
}