#pragma once
#include "SDK.hpp"

class RecvPropHook
{
public:
	RecvPropHook(RecvProp* prop, RecvVarProxyFn proxy_fn) :
		m_property(prop),
		m_original_proxy_fn(prop->m_ProxyFn)
	{
		SetProxyFunction(proxy_fn);
	}

	~RecvPropHook()
	{
		m_property->m_ProxyFn = m_original_proxy_fn;
	}

	RecvVarProxyFn GetOriginalFunction() const
	{
		return m_original_proxy_fn;
	}

	void SetProxyFunction(RecvVarProxyFn proxy_fn) const
	{
		m_property->m_ProxyFn = proxy_fn;
	}

private:
	RecvProp* m_property;
	RecvVarProxyFn m_original_proxy_fn;
};
