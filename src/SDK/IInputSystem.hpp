#pragma once

namespace sdk
{
	class IInputSystem
	{
	public:
		void EnableInput(bool bEnable)
		{
			return get_vfunc<void(__thiscall *)(IInputSystem*, bool)>(this, 11)(this, bEnable);
		}

		void*& get_window()
		{
			static uint32_t offset = 0;
			if (!offset)
				offset = *reinterpret_cast<uint32_t*>((*reinterpret_cast<char***>(this))[10] + 5);
			return *reinterpret_cast<void**>(reinterpret_cast<char*>(this) + offset);
		}
	};

}