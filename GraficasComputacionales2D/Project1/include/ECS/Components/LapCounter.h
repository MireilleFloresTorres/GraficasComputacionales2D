#pragma once
#include "ECS/Types.h"

namespace ECS {
    /**
     * @struct LapCounter
     * @brief Cuenta las vueltas completadas por una entidad que sigue un Path cerrado.
     */
    struct LapCounter {
        int laps{ 0 };
        std::size_t lastIndex{ 0 }; // índice del punto más cercano en el frame anterior
        bool initialized{ false };  // evita contar una vuelta falsa en el primer frame
        EntityID trackEntity{ 0 };
    };
}