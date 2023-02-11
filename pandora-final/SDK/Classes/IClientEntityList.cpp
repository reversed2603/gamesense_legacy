#include "../sdk.hpp"
#include "../Valve/CBaseHandle.hpp"

int IClientEntityList::GetHighestEntityIndex()
{
	using Fn = int(__thiscall*)(void*);
	return Memory::VCall<Fn>(this, Index::IClientEntityList::GetHighestEntityIndex)(this);
}

IClientEntity* IClientEntityList::GetClientEntity(int entnum)
{
	using Fn = IClientEntity * (__thiscall*)(void*, int);
	return Memory::VCall<Fn>(this, Index::IClientEntityList::GetClientEntity)(this, entnum); 
}

IClientEntity* IClientEntityList::GetClientEntityFromHandle(CBaseHandle hEnt)
{
	using Fn = IClientEntity * (__thiscall*)(void*, CBaseHandle);
	return Memory::VCall<Fn>(this, Index::IClientEntityList::GetClientEntityFromHandle)(this, hEnt);
}
