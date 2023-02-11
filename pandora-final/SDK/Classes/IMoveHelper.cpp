#include "../sdk.hpp"

void IMoveHelper::SetHost(C_BaseEntity* host)
{
	using Fn = void(__thiscall*)(void*, C_BaseEntity*);
	return Memory::VCall<Fn>(this, Index::IMoveHelper::SetHost)(this, host);
}

void IMoveHelper::ProcessImpacts( ) {
  using Fn = void(__thiscall*)(void*);
  return Memory::VCall<Fn>(this, Index::IMoveHelper::ProccesImpacts)(this);
}
