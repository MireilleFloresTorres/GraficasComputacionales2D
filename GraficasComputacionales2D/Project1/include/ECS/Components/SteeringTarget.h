#pragma once
#include "Prerequisites.h"
#include "ECS/Types.h"
namespace ECS {
    /**
     * @brief Tipos de comportamiento de dirección (steering behavior) disponibles para una entidad.
     */
    enum class SteeringBehavior { Seek = 0, Flee = 1, Arrive = 2, Wander = 3, Pursuit = 4, ObstacleAvoidance = 5, PathFollowing = 6 };

    /**
    * @brief Componente que define el objetivo y comportamiento de dirección (steering) de una entidad.
    */
    struct SteeringTarget {
        sf::Vector2f     targetPosition{ 0.f, 0.f };
        SteeringBehavior behavior{ SteeringBehavior::Seek };
        float            slowRadius{ 100.f };
        EntityID         followEntity{ 0 };
        bool             followAnEntity{ false };
        bool             enabled{ false };

        // Wander
        float wanderAngle{ 0.f };
        float wanderRadius{ 50.f };
        float wanderDistance{ 100.f };
        float wanderJitter{ 30.f };
        float wanderBoundaryRadius{ 150.f };   // qué tan lejos de targetPosition puede alejarse
        float wanderExclusionRadius{ 40.f };   // zona prohibida alrededor de targetPosition

        // Pursuit
         /** @brief Tiempo usado para predecir la posición futura del objetivo (usado en Pursuit). */
        float predictionTime{ 0.5f };


        // Obstacle Avoidance
        /** @brief Radio de detección dentro del cual los obstáculos generan repulsión. */
        float obstacleDetectionRadius{ 120.f };
        float obstacleAvoidForce{ 300.f };
    };
}