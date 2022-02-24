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

template <typename Type>
concept Updateable = requires(Type a, float dt)
{
	{ a.Update(dt) } -> std::convertible_to<void>;
};

template <ManagedVectorType ManagedType, typename IDType = ID<ManagedType>>
requires Updateable<ManagedType>
class Manager
{
public:
	template<typename ...Args>
	inline IDType Create(Args... args)
	{
		ManagedType type(std::forward<Args>(args)...);
		IDType id = container_.GetNextID();
		types_to_add_.emplace_back(std::move(id), std::move(type));
		return id;
	}

	inline void Remove(const IDType& toRemove)
	{
		ids_to_remove_.push_back(toRemove);
	}

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

	inline ManagedType& Get(const IDType& id)
	{
		return const_cast<ManagedType&>(const_cast<const Manager*>(this)->Get(id));
	}

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

	inline void Clear()
	{
		clear_container_ = true;
	}

	inline bool IsEmpty() const noexcept
	{
		return container_.IsEmpty() && types_to_add_.empty();
	}

private:
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

	ManagedVector<ManagedType> container_;
	std::vector<std::pair<IDType, ManagedType>> types_to_add_{};
	std::vector<IDType> ids_to_remove_{};

	bool clear_container_{ false };
};
