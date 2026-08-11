#pragma once
#include "Core/Window.h"
#include "ECS/System.h"
#include "ECS/Registry.h"
#include "ECS/Components/Render.h"
#include "ECS/Components/Transform.h"
#include "ECS/Components/Path.h"
#include "ECS/Components/Sprite.h"

namespace ECS {
    /**
     * @brief Sistema encargado de dibujar en pantalla las entidades con componentes visuales
     * @details Dibuja los sprites de fondo, luego las entidades con Transform y Render 
     *          de depuración de los Path  mediante DrawPaths.
     */
    class RenderSystem final : public System {
    public:

        /**
         * @brief Construye el sistema de renderizado asociándolo a una ventana de dibujo
         */
        explicit RenderSystem(Window& window) noexcept
            : m_window(window) {
        }

        /**
        * @brief Dibuja todas las entidades visuales del frame en orden de capas
        */
        void OnUpdate(Registry& registry, float /*deltatime*/) override;
        void DrawControlPoints(Registry& registry);
    private:
        void DrawPaths(Registry& registry);
        Window& m_window;
        void DrawSprites(Registry& registry);
    };
}