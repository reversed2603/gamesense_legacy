#pragma once
#include "../../SDK/sdk.hpp"
#include "LagCompensation.hpp"
#include "Autowall.hpp"

struct C_LagRecord;

// forward declaration
struct AimTarget_t;

struct AimPoint_t {
	mstudiobbox_t* m_pHitbox;
	Vector m_vecPosition;

	bool m_bCenter;
	bool m_bLethal;
	bool m_bBody;
	bool m_bHead;
	bool m_bResolved;

	int m_iHitbox;

	int m_iHitchance;
	float m_flPointscale;

	int m_iDamage;
	bool m_bPenetrated;
	int m_iTraceHitgroup;

	// the point must know it's owner/target
	AimTarget_t* m_pTarget = nullptr;

	// every point gets it's own firebulletdata
	Autowall::C_FireBulletData* m_pFireBulletData = nullptr;

	// this for hitbox logic
	bool m_bFirstPoint = false;
};

struct AimTarget_t {
	C_CSPlayer* m_pEntity = nullptr;
	C_LagRecord* m_pRecord = nullptr;

	mstudiohitboxset_t* m_pHitboxSet = nullptr;

	bool m_bHeadshotOnly = false;
	bool m_bHasLethalPoint = false;

	std::vector<AimPoint_t> m_vecAimPoints;
};

class Ragebot {
	struct Aimbot_t {
		C_CSPlayer* m_pLocal;
		CUserCmd* m_pCmd;
		bool* m_pSendPacket;
		Vector m_vecEyePosition;

		C_WeaponCSBaseGun* m_pWeapon;
		CCSWeaponInfo* m_pWeaponInfo;

		CVariables::RAGE* m_pSettings;

		std::vector<AimTarget_t> m_vecAimData;

		bool m_bDidStop;

		bool m_bShouldStop;

		int m_iShotID;
	};

	Aimbot_t m_AimbotInfo;

	static void RunAwall( AimPoint_t* pPoint );

	float CalculateDamageOnPoint(C_LagRecord* pRecord, AimTarget_t& pTarget, Vector vecPoint = Vector( ), bool bCalculatePoint = false, Hitboxes hitbox = Hitboxes::HITBOX_HEAD );

	CVariables::RAGE* GetRageSettings( );
	bool AutoScope( );

	int GetMinimalDamage( AimPoint_t* pPoint );
	C_LagRecord* FindPriorityBacktrackRecord(AimTarget_t* pTarget, int accuracy_boost = 0);
	C_LagRecord* GetBestLagRecord( AimTarget_t* pTarget );
	std::vector<Hitboxes> GetHitboxes( );
	bool RunHitscan( std::vector<Hitboxes> hitboxesToScan = std::vector<Hitboxes>( ) );
	bool FinishAimbot( );

	std::vector<C_CSPlayer*> FindTargets( );
	bool RunHitchance( AimPoint_t* pPoint, float chance );

	void AddPoint( AimTarget_t* pTarget, mstudiobbox_t* pHitbox, Vector vecPosition, int iHitbox, bool center = false );
	void AddPoints( AimTarget_t* pTarget, Hitboxes hitbox );

	bool AimAtPoint( AimPoint_t* pPoint );

	void SetupEyePosition( );
	void CockRevolver( );

	void PostAimbot( AimPoint_t* pPoint );

	bool RunInternal( );
public:
	bool AutoStop(Encrypted_t<CUserCmd> cmd);
	bool Run( bool* bSendPacket, CUserCmd* pCmd );
	
	Vector last_moveangle;
	int last_shot_command = 0;
};

extern Ragebot g_Ragebot;