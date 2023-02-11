#pragma once
#include "../../SDK/sdk.hpp"
#include "../../SDK/Displacement.hpp"

#include "../../SDK/Classes/weapon.hpp"
#include "../../SDK/Classes/Player.hpp"

#include <map>
#include <deque>
#include "AnimationSystem.hpp"

class C_LagRecord {
public:
	C_CSPlayer* player = nullptr;
	Vector m_vecMins, m_vecMaxs;
	Vector m_vecOrigin;

	QAngle m_angEyeAngles;
	QAngle m_absAngles;

	float m_flSimulationTime;

	float m_flServerLatency;
	float m_flRealTime;
	float m_flLastShotTime;
	float m_flAnimationVelocity;

	int m_RecordFlags;
	/*
		bool m_bBrokeTeleportDst;
		bool m_bLBYFlicked;
		bool m_bIsValid;
		bool m_bIsResolved;
	*/

	int m_iFlags;
	int m_iLaggedTicks = 0;

	float m_flInterpolateTime = 0.f;

	Vector m_vecVelocity;
	float m_flDuckAmount;

	alignas( 16 ) matrix3x4_t m_pMatrix[ 128 ];

	void Setup( C_CSPlayer* player, bool isBackup = false );
	void Apply( C_CSPlayer* player );
};

class C_EntityLagData {
public:
	static void UpdateRecordData( C_EntityLagData* pThis, C_CSPlayer* player, const player_info_t& info );

	void Clear( );

	std::deque<C_LagRecord> m_Records = { };
	C_LagRecord backup_record = {};
	int m_iUserID = -1;

	float m_flLastUpdateTime = 0.0f;
	float m_flRate = 0.0f;

	int m_iMissedShots = 0;
};

class LagCompensation {
public:
	void Update( );
	bool IsRecordOutOfBounds( const C_LagRecord& record, float target_time = 0.2f, int tickbase_shift = -1, bool tick_count_check = false );
	float GetLerp( );
	void SetupLerpTime();

	C_EntityLagData* GetLagData( int entindex );

	void ClearLagData( );

	void BackupOperation(bool restore = false);

	float m_flOutLatency;
	float m_flServerLatency;

private:
	struct LagCompData {
		std::map< int, C_EntityLagData > m_PlayerHistory;

		float m_flLerpTime, m_flOutLatency, m_flServerLatency;
		bool m_GetEvents = false;
	};

	LagCompData lagData;
};

extern LagCompensation g_LagCompensation;