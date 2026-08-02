#pragma once
#include "Prerequisites.h"
#include "ECS/Types.h"
namespace ECS {
    enum class SteeringBehavior { Seek = 0, Flee = 1, Arrive = 2, Wander = 3, Pursuit = 4, ObstacleAvoidance = 5, PathFollowing = 6 };
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
        float predictionTime{ 0.5f };

        // Obstacle Avoidance
        float obstacleDetectionRadius{ 120.f };
        float obstacleAvoidForce{ 300.f };
    };
}