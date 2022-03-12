//---------------------------------------------------------------------------------------
// Author:      Kenny Mecham
// Date:        03/11/2022
// Project:     Utility
// Description: C++20 concepts that are used in multiple files throughout this library
//---------------------------------------------------------------------------------------

#pragma once
#include <concepts>

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
