#pragma once
#include "../SDK.hpp"

namespace hooks
{
	// IBaseClientDLL

	using FrameStageNotify_t = void(__thiscall *)(IBaseClientDLL*, ClientFrameStage_t);

	extern void __fastcall FrameStageNotify(IBaseClientDLL* thisptr, void* edx_, ClientFrameStage_t stage);

	// IGameEventManager2

	using FireEventClientSide_t = bool(__thiscall *)(IGameEventManager2*, IGameEvent*);

	extern bool __fastcall FireEventClientSide(IGameEventManager2* thisptr, void* edx_, IGameEvent* event);

	// NetVar Proxies

	extern void __cdecl SequenceProxyFn(const CRecvProxyData* proxy_data_const, void* entity, void* output);
}
