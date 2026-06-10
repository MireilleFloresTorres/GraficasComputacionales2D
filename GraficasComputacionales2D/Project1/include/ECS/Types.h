#pragma once
#include "Prerequisites.h"

/**
 * @file Types.h
 * @brief Tipos fundamentales del Entity Component System.
 *
 * El EntityID empaqueta un índice y una versión en un único `uint64_t`.
 * Al destruir una entidad su versión se incrementa, invalidando cualquier
 * ID antiguo que apuntara al mismo índice.
 */
namespace ECS {

	using EntityIndex = uint32_t; //indice de la entidad en el array
	using EntityVersion = uint32_t; //Versiond de la entidad (para invalidar IDs antiguos) 
	using EntityID = uint64_t; //ID cmpleto que empaqueta indice y la version
	using ComponentTypeID = uint32_t; //ID de tipo para componentes, asignado en tiempo de ejeución

	//nodiscard: el valor o metodo no deve ser modificado
	// inline
	//OxFFFFFFFF C++ 
	//valor centinela para "ninguna entidad"
	/// @brief Valor centinela que representa "ninguna entidad".
	inline constexpr EntityID NULL_ENTITY = std::numeric_limits
		<EntityID> ::max(); 

//---Empaquetado y desempaquetado de EntityID ---//
/**
 * @brief Obtiene el índice de una entidad a partir de su EntityID.
 *
 * Esta función extrae los primeros 32 bits del identificador,
 * los cuales representan el índice único de la entidad.
 *
 * @param id Identificador completo de la entidad.
 * @return EntityIndex Índice de la entidad.
 */
	[[nodiscard]] inline EntityIndex GetEntityIndex(EntityID id) noexcept {
		return static_cast<EntityIndex>(id & 0xFFFFFFFF);
	}

	/**
	 * @brief Obtiene la versión de una entidad a partir de su EntityID.
	 *
	 * La versión se almacena en los 32 bits superiores del identificador.
	 * Esto ayuda a validar si una entidad sigue siendo válida o fue reutilizada.
	 *
	 * @param id Identificador completo de la entidad.
	 * @return EntityVersion Versión de la entidad.
	 */
	[[nodiscard]] inline EntityVersion GetEntityVersion(EntityID id) noexcept {
		return static_cast<EntityVersion>((id >> 32) & 0xFFFFFFFF);
	}

	/**
	 * @brief Construye un EntityID combinando índice y versión.
	 *
	 * La versión se desplaza 32 bits hacia la izquierda y luego
	 * se combina con el índice usando una operación OR binaria.
	 *
	 * @param index Índice de la entidad.
	 * @param version Versión de la entidad.
	 * @return EntityID Identificador completo de la entidad.
	 */
	[[nodiscard]] inline EntityID MakeEntityID(EntityIndex index, 
		EntityVersion version) noexcept {
		return (static_cast<EntityID>(version) << 32) |
			static_cast<EntityID>(index);
	}


	//---Generador de IDs de tipo componente ---//
	//Cada tipi T obtiene un ID único en tiempo de ejecución 
	//la primera vez que se llama a GetComponentTypeID<T>()
	
	/**
 * @brief Genera un nuevo identificador único para tipos de componentes.
 *
 * Esta función utiliza un contador estático que incrementa cada vez
 * que se solicita un nuevo ID. Cada componente registrado obtiene
 * un identificador distinto.
 *
 * @return ComponentTypeID Identificador único del componente.
 */
	[[nodiscard]] inline ComponentTypeID NextComponentTypeID() noexcept
	{
		static ComponentTypeID counter = 0;
		return counter++;
	}

	/**
	 * @brief Obtiene el identificador único asociado a un tipo de componente.
	 *
	 * Cada tipo de componente recibe un ID único la primera vez que esta
	 * función es llamada. Gracias al uso de una variable estática local,
	 * el mismo tipo siempre devolverá el mismo identificador.
	 *
	 * @tparam T Tipo del componente.
	 * @return ComponentTypeID Identificador único del tipo de componente.
	 */
	template<typename T>
	[[nodiscard]] ComponentTypeID GetComponentTypeID() noexcept
	{
		static const ComponentTypeID id = NextComponentTypeID();
		return id;
	}

	//Sparse set: Estructura de datos centralizada para almecenar componentes 
}
