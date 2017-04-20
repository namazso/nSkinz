#pragma once
#include <cstdint>

class CBaseClientState
{
public:
	void ForceFullUpdate()
	{
		// Windows m_nDeltaTick: 0x16C
		// Linux m_nDeltaTick: 0x1FC
		// We just assume this is because 64 bit vs 32 bit because
		// if it was MAX_OSPATH the difference would be larger
		*reinterpret_cast<int*>(reinterpret_cast<uintptr_t>(this) + 0x148 + sizeof(void*) * 9) = -1;
	};
};

extern CBaseClientState** g_ppClientState;
