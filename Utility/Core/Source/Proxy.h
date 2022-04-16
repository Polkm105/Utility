//---------------------------------------------------------------------------------------
// Author:      Kenny Mecham
// Date:        02/24/2022
// Project:     Utility
// Description: Holds multiple bonds between two types of objects, allowing for type-safe
//              conversions between objects using a type-safe ID system
//---------------------------------------------------------------------------------------
#pragma once
#include <unordered_map>
import <cassert>;

// A proxy type that allows for a type safe equivilancy from one object to another
template <typename To, typename From>
class Proxy
{
public:
	To Add(From id)
	{
		To proxyID(++id_counter_);
		proxy_to_internal_.emplace(proxyID, id);
		return proxyID;
	}

	bool Has(To id) const
	{
		return proxy_to_internal_.find(id) != proxy_to_internal_.end();
	}

	From Get(To id) const
	{
		return proxy_to_internal_.find(id)->second;
	}

	void Remove(To id)
	{
		assert(proxy_to_internal_.find(id) != proxy_to_internal_.end());
		proxy_to_internal_.erase(id);
	}

private:
	std::unordered_map<To, From> proxy_to_internal_;
	To::InternalType id_counter_{ 0 };
};