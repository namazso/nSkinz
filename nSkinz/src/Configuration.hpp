#pragma once
#include "ItemDefinitions.hpp"
#include "KitParser.hpp"

#include <unordered_map>
#include <array>
#include <algorithm>

struct StickerSetting
{
	void UpdateValues()
	{
		kit_vector_index = kit_vector_index < k_stickers.size() ? kit_vector_index : k_stickers.size() - 1;
		kit_index = k_stickers.at(kit_vector_index).id;
	}

	void UpdateIds()
	{
		kit_vector_index = find_if(k_stickers.begin(), k_stickers.end(), [this](const Kit_t& x)
		{
			return this->kit_index == x.id;
		}) - k_stickers.begin();
	}

	int kit_index = 0;
	int kit_vector_index = 0;
	float wear = FLT_MIN;
	float scale = 1.f;
	float rotation = 0.f;
};

struct EconomyItem_t
{
	void UpdateValues()
	{
		definition_vector_index = definition_vector_index < k_weapon_names.size() ? definition_vector_index : k_weapon_names.size() - 1;
		definition_index = k_weapon_names.at(definition_vector_index).definition_index;

		entity_quality_vector_index = entity_quality_vector_index < k_quality_names.size() ? entity_quality_vector_index : k_quality_names.size() - 1;
		entity_quality_index = k_quality_names.at(entity_quality_vector_index).index;

		if(definition_index == GLOVE_T_SIDE)
		{
			paint_kit_vector_index = paint_kit_vector_index < k_gloves.size() ? paint_kit_vector_index : k_gloves.size() - 1;
			paint_kit_index = k_gloves.at(paint_kit_vector_index).id;

			definition_override_vector_index = definition_override_vector_index < k_glove_names.size() ? definition_override_vector_index : k_glove_names.size() - 1;
			definition_override_index = k_glove_names.at(definition_override_vector_index).definition_index;
		}
		else
		{
			paint_kit_vector_index = paint_kit_vector_index < k_skins.size() ? paint_kit_vector_index : k_skins.size() - 1;
			paint_kit_index = k_skins.at(paint_kit_vector_index).id;

			definition_override_vector_index = definition_override_vector_index < k_knife_names.size() ? definition_override_vector_index : k_knife_names.size() - 1;
			definition_override_index = k_knife_names.at(definition_override_vector_index).definition_index;
		}

		for(auto& sticker : stickers)
			sticker.UpdateValues();
	}

	void UpdateIds()
	{
		definition_vector_index = find_if(k_weapon_names.begin(), k_weapon_names.end(), [this](const WeaponName_t& x)
		{
			return this->definition_index == x.definition_index;
		}) - k_weapon_names.begin();

		entity_quality_vector_index = find_if(k_quality_names.begin(), k_quality_names.end(), [this](const QualityName_t& x)
		{
			return this->entity_quality_index == x.index;
		}) - k_quality_names.begin();

		const auto& skin_set = definition_index == GLOVE_T_SIDE ? k_gloves : k_skins;

		paint_kit_vector_index = find_if(skin_set.begin(), skin_set.end(), [this](const Kit_t& x)
		{
			return this->paint_kit_index == x.id;
		}) - skin_set.begin();

		const auto& override_set = definition_index == GLOVE_T_SIDE ? k_glove_names : k_knife_names;

		definition_override_vector_index = find_if(override_set.begin(), override_set.end(), [this](const WeaponName_t& x)
		{
			return this->definition_override_index == x.definition_index;
		}) - override_set.begin();

		for(auto& sticker : stickers)
			sticker.UpdateIds();
	}

	char name[32] = "Default";
	bool enabled = false;
	int definition_vector_index = 0;
	int definition_index = 1;
	int entity_quality_vector_index = 0;
	int entity_quality_index = 0;
	int paint_kit_vector_index = 0;
	int paint_kit_index = 0;
	int definition_override_vector_index = 0;
	int definition_override_index = 0;
	int seed = 0;
	int stat_trak = 0;
	float wear = FLT_MIN;
	char custom_name[32] = "";
	std::array<StickerSetting, 5> stickers;
};

class Config
{
public:
	void Save();
	void Load();

	EconomyItem_t* GetByDefinitionIndex(int definition_index);

	std::vector<EconomyItem_t>& GetItems() { return m_items; }

	std::unordered_map<std::string, std::string>& GetIconOverrideMap() { return m_icon_overrides; }
	const char* GetIconOverride(const std::string& original) { return m_icon_overrides.count(original) ? m_icon_overrides.at(original).c_str() : nullptr; }
private:
	std::vector<EconomyItem_t> m_items;
	std::unordered_map<std::string, std::string> m_icon_overrides;
};

extern Config g_config;