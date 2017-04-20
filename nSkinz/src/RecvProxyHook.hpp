#pragma once
#include "SDK.hpp"

class RecvPropHook
{
public:
	RecvPropHook(RecvProp* pProperty, RecvVarProxyFn pProxyFn) :
		m_pProperty(pProperty),
		m_pOriginalProxyFn(pProperty->m_ProxyFn)
	{
		SetProxyFunction(pProxyFn);
	}

	~RecvPropHook()
	{
		m_pProperty->m_ProxyFn = m_pOriginalProxyFn;
	}

	RecvVarProxyFn GetOriginalFunction() const
	{
		return m_pOriginalProxyFn;
	}

	void SetProxyFunction(RecvVarProxyFn pProxyFn) const
	{
		m_pProperty->m_ProxyFn = pProxyFn;
	}

private:
	RecvProp* m_pProperty;
	RecvVarProxyFn m_pOriginalProxyFn;
};
