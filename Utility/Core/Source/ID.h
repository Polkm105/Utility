#pragma once
#include <cstddef>
#include <concepts>

template <typename Type, typename StorageType = std::size_t>
requires std::weakly_incrementable<StorageType>
class ID
{
public:
	using InternalType = StorageType;

	explicit ID(StorageType id = 0) noexcept:
		id_(id) 
	{}

	bool operator==(const ID& other) const noexcept
	{
		return Value() == other.Value();
	}

	StorageType Value() const noexcept
	{
		return id_;
	}

	bool IsValid() const noexcept
	{
		return id_ != 0;
	}

private:
	StorageType id_{ 0 };
};
