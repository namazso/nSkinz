#include "NetVarManager.hpp"
#include <cctype>
#include "../SDK.hpp"

NetVarManager::NetVarManager()
{
	for (auto pClass = g_pClient->GetAllClasses(); pClass; pClass = pClass->m_pNext)
		if (pClass->m_pRecvTable)
			DumpRecursive(pClass->m_pNetworkName, pClass->m_pRecvTable, 0);
}

void NetVarManager::DumpRecursive(const char* szBaseClass, RecvTable* pTable, uint16_t wOffset)
{
	for (auto i = 0; i < pTable->m_nProps; ++i)
	{
		auto pProperty = &pTable->m_pProps[i];

		//Skip trash array items
		if (!pProperty || isdigit(pProperty->m_pVarName[0]))
			continue;

		//We dont care about the base class, we already know that
		if (strcmp(pProperty->m_pVarName, "baseclass") == 0)
			continue;

		if (pProperty->m_RecvType == DPT_DataTable &&
			pProperty->m_pDataTable != nullptr &&
			pProperty->m_pDataTable->m_pNetTableName[0] == 'D') // Skip shitty tables
		{
			DumpRecursive(szBaseClass, pProperty->m_pDataTable, wOffset + pProperty->m_Offset);
		}

		auto strHash = std::string(szBaseClass) + "->" + pProperty->m_pVarName;
		auto dwHash = FnvHash(strHash.c_str());

		m_mapProps[dwHash] = pProperty;
		m_mapOffsets[dwHash] = wOffset + pProperty->m_Offset;
	}
}