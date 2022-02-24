#pragma once

#include <unordered_map>
#include "ID.h"

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