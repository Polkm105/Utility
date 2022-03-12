//---------------------------------------------------------------------------------------
// Author:      Kenny Mecham
// Date:        02/24/2022
// Project:     Utility
// Description: A generic type manager that utilizes a ManagedVector to keep track of all
//              it's held objects. Can be used in future ECS systems or in type-erased
//              flyweight systems in conjuntion with a factory
//---------------------------------------------------------------------------------------
#pragma once
#include <concepts>
#include <unordered_map>
#include <assert.h>
#include <type_traits>
#include <cstddef>
#include <algorithm>
#include <utility>
#include "ID.h"
#include "ManagedVector.h"
#include "CommonConcepts.h"

// Manages a vector of specified types, places and removes queued objects on the next update to prevent memory movement while updating
// the placement of objects could be optimized to be directly after the object finishes updating, and the removal could be the same
template <ManagedVectorType ManagedType, typename IDType = ID<ManagedType>>
requires Updateable<ManagedType>
class Manager
{
public:
	// Creates a held type and queues it to add to the managed vector
	template<typename ...Args>
	inline IDType Create(Args... args)
	{
		ManagedType type(std::forward<Args>(args)...);
		IDType id = container_.GetNextID();
		types_to_add_.emplace_back(std::move(id), std::move(type));
		return id;
	}

	// Removes the object with the specified ID
	inline void Remove(const IDType& toRemove)
	{
		ids_to_remove_.push_back(toRemove);
	}

	// Checks if the manager has an object with the specified ID
	inline bool Has(const IDType& id) const
	{
		if (container_.Has(id))
		{
			return true;
		}

		for (auto& pair : types_to_add_)
		{
			if (pair.first == id)
			{
				return true;
			}
		}

		return false;
	}

	// Gets a reference to the object with the specified ID
	inline ManagedType& Get(const IDType& id)
	{
		return const_cast<ManagedType&>(const_cast<const Manager*>(this)->Get(id));
	}

	// Gets a const reference to the object with the specified ID
	inline const ManagedType& Get(const IDType& id) const
	{
		if (container_.Has(id))
		{
			return container_.Get(id);
		}

		for (const auto& pair : types_to_add_)
		{
			if (pair.first == id)
			{
				return pair.second;
			}
		}
	}

	// Adds and removes queued objects, updates existing objects
	inline void Update(float dt)
	{
		ClearIfNeeded();
		AddCreatedObjects();
		RemoveQueuedObjects();

		size_t size = container_.Size();
		for (size_t i = 0; i < size; ++i)
		{
			container_[i].Update(dt);
		}
	}

	// Destroys all objects in the manager
	inline void Clear()
	{
		clear_container_ = true;
	}

	// Checks if the manager has any objects in it
	inline bool IsEmpty() const noexcept
	{
		return container_.IsEmpty() && types_to_add_.empty();
	}

private:
	// Clears the container if specified
	inline void ClearIfNeeded()
	{
		if (clear_container_)
		{
			container_.Clear();
			types_to_add_.clear();
			ids_to_remove_.clear();
			clear_container_ = false;
		}
	}

	// Adds any created objects that were queued
	inline void AddCreatedObjects()
	{
		if (!types_to_add_.empty())
		{
			for (auto&& obj : types_to_add_)
			{
				container_.Add(std::move(obj.second), std::move(obj.first));
			}

			types_to_add_.clear();
		}
	}

	//Removes any destroyed objects that were queued
	inline void RemoveQueuedObjects()
	{
		if (!ids_to_remove_.empty())
		{
			for (const IDType& id : ids_to_remove_)
			{
				if (container_.Has(id))
				{
					container_.Remove(id);
				}
			}

			ids_to_remove_.clear();
		}
	}

	ManagedVector<ManagedType> container_;												// vector that holds all the objects
	std::vector<std::pair<IDType, ManagedType>> types_to_add_{};	// queue of objects to add
	std::vector<IDType> ids_to_remove_{};													// queue of objects to remove
	bool clear_container_{ false };																// flag to indicate if the manager should be cleared
};
