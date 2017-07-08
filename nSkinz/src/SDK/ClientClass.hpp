#pragma once

class IClientNetworkable;
class ClientClass;
class RecvTable;

using CreateClientClassFn = IClientNetworkable* (*)(int, int);
using CreateEventFn = IClientNetworkable* (*)();

class ClientClass
{
public:
	CreateClientClassFn m_pCreateFn;
	CreateEventFn m_pCreateEventFn;
	const char* m_pNetworkName;
	RecvTable* m_pRecvTable;
	ClientClass* m_pNext;
	int m_ClassID;
};
