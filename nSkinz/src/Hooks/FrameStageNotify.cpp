#include "Hooks.hpp"
#include "../ItemDefinitions.hpp"
#include "../nSkinz.hpp"
#include "../Configuration.hpp"

void inline PostDataUpdateStart()
{
	auto iLocal = g_pEngine->GetLocalPlayer();
	auto pLocal = static_cast<C_BasePlayer*>(g_pEntityList->GetClientEntity(iLocal));

	Config::Get()->EmptyIconOverrides();

	if(!pLocal || pLocal->GetLifeState() != LifeState::ALIVE)
		return;

	player_info_t PlayerInfo;

	if(!g_pEngine->GetPlayerInfo(iLocal, &PlayerInfo))
		return;

	auto hWeapons = pLocal->GetWeapons();

	for(size_t i = 0; hWeapons[i] != INVALID_EHANDLE_INDEX; i++)
	{
		auto pWeapon = static_cast<C_BaseAttributableItem*>(g_pEntityList->GetClientEntityFromHandle(hWeapons[i]));

		if(!pWeapon)
			continue;

		auto& iDefIndex = pWeapon->GetItemDefinitionIndex();

		// All knives are terrorist knives.
		if(auto pActive = Config::Get()->GetByDefinitionIndex(IsKnife(iDefIndex) ? 42 : iDefIndex))
		{
			// Force fallback values to be used.
			pWeapon->GetItemIDHigh() = -1;

			// Set the owner of the weapon to our lower XUID. (fixes StatTrak)
			pWeapon->GetAccountID() = PlayerInfo.xuid_low;

			if(pActive->iDefinitionOverrideIndex && k_WeaponInfo.count(pActive->iDefinitionOverrideIndex) && k_WeaponInfo.count(iDefIndex))
			{
				// Set the weapon model index -- required for paint kits to work on replacement items after the 29/11/2016 update.
				pWeapon->GetModelIndex() = g_pModelInfo->GetModelIndex(k_WeaponInfo.at(pActive->iDefinitionOverrideIndex).szModel);

				auto& OriginalItem = k_WeaponInfo.at(iDefIndex);
				auto& ReplacementItem = k_WeaponInfo.at(pActive->iDefinitionOverrideIndex);

				if(OriginalItem.szIcon && ReplacementItem.szIcon)
					Config::Get()->AddIconOverride(OriginalItem.szIcon, ReplacementItem.szIcon);

				iDefIndex = pActive->iDefinitionOverrideIndex;
			}

			if(pActive->iEntityQualityIndex)
				pWeapon->GetEntityQuality() = pActive->iEntityQualityIndex;

			if(pActive->szCustomName[0])
				strcpy_s(pWeapon->GetCustomName(), 32, pActive->szCustomName);

			if(pActive->iPaintKitIndex && (iDefIndex != 42 && iDefIndex != 59))
				pWeapon->GetFallbackPaintKit() = pActive->iPaintKitIndex;

			if(pActive->iSeed)
				pWeapon->GetFallbackSeed() = pActive->iSeed;

			if(pActive->iStatTrak)
				pWeapon->GetFallbackStatTrak() = pActive->iStatTrak;

			pWeapon->GetFallbackWear() = pActive->flWear;
		}
	}

	auto hViewModel = pLocal->GetViewModel();

	if(hViewModel == INVALID_EHANDLE_INDEX)
		return;

	auto pViewModel = static_cast<C_BaseViewModel*>(g_pEntityList->GetClientEntityFromHandle(hViewModel));

	if(!pViewModel)
		return;

	auto hViewModelWeapon = pViewModel->GetWeapon();

	if(hViewModelWeapon == INVALID_EHANDLE_INDEX)
		return;

	auto pViewModelWeapon = static_cast<C_BaseAttributableItem*>(g_pEntityList->GetClientEntityFromHandle(hViewModelWeapon));

	if(!pViewModelWeapon)
		return;

	if(k_WeaponInfo.count(pViewModelWeapon->GetItemDefinitionIndex()))
	{
		auto& OverrideWeapon = k_WeaponInfo.at(pViewModelWeapon->GetItemDefinitionIndex());
		pViewModel->GetModelIndex() = g_pModelInfo->GetModelIndex(OverrideWeapon.szModel);
	}
}

void __fastcall hooks::FrameStageNotify(IBaseClientDLL* thisptr, void* edx_, ClientFrameStage_t stage)
{
	static auto fnFrameStageNotify = g_ClientHook->GetOriginalFunction<FrameStageNotify_t>(36);

	if(stage == FRAME_NET_UPDATE_POSTDATAUPDATE_START)
		PostDataUpdateStart();

	return fnFrameStageNotify(thisptr, stage);
}
