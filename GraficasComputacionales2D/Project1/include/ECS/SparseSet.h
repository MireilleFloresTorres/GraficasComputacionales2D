#pragma once
#include "Prerequisites.h"
#include "Types.h"

namespace ECS{
	class
	SparseSet {
	public: 
		SparseSet(); 
		virtual ~SparseSet() = default; 

	// Consultas
	[[nodisdcard]] bool Contains(EntityID entity) const noexcept {
		const EntityIndex idx = GetEntityIndex(entity); 
		if (idx >= m_sparse.size()) return false; 
		const EntityIndex denseIdx = m_sparse[idx]; 
		return denseIdx < m_dense.size() && m_dense[denseIdx] == entity; 
	}

	
	[[nodiscard]] size_t Size() const noexcept { return m_dense.size(); }
	[[nodiscard]] bool empty() const noexcept { return m_dense.empty(); }

	[[nodiscard]] const std::vector<EntityID>& GetEntities() const noexcept {
		return m_dense; 
	}



	//---- Eliminaciín (swap-with-last) ----
	//las subclases deben llamar a esta base después de 
	//sincronizar sus prorpios arrays (ver component pool:: remove) 
	virtual void Remove(EntityID entity) {

		const EntityIndex sparseIdx = GetEntityIndex(entity); 
		const EntityIndex denseidx = m_sparse[sparseIdx];
		const EntityID last = m_dense.back(); 

		//mueve el ultimo elemento al hueco
		m_dense[denseidx] = last;
		m_sparse[GetEntityIndex(last)] = denseidx;
		m_dense.pop_back(); 

		//invlida la entrada eliminada
		m_sparse[sparseIdx] = INVALID; 
	}

	virtual void Clear()
	{
		m_sparse.clear(); 
		m_dense.clear(); 
	}

	protected: 
	//RESERCA ESPACIO EN M_SPARSE Y REGISTRA LA ENTIDAD EN M_DENSE
	//Devuelve el dense Index asignadao

	EntityIndex InsertEntity(EntityID entity)
	{
		const EntityIndex sparseIdx = GetEntityIndex(entity);
		const EntityIndex denseIdx = static_cast<EntityIndex>(m_dense.size()); 

		if (sparseIdx >= m_sparse.size()) 
			m_sparse.resize(sparseIdx + 1, INVALID); 
			assert(m_sparse[sparseIdx] == INVALID && "La entidad ya está en el set"); 

			m_sparse[sparseIdx] = denseIdx;
			m_dense.push_back(entity); 
			return denseIdx; 
		
	}
	
	protected: 
		static constexpr EntityIndex INVALID = std::numeric_limits<EntityIndex>::max(); 

		std::vector<EntityIndex> m_sparse; //sparse[entityIndex] -> dense index
		std::vector<EntityID> m_dense; //dense[i] -> EntityID
	};
}