#include "nSkinz.hpp"
#include "Hooks/Hooks.hpp"
#include "Utilities/Platform.hpp"
#include "Renderer.hpp"
#include "PaintKitParser.hpp"

IBaseClientDLL*		g_pClient;
IClientEntityList*	g_pEntityList;
IVEngineClient*		g_pEngine;
IVModelInfoClient*	g_pModelInfo;
IGameEventManager2*	g_pGameEvents;
ILocalize*			g_pLocalize;

CBaseClientState**	g_ppClientState;

std::unique_ptr<Renderer> g_pRenderer;

std::unique_ptr<VMTHook> g_ClientHook;
std::unique_ptr<VMTHook> g_GameEventsHook;
std::unique_ptr<VMTHook> g_D3D9Hook;

std::unique_ptr<RecvPropHook> g_SequenceHook;

void __stdcall Initialize(void* pInstance)
{
	g_pClient = CaptureInterface<IBaseClientDLL>("client.dll", CLIENT_DLL_INTERFACE_VERSION);
	g_pEntityList = CaptureInterface<IClientEntityList>("client.dll", VCLIENTENTITYLIST_INTERFACE_VERSION);
	g_pEngine = CaptureInterface<IVEngineClient>("engine.dll", VENGINE_CLIENT_INTERFACE_VERSION);
	g_pModelInfo = CaptureInterface<IVModelInfoClient>("engine.dll", VMODELINFO_CLIENT_INTERFACE_VERSION);
	g_pGameEvents = CaptureInterface<IGameEventManager2>("engine.dll", INTERFACEVERSION_GAMEEVENTSMANAGER2);
	g_pLocalize = CaptureInterface<ILocalize>("localize.dll", ILOCALIZE_CLIENT_INTERFACE_VERSION);

	// Get skins
	GetPaintKits();

	g_ppClientState = *reinterpret_cast<CBaseClientState***>(GetVirtualFunction<uintptr_t>(g_pEngine, 12) + 16);

	g_pRenderer = std::make_unique<Renderer>();

	g_ClientHook = std::make_unique<VMTHook>(g_pClient);
	g_ClientHook->HookFunction(hooks::FrameStageNotify, 36);

	g_GameEventsHook = std::make_unique<VMTHook>(g_pGameEvents);
	g_GameEventsHook->HookFunction(hooks::FireEventClientSide, 9);

	auto pSequenceProp = C_BaseViewModel::GetSequenceProp();

	g_SequenceHook = std::make_unique<RecvPropHook>(pSequenceProp, hooks::SequenceProxyFn);
}

#include <windows.h>

bool __stdcall DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID pReserved)
{
	if (dwReason == DLL_PROCESS_ATTACH)
		CreateThread(nullptr, 0, LPTHREAD_START_ROUTINE(Initialize), hInstance, 0, nullptr);

	return true;
}
