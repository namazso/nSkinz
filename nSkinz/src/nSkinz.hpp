#pragma once
#include "SDK.hpp"
#include "RecvProxyHook.hpp"
#include <memory>

extern std::unique_ptr<VMTHook> g_ClientHook;
extern std::unique_ptr<VMTHook> g_GameEventsHook;

extern std::unique_ptr<RecvPropHook> g_SequenceHook;
