#include "Platform.hpp"
#include "Interface.hpp"
#include <vector>
#include <algorithm>

// Platform tools for windows. Maybe I'll make linux ones too

#include <Windows.h>
#include <psapi.h>

void* platform::CaptureInterface(const char* szModule, const char* szName)
{
	HMODULE hModule = nullptr;

	while (!hModule)
	{
		hModule = GetModuleHandleA(szModule);
		Sleep(10);
	}

	auto fnCreateInterface = reinterpret_cast<CreateInterfaceFn>(GetProcAddress(hModule, "CreateInterface"));

	return fnCreateInterface(szName, nullptr);
}

uintptr_t platform::FindPattern(const char* szModule, const char* szPattern, const char* szMask)
{
	MODULEINFO ModuleInfo = {};
	GetModuleInformation(GetCurrentProcess(), GetModuleHandleA(szModule), &ModuleInfo, sizeof(MODULEINFO));

	auto address = reinterpret_cast<uint8_t*>(ModuleInfo.lpBaseOfDll);
	auto size = ModuleInfo.SizeOfImage;

	std::vector<std::pair<uint8_t, bool>> vSignature;

	for (size_t i = 0; szMask[i]; i++)
		vSignature.push_back(std::make_pair(szPattern[i], szMask[i] == 'x'));

	auto ret = std::search(address, address + size, vSignature.begin(), vSignature.end(),
		[](unsigned char curr, std::pair<uint8_t, bool> curr_pattern)
	{
		return (!curr_pattern.second) || curr == curr_pattern.first;
	});

	return ret == address + size ? 0 : reinterpret_cast<uintptr_t>(ret);
}

bool platform::IsCodePtr(void* p)
{
	constexpr const DWORD protect_flags = PAGE_EXECUTE | PAGE_EXECUTE_READ | PAGE_EXECUTE_READWRITE | PAGE_EXECUTE_WRITECOPY;

	MEMORY_BASIC_INFORMATION out;
	VirtualQuery(p, &out, sizeof out);

	return out.Type
		&& !(out.Protect & (PAGE_GUARD | PAGE_NOACCESS))
		&& out.Protect & protect_flags;
}
