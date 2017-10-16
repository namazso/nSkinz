#pragma once
#include <cstdint>

class CBaseClientState
{
public:
	void ForceFullUpdate()
	{
		*reinterpret_cast<int*>(reinterpret_cast<uintptr_t>(this) + 0x174) = -1;
	};
};

extern CBaseClientState** g_client_state;
