#include "Skins.hpp"
#include "SDK.hpp"
#include <vdf_parser.hpp>
#include <codecvt>

using namespace tyti;

vecSkinInfo k_Skins;
vecSkinInfo k_Gloves;

void GetSkins(vecSkinInfo& vecSkins, vecSkinInfo& vecGloves, std::string strModName)
{
	std::ifstream items_game_file(strModName + "/scripts/items/items_game.txt");
	auto items_game = vdf::read(items_game_file);
	items_game_file.close();

	static std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;

	for(auto& kit : items_game.childs["paint_kits"].childs)
	{
		auto id = std::stoi(kit.second.name);
		
		if (id == 9001)
			continue;

		auto desc = kit.second.attribs["description_tag"].substr(1);

		const wchar_t* wname = g_pLocalize->Find(desc.c_str());
		auto name = converter.to_bytes(wname);

		if (id < 10000)
			vecSkins.push_back({ id, desc, name });
		else
			vecGloves.push_back({ id, desc, name });
	}

	std::sort(vecSkins.begin(), vecSkins.end());
	std::sort(vecGloves.begin(), vecGloves.end());
}