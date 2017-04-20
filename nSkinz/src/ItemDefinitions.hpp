#pragma once
#include <map>
#include <vector>

inline bool IsKnife(int i) { return i >= 500 || i == 59 || i == 42; }

typedef struct Item_s
{
	constexpr Item_s(const char* szDisplayName, const char* szEntityName, const char* szModel, const char* szIcon = nullptr) :
		szDisplayName(szDisplayName),
		szEntityName(szEntityName),
		szModel(szModel),
		szIcon(szIcon)
	{}

	const char* szDisplayName = nullptr;
	const char* szEntityName = nullptr;
	const char* szModel = nullptr;
	const char* szIcon = nullptr;
} Item_t;

typedef struct WeaponName_s
{
	constexpr WeaponName_s(int iDefinitionIndex, const char* szName) :
		iDefinitionIndex(iDefinitionIndex),
		szName(szName)
	{}

	int iDefinitionIndex = 0;
	const char* szName = nullptr;
} WeaponName_t;

typedef struct QualityName_s
{
	constexpr QualityName_s(int iIndex, const char* szName) :
		iIndex(iIndex),
		szName(szName)
	{}

	int iIndex = 0;
	const char* szName = nullptr;
} QualityName_t;

/*typedef struct Weapon_s
{
	constexpr Weapon_s(int iOrderId, int iDefIndex, int iOverrideFor, bool bDisplayInMenu, const char* szDisplayName, const char* szEntityName, const char* szModel, const char* szIcon = nullptr) :
		iOrderId(iOrderId),
		iDefIndex(iDefIndex),
		iOverrideFor(iOverrideFor),
		bDisplayInMenu(bDisplayInMenu),
		szDisplayName(szDisplayName),
		szEntityName(szEntityName),
		szModel(szModel),
		szIcon(szIcon)
	{}

	int iOrderId = 0;
	int iDefIndex = 0;
	int iOverrideFor = 0;
	bool bDisplayInMenu = false;
	const char* szDisplayName = nullptr;
	const char* szEntityName = nullptr;
	const char* szModel = nullptr;
	const char* szIcon = nullptr;
}
Weapon_t;*/

extern const std::map<size_t, Item_t> k_WeaponInfo;
extern const std::vector<WeaponName_s> k_KniveNames;
extern const std::vector<WeaponName_t> k_WeaponNames;
extern const std::vector<QualityName_t> k_QualityNames;