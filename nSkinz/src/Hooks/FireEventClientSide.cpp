#include "Hooks.hpp"
#include "../Configuration.hpp"
#include "../nSkinz.hpp"

bool __fastcall hooks::FireEventClientSide(IGameEventManager2* thisptr, void*, IGameEvent* event)
{
	static auto original_fn = g_game_event_manager_hook->GetOriginalFunction<FireEventClientSide_t>(9);

	// Filter to only the events we're interested in.
	if(!strcmp(event->GetName(), "player_death")
		&& g_engine->GetPlayerForUserID(event->GetInt("attacker")) == g_engine->GetLocalPlayer())
		if(auto icon_override = g_config.GetIconOverride(event->GetString("weapon")))
			event->SetString("weapon", icon_override);

	return original_fn(thisptr, event);
}
