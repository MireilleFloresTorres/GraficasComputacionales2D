#pragma once
#include "Prerequisites.h"
#include <vector>

namespace ECS {
    /**
     * @brief Representa una trayectoria definida mediante losp untos de control
     */
    struct Path {
        /** @brief Puntos utilizados para definir la trayectoria. */
        std::vector<sf::Vector2f> controlPoints;

        /** @brief Puntos suavizados generados a partir de los puntos de control. */
        std::vector<sf::Vector2f> points;
        //radio de la tayectoriA
        float radius{ 40.f };

        //Cantidad de segmentos entre cuervas
        int   segmentsPerCurve{ 20 };

        void GenerateSmoothPoints();

    private:

        /**
        * @brief Interpola un punto mediante Catmull-Rom centrípeta
		* @param p0 Primer punto de control y varios puntos de control
        * @return Punto interpolado.
        */
        static sf::Vector2f CentripetalCatmullRom(const sf::Vector2f& p0, const sf::Vector2f& p1,
            const sf::Vector2f& p2, const sf::Vector2f& p3, float t);
        /**
        * @brief Calcula el intervalo entre dos puntos a y b 
        */
        static float GetKnotInterval(const sf::Vector2f& a, const sf::Vector2f& b);
    };
}