
#include <cstring>
#include "SDK.hpp"
#include "Configuration.hpp"

enum class EStickerAttributeType
{
	Index,
	Wear,
	Scale,
	Rotation
};

static uint16_t s_econ_item_interface_wrapper_offset = 0;

namespace Hooks
{
	static void* oGetStickerAttributeBySlotIndexFloat;

	static float __fastcall GetStickerAttributeBySlotIndexFloat(void* thisptr, void*, int slot, EStickerAttributeType attribute, float unknown)
	{
		auto item = reinterpret_cast<C_BaseAttributableItem*>(uintptr_t(thisptr) - s_econ_item_interface_wrapper_offset);

		auto defindex = item->GetItemDefinitionIndex();

		auto config = g_config.GetByDefinitionIndex(defindex);

		if(config)
		{
			switch(attribute)
			{
			case EStickerAttributeType::Wear:
				return config->stickers[slot].wear;
			case EStickerAttributeType::Scale:
				return config->stickers[slot].scale;
			case EStickerAttributeType::Rotation:
				return config->stickers[slot].rotation;
			default:
				break;
			}
		}

		return reinterpret_cast<decltype(GetStickerAttributeBySlotIndexFloat)*>(oGetStickerAttributeBySlotIndexFloat)(thisptr, nullptr, slot, attribute, unknown);
	}

	static void* oGetStickerAttributeBySlotIndexInt;

	static unsigned int __fastcall GetStickerAttributeBySlotIndexInt(void* thisptr, void*, int slot, EStickerAttributeType attribute, unsigned unknown)
	{
		auto item = reinterpret_cast<C_BaseAttributableItem*>(uintptr_t(thisptr) - s_econ_item_interface_wrapper_offset);

		if(attribute == EStickerAttributeType::Index)
		{
			auto defindex = item->GetItemDefinitionIndex();

			auto config = g_config.GetByDefinitionIndex(defindex);

			if(config)
				return config->stickers[slot].kit_index;
		}

		return reinterpret_cast<decltype(GetStickerAttributeBySlotIndexInt)*>(oGetStickerAttributeBySlotIndexInt)(thisptr, nullptr, slot, attribute, unknown);
	}
}

void ApplyStickerHooks(C_BaseAttributableItem* item)
{
	if(!s_econ_item_interface_wrapper_offset)
		s_econ_item_interface_wrapper_offset = NetVarManager::Get().GetOffset(FnvHash("CEconEntity->m_Item")) + 0xC;

	void**& vmt = *reinterpret_cast<void***>(uintptr_t(item) + s_econ_item_interface_wrapper_offset);

	static void** hooked_vmt = nullptr;

	if(!hooked_vmt)
	{
		size_t size = 0;

		while(platform::IsCodePtr(vmt[size]))
			++size;

		hooked_vmt = new void*[size];
		memcpy(hooked_vmt, vmt, size * sizeof(void*));

		Hooks::oGetStickerAttributeBySlotIndexFloat = hooked_vmt[4];
		hooked_vmt[4] = reinterpret_cast<void*>(&Hooks::GetStickerAttributeBySlotIndexFloat);

		Hooks::oGetStickerAttributeBySlotIndexInt = hooked_vmt[5];
		hooked_vmt[5] = reinterpret_cast<void*>(&Hooks::GetStickerAttributeBySlotIndexInt);
	}

	vmt = hooked_vmt;
}