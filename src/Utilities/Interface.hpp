#pragma once
#include "Platform.hpp"

using CreateInterfaceFn = void* (*)(const char*, int*);

template <class T>
T* CaptureInterface(const char* module, const char* name)
{
	return reinterpret_cast<T*>(platform::CaptureInterface(module, name));
}
