#pragma once

template <typename Fn = void*>
Fn GetVirtualFunction(void* pClassBase, size_t nIndex)
{
	return (*reinterpret_cast<Fn**>(pClassBase))[nIndex];
}
