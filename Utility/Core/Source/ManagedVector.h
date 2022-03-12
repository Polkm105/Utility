//---------------------------------------------------------------------------------------
// Author:      Kenny Mecham
// Date:        02/24/2022
// Project:     Utility
// Description: A managed vector that keeps all elements contiguous in memory while also
//              offering type-safe ID access and remove. Very fast to, but also very 
//              large in memory 
//---------------------------------------------------------------------------------------
#pragma once
#include <vector>
#include <unordered_map>
#include <concepts>
#include <type_traits>
#include <utility>
#include <assert.h>
#include "ID.h"

template <typename T>
concept ManagedVectorType = requires(T a)
{
	std::move_constructible<T>;
	std::is_move_assignable_v<T>;
};

// A Vector that keeps all object contigously in memory
// allows for average of O(1) time for add, remove, find, and access 
// uses swap-pop to keep memory contiguous and utilizes hash maps + TypeSafeID's for fast lookups
template <ManagedVectorType ManagedType, typename IDType = ID<ManagedType>>
class ManagedVector
{
public:
	using InternalID = IDType;
	using InternalType = ManagedType;

	// Adds a type to the managed with the specified ID
	inline IDType Add(ManagedType&& type, IDType id = IDType(0))
	{
		if (!id.IsValid())
		{
			id = IDType(++id_counter_);
		}

		assert(id_to_index_.find(id) == id_to_index_.end());

		size_t index = container_.size();
		index_to_id_[index] = id;
		id_to_index_[id] = index;
		container_.push_back(std::move(type));
		return id;
	}

	// Removes a type from the managed that had the specified ID O(1) time
	inline void Remove(const IDType& idToDestroy)
	{
		auto idToIndexIter = id_to_index_.find(idToDestroy);

		assert(idToIndexIter != id_to_index_.end());	// make sure the ID is valid

		size_t indexToDestroy = idToIndexIter->second;

		size_t indexToMove = container_.size() - 1;
		auto indexToIdIterToMove = index_to_id_.find(indexToMove);
		assert(indexToIdIterToMove != index_to_id_.end());
		IDType idToMove = indexToIdIterToMove->second;

		// check umaps against each other
#ifdef _DEBUG
		ValidateMaps(idToMove, indexToMove);
		ValidateMaps(idToDestroy, indexToDestroy);
#endif

		id_to_index_.erase(idToDestroy);
		id_to_index_.erase(idToMove);
		index_to_id_.erase(indexToDestroy);
		index_to_id_.erase(indexToMove);

		// only update the maps
		if (indexToDestroy != indexToMove)
		{
			container_[indexToDestroy] = std::move(container_[indexToMove]);
			index_to_id_.emplace(indexToDestroy, idToMove);
			id_to_index_.emplace(idToMove, indexToDestroy);
		}

		container_.pop_back();
	}

	// Removes a specified object from the managed vector, O(n) time
	inline void Remove(const ManagedType& type)
	{
		size_t num_objects = container_.size();
		for (size_t i = 0; i < num_objects; ++i)
		{
			if (container_[i] == type)
			{
				const auto& ID = index_to_id_[i];
				Remove(ID);
			}
		}
	}

	// clears the vector
	inline void Clear() noexcept
	{
		container_.clear();
		index_to_id_.clear();
		id_to_index_.clear();
	}

	// gets the next valid ID, and increments the ID counter
	inline IDType GetNextID() noexcept
	{
		return IDType(++id_counter_);
	}

	// Checks if the vector is empty
	inline bool IsEmpty() const
	{
		return container_.empty();
	}

	// Checks if an object with the specified ID exists within the vector
	inline bool Has(const IDType& id) const
	{
		return id_to_index_.find(id) != id_to_index_.end();
	}

	// Gets a reference to a object with the specified ID
	inline ManagedType& Get(const IDType& id)
	{
		return const_cast<ManagedType&>(const_cast<const ManagedVector*>(this)->Get(id));
	}

	// Gets a const reference to an object with the specified ID
	inline const ManagedType& Get(const IDType& id) const
	{
		auto iter = id_to_index_.find(id);
		assert(iter != id_to_index_.end()); // Given id is not valid within the managed container
		return container_[iter->second];
	}

	// Gets the number of objects currently in the vector
	inline size_t Size() const
	{
		return container_.size();
	}

	// Gets a reference to the object at the specified index
	ManagedType& operator[](size_t index)
	{
		return container_[index];
	}

	// Gets a const reference to the object at the specified index
	const ManagedType& operator[](size_t index) const
	{
		return const_cast<ManagedVector&>(*this)[index];
	}

	// Gets an iterator to the beginning of the vector
	friend std::vector<ManagedType>::iterator begin(ManagedVector& managedVec)
	{
		return managedVec.container_.begin();
	}

	// Gets an iterator to the end of the vector
	friend std::vector<ManagedType>::iterator end(ManagedVector& managedVec)
	{
		return managedVec.container_.end();
	}

private:
	std::vector<ManagedType> container_{};							// the container holding all the objects
	std::unordered_map<size_t, IDType> index_to_id_{};	// the hash map that converts an objects index to it's ID
	std::unordered_map<IDType, size_t> id_to_index_{};	// the hash map that converst an objects ID to it's index
	IDType::InternalType id_counter_{ 0 };							// the interanal ID counter that allows for Unique ID's for each object

	// makes sure that the index_to_id and id_to_index map match 
	inline void ValidateMaps(IDType id, size_t index) const
	{
		assert(id_to_index_.size() == index_to_id_.size());
		auto idToIndex = id_to_index_.find(id);
		assert(idToIndex != id_to_index_.end());
		assert(idToIndex->second == index);
	}
};

template <typename Type, typename StorageType>
struct std::hash<ID<Type, StorageType>>
{
	std::size_t operator()(const ID<Type, StorageType>& id) const
	{
		return std::hash<StorageType>{}(id.Value());
	}
};
