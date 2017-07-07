#pragma once
#include "../SDK/DataTable.hpp"
#include "FnvHash.hpp"
#include <map>

class NetVarManager
{
private:
	struct StoredPropData
	{
		RecvProp* prop_ptr;
		uint16_t class_relative_offset;
	};

public:
	static NetVarManager& Get()
	{
		static NetVarManager instance;
		return instance;
	}

	unsigned short GetOffset(unsigned int hash) { return m_props[hash].class_relative_offset; }
	RecvProp* GetPropPtr(unsigned int hash) { return m_props[hash].prop_ptr; }

	// Prevent instruction cache pollution caused by automatic
	// inlining of Get and GetOffset every netvar usage when there
	// are a lot of netvars
	__declspec(noinline) static uint16_t GetOffsetByHash(fnv_t hash)
	{
		return Get().GetOffset(hash);
	}

private:
	NetVarManager();
	void DumpRecursive(const char* base_class, RecvTable* table, uint16_t offset);

private:
	std::map<fnv_t, StoredPropData> m_props;
};

#define PNETVAR(funcname, type, netvarname) type* funcname() \
{ \
	constexpr fnv_t hash = FnvHash(netvarname); \
	static uint16_t offset = 0; \
	if(!offset) offset = NetVarManager::GetOffsetByHash(hash); \
	return reinterpret_cast<type*>(uintptr_t(this) + offset); \
}

#define NETVAR(funcname, type, netvarname) type& funcname() \
{ \
	constexpr fnv_t hash = FnvHash(netvarname); \
	static uint16_t offset = 0; \
	if(!offset) offset = NetVarManager::GetOffsetByHash(hash); \
	return *reinterpret_cast<type*>(uintptr_t(this) + offset); \
}

#define NETPROP(funcname, netvarname) static RecvProp* funcname() \
{ \
	constexpr fnv_t hash = FnvHash(netvarname); \
	static auto prop_ptr = NetVarManager::Get().GetPropPtr(hash); \
	return prop_ptr; \
}