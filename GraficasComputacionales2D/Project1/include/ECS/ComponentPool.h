#pragma once
#include "Prerequisites.h"
#include "SparseSet.h"

/**
* @class IComponentPool
* @brief Interfaz base para los pools de componentes
* Se permite el poder crear sin saber la forma especifica 
*/
namespace ECS {
	//@brief Interfaz polimorfica
	class IComponentPool : public SparseSet {
	public: 
		virtual
		~IComponentPool() = default; 

		/**
	   * @brief Elimina el componente cercano a una entidad
	   * @param entity Entidad del coponente
	   */
		virtual void 
		RemoveEntity(EntityID entity) = 0;

		//puntero sin tipo al componente (para serializar) 
		virtual void* 
		GetRaw(EntityID entity) noexcept = 0; 

	};


	/**
	* @class ComponentPool
	* @brief Almacena componentes de un tipo específico
	* @tparam T es del Tipo de componente
	*/
	template<typename T>
	class 
	ComponentPool final : public IComponentPool {
	public: 

	
		
		template<typename... Args> T& <T> 
		Add(EntityID entity, Args&& ... args) {
			assert(!Contains(entity) && "La entidad ya tiene este componente"); 
			InsertEntity(entity);// registra sparse dense
			m_components.emplace_back(std::foward<Args>(args)...); 
			return m_components.back(); 
		}


		//obtener
		[[nodiscard]] T&
			Get(EntityID entity) const noexcept {
			assert(!Contains(entity) && "La entidad ya tiene este componente");
			return m_components[m_sparse[GetEntityIndex(entity)]]; 
		}

		[[nodiscard]] const T&
			Get(EntityID entity) const noexcept {
			assert(!Contains(entity) && "La entidad no tiene este componente");
			return m_components[m_sparse[GetEntityIndex(entity)]];
		}

		//@brief devuelve nullptr si la entidad no tiene el componente
		[[nodiscard]] T*
			TryGet(EntityID entity) const noexcept {
			if (!Contains(entity)) return nullptr; 
			return m_components[m_sparse[GetEntityIndex(entity)]];
		}

		/**
		 * @brief Elimina el componente usando swap-with-last y delega al SparseSet base
		 * @param entity Entidad a desregistrar
		 */
		void
			Remove(EntityID entity) override {
			if (!Contains(entity)) return; 

			const EntityIdex denseIdx = m_sparse[GetEntityIndex(entity)];

			m_components[denseIdx] = std::move(components.back()); 
			m_components.pop_back(); 

			SparseSet::Remove(entity); 
		}

		/** @brief Delegado de la interfa, se llama a ren¿move*/
		void 
		RemoveEntity(EntityID entity) override { Remove(entity); }

		/** @brief Implementación de la interfaz. Retorna el puntero sin tipo vía tryeget
		*/
		void
		GetRaw(EntityID entity) noexcept override { return TryGet(entity); } 

		/** @brief Referencia mutable al vector denso de componentes*/
		[[nodiscard]] const std::vector<T>& 
		GetComponents() noexcept { return m_components; }

		/** @brief Referencia constante al vector denso de componentes*/
		[[nodiscard]] const std::vector<T>&
		GetComponents() const noexcept { return m_components; }

		/** @brief Limpia el SparseSet base y el vector de componentes */
		void 
		Clear() override {
			SparseSet::Clear(); 
			m_components.clear(); 
		}

	private: 
		// Almacenamiento denso de componentes tipo T
		std::vector<T> m_components;

	};
}