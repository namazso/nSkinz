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
	};

}