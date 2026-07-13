#pragma once
#include "ECS/Types.h"
#include "ECS/ComponentPool.h"
#include "ECS/View.h"
#include "ECS/System.h"

namespace ECS {
    /**
    * @class Registry
    * @brief Punto de acceso único para crear/destruir entidades, añadir/consultar componentes,
    * obtener Views y ejecutar los sistemas del ECS.
    */
    class
        Registry {
    public:
        /// @brief Crea una nueva entidad, reutilizando índices libres si existen.
        /// @return ID de la entidad creada
        EntityID CreateEntity() {
            EntityIndex idx;
            if (!m_freeList.empty()) {
                idx = m_freeList.front();
                m_freeList.pop();
            }
            else {
                idx = static_cast<EntityIndex>(m_versions.size());
                m_versions.push_back(0);
                m_entities.push_back(NULL_ENTITY);   // placeholder
            }

            EntityID id = MakeEntityID(idx, m_versions[idx]);
            m_entities[idx] = id;
            return id;
        }

        /// @brief Destruye una entidad: elimina todos sus componentes e invalida su ID
        /// @param entity Entidad a destruir.
        void
            DestroyEntity(EntityID entity) {
            assert(IsAlive(entity) && "DestroyEntity: entidad inválida o ya destruida");

            // Elimina todos los componentes de esta entidad
            for (auto& [typeID, pool] : m_componentPools)
                pool->RemoveEntity(entity);

            // Incrementa versión  los IDs viejos quedan inválidos
            const EntityIndex idx = GetEntityIndex(entity);
            ++m_versions[idx];
            m_entities[idx] = NULL_ENTITY;
            m_freeList.push(idx);
        }

        /// @brief Indica si una entidad sigue viva (no destruida).
        /// @param entity Entidad a verificar.
        [[nodiscard]] bool
            IsAlive(EntityID entity) const noexcept {
            const EntityIndex idx = GetEntityIndex(entity);
            return idx < m_entities.size() && m_entities[idx] == entity;
        }

        /// @brief Número de entidades vivas actualmente.
        [[nodiscard]] std::size_t
            EntityCount() const noexcept {
            return m_entities.size() - m_freeList.size();
        }

        // Todas las ranuras (incluye NULL_ENTITY para los huecos libres).
        // Útil para el Serializer; filtra con IsAlive.
        [[nodiscard]] const std::vector<EntityID>&
            GetAllEntities() const noexcept {
            return m_entities;
        }

        /**@brief Añade un componente T a la entidad, construido in - place con los argumentos dados.
        * @tparam T Tipo de componente.
        * @param entity Entidad destino.
        * @param args Argumentos del constructor de T
        */        template<typename T, typename... Args> T&
            AddComponent(EntityID entity, Args&&... args) {
            assert(IsAlive(entity) && "AddComponent: entidad inválida");
            return GetOrCreatePool<T>()->Add(entity, std::forward<Args>(args)...);
        }

        /// @brief Elimina el componente T de la entidad (no-op si no lo tiene).
        template<typename T> void
            RemoveComponent(EntityID entity) {
            if (auto* pool = GetPool<T>())
                pool->Remove(entity);
        }

        /// @brief  el componente (o lo añade si no existía).
        template<typename T>
        T& SetComponent(EntityID entity, T value) {
            assert(IsAlive(entity) && "SetComponent: entidad inválida");
            auto* pool = GetOrCreatePool<T>();
            if (pool->Contains(entity)) {
                pool->Get(entity) = std::move(value);
                return pool->Get(entity);
            }
            return pool->Add(entity, std::move(value));
        }

        /// @brief Indica si la entidad posee un componente de tipo T.
        template<typename T>
        [[nodiscard]] bool HasComponent(EntityID entity) const noexcept {
            const auto* pool = GetPoolConst<T>();
            return pool && pool->Contains(entity);
        }

        /// @brief Obtiene el componente T de la entidad (acceso garantizado, assert si no existe).
        template<typename T>
        [[nodiscard]] T& GetComponent(EntityID entity) {
            assert(IsAlive(entity));
            auto* pool = GetPool<T>();
            assert(pool && "GetComponent: pool no existe para este tipo");
            return pool->Get(entity);
        }

        /// @brief Versión const de GetComponent.
        template<typename T>
        [[nodiscard]] const T& GetComponent(EntityID entity) const
        {
            assert(IsAlive(entity));
            const auto* pool = GetPoolConst<T>();
            assert(pool && "GetComponent: pool no existe para este tipo");
            return pool->Get(entity);
        }

        ///@brief Acceso seguro: devuelve nullptr si la entidad no tiene el componente.
        template<typename T>
        [[nodiscard]] T* TryGetComponent(EntityID entity) noexcept
        {
            auto* pool = GetPool<T>();
            return pool ? pool->TryGet(entity) : nullptr;
        }

        //  Views (queries multi-componente)
        // Ejemplo: registry.GetView<Transform, Velocity>()
        template<typename... Components>
        [[nodiscard]] View<Components...> GetView() {
            return View<Components...>(GetOrCreatePool<Components>()...);
        }

        //  Sistemas
        template<typename T, typename... Args>
        T& AddSystem(Args&&... args)
        {
            static_assert(std::is_base_of_v<System, T>, "T debe derivar de ECS::System");
            auto system = std::make_unique<T>(std::forward<Args>(args)...);
            T& ref = *system;
            system->OnStart(*this);
            m_systems.push_back(std::move(system));
            return ref;
        }

        /// @brief Ejecuta OnUpdate en todos los sistemas habilitados
        /// @param deltaTime Delta time del frame actual
        void UpdateSystems(float deltaTime)
        {
            for (auto& system : m_systems)
                if (system->IsEnabled())
                    system->OnUpdate(*this, deltaTime);
        }

        /// @brief Llama a OnDestroy en todos los sistemas y los elimina del registry
        void RemoveAllSystems()
        {
            for (auto& system : m_systems)
                system->OnDestroy(*this);
            m_systems.clear();
        }

        //  Utilidades
        ///@brief Destruye todo: entidades, componentes y sistemas.
        void
            Clear() {
            RemoveAllSystems();
            for (auto& [typeID, pool] : m_componentPools)
                pool->Clear();
            m_entities.clear();
            m_versions.clear();
            while (!m_freeList.empty()) m_freeList.pop();
        }

        /// Acceso a pools sin tipo (para el Serializer)
        [[nodiscard]] const std::unordered_map<ComponentTypeID, std::unique_ptr<IComponentPool>>&
            GetPools() const noexcept { return m_componentPools; }

    private:
        /// @brief Helpers privados 
        template<typename T>
        ComponentPool<T>* GetOrCreatePool() {
            const ComponentTypeID typeID = GetComponentTypeID<T>();
            auto it = m_componentPools.find(typeID);
            if (it == m_componentPools.end())
            {
                auto [newIt, ok] = m_componentPools.emplace(
                    typeID, std::make_unique<ComponentPool<T>>());
                return static_cast<ComponentPool<T>*>(newIt->second.get());
            }
            return static_cast<ComponentPool<T>*>(it->second.get());
        }

        /// @brief Obtiene el pool de T (mutable), o nullptr si no existe
        template<typename T>
        ComponentPool<T>* GetPool() noexcept {
            const ComponentTypeID typeID = GetComponentTypeID<T>();
            auto it = m_componentPools.find(typeID);
            return (it != m_componentPools.end())
                ? static_cast<ComponentPool<T>*>(it->second.get())
                : nullptr;
        }

        /// @brief Obtiene el pool de T (const), o nullptr si no existe
        template<typename T>
        const ComponentPool<T>* GetPoolConst() const noexcept {
            const ComponentTypeID typeID = GetComponentTypeID<T>();
            auto it = m_componentPools.find(typeID);
            return (it != m_componentPools.end())
                ? static_cast<const ComponentPool<T>*>(it->second.get())
                : nullptr;
        }

    private:
        // Entidades 
        std::vector<EntityID>      m_entities; ///< IDs por índice, NULL_ENTITY en huecos libres.
        std::vector<EntityVersion> m_versions; ///< Versión actual de cada índice, para invalidar IDs viejos.
        std::queue<EntityIndex>    m_freeList; ///< Índices libres reutilizables al crear entidades


        //  Componentes 
        std::unordered_map<ComponentTypeID, std::unique_ptr<IComponentPool>> m_componentPools;

        // Sistemas 
        std::vector<std::unique_ptr<System>> m_systems;
    };
}