#pragma once
#include "../SDK/DataTable.hpp"
#include "FnvHash.hpp"
#include <map>

class NetVarManager
{
public:
	static NetVarManager& Get()
	{
		static NetVarManager Instance;
		return Instance;
	}

	unsigned short GetOffset(unsigned int wHash) { return m_mapOffsets[wHash]; }
	RecvProp* GetPropPtr(unsigned int wHash) { return m_mapProps[wHash]; }

private:
	NetVarManager();
	void DumpRecursive(const char* szBaseClass, RecvTable* pTable, uint16_t wOffset);

private:
	std::map<fnv_t, RecvProp*> m_mapProps;
	std::map<fnv_t, uint16_t> m_mapOffsets;
};

#define PNETVAR(funcname, type, netvarname) type* funcname() \
{ \
	constexpr fnv_t hash = FnvHash(netvarname); \
	static auto offset = NetVarManager::Get().GetOffset(hash); \
	return reinterpret_cast<type*>(uintptr_t(this) + offset); \
}

#define NETVAR(funcname, type, netvarname) type& funcname() \
{ \
	constexpr fnv_t hash = FnvHash(netvarname); \
	static auto offset = NetVarManager::Get().GetOffset(hash); \
	return *reinterpret_cast<type*>(uintptr_t(this) + offset); \
}

#define NETPROP(funcname, netvarname) static RecvProp* funcname() \
{ \
	constexpr fnv_t hash = FnvHash(netvarname); \
	static auto pProp = NetVarManager::Get().GetPropPtr(hash); \
	return pProp; \
}