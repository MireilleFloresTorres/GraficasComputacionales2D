#pragma once
#include "Prerequisites.h"
#include "ECS/Types.h"
namespace ECS {
    /**
    * @enum SteeringBehavior
    * @brief Tipos de comportamiento de steering disponibles.
    */
    enum class SteeringBehavior { Seek = 0, Flee = 1, Arrive = 2, Wander = 3, Pursuit = 4, ObstacleAvoidance = 5 };

    /**
    * @struct SteeringTarget
    * @brief Componente que configura el comportamiento de steering de una entidad.
    */
    struct SteeringTarget {
        sf::Vector2f     targetPosition{ 0.f, 0.f }; ///< Posición objetivo fija (si no se sigue a una entidad)
        SteeringBehavior behavior{ SteeringBehavior::Seek }; ///< Comportamiento de steering activo
        float            slowRadius{ 100.f };  ///< Radio de desaceleración, usado en Arrive
        EntityID         followEntity{ 0 }; ///< Entidad objetivo a seguir (si followAnEntity es true)
        bool             followAnEntity{ false }; ///< Indica si se sigue una entidad en vez de una posición fija
        bool             enabled{ false }; ///< Habilita o deshabilita el steering


        // Wander
        float wanderAngle{ 0.f };
        float wanderRadius{ 50.f };
        float wanderDistance{ 100.f };
        float wanderJitter{ 30.f };

        // Pursuit
        float predictionTime{ 0.5f };

        // Obstacle Avoidance
        float obstacleDetectionRadius{ 120.f };
        float obstacleAvoidForce{ 300.f };
    };
}