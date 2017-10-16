#include "Platform.hpp"
#include "Interface.hpp"
#include <vector>
#include <algorithm>

// Platform tools for windows. Maybe I'll make linux ones too

#include <Windows.h>
#include <psapi.h>

void* platform::CaptureInterface(const char* module_name, const char* interface_name)
{
	HMODULE module_base = nullptr;

	while (!module_base)
	{
		module_base = GetModuleHandleA(module_name);
		Sleep(10);
	}

	auto create_interface_fn = reinterpret_cast<CreateInterfaceFn>(GetProcAddress(module_base, "CreateInterface"));

	return create_interface_fn(interface_name, nullptr);
}

uintptr_t platform::FindPattern(const char* module_name, const char* pattern, const char* mask)
{
	MODULEINFO module_info = {};
	K32GetModuleInformation(GetCurrentProcess(), GetModuleHandleA(module_name), &module_info, sizeof(MODULEINFO));

	auto address = reinterpret_cast<uint8_t*>(module_info.lpBaseOfDll);
	auto size = module_info.SizeOfImage;

	std::vector<std::pair<uint8_t, bool>> signature;

	for (size_t i = 0; mask[i]; i++)
		signature.push_back(std::make_pair(pattern[i], mask[i] == 'x'));

	auto ret = std::search(address, address + size, signature.begin(), signature.end(),
		[](unsigned char curr, std::pair<uint8_t, bool> curr_pattern)
	{
		return (!curr_pattern.second) || curr == curr_pattern.first;
	});

	return ret == address + size ? 0 : reinterpret_cast<uintptr_t>(ret);
}

bool platform::IsCodePtr(void* ptr)
{
	constexpr const DWORD protect_flags = PAGE_EXECUTE | PAGE_EXECUTE_READ | PAGE_EXECUTE_READWRITE | PAGE_EXECUTE_WRITECOPY;

	MEMORY_BASIC_INFORMATION out;
	VirtualQuery(ptr, &out, sizeof out);

	return out.Type
		&& !(out.Protect & (PAGE_GUARD | PAGE_NOACCESS))
		&& out.Protect & protect_flags;
}
