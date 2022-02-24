#pragma once
#include <functional>
#include <utility>
#include "ManagedVector.h"
#include "Proxy.h"


template <typename ... FunctionInputs>
class Delegate
{
public:
	using FunctionType = std::function<void(FunctionInputs...)>;
	using IDType = ID<Delegate<FunctionInputs...>>;

	IDType Register(FunctionType func)
	{
		auto id = functions_.Add(std::move(func));
		return id;
	}

	bool Has(const IDType& id) const
	{
		return functions_.Has(id);
	}

	void UnRegister(IDType id)
	{
		functions_.Remove(id);
	}

	void Invoke(FunctionInputs... args) const
	{
		size_t size = functions_.Size();
		for (size_t i = 0; i < size; ++i)
		{
			functions_[i](std::forward<FunctionInputs>(args)...);
		}
	}

private:
	ManagedVector<FunctionType, IDType> functions_;
};
