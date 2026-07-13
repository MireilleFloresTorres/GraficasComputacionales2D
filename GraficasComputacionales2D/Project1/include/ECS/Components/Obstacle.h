#pragma once

namespace ECS {
 /**
 * @struct Obstacle
 * @brief Componente etiqueta (tag) vacío que marca una entidad como obstáculo para el sistema de IA
 * IASystem::computeAvoidance la considere al calcular fuerzas de evasión en el comportamiento
 */
    struct Obstacle {}; 
}