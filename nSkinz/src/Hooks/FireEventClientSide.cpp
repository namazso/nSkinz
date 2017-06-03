#include "Hooks.hpp"
#include "../Configuration.hpp"
#include "../nSkinz.hpp"

bool __fastcall hooks::FireEventClientSide(IGameEventManager2* thisptr, void* edx_, IGameEvent* pEvent)
{
	static auto fnFireEventClientSide = g_GameEventsHook->GetOriginalFunction<FireEventClientSide_t>(9);

	// Filter to only the events we're interested in.
	if(!strcmp(pEvent->GetName(), "player_death")
		&& g_pEngine->GetPlayerForUserID(pEvent->GetInt("attacker")) == g_pEngine->GetLocalPlayer())
		if(auto szOverride = Config::Get()->GetIconOverride(pEvent->GetString("weapon")))
			pEvent->SetString("weapon", szOverride);

	return fnFireEventClientSide(thisptr, pEvent);
}
