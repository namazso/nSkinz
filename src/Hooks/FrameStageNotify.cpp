#include "Hooks.hpp"
#include "../ItemDefinitions.hpp"
#include "../nSkinz.hpp"
#include "../Configuration.hpp"
#include "../StickerChanger.hpp"

static void EraseOverrideIfExistsByIndex(int definition_index)
{
	// We have info about the item not needed to be overridden
	if(k_weapon_info.count(definition_index))
	{

		auto& icon_override_map = g_config.GetIconOverrideMap();

		const auto& original_item = k_weapon_info.at(definition_index);

		// We are overriding its icon when not needed
		if(original_item.icon && icon_override_map.count(original_item.icon))
			icon_override_map.erase(icon_override_map.at(original_item.icon)); // Remove the leftover override
	}
}

static void ApplyConfigOnAttributableItem(C_BaseAttributableItem* item, const EconomyItem_t* config, unsigned xuid_low)
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

	auto& icon_override_map = g_config.GetIconOverrideMap();

	if(config->definition_override_index // We need to override defindex
		&& config->definition_override_index != definition_index // It is not yet overridden
		&& k_weapon_info.count(config->definition_override_index)) // We have info about what we gonna override it to
	{
		unsigned old_definition_index = definition_index;

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
		EraseOverrideIfExistsByIndex(definition_index);
	}

	ApplyStickerHooks(item);
}

static CreateClientClassFn GetWearableCreateFn()
{
	auto clazz = g_client->GetAllClasses();

	while(strcmp(clazz->m_pNetworkName, "CEconWearable"))
		clazz = clazz->m_pNext;

	return clazz->m_pCreateFn;
}

static void PostDataUpdateStart()
{
	auto local_index = g_engine->GetLocalPlayer();
	auto local = static_cast<C_BasePlayer*>(g_entity_list->GetClientEntity(local_index));

	if(!local)
		return;

	player_info_t player_info;

	if(!g_engine->GetPlayerInfo(local_index, &player_info))
		return;

	// Handle glove config
	{
		auto wearables = local->GetWearables();

		auto glove_config = g_config.GetByDefinitionIndex(GLOVE_T_SIDE);

		static CBaseHandle glove_handle = 0;

		auto glove = reinterpret_cast<C_BaseAttributableItem*>(g_entity_list->GetClientEntityFromHandle(wearables[0]));

		if(!glove) // There is no glove
		{
			// Try to get our last created glove
			auto our_glove = reinterpret_cast<C_BaseAttributableItem*>(g_entity_list->GetClientEntityFromHandle(glove_handle));

			if(our_glove) // Our glove still exists
			{
				wearables[0] = glove_handle;
				glove = our_glove;
			}
		}

		if(local->GetLifeState() != LifeState::ALIVE)
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
				static auto create_wearable_fn = GetWearableCreateFn();

				auto entry = g_entity_list->GetHighestEntityIndex() + 1;
				auto serial = rand() % 0x1000;

				//glove = static_cast<C_BaseAttributableItem*>(create_wearable_fn(entry, serial));
				create_wearable_fn(entry, serial);
				glove = reinterpret_cast<C_BaseAttributableItem*>(g_entity_list->GetClientEntity(entry));

				// He he
				{
					static auto set_abs_origin_fn = reinterpret_cast<void(__thiscall*)(void*, const Vector&)>
						(platform::FindPattern("client.dll", "\x55\x8B\xEC\x83\xE4\xF8\x51\x53\x56\x57\x8B\xF1", "xxxxxxxxxxxx"));

					static const Vector new_pos = { 10000.f, 10000.f, 10000.f };

					set_abs_origin_fn(glove, new_pos);
				}

				wearables[0] = entry | serial << 16;

				// Let's store it in case we somehow lose it.
				glove_handle = wearables[0];
			}

			// Thanks, Beakers
			*reinterpret_cast<int*>(uintptr_t(glove) + 0x64) = -1;

			ApplyConfigOnAttributableItem(glove, glove_config, player_info.xuid_low);
		}
	}

	// Handle weapon configs
	{
		auto weapons = local->GetWeapons();

		for(size_t i = 0; weapons[i] != INVALID_EHANDLE_INDEX; i++)
		{
			auto weapon = static_cast<C_BaseAttributableItem*>(g_entity_list->GetClientEntityFromHandle(weapons[i]));

			if(!weapon)
				continue;

			auto& definition_index = weapon->GetItemDefinitionIndex();

			// All knives are terrorist knives.
			if(auto active_conf = g_config.GetByDefinitionIndex(IsKnife(definition_index) ? WEAPON_KNIFE : definition_index))
				ApplyConfigOnAttributableItem(weapon, active_conf, player_info.xuid_low);
			else
				EraseOverrideIfExistsByIndex(definition_index);
		}
	}

	auto view_model_handle = local->GetViewModel();

	if(view_model_handle == INVALID_EHANDLE_INDEX)
		return;

	auto view_model = static_cast<C_BaseViewModel*>(g_entity_list->GetClientEntityFromHandle(view_model_handle));

	if(!view_model)
		return;

	auto view_model_weapon_handle = view_model->GetWeapon();

	if(view_model_weapon_handle == INVALID_EHANDLE_INDEX)
		return;

	auto view_model_weapon = static_cast<C_BaseAttributableItem*>(g_entity_list->GetClientEntityFromHandle(view_model_weapon_handle));

	if(!view_model_weapon)
		return;

	if(k_weapon_info.count(view_model_weapon->GetItemDefinitionIndex()))
	{
		auto& override_model = k_weapon_info.at(view_model_weapon->GetItemDefinitionIndex()).model;
		view_model->GetModelIndex() = g_model_info->GetModelIndex(override_model);
	}
}

void __fastcall hooks::FrameStageNotify(IBaseClientDLL* thisptr, void*, ClientFrameStage_t stage)
{
	static auto original_fn = g_client_hook->GetOriginalFunction<FrameStageNotify_t>(36);

	if(stage == FRAME_NET_UPDATE_POSTDATAUPDATE_START)
		PostDataUpdateStart();

	return original_fn(thisptr, stage);
}
