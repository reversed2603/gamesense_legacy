#pragma once

class IClientEntityList
{
public:
	IClientEntity* GetClientEntity(int entnum);
	IClientEntity* GetClientEntityFromHandle(CBaseHandle hEnt);
	int GetHighestEntityIndex();
};
