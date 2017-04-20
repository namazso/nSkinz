#include "Configuration.hpp"
#include "ItemDefinitions.hpp"
#include "SDK.hpp"

#include <fstream>

void Config::Save()
{
	FILE* fp;
	if(fopen_s(&fp, "nSkinz_1.bin", "wb"))
		return;
	auto size = m_vItems.size();
	fwrite(&size, sizeof(size), 1, fp);
	fwrite(m_vItems.data(), sizeof(EconomyItem_t), m_vItems.size(), fp);
	fclose(fp);
}

void Config::Load()
{
	FILE* fp;
	if(fopen_s(&fp, "nSkinz_1.bin", "rb"))
		return;
	size_t size;
	fread(&size, sizeof(size), 1, fp);
	auto pItems = new EconomyItem_t[size];
	fread(pItems, sizeof(EconomyItem_t), size, fp);
	m_vItems.clear();
	m_vItems.insert(m_vItems.begin(), pItems, pItems + size);
	for(auto& x : m_vItems)
		x.UpdateIds();
	delete[] pItems;
	fclose(fp);

	(*g_ppClientState)->ForceFullUpdate();
}

EconomyItem_t* Config::GetByDefinitionIndex(int iDefinitionIndex)
{
	for(auto& x : m_vItems)
		if(x.bEnabled && x.iDefinitionIndex == iDefinitionIndex)
			return &x;

	return nullptr;
}
