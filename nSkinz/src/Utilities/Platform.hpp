#pragma once
#include <cstdint>

namespace platform
{
	extern void* CaptureInterface(const char* module_name, const char* interface_name);
	extern uintptr_t FindPattern(const char* module_name, const char* pattern, const char* mask);
	extern bool IsCodePtr(void* ptr);
}
