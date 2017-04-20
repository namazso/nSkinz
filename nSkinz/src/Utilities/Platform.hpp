#pragma once
#include <cstdint>

namespace platform
{
	extern void* CaptureInterface(const char* szModule, const char* szName);
	extern uintptr_t FindPattern(const char* szModule, const char* szPattern, const char* szMask);
	extern bool IsCodePtr(void* p);
}
