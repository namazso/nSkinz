#pragma once
#include "ItemDefinitions.hpp"
#include "PaintKitParser.hpp"

#include <unordered_map>
#include <algorithm>

struct EconomyItem_t
{
	void UpdateValues()
	{
		iDefinitionIndex = k_weapon_names[iDefinitionId].definition_index;
		iEntityQualityIndex = k_quality_names[iEntityQualityId].index;
		iPaintKitIndex = k_skins[iPaintKitId].id;
		iDefinitionOverrideIndex = k_knife_names[iDefinitionOverrideId].definition_index;
	}

	void UpdateIds()
	{
		iDefinitionId = find_if(k_weapon_names.begin(), k_weapon_names.end(), [this](const WeaponName_t& x)
		{
			return this->iDefinitionIndex == x.definition_index;
		}) - k_weapon_names.begin();

		iEntityQualityId = find_if(k_quality_names.begin(), k_quality_names.end(), [this](const QualityName_t& x)
		{
			return this->iEntityQualityIndex == x.index;
		}) - k_quality_names.begin();

		iPaintKitId = find_if(k_skins.begin(), k_skins.end(), [this](const PaintKit_t& x)
		{
			return this->iPaintKitIndex == x.id;
		}) - k_skins.begin();

		iDefinitionOverrideId = find_if(k_knife_names.begin(), k_knife_names.end(), [this](const WeaponName_t& x)
		{
			return this->iDefinitionOverrideIndex == x.definition_index;
		}) - k_knife_names.begin();
	}

	char szName[32] = "Default";
	bool bEnabled = false;
	int iDefinitionId = 0;
	int iDefinitionIndex = 1;
	int iEntityQualityId = 0;
	int iEntityQualityIndex = 0;
	int iPaintKitId = 0;
	int iPaintKitIndex = 0;
	int iDefinitionOverrideId = 0;
	int iDefinitionOverrideIndex = 0;
	int iSeed = 0;
	int iStatTrak = 0;
	float flWear = FLT_MIN;
	char szCustomName[32] = "";

	// In case of adding more bullshit
	uint64_t _reserved1 = 0xFFFFFFFFFFFFFFFF;
	uint64_t _reserved2 = 0xFFFFFFFFFFFFFFFF;
	uint64_t _reserved3 = 0xFFFFFFFFFFFFFFFF;
	uint64_t _reserved4 = 0xFFFFFFFFFFFFFFFF;
};

class Config
{
public:
	static Config* Get()
	{
		static Config Instance;

		return &Instance;
	}

	void Save();
	void Load();

	EconomyItem_t* GetByDefinitionIndex(int iDefinitionIndex);

	std::vector<EconomyItem_t>& GetItems() { return m_vItems; }

	void AddIconOverride(std::string sOriginal, std::string sOverride) { m_mapIconOverrides[sOriginal] = sOverride; }
	void EmptyIconOverrides() { m_mapIconOverrides.clear(); }
	const char* GetIconOverride(std::string sOriginal) { return m_mapIconOverrides.count(sOriginal) ? m_mapIconOverrides[sOriginal].c_str() : nullptr; }
private:
	std::vector<EconomyItem_t> m_vItems;
	std::unordered_map<std::string, std::string> m_mapIconOverrides;
};
