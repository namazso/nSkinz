#include "Hooks.hpp"
#include "../nSkinz.hpp"
#include <unordered_map>
#include <functional>

enum ESequence
{
	SEQUENCE_DEFAULT_DRAW = 0,
	SEQUENCE_DEFAULT_IDLE1 = 1,
	SEQUENCE_DEFAULT_IDLE2 = 2,
	SEQUENCE_DEFAULT_LIGHT_MISS1 = 3,
	SEQUENCE_DEFAULT_LIGHT_MISS2 = 4,
	SEQUENCE_DEFAULT_HEAVY_MISS1 = 9,
	SEQUENCE_DEFAULT_HEAVY_HIT1 = 10,
	SEQUENCE_DEFAULT_HEAVY_BACKSTAB = 11,
	SEQUENCE_DEFAULT_LOOKAT01 = 12,

	SEQUENCE_BUTTERFLY_DRAW = 0,
	SEQUENCE_BUTTERFLY_DRAW2 = 1,
	SEQUENCE_BUTTERFLY_LOOKAT01 = 13,
	SEQUENCE_BUTTERFLY_LOOKAT03 = 15,

	SEQUENCE_FALCHION_IDLE1 = 1,
	SEQUENCE_FALCHION_HEAVY_MISS1 = 8,
	SEQUENCE_FALCHION_HEAVY_MISS1_NOFLIP = 9,
	SEQUENCE_FALCHION_LOOKAT01 = 12,
	SEQUENCE_FALCHION_LOOKAT02 = 13,

	SEQUENCE_DAGGERS_IDLE1 = 1,
	SEQUENCE_DAGGERS_LIGHT_MISS1 = 2,
	SEQUENCE_DAGGERS_LIGHT_MISS5 = 6,
	SEQUENCE_DAGGERS_HEAVY_MISS2 = 11,
	SEQUENCE_DAGGERS_HEAVY_MISS1 = 12,

	SEQUENCE_BOWIE_IDLE1 = 1,
};

inline int RandomSequence(int low, int high)
{
	return rand() % (high - low + 1) + low;
}

// Map of animation fixes
const static std::unordered_map<std::string, std::function<int(int)>> mapAnimFixes
{
	{"models/weapons/v_knife_butterfly.mdl", [](int nSequence) -> int
		{
			switch(nSequence)
			{
			case SEQUENCE_DEFAULT_DRAW:
				return RandomSequence(SEQUENCE_BUTTERFLY_DRAW, SEQUENCE_BUTTERFLY_DRAW2);
			case SEQUENCE_DEFAULT_LOOKAT01:
				return RandomSequence(SEQUENCE_BUTTERFLY_LOOKAT01, SEQUENCE_BUTTERFLY_LOOKAT03);
			default:
				return nSequence + 1;
			}
		}},
	{"models/weapons/v_knife_falchion_advanced.mdl", [](int nSequence) -> int
		{
			switch(nSequence)
			{
			case SEQUENCE_DEFAULT_IDLE2:
				return SEQUENCE_FALCHION_IDLE1;
			case SEQUENCE_DEFAULT_HEAVY_MISS1:
				return RandomSequence(SEQUENCE_FALCHION_HEAVY_MISS1, SEQUENCE_FALCHION_HEAVY_MISS1_NOFLIP);
			case SEQUENCE_DEFAULT_LOOKAT01:
				return RandomSequence(SEQUENCE_FALCHION_LOOKAT01, SEQUENCE_FALCHION_LOOKAT02);
			case SEQUENCE_DEFAULT_DRAW:
			case SEQUENCE_DEFAULT_IDLE1:
				return nSequence;
			default:
				return nSequence - 1;
			}
		}},
	{"models/weapons/v_knife_push.mdl", [](int nSequence) -> int
		{
			switch(nSequence)
			{
			case SEQUENCE_DEFAULT_IDLE2:
				return SEQUENCE_DAGGERS_IDLE1;
			case SEQUENCE_DEFAULT_LIGHT_MISS1:
			case SEQUENCE_DEFAULT_LIGHT_MISS2:
				return RandomSequence(SEQUENCE_DAGGERS_LIGHT_MISS1, SEQUENCE_DAGGERS_LIGHT_MISS5);
			case SEQUENCE_DEFAULT_HEAVY_MISS1:
				return RandomSequence(SEQUENCE_DAGGERS_HEAVY_MISS2, SEQUENCE_DAGGERS_HEAVY_MISS1);
			case SEQUENCE_DEFAULT_HEAVY_HIT1:
			case SEQUENCE_DEFAULT_HEAVY_BACKSTAB:
			case SEQUENCE_DEFAULT_LOOKAT01:
				return nSequence + 3;
			case SEQUENCE_DEFAULT_DRAW:
			case SEQUENCE_DEFAULT_IDLE1:
				return nSequence;
			default:
				return nSequence + 2;
			}
		}},
	{"models/weapons/v_knife_survival_bowie.mdl", [](int nSequence) -> int
		{
			switch(nSequence)
			{
			case SEQUENCE_DEFAULT_DRAW:
			case SEQUENCE_DEFAULT_IDLE1:
				return nSequence;
			case SEQUENCE_DEFAULT_IDLE2:
				return SEQUENCE_BOWIE_IDLE1;
			default:
				return nSequence - 1;
			}
		}}
};

// Replacement function that will be called when the view model animation sequence changes.
void __cdecl hooks::SequenceProxyFn(const CRecvProxyData* proxy_data_const, void* entity, void* output)
{
	static auto fnSequenceProxy = g_SequenceHook->GetOriginalFunction();

	auto pLocal = static_cast<C_BasePlayer*>(g_pEntityList->GetClientEntity(g_pEngine->GetLocalPlayer()));

	if(!pLocal || pLocal->GetLifeState() != LifeState::ALIVE)
		return fnSequenceProxy(proxy_data_const, entity, output);

	// Remove the constness from the proxy data allowing us to make changes.
	auto pData = const_cast<CRecvProxyData*>(proxy_data_const);
	
	auto pViewModel = static_cast<C_BaseViewModel*>(entity);

	if(pViewModel && pViewModel->GetOwner())
	{
		if(pViewModel->GetOwner() == INVALID_EHANDLE_INDEX)
			return fnSequenceProxy(proxy_data_const, entity, output);

		auto pOwner = static_cast<C_BasePlayer*>(g_pEntityList->GetClientEntityFromHandle(pViewModel->GetOwner()));

		if(pOwner == g_pEntityList->GetClientEntity(g_pEngine->GetLocalPlayer()))
		{
			// Get the filename of the current view model.
			auto pKnifeModel = g_pModelInfo->GetModel(pViewModel->GetModelIndex());
			auto szModel = g_pModelInfo->GetModelName(pKnifeModel);

			if(mapAnimFixes.count(szModel))
				pData->m_Value.m_Int = mapAnimFixes.at(szModel)(pData->m_Value.m_Int);
		}
	}

	// Call the original function with our edited data.
	fnSequenceProxy(pData, entity, output);
}
