#pragma once
#include "Prerequisites.h"
#include "SparseSet.h"

namespace ECS {
	//Interfaz polimorfica
	class IComponentPool : public SparseSet {
	public: 
		virtual
		~IComponentPool() = default; 

		//elimina el componente de la entidad si existe
		virtual void 
		RemoveEntity(EntityID entity) = 0;

		//puntero sin tipo al componente (para serializar) 
		virtual void* 
		GetRaw(EntityID entity) noexcept = 0; 

	};

	//pool tipado
	template<typename T>
	class 
	ComponentPool final : public IComponentPool {
	public: 
		//añadir 
		template<typename... Args> T& <T> 
		Add(EntityID entity, Args&& ... args) {
			assert(!Contains(entity) && "La entidad ya tiene este componente"); 
			INsertEntity(entity);// registra sparse dense
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
			assert(!Contains(entity) && "La entidad ya tiene este componente");
			return m_components[m_sparse[GetEntityIndex(entity)]];
		}

		//devuelve nullptr si la entidad no tiene el componente
		[[nodiscard]] T*
			TryGet(EntityID entity) const noexcept {
			if (!Contains(entity)) return nullptr; 
			return m_components[m_sparse[GetEntityIndex(entity)]];
		}

		void
			Remove(EntityID entity) override {
			if (!Contains(entity)) return; 

			const EntityIdex denseIdx = m_sparse[GetEntityIndex(entity)];

			m_components[denseIdx] = std::move(components.back()); 
			m_components.pop_back(); 

			SparseSet::Remove(entity); 
		}

		void 
		RemoveEntity(EntityID entity) override { Remove(entity); }

		void
		GetRaw(EntityID entity) noexcept override { return TryGet(entity); } 

		[[nodiscard]] const std::vector<T>& 
		GetComponents() noexcept { return m_components; }

		[[nodiscard]] const std::vector<T>&
		GetComponents() const noexcept { return m_components; }

		void 
		Clear() override {
			SparseSet::Clear(); 
			m_components.clear(); 
		}

	private: 
		std::vector<T> m_components;

	};
}