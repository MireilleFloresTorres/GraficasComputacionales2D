#pragma once
#include "ECS/ComponentPool.h"

/**
 * @class View
 * @brief Query multi-componente: itera solo entidades que posean todos los tipos indicados
 * @tparam Components LOS tipos de componentes requeridos
 */
namespace ECS {
	template<typename... Components>
	class View {
	public: 

		/** @brief Construye la view a partir de los pools dados y localiza el más pequeño*/
		explicit View(ComponentPool<Components>* ... pools) noexcept
		: m_pools(pools...) {
		FindSmallest(); 
	}
	
		/**
		* @brief Itera entidades válidas e invoca `func(EntityID, Components&...)`.
		*
		* Recorre en orden inverso para permitir eliminaciones seguras durante la iteración.
		* @tparam Func Callable con firma `void(EntityID, Components&...)`.
		*/
	template<typename Func> 
		void Each(Func&& func) {
		if (!m_smallest) return; 

		const auto& entities = m_smallest->GetEntities(); 
		//Recorrido inverso -> seguro al eliminar duranete al iteración
		for (std::size_t i = entities.size(); i > 0; --i) {

			const EntityID entity = entities[i - 1]; 
			if (AllHave(entity))
			{
				std::apply(
					[&](auto*... pools) {
						func(entity, pools->Get(entity)...);
					},
					m_pools); 
			}
		}
	}

	/**
	 * @brief Itera entidades válidas invocando `func(EntityID)` sin desempaquetar componentes.
	 *
	 * Útil cuando se accede a los componentes manualmente.
	 * @tparam Func Callable con firma `void(EntityID)`.
	 */
	template<typename Func> 
	void EachEntityt(Func&& func)
	{
		if (!m_smallest) return;
		const auto& entities = m_smallest->GetEntities(); 
		for (std::size_t i = entities.size(); i > 0; --)
		{
			const EntityID entity = entities[i - 1]; 
			if (AllHave(entity))
				func(entity); 
		}
	}

	[[nodiscard ]] bool Empty() const noexcept { return !m_smallest || m_smallest->empty(); }
	[[nodiscard ]] std::size_t Size() const noexcept { return m_smallest ? m_smallest->Size() : 0;  }

	private:
	/**
	* @brief Selecciona recursivamente el pool con menos elementos (mejor filtro inicial).
	* @tparam I Índice de iteración en tiempo de compilación.
	*/
	template<std::size_t I = 0>
	void FindSmallest() noexcept {
		if constexpr (I < sizeof...(Components)) {
			auto* pool = std::get<I>(m_pools); 
			if (pool && (!m_smallest || pool->Size() < m_smallest->Size()))
				m_smallest = pool;
			FindSmallest<I + 1>(); 
		}
	}

	/** @brief Retorna `true` si todos los pools contienen la entidad indicada. */
	[[nodiscard]] bool AllHave(EntityID entity) const noexcept 
	{
		return std::apply(
			[entity](auto*... pools) noexcept {
				return (... &&(pools && pools->Contains(entity)));
			},
			m_pools); 
	}

	private: 
	std::tuple<ComponentPool<Components>* ...> m_pools;
	const SparseSet* m_smallest = nullptr; 
	};

}