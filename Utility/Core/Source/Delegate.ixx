//---------------------------------------------------------------------------------------
// Author:      Kenny Mecham
// Date:        02/24/2022
// Project:     Utility
// Description: A basic delegate system similar to C# that allows for the binding of 
//              different functions with a specified signature, used mainly for event 
//              systems
//---------------------------------------------------------------------------------------
export module Delegate;
import ManagedVector;
import TypeSafeID;
import <functional>;
import <utility>;
import <algorithm>;



// A generic delegate system that allows
// All functions are O(1) time on average unless otherwise specified
export template <typename ... FunctionInputs>
class Delegate
{
public:
	using FunctionType = std::function<void(FunctionInputs...)>;
	using IDType = TypeSafeID<Delegate<FunctionInputs...>>;

	

	Delegate() = default;
	Delegate(const Delegate& rhs) noexcept:
		functions_()
	{}

	Delegate& operator=(const Delegate& rhs) noexcept
	{
		return *this;
	}

	Delegate(Delegate&& rhs) noexcept:
		functions_(std::move(rhs.functions_))
	{}

	Delegate& operator=(Delegate&& rhs) noexcept
	{
		functions_ = std::move(rhs.functions_);
	}

	~Delegate() = default;

	// adds the given function to be called when this delegate is invoked
	IDType Register(const FunctionType& func)
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
	void UnRegister(const IDType& id)
	{
		functions_.Remove(id);
	}

	void Unregister(const FunctionType& func)
	{
		const auto& iter = std::ranges::find(functions_, func);
		functions_.erase(iter);
	}

	// invokes all functions that are registered with this delegate, O(n) time
	void Invoke(FunctionInputs&&... args) const
	{
		for (const auto& func : functions_)
		{
			func(std::forward<FunctionInputs>(args)...);
		}
	}

	// adds a function to the delegate
	Delegate& operator+=(const FunctionType& func)
	{
		Register(func);
	}

	// Unregisters a specified function, O(n) time
	Delegate& operator-=(const FunctionType& func)
	{
		UnRegister(func);
	}

	// Unregisters the function with the specified ID
	Delegate& operator-=(const IDType& id)
	{
		UnRegister(id);
	}

private:
	ManagedVector<FunctionType, IDType> functions_{};
};
