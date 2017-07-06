#include "PaintKitParser.hpp"
#include "Utilities\Platform.hpp"
#include "nSkinz.hpp"

#include <algorithm>
#include <codecvt>


std::vector<PaintKit_t> k_skins;
std::vector<PaintKit_t> k_gloves;


class CCStrike15ItemSchema;
class CCStrike15ItemSystem;
struct CPaintKit;
struct Node_t;
struct Head_t;

struct Head_t
{
	Node_t* pMemory;		//0x0000
	int nAllocationCount;	//0x0004
	int nGrowSize;			//0x0008
	int nStartElement;		//0x000C
	int nNextAvailable;		//0x0010
	int _unknown;			//0x0014
	int nLastElement;		//0x0018
}; //Size=0x001C

struct Node_t
{
	int nPreviousId;		//0x0000
	int nNextId;			//0x0004
	void* _unknown_ptr;		//0x0008
	int _unknown;			//0x000C
	int iPaintKitId;		//0x0010
	CPaintKit* pPaintKit;	//0x0014
}; //Size=0x0018

// could use CUtlString but this is just easier and CUtlString isn't needed anywhere else
struct String_t
{
	char* szBuffer;	//0x0000
	int nCapacity;	//0x0004
	int nGrowSize;	//0x0008
	int nLength;	//0x000C
}; //Size=0x0010

struct CPaintKit
{
	int iIndex;				//0x0000
	String_t Name;			//0x0004
	String_t NiceName;		//0x0014
	String_t Tag;			//0x0024
	String_t ShortName;		//0x0034
	String_t _unknown;		//0x0044
	char pad_0x0054[0x8C];	//0x0054
}; //Size=0x00E0


void GetPaintKits()
{
	// Search the relative calls

	// call    ItemSystem
	// push    dword ptr [esi+0Ch]
	// lea     ecx, [eax+4]
	// call    CEconItemSchema::GetPaintKitDefinition

	auto sig_address = platform::FindPattern("client.dll", "\xE8\x00\x00\x00\x00\xFF\x76\x0C\x8D\x48\x04\xE8", "x????xxxxxxx");

	// Skip the opcode, read rel32 address
	auto item_system_offset = *reinterpret_cast<int32_t*>(sig_address + 1);
	// Add the offset to the end of the instruction
	auto item_system_fn = reinterpret_cast<CCStrike15ItemSystem* (*)()>(sig_address + 5 + item_system_offset);

	// Skip the instructions between, skip the opcode, read rel32 address
	auto get_paint_kit_definition_offset = *reinterpret_cast<int32_t*>(sig_address + 11 + 1);
	// Add the offset to the end of the instruction
	auto get_paint_kit_definition_fn = reinterpret_cast<CPaintKit* (__thiscall *)(int id)>(sig_address + 11 + 5 + get_paint_kit_definition_offset);

	// The last offset is nHeadElement, we need that

	// push    ebp
	// mov     ebp, esp
	// sub     esp, 0Ch
	// mov     eax, [ecx+298h]

	// Skip instructions, skip opcode, read offset
	auto start_element_offset = *reinterpret_cast<uint32_t*>(uintptr_t(get_paint_kit_definition_fn) + 8 + 2);

	// Calculate head base from nStartElement's offset
	auto head_offset = start_element_offset - 12;

	// Skip VTable, first member variable of ItemSystem is ItemSchema
	auto item_schema = reinterpret_cast<CCStrike15ItemSchema*>(uintptr_t(item_system_fn()) + sizeof(void*));

	auto map_head = reinterpret_cast<Head_t*>(uintptr_t(item_schema) + head_offset);

	// in case this is called multiple times for some reason
	k_skins.clear();

	static std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;

	for (int i = 0; i <= map_head->nLastElement; ++i)
	{
		auto paint_kit = map_head->pMemory[i].pPaintKit;

		if (paint_kit->iIndex == 9001)
			continue;

		const wchar_t* wide_name = g_localize->Find(paint_kit->Tag.szBuffer + 1);
		auto name = converter.to_bytes(wide_name);

		if (paint_kit->iIndex < 10000)
			k_skins.push_back(PaintKit_t{ paint_kit->iIndex, name });
		else
			k_gloves.push_back(PaintKit_t{ paint_kit->iIndex, name });
	}

	std::sort(k_skins.begin(), k_skins.end());
	std::sort(k_gloves.begin(), k_gloves.end());
}
