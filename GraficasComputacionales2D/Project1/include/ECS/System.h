#pragma once
/**
 * @file System.h
 * @brief Clase base para todos los sistemas del ECS.
 *
 * Un sistema contiene únicamente lógica, nunca datos.
 * Los datos viven en los componentes.
 *
 * Ciclo de vida: `OnStart` ->  `OnUpdate` (cada frame) -> `OnDestroy`.
 */

namespace ECS {

	// @brief Forward declaration para evitar inclusión circular
	class Registry; 

	/**
	 * @class System
	 * @brief Interfaz base que deben heredar todos los sistemas del motor
	 */
	class
		System {
	public:
		virtual 		~System() = default; 

		/** @brief Inicialización, reservar recursos, suscribirse a eventos, etc */
		virtual void 
		OnStart(Registry& /*Registry*/) {}

		/** @brief Lógica frame a frame. Obligatorio implementar en subclases*/
		virtual void
		OnUpdate(Registry& registry, float deltaTime) = 0;

		/** @brief Limpieza al destruir o desregistrar el sistema*/
		virtual void 
		OnDestroy(Registry& /*Registry*/) {}

		/** @brief Activa o desactiva el sistema sin destruirlo*/
		virtual void 
		SetEnable(bool enabled) noexcept {m_enabled = enabled;}

		void SetEnable(bool enabled) noexcept { m_enabled = enabled;  }

		/** @brief Retorna true si el sistema está activo. */
		[[nodiscard]] bool IsEnabled() const noexcept { return m_enabled;}

	private:
		bool m_enabled = true;
	};
}