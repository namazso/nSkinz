#include "Configuration.hpp"
#include "SDK.hpp"

#include <fstream>

// Toptier configuration system

void Config::Save()
{
	FILE* fp;
	if(fopen_s(&fp, "nSkinz_1.bin", "wb"))
		return;
	auto size = m_items.size();
	fwrite(&size, sizeof size, 1, fp);
	fwrite(m_items.data(), sizeof(EconomyItem_t), m_items.size(), fp);
	fclose(fp);
}

void Config::Load()
{
	FILE* fp;
	if(fopen_s(&fp, "nSkinz_1.bin", "rb"))
		return;
	size_t size;
	fread(&size, sizeof size, 1, fp);
	auto items = new EconomyItem_t[size];
	fread(items, sizeof(EconomyItem_t), size, fp);
	m_items.clear();
	m_items.insert(m_items.begin(), items, items + size);
	for(auto& x : m_items)
		x.UpdateIds();
	delete[] items;
	fclose(fp);

	(*g_client_state)->ForceFullUpdate();
}

EconomyItem_t* Config::GetByDefinitionIndex(int definition_index)
{
	for(auto& x : m_items)
		if(x.enabled && x.definition_index == definition_index)
			return &x;

	return nullptr;
}
