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
#include "config.hpp"
#include "SDK.hpp"

#include <fstream>
#include <nlohmann/json.hpp>

config g_config;

using json = nlohmann::json;

auto to_json(json& j, const sticker_setting& sticker) -> void
{
	j = json
	{
		{"kit", sticker.kit_index},
		{"wear", sticker.wear},
		{"scale", sticker.scale},
		{"rotation", sticker.rotation}
	};
}

auto from_json(const json& j, sticker_setting& sticker) -> void
{
	sticker.kit_index = j.at("kit").get<int>();
	sticker.wear = j.at("wear").get<float>();
	sticker.scale = j.at("scale").get<float>();
	sticker.rotation = j.at("rotation").get<float>();
	sticker.update_ids();
}

auto to_json(json& j, const item_setting& item) -> void
{
	j = json
	{
		{"name", item.name},
		{"enabled", item.enabled},
		{"definition_index", item.definition_index},
		{"entity_quality_index", item.entity_quality_index},
		{"paint_kit_index", item.paint_kit_index},
		{"definition_override_index", item.definition_override_index},
		{"seed", item.seed},
		{"stat_trak", item.stat_trak},
		{"wear", item.wear},
		{"custom_name", item.custom_name},
		{"stickers", item.stickers},
	};
}

auto from_json(const json& j, item_setting& item) -> void
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
	auto stickers = j.at("stickers").get<std::vector<sticker_setting>>();
	// Fuck you std::copy
	//std::copy(stickers.begin(), stickers.end(), item.stickers);

	const auto size = std::min(stickers.size(), item.stickers.size());

	for(std::size_t i = 0; i < size; ++i)
		item.stickers.at(i) = stickers.at(i);

	item.update_ids();
}

auto config::save() -> void
{
	std::ofstream("nSkinz.json") << json(m_items);
}

auto config::load() -> void
{
	try
	{
		m_items = json::parse(std::ifstream("nSkinz.json")).get<std::vector<item_setting>>();
		(*g_client_state)->ForceFullUpdate();
	}
	catch(const std::exception&)
	{
		// Config file doesn't exists or is malformed, just ignore it
	}
}

auto config::get_by_definition_index(const int definition_index) -> item_setting*
{
	for(auto& x : m_items)
		if(x.enabled && x.definition_index == definition_index)
			return &x;

	return nullptr;
}
