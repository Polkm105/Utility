//---------------------------------------------------------------------------------------
// Author:      Kenny Mecham
// Date:        02/24/2022
// Project:     Utility
// Description: A type safe ID system used in managers and other systems that hold data
//              for specific types.
//---------------------------------------------------------------------------------------
#pragma once
#include <cstddef>
#include <concepts>

// A type safe ID system
// binds to the type that it is an ID for, and the underlying type to use for it's ID
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
