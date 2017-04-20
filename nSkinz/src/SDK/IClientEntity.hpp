#pragma once

struct bf_read;
using CBaseHandle = unsigned long;

constexpr auto INVALID_EHANDLE_INDEX = 0xFFFFFFFF;

class IHandleEntity
{
public:
	virtual ~IHandleEntity() {};
};

class IClientUnknown: public IHandleEntity
{
public:
	virtual ~IClientUnknown() {};
};

class IClientThinkable
{
public:
	virtual ~IClientThinkable() {};
};

class IClientRenderable
{
public:
	virtual ~IClientRenderable() {};
};

class IClientNetworkable
{
public:
	virtual IClientUnknown*	GetIClientUnknown() = 0;
	virtual void			Release() = 0;
	virtual ClientClass*	GetClientClass() = 0;
	virtual void			NotifyShouldTransmit(int state) = 0;
	virtual void			OnPreDataChanged(int updateType) = 0;
	virtual void			OnDataChanged(int updateType) = 0;
	virtual void			PreDataUpdate(int updateType) = 0;
	virtual void			PostDataUpdate(int updateType) = 0;
	virtual void			__pad8() = 0;
	virtual bool			IsDormant() = 0;
	virtual int				GetIndex() const = 0;
	virtual void			ReceiveMessage(int classID, bf_read& msg) = 0;
	virtual void*			GetDataTableBasePtr() = 0;
	virtual void			SetDestroyedOnRecreateEntities() = 0;
};

class IClientEntity: public IClientUnknown, public IClientRenderable, public IClientNetworkable, public IClientThinkable
{
public:
	virtual ~IClientEntity() {};
};
