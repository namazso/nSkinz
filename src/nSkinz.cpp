#include "nSkinz.hpp"
#include "Hooks/Hooks.hpp"
#include "Renderer.hpp"
#include "KitParser.hpp"
#include "UpdateCheck.hpp"
#include "Configuration.hpp"

IBaseClientDLL*		g_client;
IClientEntityList*	g_entity_list;
IVEngineClient*		g_engine;
IVModelInfoClient*	g_model_info;
IGameEventManager2*	g_game_event_manager;
ILocalize*			g_localize;

CBaseClientState**	g_client_state;

VMTHook* g_client_hook;
VMTHook* g_game_event_manager_hook;

RecvPropHook* g_sequence_hook;

void __stdcall Initialize(void* instance)
{
	g_client = CaptureInterface<IBaseClientDLL>("client.dll", CLIENT_DLL_INTERFACE_VERSION);
	g_entity_list = CaptureInterface<IClientEntityList>("client.dll", VCLIENTENTITYLIST_INTERFACE_VERSION);
	g_engine = CaptureInterface<IVEngineClient>("engine.dll", VENGINE_CLIENT_INTERFACE_VERSION);
	g_model_info = CaptureInterface<IVModelInfoClient>("engine.dll", VMODELINFO_CLIENT_INTERFACE_VERSION);
	g_game_event_manager = CaptureInterface<IGameEventManager2>("engine.dll", INTERFACEVERSION_GAMEEVENTSMANAGER2);
	g_localize = CaptureInterface<ILocalize>("localize.dll", ILOCALIZE_CLIENT_INTERFACE_VERSION);

	g_client_state = *reinterpret_cast<CBaseClientState***>(GetVirtualFunction<uintptr_t>(g_engine, 12) + 0x10);

	CheckUpdate();

	// Get skins
	InitializeKits();

	g_config.Load();

	Render::Initialize();

	g_client_hook = new VMTHook(g_client);
	g_client_hook->HookFunction(reinterpret_cast<void*>(hooks::FrameStageNotify), 36);

	g_game_event_manager_hook = new VMTHook(g_game_event_manager);
	g_game_event_manager_hook->HookFunction(reinterpret_cast<void*>(hooks::FireEventClientSide), 9);

	auto sequence_prop = C_BaseViewModel::GetSequenceProp();

	g_sequence_hook = new RecvPropHook(sequence_prop, hooks::SequenceProxyFn);
}

// If we aren't unloaded correctly (like when you close csgo)
// we should just leak the hooks, since the hooked instances
// might be already destroyed
void __stdcall UnInitialize()
{
	Render::Uninitialize();

	delete g_client_hook;
	delete g_game_event_manager_hook;

	delete g_sequence_hook;
}

#include <windows.h>

bool WINAPI DllMain(HINSTANCE instance, DWORD reason, LPVOID reserved)
{
	if (reason == DLL_PROCESS_ATTACH)
		CreateThread(nullptr, 0, LPTHREAD_START_ROUTINE(Initialize), instance, 0, nullptr);

	return true;
}
