#pragma once
#include "Platform.hpp"

using CreateInterfaceFn = void* (*)(const char*, int*);

template <class T>
T* CaptureInterface(const char* szModule, const char* szName)
{
	return reinterpret_cast<T*>(platform::CaptureInterface(szModule, szName));
}
