#pragma once
#include "ItemDefinitions.hpp"
#include "PaintKitParser.hpp"

#include <unordered_map>
#include <algorithm>

struct EconomyItem_t
{
	void UpdateValues()
	{
		iDefinitionIndex = k_WeaponNames[iDefinitionId].iDefinitionIndex;
		iEntityQualityIndex = k_QualityNames[iEntityQualityId].iIndex;
		iPaintKitIndex = k_Skins[iPaintKitId].id;
		iDefinitionOverrideIndex = k_KniveNames[iDefinitionOverrideId].iDefinitionIndex;
	}

	void UpdateIds()
	{
		iDefinitionId = find_if(k_WeaponNames.begin(), k_WeaponNames.end(), [this](const WeaponName_t& x)
		{
			return this->iDefinitionIndex == x.iDefinitionIndex;
		}) - k_WeaponNames.begin();

		iEntityQualityId = find_if(k_QualityNames.begin(), k_QualityNames.end(), [this](const QualityName_t& x)
		{
			return this->iEntityQualityIndex == x.iIndex;
		}) - k_QualityNames.begin();

		iPaintKitId = find_if(k_Skins.begin(), k_Skins.end(), [this](const PaintKit_t& x)
		{
			return this->iPaintKitIndex == x.id;
		}) - k_Skins.begin();

		iDefinitionOverrideId = find_if(k_KniveNames.begin(), k_KniveNames.end(), [this](const WeaponName_t& x)
		{
			return this->iDefinitionOverrideIndex == x.iDefinitionIndex;
		}) - k_KniveNames.begin();
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
