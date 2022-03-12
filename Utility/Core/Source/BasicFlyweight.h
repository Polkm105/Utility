//---------------------------------------------------------------------------------------
// Author:      Kenny Mecham
// Date:        03/11/2022
// Project:     Utility
// Description: A basic flyweight pattern implementation that weakens dependencies on
//              the held types through type erasure. Can be used as a baseline for future
//              type erased containers in other projects
//---------------------------------------------------------------------------------------
#pragma once
#include <concepts>
#include <vector>
#include <memory>
#include <algorithm>
#include <functional>

template <typename Type>
concept Updateable = requires(Type a, float dt)
{
	{ a.Update(dt) } -> std::convertible_to<void>;
};

template <typename Type>
concept Initable = requires(Type a)
{
	{ a.Init() } -> std::convertible_to<void>;
};

// Initial funciton template definitions to prevent compiler confusions within the flyweight class
template <Updateable T>
void Update(T& type, float dt)
{
	type.Update(dt);
}

template <Initable T>
void Init(T& type)
{
	type.Init();
}

// The Flyweight class itself, allows for any type to be added as long as the type has the required free-function overloads
// Required overloads: void Init(), void Update(float dt)
class BasicFlyweight
{
public:
	struct FlyweightTypeConcept;

	template<typename T>
	class FlyweightType;

	// adds a specified type constructed with the given arguements. Types are NOT stored contiguously in memory
	template<typename T, typename ... Args>
	void Add(Args&& ...args)
	{
		std::unique_ptr<FlyweightType<T>> ptr(new FlyweightType<T>(std::forward<Args>(args)...));
		types_.emplace_back(std::move(ptr));
	}

	// remove a previously added type
	void Remove(size_t index)
	{
		types_.erase(types_.begin() + index);
	}

	// initialize all added types
	void Init()
	{
		std::ranges::for_each(types_, [](auto& typePtr) { typePtr->Init(); });
	}

	// update all added types
	void Update(float dt)
	{
		std::ranges::for_each(types_, [dt](auto& typePtr) { typePtr->Update(dt); });
	}

private:

	// indicates the required functions for any type given to the flyweight system
	struct FlyweightTypeConcept
	{
		virtual void Init() = 0;
		virtual void Update(float dt) = 0;
		virtual ~FlyweightTypeConcept() {};
	};

	// Type erasure to reduce dependencies on any added type 
	// Also reduces cost compared to multiple virtual functions in an inheritance heirarchy
	template<typename T>
	class FlyweightType : public FlyweightTypeConcept
	{
	public:
		template<typename ...Args>
		FlyweightType(Args&&... args) :
			base_(std::forward<Args>(args)...)
		{}

		void Init() override
		{
			::Init(base_);
		}

		void Update(float dt) override
		{
			::Update(base_, dt);
		}

	private:
		T base_;
	};

	// a managed vector for speed optimized adding, removing, traversing, and accessing
	std::vector<std::unique_ptr<FlyweightTypeConcept>> types_;
};
