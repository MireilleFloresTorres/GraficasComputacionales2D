#pragma once
#include "Prerequisites.h"

namespace ECS {

    /**
    * @struct Physics
    * @brief Componente que almacena las propiedades físicas de una entidad
    * sistemas de física y de steering behaviors
    * (Seek, Flee, Arrive, Wander, Pursuit, Obstacle Avoidance) para
    * calcular e integrar el movimiento de la entidad.
    */
    struct Physics {

        /// Velocidad actual de la entidad
        sf::Vector2f velocity{ 0.f, 0.f };
        /// Aceleración actual de la misma entidad
        sf::Vector2f acceleration{ 0.f, 0.f };
        /// Velocidad máxima permitida
        float maxSpeed{ 150.f };
        /// Fuerza máxima que se puede aplicar
        float maxForce{ 10.f };
        /// Masa de la entidad
        float mass{ 1.f };
    };
}