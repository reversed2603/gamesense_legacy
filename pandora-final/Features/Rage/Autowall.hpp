#pragma once
#include "../../source.hpp"

namespace Autowall
{
	struct C_FireBulletData {
		C_FireBulletData( ) = default;
		~C_FireBulletData( ) = default;

		Vector m_vecStart = Vector( 0, 0, 0 );
		Vector m_vecDirection = Vector( 0, 0, 0 );
		Vector m_vecPos = Vector( 0, 0, 0 );

		CGameTrace m_EnterTrace;

		int m_iPenetrationCount = 4;
		int m_iHitgroup = -1;

		float m_flTraceLength = 0.f;
		float m_flCurrentDamage = -1.f;

		// distance to point
		float m_flMaxLength = 0.0f;
		float m_flPenetrationDistance = 0.0f;

		// should penetrate walls? 
		bool m_bPenetration = false;

		bool m_bShouldIgnoreDistance = false;

		ITraceFilter* m_Filter = nullptr; // TODO: implement
		C_CSPlayer* m_Player = nullptr; // attacker
		C_CSPlayer* m_TargetPlayer = nullptr;  // autowall target ( could be nullptr if just trace attack )
		C_WeaponCSBaseGun* m_Weapon = nullptr; // attacker weapon
		CCSWeaponInfo* m_WeaponData = nullptr;

		surfacedata_t* m_EnterSurfaceData = nullptr;
	};

	bool IsBreakable( C_BaseEntity* entity );
	bool IsArmored( C_CSPlayer* player, int hitgroup );
	float ScaleDamage( C_CSPlayer* player, float damage, float armor_ratio, int hitgroup );
	void TraceLine( const Vector& start, const Vector& end, uint32_t mask, ITraceFilter* ignore, CGameTrace* ptr );
	void ClipTraceToPlayers( const Vector& vecAbsStart, const Vector& vecAbsEnd, uint32_t mask, ITraceFilter* filter, CGameTrace* tr, float flMaxRange = 60.0f, float flMinRange = 0.0f );
	void ClipTraceToPlayer( const Vector vecAbsStart, const Vector& vecAbsEnd, uint32_t mask, ITraceFilter* filter, CGameTrace* tr, C_FireBulletData* data );
	bool TraceToExit( CGameTrace* enter_trace, Vector start, Vector direction, CGameTrace* exit_trace );
	bool HandleBulletPenetration( C_FireBulletData* data );
	float FireBullets( C_FireBulletData* data );
}
