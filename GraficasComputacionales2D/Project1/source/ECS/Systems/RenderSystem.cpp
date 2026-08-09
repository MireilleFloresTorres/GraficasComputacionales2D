#include "ECS/Systems/RenderSystem.h"

namespace ECS {

    void RenderSystem::OnUpdate(Registry& registry, float /*deltatime*/) {
        DrawSprites(registry); // fondo primero

        registry.GetView<Transform, Render>().Each(
            [this](EntityID /*entity*/, Transform& t, Render& r) {
                if (!r.shape || !r.visible) return;
                r.shape->setPosition(t.position);
                r.shape->setRotation(sf::degrees(t.rotation));
                r.shape->setScale(t.scale);
                if (!r.texture)
                    r.shape->setFillColor(r.fillColor);
                m_window.draw(*r.shape);
            });

        //DrawPaths(registry);// el sprite como visual de la pista
    }

    void RenderSystem::DrawSprites(Registry& registry) {
        registry.GetView<Sprite>().Each(
            [this](EntityID /*entity*/, Sprite& s) {
                if (!s.sprite) return;
                m_window.draw(*s.sprite);
            });
    }

    void RenderSystem::DrawPaths(Registry& registry) {
        registry.GetView<Path>().Each(
            [this](EntityID /*entity*/, Path& path) {
                if (path.points.size() < 2) return;

                const std::size_t n = path.points.size();

                sf::VertexArray road(sf::PrimitiveType::TriangleStrip);
                for (std::size_t i = 0; i <= n; ++i) {
                    const sf::Vector2f& p0 = path.points[i % n];
                    const sf::Vector2f& p1 = path.points[(i + 1) % n];
                    sf::Vector2f dir = p1 - p0;
                    float len = std::sqrt(dir.x * dir.x + dir.y * dir.y);
                    sf::Vector2f normal = (len > 0.f)
                        ? sf::Vector2f(-dir.y / len, dir.x / len)
                        : sf::Vector2f(0.f, 0.f);

                    sf::Vertex vTop, vBottom;
                    vTop.position = p0 + normal * path.radius;
                    vTop.color = sf::Color(100, 100, 100, 120); // semi-transparente para no tapar el sprite
                    vBottom.position = p0 - normal * path.radius;
                    vBottom.color = sf::Color(100, 100, 100, 120);

                    road.append(vTop);
                    road.append(vBottom);
                }
                m_window.draw(road);

                sf::VertexArray centerLine(sf::PrimitiveType::LineStrip);
                for (std::size_t i = 0; i <= n; ++i) {
                    sf::Vertex v;
                    v.position = path.points[i % n];
                    v.color = sf::Color::Red; // rojo brillante para verlo fácil sobre el sprite
                    centerLine.append(v);
                }
                m_window.draw(centerLine);
            });
    }

}