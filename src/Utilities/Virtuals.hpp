#pragma once

template <typename Fn = void*>
Fn GetVirtualFunction(void* class_base, size_t index)
{
	return (*reinterpret_cast<Fn**>(class_base))[index];
}
