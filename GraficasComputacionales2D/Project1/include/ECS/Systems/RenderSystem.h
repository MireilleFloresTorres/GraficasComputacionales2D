#pragma once
#include "Core/Window.h"
#include "ECS/System.h"
#include "ECS/Registry.h"
#include "ECS/Components/Render.h"
#include "ECS/Components/Transform.h"
//ECS:: system/rendersystem.h
//Logica pura: recorre todas las entidades que tengan a la vez
//transform

namespace ECS {
	class RenderSystem final : public System {
	public: 
		explicit RenderSystem(Window& window) noexcept
			: m_window(window) {

		}

        void OnUpdate(Registry& registry, float /*deltatime*/) override {
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
        }
	private:
			Window& m_window; 
	};
}