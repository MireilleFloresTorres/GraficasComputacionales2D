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
    class IASystem final : public System {
    public:
        void OnUpdate(Registry& registry, float dt) override;

    private:
        static sf::Vector2f SeekForce(const sf::Vector2f& pos, const Physics& ph, const SteeringTarget& st);
        static sf::Vector2f FleeForce(const sf::Vector2f& pos, const Physics& ph, const SteeringTarget& st);
        static sf::Vector2f ArriveForce(const sf::Vector2f& pos, const Physics& ph, const SteeringTarget& st);
        static sf::Vector2f WanderForce(const sf::Vector2f& pos, const Physics& ph, SteeringTarget& st, float dt);
        static sf::Vector2f PursuitForce(const sf::Vector2f& pos, const Physics& ph, const SteeringTarget& st, const sf::Vector2f& targetVelocity);
        static sf::Vector2f ObstacleAvoidanceForce(Registry& registry, EntityID self, const sf::Vector2f& pos, const SteeringTarget& st);
        static sf::Vector2f PathFollowingForce(Registry& registry, const Transform& t, const Physics& ph, const SteeringTarget& st);

        static sf::Vector2f getNormalPoint(const sf::Vector2f& p, const sf::Vector2f& a, const sf::Vector2f& b);
        static float randomJitter();
        static float length(sf::Vector2f v);
        static sf::Vector2f normalize(sf::Vector2f v);
        static sf::Vector2f limit(sf::Vector2f v, float max);
        static void UpdateLapCounters(Registry& registry);
    };
}