//---------------------------------------------------------------------------------------
// Author:      Kenny Mecham
// Date:        02/24/2022
// Project:     Utility
// Description: A basic generic Factory template that allows for specification of the 
//              generic types that can be created in future projects
//---------------------------------------------------------------------------------------

#pragma once
#include <string>
#include <unordered_map>
#include <functional>
#include <utility>
#include <cstddef>
#include <concepts>
#include <type_traits>
#include <assert.h>

// C++20 concept for easier to read compile errors and explicit needs for the Container type required by Factory
template <typename ContainerType, typename KeyType, typename ValueType>
concept Container = requires(ContainerType container, KeyType key, ValueType value)
{
	{ container.emplace(key, value) };
	{ container.erase(key) };
	{ container.find(key) };
	{ container.end() };
};

// A generic factory that allows for the registration and creation of types
// any type can be created as long as it is convertible to the specified object type
template <
	typename ObjectType,
	typename FunctionType = std::function<ObjectType()>,
	typename KeyType = std::string,
	typename ContainerType = std::unordered_map<KeyType, FunctionType>>
	requires Container<ContainerType, KeyType, FunctionType>
class Factory
{
public:
	inline Factory() :
		container_()
	{}

	Factory(const Factory&) = delete;
	Factory& operator=(const Factory&) = delete;

	Factory(Factory&& other) :
		container_(std::move(other.container_)) 
	{}

	Factory& operator=(Factory&& other)
	{
		container_ = std::move(other.container_);
	}

	// Relates the specified key and function, allowing for creation using Create(key)
	template <typename KeyType>
	inline void Register(const KeyType& key, const FunctionType& func)
	{
		container_.emplace(key, func);
	}

	// Removes the ability to create the type related to the specified key
	template <typename KeyType>
	inline void UnRegister(const KeyType& key)
	{
		container_.erase(key);
	}

	// Calls the registered Function related to the specified key, passes the given args to that function
	template <typename KeyType, typename ...Args>
	inline ObjectType Create(const KeyType& key, Args&&... args) const
	{
		auto iter = container_.find(key);
		assert(iter != container_.end());
		return iter->second(std::forward<Args>(args)...);
	}

private:
	ContainerType container_{};	// the container that holds all of the key + func relations
};
