#pragma once
#include "Platform.hpp"
#include <cstring>

class VMTHook
{
public:
	VMTHook(void* class_base)
	{
		m_class = reinterpret_cast<void***>(class_base);
		m_old_vmt = *m_class;

		size_t size = 0;
		while(m_old_vmt[size] && platform::IsCodePtr(m_old_vmt[size]))
			++size;

		m_new_vmt = new void*[size];
		memcpy(m_new_vmt, m_old_vmt, sizeof(void*) * size);
		*m_class = m_new_vmt;
	}

	~VMTHook()
	{
		*m_class = m_old_vmt;
		delete[] m_new_vmt;
	}

	void* HookFunction(void* hooked_fn, size_t index)
	{
		m_new_vmt[index] = hooked_fn;
		return m_old_vmt[index];
	}

	template <typename Fn = uintptr_t>
	Fn GetOriginalFunction(int index) { return reinterpret_cast<Fn>(m_old_vmt[index]); }

private:
	void*** m_class = nullptr;
	void** m_new_vmt = nullptr;
	void** m_old_vmt = nullptr;
};
