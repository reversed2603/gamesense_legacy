#include "../sdk.hpp"

const char* IPanel::GetName(VPANEL vguiPanel)
{
	using Fn = const char* (__thiscall*)(void*, VPANEL);
	return Memory::VCall<Fn>(this, Index::IPanel::GetName)(this, vguiPanel);
}
