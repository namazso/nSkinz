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
#include "hooks.hpp"
#include "../item_definitions.hpp"
#include "../nSkinz.hpp"
#include "../config.hpp"
#include "../sticker_changer.hpp"

static auto erase_override_if_exists_by_index(const int definition_index) -> void
{
	// We have info about the item not needed to be overridden
	if(k_weapon_info.count(definition_index))
	{
		auto& icon_override_map = g_config.get_icon_override_map();

		const auto& original_item = k_weapon_info.at(definition_index);

		// We are overriding its icon when not needed
		if(original_item.icon && icon_override_map.count(original_item.icon))
			icon_override_map.erase(icon_override_map.at(original_item.icon)); // Remove the leftover override
	}
}

static auto apply_config_on_attributable_item(sdk::C_BaseAttributableItem* item, const item_setting* config,
	const unsigned xuid_low) -> void
{
	// Force fallback values to be used.
	item->GetItemIDHigh() = -1;

	// Set the owner of the weapon to our lower XUID. (fixes StatTrak)
	item->GetAccountID() = xuid_low;

	if(config->entity_quality_index)
		item->GetEntityQuality() = config->entity_quality_index;

	if(config->custom_name[0])
		strcpy_s(item->GetCustomName(), config->custom_name);

	if(config->paint_kit_index)
		item->GetFallbackPaintKit() = config->paint_kit_index;

	if(config->seed)
		item->GetFallbackSeed() = config->seed;

	if(config->stat_trak)
		item->GetFallbackStatTrak() = config->stat_trak;

	item->GetFallbackWear() = config->wear;

	auto& definition_index = item->GetItemDefinitionIndex();

	auto& icon_override_map = g_config.get_icon_override_map();

	if(config->definition_override_index // We need to override defindex
		&& config->definition_override_index != definition_index // It is not yet overridden
		&& k_weapon_info.count(config->definition_override_index)) // We have info about what we gonna override it to
	{
		const auto old_definition_index = definition_index;

		definition_index = config->definition_override_index;

		const auto& replacement_item = k_weapon_info.at(config->definition_override_index);

		// Set the weapon model index -- required for paint kits to work on replacement items after the 29/11/2016 update.
		//item->GetModelIndex() = g_model_info->GetModelIndex(k_weapon_info.at(config->definition_override_index).model);
		item->SetModelIndex(g_model_info->GetModelIndex(replacement_item.model));
		item->GetClientNetworkable()->PreDataUpdate(0);

		// We didn't override 0, but some actual weapon, that we have data for
		if(old_definition_index && k_weapon_info.count(old_definition_index))
		{
			const auto& original_item = k_weapon_info.at(old_definition_index);

			if(original_item.icon && replacement_item.icon)
				icon_override_map[original_item.icon] = replacement_item.icon;
		}
	}
	else
	{
		erase_override_if_exists_by_index(definition_index);
	}

	apply_sticker_changer(item);
}

static auto get_wearable_create_fn() -> sdk::CreateClientClassFn
{
	auto clazz = g_client->GetAllClasses();

	// Please, if you gonna paste it into a cheat use classids here. I use names because they
	// won't change in the foreseeable future and i dont need high speed, but chances are
	// you already have classids, so use them instead, they are faster.
	while(strcmp(clazz->m_pNetworkName, "CEconWearable"))
		clazz = clazz->m_pNext;

	return clazz->m_pCreateFn;
}

static auto post_data_update_start() -> void
{
	const auto local_index = g_engine->GetLocalPlayer();
	const auto local = static_cast<sdk::C_BasePlayer*>(g_entity_list->GetClientEntity(local_index));

	if(!local)
		return;

	/*if(auto player_resource = *g_player_resource)
	{
		player_resource->GetCoins()[local_index] = 890;
		player_resource->GetMusicKits()[local_index] = 3;
		player_resource->GetRanks()[local_index] = 1;
		player_resource->GetWins()[local_index] = 1337;
	}*/

	sdk::player_info_t player_info;

	if(!g_engine->GetPlayerInfo(local_index, &player_info))
		return;

	// Handle glove config
	{
		const auto wearables = local->GetWearables();

		const auto glove_config = g_config.get_by_definition_index(GLOVE_T_SIDE);

		static auto glove_handle = sdk::CBaseHandle(0);

		auto glove = reinterpret_cast<sdk::C_BaseAttributableItem*>(g_entity_list->GetClientEntityFromHandle(wearables[0]));

		if(!glove) // There is no glove
		{
			// Try to get our last created glove
			const auto our_glove = reinterpret_cast<sdk::C_BaseAttributableItem*>(g_entity_list->GetClientEntityFromHandle(glove_handle));

			if(our_glove) // Our glove still exists
			{
				wearables[0] = glove_handle;
				glove = our_glove;
			}
		}

		if(local->GetLifeState() != sdk::LifeState::ALIVE)
		{
			// We are dead but we have a glove, destroy it
			if(glove)
			{
				glove->GetClientNetworkable()->SetDestroyedOnRecreateEntities();
				glove->GetClientNetworkable()->Release();
			}

			return;
		}

		if(glove_config && glove_config->definition_override_index)
		{
			// We don't have a glove, but we should
			if(!glove)
			{
				static auto create_wearable_fn = get_wearable_create_fn();

				const auto entry = g_entity_list->GetHighestEntityIndex() + 1;
				const auto serial = rand() % 0x1000;

				//glove = static_cast<C_BaseAttributableItem*>(create_wearable_fn(entry, serial));
				create_wearable_fn(entry, serial);
				glove = reinterpret_cast<sdk::C_BaseAttributableItem*>(g_entity_list->GetClientEntity(entry));
				assert(glove);

				// He he
				{
					static auto set_abs_origin_addr = platform::find_pattern("client.dll", "\x55\x8B\xEC\x83\xE4\xF8\x51\x53\x56\x57\x8B\xF1", "xxxxxxxxxxxx");

					const auto set_abs_origin_fn = reinterpret_cast<void(__thiscall*)(void*, const sdk::Vector&)>(set_abs_origin_addr);

					static constexpr sdk::Vector new_pos = { 10000.f, 10000.f, 10000.f };

					set_abs_origin_fn(glove, new_pos);
				}

				wearables[0] = entry | serial << 16;

				// Let's store it in case we somehow lose it.
				glove_handle = wearables[0];
			}

			// Thanks, Beakers
			glove->GetIndex() = -1;

			apply_config_on_attributable_item(glove, glove_config, player_info.xuid_low);
		}
	}

	// Handle weapon configs
	{
		auto& weapons = local->GetWeapons();

		for(auto weapon_handle : weapons)
		{
			if(weapon_handle == sdk::INVALID_EHANDLE_INDEX)
				break;

			auto weapon = static_cast<sdk::C_BaseAttributableItem*>(g_entity_list->GetClientEntityFromHandle(weapon_handle));

			if(!weapon)
				continue;

			auto& definition_index = weapon->GetItemDefinitionIndex();

			// All knives are terrorist knives.
			if(const auto active_conf = g_config.get_by_definition_index(is_knife(definition_index) ? WEAPON_KNIFE : definition_index))
				apply_config_on_attributable_item(weapon, active_conf, player_info.xuid_low);
			else
				erase_override_if_exists_by_index(definition_index);
		}
	}

	const auto view_model_handle = local->GetViewModel();

	if(view_model_handle == sdk::INVALID_EHANDLE_INDEX)
		return;

	const auto view_model = static_cast<sdk::C_BaseViewModel*>(g_entity_list->GetClientEntityFromHandle(view_model_handle));

	if(!view_model)
		return;

	const auto view_model_weapon_handle = view_model->GetWeapon();

	if(view_model_weapon_handle == sdk::INVALID_EHANDLE_INDEX)
		return;

	const auto view_model_weapon = static_cast<sdk::C_BaseAttributableItem*>(g_entity_list->GetClientEntityFromHandle(view_model_weapon_handle));

	if(!view_model_weapon)
		return;

	if (k_weapon_info.count(view_model_weapon->GetItemDefinitionIndex()))
	{
		const auto override_model = k_weapon_info.at(view_model_weapon->GetItemDefinitionIndex()).model;
		auto override_model_index = g_model_info->GetModelIndex(override_model);
		view_model->GetModelIndex() = override_model_index;

		auto world_model_handle = view_model_weapon->GetWeaponWorldModel();

		if (world_model_handle == sdk::INVALID_EHANDLE_INDEX)
			return;

		const auto world_model = static_cast<sdk::CBaseWeaponWorldModel*>(g_entity_list->GetClientEntityFromHandle(world_model_handle));

		if(!world_model)
			return;

		world_model->GetModelIndex() = override_model_index + 1;
	}
}

auto __fastcall hooks::FrameStageNotify::hooked(sdk::IBaseClientDLL* thisptr, void*, const sdk::ClientFrameStage_t stage) -> void
{
	if(stage == sdk::FRAME_NET_UPDATE_POSTDATAUPDATE_START)
		post_data_update_start();

	return m_original(thisptr, nullptr, stage);
}

hooks::FrameStageNotify::Fn* hooks::FrameStageNotify::m_original;