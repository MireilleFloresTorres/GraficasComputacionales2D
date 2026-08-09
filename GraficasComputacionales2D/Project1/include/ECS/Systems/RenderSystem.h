#pragma once
#include "Core/Window.h"
#include "ECS/System.h"
#include "ECS/Registry.h"
#include "ECS/Components/Render.h"
#include "ECS/Components/Transform.h"
#include "ECS/Components/Path.h"
#include "ECS/Components/Sprite.h"

namespace ECS {
    class RenderSystem final : public System {
    public:
        explicit RenderSystem(Window& window) noexcept
            : m_window(window) {
        }
        void OnUpdate(Registry& registry, float /*deltatime*/) override;
    private:
        void DrawPaths(Registry& registry);
        Window& m_window;
        void DrawSprites(Registry& registry);
    };
}