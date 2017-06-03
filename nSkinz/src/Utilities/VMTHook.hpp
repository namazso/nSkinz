#pragma once
#include "Platform.hpp"
#include <cstring>

class VMTHook
{
public:
	VMTHook(void* pClass)
	{
		m_pClass = reinterpret_cast<void***>(pClass);
		m_pOldVMT = *m_pClass;

		m_iSize = 0;
		while(m_pOldVMT[m_iSize] && platform::IsCodePtr(m_pOldVMT[m_iSize]))
			++m_iSize;

		m_pNewVMT = new void*[m_iSize];
		memcpy(m_pNewVMT, m_pOldVMT, sizeof(void*) * m_iSize);
		*m_pClass = m_pNewVMT;
	}

	~VMTHook()
	{
		*m_pClass = m_pOldVMT;
		delete[] m_pNewVMT;
	}

	void* HookFunction(void* pfn, size_t iIndex) const
	{
		if(iIndex <= m_iSize)
		{
			m_pNewVMT[iIndex] = pfn;
			return m_pOldVMT[iIndex];
		}
		return nullptr;
	}

	template <typename Fn>
	Fn GetOriginalFunction(int iIndex) { return reinterpret_cast<Fn>(m_pOldVMT[iIndex]); }

private:
	void*** m_pClass = nullptr;
	void** m_pNewVMT = nullptr;
	void** m_pOldVMT = nullptr;
	size_t m_iSize = 0;
};
