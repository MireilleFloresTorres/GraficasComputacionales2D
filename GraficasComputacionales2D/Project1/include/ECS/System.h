#pragma once
//ECS ::System.h
// Clase base para todos los sistemas del motor
//
//un sistema contiene unicmaente lógica, nunca datos
//Los datos vieven en los componentes 
//
//Ciclo de vida:
//OnStart-> llamado una vez al registrar el sistema
//Onupdate-> llamado cada frame
//OnStop -> llamado al destruir o desrefistrar el sistema

namespace ECS {

	//Foward declaration para evitar in lcusión circular
	class Registry; 

	class
		System {
	public:
		virtual 		~System() = default; 

		//inicialización: reservar recursos, suscribirse a eventos etc.
		virtual void 
		OnStart(Registry& /*Registry*/) {}

		//logica frame a frame
		virtual void
		OnUpdate(Registry& registry, float deltaTime) = 0;

		//limpieza al destruir el sistema
		virtual void 
		OnDestroy(Registry& /*Registry*/) {}

		//opcional: activa/desactica el sistema sin destruirlo
		virtual void 
		SetEnable(bool enabled) noexcept {m_enabled = enabled;}

		void SetEnable(bool enabled) noexcept { m_enabled = enabled;  }
		[[nodiscard]] bool IsEnabled() const noexcept { return m_enabled;}

	private:
		bool m_enabled = true;
	};
}