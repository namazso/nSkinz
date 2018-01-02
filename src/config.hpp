/* This file is part of nSkinz by namazso, licensed under the MIT license:
*
* MIT License
*
* Copyright (c) namazso 2018
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in all
* copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
*/
#pragma once
#include "item_definitions.hpp"
#include "kit_parser.hpp"

#include <unordered_map>
#include <array>
#include <algorithm>

struct sticker_setting
{
	void update_values()
	{
		kit_vector_index = kit_vector_index < int(k_stickers.size()) ? kit_vector_index : k_stickers.size() - 1;
		kit_index = k_stickers.at(kit_vector_index).id;
	}

	void update_ids()
	{
		kit_vector_index = std::find_if(begin(k_stickers), end(k_stickers), [this](const paint_kit& x)
		{
			return this->kit_index == x.id;
		}) - k_stickers.begin();
	}

	int kit_index = 0;
	int kit_vector_index = 0;
	float wear = std::numeric_limits<float>::min();
	float scale = 1.f;
	float rotation = 0.f;
};

struct item_setting
{
	void update_values()
	{
		definition_vector_index = definition_vector_index < int(k_weapon_names.size()) ? definition_vector_index : k_weapon_names.size() - 1;
		definition_index = k_weapon_names.at(definition_vector_index).definition_index;

		entity_quality_vector_index = entity_quality_vector_index < int(k_quality_names.size()) ? entity_quality_vector_index : k_quality_names.size() - 1;
		entity_quality_index = k_quality_names.at(entity_quality_vector_index).index;

		if(definition_index == GLOVE_T_SIDE)
		{
			paint_kit_vector_index = paint_kit_vector_index < int(k_gloves.size()) ? paint_kit_vector_index : k_gloves.size() - 1;
			paint_kit_index = k_gloves.at(paint_kit_vector_index).id;

			definition_override_vector_index = definition_override_vector_index < int(k_glove_names.size()) ? definition_override_vector_index : k_glove_names.size() - 1;
			definition_override_index = k_glove_names.at(definition_override_vector_index).definition_index;
		}
		else
		{
			paint_kit_vector_index = paint_kit_vector_index < int(k_skins.size()) ? paint_kit_vector_index : k_skins.size() - 1;
			paint_kit_index = k_skins.at(paint_kit_vector_index).id;

			definition_override_vector_index = definition_override_vector_index < int(k_knife_names.size()) ? definition_override_vector_index : k_knife_names.size() - 1;
			definition_override_index = k_knife_names.at(definition_override_vector_index).definition_index;
		}

		for(auto& sticker : stickers)
			sticker.update_values();
	}

	void update_ids()
	{
		definition_vector_index = find_if(k_weapon_names.begin(), k_weapon_names.end(), [this](const weapon_name& x)
		{
			return this->definition_index == x.definition_index;
		}) - k_weapon_names.begin();

		entity_quality_vector_index = find_if(k_quality_names.begin(), k_quality_names.end(), [this](const quality_name& x)
		{
			return this->entity_quality_index == x.index;
		}) - k_quality_names.begin();

		const auto& skin_set = definition_index == GLOVE_T_SIDE ? k_gloves : k_skins;

		paint_kit_vector_index = find_if(skin_set.begin(), skin_set.end(), [this](const paint_kit& x)
		{
			return this->paint_kit_index == x.id;
		}) - skin_set.begin();

		const auto& override_set = definition_index == GLOVE_T_SIDE ? k_glove_names : k_knife_names;

		definition_override_vector_index = find_if(override_set.begin(), override_set.end(), [this](const weapon_name& x)
		{
			return this->definition_override_index == x.definition_index;
		}) - override_set.begin();

		for(auto& sticker : stickers)
			sticker.update_ids();
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
	float wear = std::numeric_limits<float>::min();
	char custom_name[32] = "";
	std::array<sticker_setting, 5> stickers;
};

class config
{
public:
	config()
	{
		// Ghetto fix for possible race conditions
		m_items.reserve(128);

		// Default config
		m_items.push_back(item_setting());
	}

	auto save() -> void;
	auto load() -> void;

	auto get_by_definition_index(int definition_index) -> item_setting*;

	auto get_items() -> std::vector<item_setting>&
	{
		return m_items;
	}

	auto get_icon_override_map() -> std::unordered_map<std::string_view, std::string_view>&
	{
		return m_icon_overrides;
	}

	auto get_icon_override(const std::string_view original) const -> const char*
	{
		return m_icon_overrides.count(original) ? m_icon_overrides.at(original).data() : nullptr;
	}

private:
	std::vector<item_setting> m_items;
	std::unordered_map<std::string_view, std::string_view> m_icon_overrides;
};

extern config g_config;