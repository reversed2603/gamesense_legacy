#pragma once
#include "../../SDK/sdk.hpp"
#include "LagCompensation.hpp"

class AlternativeAttack {
public:
	void RunAttacks( bool* sendPacket, Encrypted_t<CUserCmd> pCmd );

private:
	struct AlternativeAttackData {
		C_CSPlayer* m_pCurrentTarget = nullptr;
		C_CSPlayer* m_pLocalPlayer = nullptr;
		C_WeaponCSBaseGun* m_pLocalWeapon = nullptr;
		Encrypted_t<CCSWeaponInfo> m_pWeaponInfo = nullptr;
		Encrypted_t<CUserCmd> m_pCmd = nullptr;
		Vector m_vecEyePos;
	};

	bool HandleZeusBot( C_CSPlayer* pPlayer, bool* sendPacket );
	bool HandleKnifeBot( C_CSPlayer* pPlayer, bool* sendPacket );
	
	int GetMinimalHp( );
	int DeterminateHitType( bool stabType, Vector eyeDelta );

	bool IsBehind( C_CSPlayer* pPlayer );

	AlternativeAttackData m_AlternativeAttackData;
};

extern AlternativeAttack g_AlternativeAttack;