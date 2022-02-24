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

template <ManagedVectorType ManagedType, typename IDType = ID<ManagedType>>
class ManagedVector
{
public:
	using InternalID = IDType;
	using InternalType = ManagedType;

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

	inline void Clear() noexcept
	{
		container_.clear();
		index_to_id_.clear();
		id_to_index_.clear();
		id_counter_ = 0;
	}

	inline IDType GetNextID() noexcept
	{
		return IDType(++id_counter_);
	}

	inline bool IsEmpty() const
	{
		return container_.empty();
	}

	inline bool Has(const IDType& id) const
	{
		return id_to_index_.find(id) != id_to_index_.end();
	}

	inline ManagedType& Get(const IDType& id)
	{
		return const_cast<ManagedType&>(const_cast<const ManagedVector*>(this)->Get(id));
	}

	inline const ManagedType& Get(const IDType& id) const
	{
		auto iter = id_to_index_.find(id);
		assert(iter != id_to_index_.end()); // Given id is not valid within the managed container
		return container_[iter->second];
	}

	inline size_t Size() const
	{
		return container_.size();
	}

	ManagedType& operator[](size_t index)
	{
		return container_[index];
	}

	const ManagedType& operator[](size_t index) const
	{
		return const_cast<ManagedVector&>(*this)[index];
	}

private:
	std::vector<ManagedType> container_{};
	std::unordered_map<size_t, IDType> index_to_id_{};
	std::unordered_map<IDType, size_t> id_to_index_{};
	IDType::InternalType id_counter_{ 0 };

	inline void ValidateMaps(IDType id, size_t index) const
	{
		assert(id_to_index_.size() == index_to_id_.size());
		auto idToIndex = id_to_index_.find(id);
		assert(idToIndex != id_to_index_.end());
		assert(idToIndex->second == index);
	}
};

template <typename Type, typename From>
struct std::hash<ID<Type, From>>
{
	std::size_t operator()(const ID<Type, From>& id) const
	{
		return std::hash<From>{}(id.Value());
	}
};
