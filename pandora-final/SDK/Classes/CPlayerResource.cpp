#include "CPlayerResource.hpp"
#include "../Displacement.hpp"

int CSPlayerResource::GetPlayerPing(int idx) {
	static auto m_iPing = Engine::g_PropManager.GetOffset(XorStr("DT_PlayerResource"), XorStr("m_iPing"));
	return *(int*)((uintptr_t)this + m_iPing + idx * 4);
}

int CSPlayerResource::GetPlayerAssists(int idx) {
	static auto m_iAssists = Engine::g_PropManager.GetOffset(XorStr("DT_PlayerResource"), XorStr("m_iAssists"));
	return *(int*)((uintptr_t)this + m_iAssists + idx * 4);
}

int CSPlayerResource::GetPlayerKills(int idx) {
	static auto m_iKills = Engine::g_PropManager.GetOffset(XorStr("DT_PlayerResource"), XorStr("m_iKills"));
	return *(int*)((uintptr_t)this + m_iKills + idx * 4);
}

int CSPlayerResource::GetPlayerDeaths(int idx) {
	static auto m_iDeaths = Engine::g_PropManager.GetOffset(XorStr("DT_PlayerResource"), XorStr("m_iDeaths"));
	return *(int*)((uintptr_t)this + m_iDeaths + idx * 4);
}

Vector& CSPlayerResource::m_bombsiteCenterA( ) {
	return *( Vector* )( ( uintptr_t )this + Engine::Displacement.DT_CSPlayerResource.m_bombsiteCenterA );
}

Vector& CSPlayerResource::m_bombsiteCenterB( ) {
	return *( Vector* )( ( uintptr_t )this + Engine::Displacement.DT_CSPlayerResource.m_bombsiteCenterB );
}
