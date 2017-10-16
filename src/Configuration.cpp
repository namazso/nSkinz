#include "Configuration.hpp"
#include "SDK.hpp"

#include <fstream>
#include <json.hpp>

Config g_config;

using json = nlohmann::json;

void to_json(json& j, const StickerSetting& sticker)
{
	j = json
	{
		{ "kit", sticker.kit_index },
		{ "wear", sticker.wear },
		{ "scale", sticker.scale },
		{ "rotation", sticker.rotation }
	};
}

void from_json(const json& j, StickerSetting& sticker)
{
	sticker.kit_index = j.at("kit").get<int>();
	sticker.wear = j.at("wear").get<float>();
	sticker.scale = j.at("scale").get<float>();
	sticker.rotation = j.at("rotation").get<float>();
	sticker.UpdateIds();
}

void to_json(json& j, const EconomyItem_t& item)
{
	j = json
	{
		{ "name", item.name },
		{ "enabled", item.enabled },
		{ "definition_index", item.definition_index },
		{ "entity_quality_index", item.entity_quality_index },
		{ "paint_kit_index", item.paint_kit_index },
		{ "definition_override_index", item.definition_override_index },
		{ "seed", item.seed },
		{ "stat_trak", item.stat_trak },
		{ "wear", item.wear },
		{ "custom_name", item.custom_name },
		{ "stickers", item.stickers },
	};
}

void from_json(const json& j, EconomyItem_t& item)
{
	strcpy_s(item.name, j.at("name").get<std::string>().c_str());
	item.enabled = j.at("enabled").get<bool>();
	item.definition_index = j.at("definition_index").get<int>();
	item.entity_quality_index = j.at("entity_quality_index").get<int>();
	item.paint_kit_index = j.at("paint_kit_index").get<int>();
	item.definition_override_index = j.at("definition_override_index").get<int>();
	item.seed = j.at("seed").get<int>();
	item.stat_trak = j.at("stat_trak").get<int>();
	item.wear = j.at("wear").get<float>();
	strcpy_s(item.custom_name, j.at("custom_name").get<std::string>().c_str());
	auto stickers = j.at("stickers").get<std::vector<StickerSetting>>();
	// Fuck you std::copy
	//std::copy(stickers.begin(), stickers.end(), item.stickers);

	auto size = stickers.size() < item.stickers.size() ? stickers.size() : item.stickers.size();

	for(size_t i = 0; i < size; ++i)
		item.stickers[i] = stickers[i];

	item.UpdateIds();
}

void Config::Save()
{
	std::ofstream("nSkinz.json") << json(m_items);
}

void Config::Load()
{
	try
	{
		m_items = json::parse(std::ifstream("nSkinz.json")).get<std::vector<EconomyItem_t>>();
		(*g_client_state)->ForceFullUpdate();
	}
	catch(const std::exception&)
	{
		// Config file doesn't exists or is malformed, just ignore it
	}
}

EconomyItem_t* Config::GetByDefinitionIndex(int definition_index)
{
	for(auto& x : m_items)
		if(x.enabled && x.definition_index == definition_index)
			return &x;

	return nullptr;
}
