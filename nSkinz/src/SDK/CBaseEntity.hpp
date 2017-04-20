#pragma once
#include "IClientEntity.hpp"
#include "../Utilities/NetVarManager.hpp"

enum class LifeState
{
	ALIVE = 0,
	DYING,
	DEAD,
	RESPAWNABLE,
	DISCARDBODY,
};

class C_BaseEntity: public IClientEntity
{
public:
	NETVAR(GetModelIndex, int, "CBaseEntity->m_nModelIndex")
};

class C_BaseCombatCharacter : public C_BaseEntity
{
public:
	PNETVAR(GetWeapons, CBaseHandle, "CBaseCombatCharacter->m_hMyWeapons")
};

class C_BasePlayer: public C_BaseCombatCharacter
{
public:
	NETVAR(GetLifeState, LifeState, "CBasePlayer->m_lifeState")
	NETVAR(GetViewModel, CBaseHandle, "CBasePlayer->m_hViewModel[0]")
};

class C_BaseCombatWeapon: public C_BaseEntity
{
public:
	NETVAR(GetViewModelIndex, int, "CBaseCombatWeapon->m_iViewModelIndex")
};

class C_BaseAttributableItem: public C_BaseCombatWeapon
{
public:
	NETVAR(GetAccountID, unsigned, "CBaseAttributableItem->m_iAccountID")
	NETVAR(GetItemDefinitionIndex, unsigned, "CBaseAttributableItem->m_iItemDefinitionIndex")
	NETVAR(GetItemIDHigh, unsigned, "CBaseAttributableItem->m_iItemIDHigh")
	NETVAR(GetEntityQuality, int, "CBaseAttributableItem->m_iEntityQuality")
	PNETVAR(GetCustomName, char, "CBaseAttributableItem->m_szCustomName")
	NETVAR(GetFallbackPaintKit, unsigned, "CBaseAttributableItem->m_nFallbackPaintKit")
	NETVAR(GetFallbackSeed, unsigned, "CBaseAttributableItem->m_nFallbackSeed")
	NETVAR(GetFallbackWear, float, "CBaseAttributableItem->m_flFallbackWear")
	NETVAR(GetFallbackStatTrak, int, "CBaseAttributableItem->m_nFallbackStatTrak")
};

class C_BaseViewModel: public C_BaseEntity
{
public:
	NETVAR(GetOwner, CBaseHandle, "CBaseViewModel->m_hOwner")
	NETVAR(GetWeapon, int, "CBaseViewModel->m_hWeapon")
	NETPROP(GetSequenceProp, "CBaseViewModel->m_nSequence")
};
