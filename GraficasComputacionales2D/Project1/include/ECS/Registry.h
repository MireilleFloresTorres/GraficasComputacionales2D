#pragma once
#include "ECS/Types.h"
#include "ECS/ComponentPool.h"
#include "ECS/View.h"
#include "ECS/System.h"
//INCLUDE ecs Y SISTEMS

namespace  ECS {
	class Registry {
	public: 
		EntityID CreateEntity() {
			EntityIndex idx; 
			if (!m_freelist.empty()) {
				idx = m_freelist.front(); 
				m_freelist.pop(); 
			}
			else {
				idx = static_cast<EntityIndex>(m_versions.size()); 
				m_versions.push_back(0); 
				m_entities.push_back(NULL_ENTITY); //placeholder
			}

			EntityID id = MakeEntityID(idx, m_versions[idx]);
			m_entities[idx] = id; 
			return id; 
		}
		
		void
			DestroyEntity(EntityID entity) {
			assert(IsAlive(entity) && "DestroyEntity: entidad inválida o ya destuida"); 

			for (auto& [typeID, pool] : m_componentPools)
				pool->RemoveEntity(entity); 

			const EntityIndex idx = GetEntityIndex(entity); 
			++m_versions[idx]; //invalida IDs antiguos
			m_entities[idx] = NULL_ENTITY; 
			m_freelist.push(idx); 
		}
		
		[[nodiscard]] bool
			IsAlive(EntityID entity) const noexcept {
			const EntityIndex idx = GetEntityIndex(entity); 
			return idx < m_entities.size() && m_entities[idx] == entity; 
		}

		[[nodiscard]] std:: size_t
			EntityCount() const noexcept {

			return m_entities.size() - m_freelist.size(); 
		}

		[[nodiscard]]const std::vector<EntityID> &
			GetAllEntities() const noexcept {
			return m_entities; 
		}

		template<typename T, typename... Args> T&
			AddComponent(EntityID entity, Args&& ... args) {
			assert(IsAlive(entity) && "AddComponent: entidad ivalida"); 
			return GetorCreatePool<T>()->Add(entity, std::forward<Args>(args)...);
		}

		template<typename T> void
			RemoveComponent(EntityID entity) {
			if(auto* pool = GetPool<T>()) 
				pool->Remove(entity);
		}

		template<typename T> 
		[[nodiscard]] bool HasComponent(EntityID entity) const noexcept {
			const auto* pool = GetPoolConst<T>();
			return pool && pool->Contains(entity);
		}

		template<typename T> 
		[[nodiscard]] T& GetComponent(EntityID entity) const {
			assert(IsAlive(entity) && "GetComponent: entidad inválida"); 
			auto* pool = GetPool<T>(); 
			assert(pool && pool->Contains(entity) && "GetComponent: pool no existe apra este tipo"); 
			return pool->Get(entity); 
		}

		template<typename T>
		[[nodiscard]] const T& GetComponent(EntityID entity) const {
			assert(IsAlive(entity));
			const auto* pool = GetPool<T>();
			assert(pool && "GetComponent:pool no existe para este tipi");
			return pool->Get(entity);
		}

		//acceso seguro que devuelve el puntero nullo si no tiene el componente
		template<typename T>
		[[nodiscard]] T* TryGetComponent(EntityID entity) noexcept {
			auto* pool = GetPool<T>();
			return pool ? pool->TryGet(entity): nullptr;
		}

		//viewa (queries multi-componente)
		//ejemplo: registry.GetView<Transform, Velocity>()
		template<typename... Components>
		[[nodiscard ]] View<Components...> GetView() {
			return View<Components...>(GetOrCreatedPool<Components>()...); 
		}

		template<typename T, typename... Args> 
		T& AddSystem(Args&&... args)
		{
			static_assert(std::is_base_of_v<System, T>, "T must derive from System");
			auto system = std::make_unique<T>(std::forward<Args>(args)...);
			T& ref = *system; 
			system->OnStart(*this); 
			m_systems.push_back(std::move(system)); 
			return ref; 
		}

		void
		UpdateSystems(float deltaTime)
		{
			for(auto& system : m_systems)
				if(system->IsEnabled())
				system->OnUpdate(*this, deltaTime);
		}

		void
		RemoveAllSystems()
		{
			for (auto& system : m_systems)
				system->OnDestroy(*this); 
			m_systems.clear();  
		}

		//utilidades 
		//destruye tood: entidades, componentes y sistemas
		void
		Clear() {
			RemoveAllSystems(); 
			for (auto& [typeID, pool] : m_componentPools)
				pool->Clear(); 
			m_entities.clear(); 
			m_versions.clear(); 
			while (!m_freelist.empty()) m_freelist.pop(); 
		}

		//Acceso a pools sin tipo (para el serializer) 
		[[nodiscard]] const std::unordered_map<ComponentTypeID, std::unique_ptr<IComponentPool>>&
			GetPools() const noexcept { return m_componentPools;  }

	private: 
		//----- helpers privados -----
		template<typename T> 
		ComponentPool<T>* GetOrCreatePool()
		{
			const ComponentTypeID typeID = GetComponentTypeID<T>(); 
			auto it = m_componentPools.find(typeID); 
			if (it == m_componentPools.end()) {
				auto [newit, ok] = m_componentPools.emplace(
					typeID, std::make_unique<ComponentPool<T>>());
				return static_cast<ComponentPool<T>*>(newit->second.get()); 
			}
			return static_cast<ComponentPool<T>*>(it->second.get());
		}

		template<typename T> 
		ComponentPool<T>* GetPool() noexcept {
			const ComponentTypeID typeID = GetComponentTypeID<T>(); 
			auto it = m_componentPools.find(typeID); 
			return (it != m_componentPools.end() )
				? static_cast<ComponentPool<T>*>(it->second.get()) 
				: nullptr; 
		}

		template<typename T>
		ComponentPool<T>* GetPoolConst() const noexcept {
			const ComponentTypeID typeID = GetComponentTypeID<T>();
			auto it = m_componentPools.find(typeID);
			return (it != m_componentPools.end())
				? static_cast<ComponentPool<T>*>(it->second.get())
				: nullptr;
		}
		

	private:
		//----Entidades------
		std::vector<EntityID>      m_entities; 
		std::vector<EntityVersion> m_versions;
		std::queue<EntityIndex>   m_freelist; 

		//----Componentes------
		std::unordered_map<ComponentTypeID, std::unique_ptr<IComponentPool>> m_componentPools;

		//----Sistemas------
		std::vector<std::unique_ptr<System>> m_systems; 
	};
}
