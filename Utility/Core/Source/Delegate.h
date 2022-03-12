//---------------------------------------------------------------------------------------
// Author:      Kenny Mecham
// Date:        02/24/2022
// Project:     Utility
// Description: A basic delegate system similar to C# that allows for the binding of 
//              different functions with a specified signature, used mainly for event 
//              systems
//---------------------------------------------------------------------------------------

#pragma once
#include <functional>
#include <utility>
#include "ManagedVector.h"

// A generic delegate system that allows
// All functions are O(1) time on average unless otherwise specified
template <typename ... FunctionInputs>
class Delegate
{
public:
	using FunctionType = std::function<void(FunctionInputs...)>;
	using IDType = ID<Delegate<FunctionInputs...>>;

	// adds the given function to be called when this delegate is invoked
	IDType Register(FunctionType func)
	{
		auto id = functions_.Add(std::move(func));
		return id;
	}

	// checks if the delegate has a function with the specified ID
	bool Has(const IDType& id) const
	{
		return functions_.Has(id);
	}

	// removes the function with the specified ID from being called when the delegate is invoked
	void UnRegister(IDType id)
	{
		functions_.Remove(id);
	}

	// invokes all functions that are registered with this delegate, O(n) time
	void Invoke(FunctionInputs&&... args) const
	{
		size_t size = functions_.Size();
		for (size_t i = 0; i < size; ++i)
		{
			functions_[i](std::forward<FunctionInputs>(args)...);
		}
	}

	// adds a function to the delegate
	Delegate& operator+=(FunctionType func)
	{
		Register(func);
	}

	// Unregisters a specified function, O(n) time
	Delegate& operator-=(FunctionType func)
	{
		UnRegister(func);
	}

	// Unregisters the function with the specified ID
	Delegate& operator-=(const IDType& id)
	{
		UnRegister(id);
	}

private:
	ManagedVector<FunctionType, IDType> functions_;
};
