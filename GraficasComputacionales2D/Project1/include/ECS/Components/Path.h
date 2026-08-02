#pragma once
#include "Prerequisites.h"
#include <vector>

namespace ECS {
    /**
     * @struct Path
     * @brief Circuito cerrado definido por puntos de control, suavizado con Catmull-Rom.
     */
    struct Path {
        std::vector<sf::Vector2f> controlPoints; // Puntos que tú defines a mano
        std::vector<sf::Vector2f> points;        // Puntos suavizados (generados)
        float radius{ 40.f };
        int   segmentsPerCurve{ 20 };

        void GenerateSmoothPoints();

    private:
        static sf::Vector2f CatmullRom(const sf::Vector2f& p0, const sf::Vector2f& p1,
            const sf::Vector2f& p2, const sf::Vector2f& p3, float t);
    };
}