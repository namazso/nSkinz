#include "Hooks.hpp"
#include "../ItemDefinitions.hpp"
#include "../nSkinz.hpp"
#include "../Configuration.hpp"

void inline PostDataUpdateStart()
{
	auto local_index = g_engine->GetLocalPlayer();
	auto local = static_cast<C_BasePlayer*>(g_entity_list->GetClientEntity(local_index));

	Config::Get()->EmptyIconOverrides();

	if(!local || local->GetLifeState() != LifeState::ALIVE)
		return;

	player_info_t player_info;

	if(!g_engine->GetPlayerInfo(local_index, &player_info))
		return;

	auto weapons = local->GetWeapons();

	for(size_t i = 0; weapons[i] != INVALID_EHANDLE_INDEX; i++)
	{
		auto weapon = static_cast<C_BaseAttributableItem*>(g_entity_list->GetClientEntityFromHandle(weapons[i]));

		if(!weapon)
			continue;

		auto& definition_index = weapon->GetItemDefinitionIndex();

		// All knives are terrorist knives.
		if(auto active_conf = Config::Get()->GetByDefinitionIndex(IsKnife(definition_index) ? 42 : definition_index))
		{
			// Force fallback values to be used.
			weapon->GetItemIDHigh() = -1;

			// Set the owner of the weapon to our lower XUID. (fixes StatTrak)
			weapon->GetAccountID() = player_info.xuid_low;

			if(active_conf->iDefinitionOverrideIndex && k_weapon_info.count(active_conf->iDefinitionOverrideIndex) && k_weapon_info.count(definition_index))
			{
				// Set the weapon model index -- required for paint kits to work on replacement items after the 29/11/2016 update.
				weapon->GetModelIndex() = g_model_info->GetModelIndex(k_weapon_info.at(active_conf->iDefinitionOverrideIndex).model);

				auto& original_item = k_weapon_info.at(definition_index);
				auto& replacement_item = k_weapon_info.at(active_conf->iDefinitionOverrideIndex);

				if(original_item.icon && replacement_item.icon)
					Config::Get()->AddIconOverride(original_item.icon, replacement_item.icon);

				definition_index = active_conf->iDefinitionOverrideIndex;
			}

			if(active_conf->iEntityQualityIndex)
				weapon->GetEntityQuality() = active_conf->iEntityQualityIndex;

			if(active_conf->szCustomName[0])
				strcpy_s(weapon->GetCustomName(), 32, active_conf->szCustomName);

			if(active_conf->iPaintKitIndex && (definition_index != 42 && definition_index != 59))
				weapon->GetFallbackPaintKit() = active_conf->iPaintKitIndex;

			if(active_conf->iSeed)
				weapon->GetFallbackSeed() = active_conf->iSeed;

			if(active_conf->iStatTrak)
				weapon->GetFallbackStatTrak() = active_conf->iStatTrak;

			weapon->GetFallbackWear() = active_conf->flWear;
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
		auto& OverrideWeapon = k_weapon_info.at(view_model_weapon->GetItemDefinitionIndex());
		view_model->GetModelIndex() = g_model_info->GetModelIndex(OverrideWeapon.model);
	}
}

void __fastcall hooks::FrameStageNotify(IBaseClientDLL* thisptr, void*, ClientFrameStage_t stage)
{
	static auto original_fn = g_client_hook->GetOriginalFunction<FrameStageNotify_t>(36);

	if(stage == FRAME_NET_UPDATE_POSTDATAUPDATE_START)
		PostDataUpdateStart();

	return original_fn(thisptr, stage);
}
