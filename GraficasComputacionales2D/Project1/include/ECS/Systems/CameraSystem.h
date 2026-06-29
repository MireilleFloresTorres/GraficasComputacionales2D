#pragma once
#include "ECS/System.h"
#include "ECS/Registry.h"
#include "ECS/Components/Transform.h"
#include "Core/Window.h"
#include "ECS/Components/Camera.h"
#include <cmath>

/**
* @brief ECS Systems/CameraSystem.h
* LÓGICA de camaera. Busca la camara actica
* interpola su posiciín hacie el objetivo a seguir y aplica la 
* vista resultatnte al a ventana.
* 
* ORDEN: debe registrsrse antes del renderSystem, porque 
* setview afecta a todo lo que se sibuje DESPUÉS. 
*/

namespace ECS {
	class CameraSystem final : public System {
	public: 
		explicit CameraSystem(Window& window) noexcept
			: m_window(window) {

		}

		void OnUpdate(Registry& registry, float deltaTime) override {
			registry.GetView < Transform, Camera >().Each(
				[this, &registry, deltaTime](EntityID, Transform& camT, Camera& cam) {
					if (!cam.active) return;

					/**
					* Seguimiento interpolado
					* Suavizado exponencial IDEPENDIENTE DEL FRAMERATE:
					*  t= 1 e^(.speed* dt)
					* Garatiza el mismo movimiento a 30 o 144 FPS, a
					* diferencia de un lerp con factor fijo
					*/

					if (cam.followTarget != NULL_ENTITY &&
						registry.IsAlive(cam.followTarget)) {

						if (auto* targetT = registry.TryGetComponent<Transform>(cam.followTarget)) {
							const float t = 1.f - std::exp(-cam.followSpeed * deltaTime);
							camT.position += (targetT->position - camT.position) * t;
						}
					}

					m_window.applyCameraView(camT.position, cam.zoom);
				});
		}

	private:
		Window& m_window; 
	};
}