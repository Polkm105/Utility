#pragma once
#include <string>
#include <unordered_map>
#include <functional>
#include <utility>
#include <cstddef>
#include <concepts>
#include <type_traits>
#include <assert.h>

template <typename ContainerType, typename KeyType, typename ValueType>
concept Container = requires(ContainerType container, KeyType key, ValueType value)
{
	{ container.emplace(key, value) };
	{ container.erase(key) };
	{ container.find(key) };
	{ container.end() };
};

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

	template <typename KeyType>
	inline void Register(const KeyType& key, const FunctionType& func)
	{
		container_.emplace(key, func);
	}

	template <typename KeyType>
	inline void UnRegister(const KeyType& key)
	{
		container_.erase(key);
	}

	template <typename KeyType, typename ...Args>
	ObjectType Create(const KeyType& key, Args&&... args) const
	{
		auto iter = container_.find(key);
		assert(iter != container_.end());
		return iter->second(std::forward<Args>(args)...);
	}

private:
	ContainerType container_;
};
