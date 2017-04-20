#pragma once
#include <string>
#include <vector>

struct SkinInfo_t
{
	int iIndex;
	std::string strShortName;
	std::string strName;

	bool operator < (const SkinInfo_t& other) const { return (strName < other.strName); }
};

using vecSkinInfo = std::vector<SkinInfo_t>;

extern vecSkinInfo k_Skins;
extern vecSkinInfo k_Gloves;

extern void GetSkins(vecSkinInfo& vecSkins, vecSkinInfo& vecGloves, std::string strModName);