#include "PaintKitParser.hpp"
#include "Utilities\Platform.hpp"
#include "nSkinz.hpp"

#include <algorithm>
#include <codecvt>


std::vector<PaintKit_t> k_Skins;
std::vector<PaintKit_t> k_Gloves;


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

// could use CUtlString but this is just easier & CUtlString isn't needed anywhere else
struct String_t
{
	char* szBuffer;	//0x0000 
	int nCapacity;	//0x0004 
	int _unknown;	//0x0008 
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

	auto dwSigAddress = platform::FindPattern("client.dll", "\xE8\x00\x00\x00\x00\xFF\x76\x0C\x8D\x48\x04\xE8", "x????xxxxxxx");

	// Skip the opcode, read rel32 address
	auto iItemSystemOffset = *reinterpret_cast<int32_t*>(dwSigAddress + 1);
	// Add the offset to the end of the instruction
	auto pItemSystem = reinterpret_cast<CCStrike15ItemSystem* (*)()>(dwSigAddress + 5 + iItemSystemOffset);

	// Skip the instructions between, skip the opcode, read rel32 address
	auto iGetPaintKitDefinitionOffset = *reinterpret_cast<int32_t*>(dwSigAddress + 11 + 1);
	// Add the offset to the end of the instruction
	auto pGetPaintKitDefinition = reinterpret_cast<CPaintKit* (__thiscall *)(int id)>(dwSigAddress + 11 + 5 + iGetPaintKitDefinitionOffset);

	// The last offset is nHeadElement, we need that

	// push    ebp
	// mov     ebp, esp
	// sub     esp, 0Ch
	// mov     eax, [ecx+298h]

	// Skip instructions, skip opcode, read offset
	auto dwStartElementOffset = *reinterpret_cast<uint32_t*>(uintptr_t(pGetPaintKitDefinition) + 8 + 2);

	// Calculate head base from nStartElement's offset
	auto dwHeadOffset = dwStartElementOffset - 12;

	// Skip VTable, first member variable of ItemSystem is ItemSchema
	auto pItemSchema = reinterpret_cast<CCStrike15ItemSchema*>(uintptr_t(pItemSystem()) + sizeof(void*));

	auto pHead = reinterpret_cast<Head_t*>(uintptr_t(pItemSchema) + dwHeadOffset);

	// in case this is called multiple times for some reason
	k_Skins.clear();

	static std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;

	for (int i = 0; i <= pHead->nLastElement; ++i)
	{
		auto pKit = pHead->pMemory[i].pPaintKit;

		if (pKit->iIndex == 9001)
			continue;

		const wchar_t* wstrName = g_pLocalize->Find(pKit->Tag.szBuffer + 1);
		auto name = converter.to_bytes(wstrName);

		if (pKit->iIndex < 10000)
			k_Skins.push_back(PaintKit_t{ pKit->iIndex, name });
		else
			k_Gloves.push_back(PaintKit_t{ pKit->iIndex, name });
	}

	std::sort(k_Skins.begin(), k_Skins.end());
	std::sort(k_Gloves.begin(), k_Gloves.end());
}
