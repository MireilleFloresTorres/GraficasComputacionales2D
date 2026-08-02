#include "ECS/Components/Path.h"

namespace ECS {

    void Path::GenerateSmoothPoints() {
        points.clear();
        const int n = static_cast<int>(controlPoints.size());
        if (n < 3) return;

        for (int i = 0; i < n; ++i) {
            const sf::Vector2f& p0 = controlPoints[(i - 1 + n) % n];
            const sf::Vector2f& p1 = controlPoints[i];
            const sf::Vector2f& p2 = controlPoints[(i + 1) % n];
            const sf::Vector2f& p3 = controlPoints[(i + 2) % n];

            for (int j = 0; j < segmentsPerCurve; ++j) {
                float t = static_cast<float>(j) / static_cast<float>(segmentsPerCurve);
                points.push_back(CatmullRom(p0, p1, p2, p3, t));
            }
        }
    }

    sf::Vector2f Path::CatmullRom(const sf::Vector2f& p0, const sf::Vector2f& p1,
        const sf::Vector2f& p2, const sf::Vector2f& p3, float t) {
        float t2 = t * t;
        float t3 = t2 * t;

        float x = 0.5f * ((2.f * p1.x) +
            (-p0.x + p2.x) * t +
            (2.f * p0.x - 5.f * p1.x + 4.f * p2.x - p3.x) * t2 +
            (-p0.x + 3.f * p1.x - 3.f * p2.x + p3.x) * t3);

        float y = 0.5f * ((2.f * p1.y) +
            (-p0.y + p2.y) * t +
            (2.f * p0.y - 5.f * p1.y + 4.f * p2.y - p3.y) * t2 +
            (-p0.y + 3.f * p1.y - 3.f * p2.y + p3.y) * t3);

        return { x, y };
    }

}