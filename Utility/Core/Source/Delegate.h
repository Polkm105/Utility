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
	using FunctionIDType = ID<FunctionType>;

	IDType Register(FunctionType func)
	{
		auto id = functions_.Add(std::move(func));
		return id_convertor_.Add(id);
	}

	bool Has(const IDType& id) const
	{
		return functions_.Has(id_convertor_.Get(id));
	}

	void UnRegister(IDType id)
	{
		functions_.Remove(id_convertor_.Get(id));
		id_convertor_.Remove(id);
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
	using VectorType = ManagedVector<FunctionType, FunctionIDType>;

	VectorType functions_;
	Proxy<IDType, FunctionIDType> id_convertor_;
};
