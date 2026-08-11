#include "ECS/Components/Path.h"
#include <cmath>

namespace ECS {

    /**
   * @brief Genera los puntos suavizados que forman la trayectoria
   *
   * Recorre todos los puntos de control y genera una curva suave entre
   * cada punto usando la formula de Catmull-Rom centrípeta (Cosa horrible)
   *
   * Los puntos generados se almacenan en el contenedor "points"
   * La trayectoria se considera cerrada, por lo que los puntos al inicio
   * y al final se conectan entre sí mediante el operador del modulo
   * Si existen menos de tres puntos de control, no se genera ninguna
   * trayectoria
   *
   * @note La cantidad de puntos generados depende de `segmentsPerCurve`.
   */
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
                points.push_back(CentripetalCatmullRom(p0, p1, p2, p3, t));
            }
        }
    }

    /**
   * @brief Calcula el intervalo entre dos puntos de control
   *
   * Calcula la distancia  entre los puntos y aplica la raíz cuadrada 
   * de la distancia para obtener el intervalo
   *
   * Entonces se utiliza alpha = 0.5 correspondiente a la variante
   *
   * @param a Primer punto de control.
   * @param b Segundo punto de control.
   *
   * @return Intervalo entre los puntos utilizado como parámetro de la curva.
   */
    float Path::GetKnotInterval(const sf::Vector2f& a, const sf::Vector2f& b) {
        float dx = b.x - a.x;
        float dy = b.y - a.y;
        float dist = std::sqrt(dx * dx + dy * dy);
        return std::pow(dist, 0.5f); // alpha = 0.5 (centripetal)
    }


    /**
    * @brief Interpola un punto mediante Catmull-Rom centrípeta.
    *
    * @param p0 Primer punto de control
    * @param p1 Punto inicial de la curva
    * @param p2 Punto final de la curva
    * @param p3 Cuarto punto de control
    * @param t Posición de interpolación entre 0 y 1
    * @return Punto interpolado de la trayectoria
    */
    sf::Vector2f Path::CentripetalCatmullRom(const sf::Vector2f& p0, const sf::Vector2f& p1,
        const sf::Vector2f& p2, const sf::Vector2f& p3, float t) {

        float t0 = 0.f;
        float t1 = t0 + GetKnotInterval(p0, p1);
        float t2 = t1 + GetKnotInterval(p1, p2);
        float t3 = t2 + GetKnotInterval(p2, p3);

        // Evita división por cero si dos puntos coinciden
        if (t1 == t0) t1 += 0.0001f;
        if (t2 == t1) t2 += 0.0001f;
        if (t3 == t2) t3 += 0.0001f;

        float tt = t1 + t * (t2 - t1); // mapea t [0,1] al intervalo real [t1, t2]

        sf::Vector2f A1 = p0 * ((t1 - tt) / (t1 - t0)) + p1 * ((tt - t0) / (t1 - t0));
        sf::Vector2f A2 = p1 * ((t2 - tt) / (t2 - t1)) + p2 * ((tt - t1) / (t2 - t1));
        sf::Vector2f A3 = p2 * ((t3 - tt) / (t3 - t2)) + p3 * ((tt - t2) / (t3 - t2));

        sf::Vector2f B1 = A1 * ((t2 - tt) / (t2 - t0)) + A2 * ((tt - t0) / (t2 - t0));
        sf::Vector2f B2 = A2 * ((t3 - tt) / (t3 - t1)) + A3 * ((tt - t1) / (t3 - t1));

        sf::Vector2f C = B1 * ((t2 - tt) / (t2 - t1)) + B2 * ((tt - t1) / (t2 - t1));

        return C;
    }

}