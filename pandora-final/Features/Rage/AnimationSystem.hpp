#pragma once
#include "../../SDK/Classes/Player.hpp"
#include "../../SDK/Valve/CBaseHandle.hpp"

#include <map>
#include <deque>

// forward declaration
class CServerSetupBones;

enum ELagRecordFlags {
	RF_None = ( 1 << 1 ),
	RF_BrokeTeleportDst = ( 1 << 1 ),
	RF_LBYFlicked = ( 1 << 2 ),
	RF_IsInvalid = ( 1 << 3 ),
	RF_IsResolved = ( 1 << 4 ),
	RF_FakeWalking = ( 1 << 5 ),
	RF_Shot = (1 << 6),
	RF_ShotNoPSilent = (1 << 7),
};

struct SimulationRestore {
	int m_fFlags;
	float m_flDuckAmount;
	float m_flFeetCycle;
	float m_flMoveWeight;
	QAngle m_angEyeAngles;
	Vector m_vecOrigin;
};

class AnimationRecord {
public:
	C_CSPlayer* m_pEntity;

	int m_AnimationFlags;

	Vector m_vecOrigin;
	Vector m_vecVelocity;
	Vector m_vecAbsVelocity;
	QAngle m_angEyeAngles;

	int m_fFlags;
	int m_iChokeTicks;

	float m_flChokeTime;
	float m_flSimulationTime;
	float m_flAnimationTime;
	float m_flLowerBodyYawTarget;
	float m_flShotTime;
	float m_flDuckAmount;
	float m_flDelta;

	float m_flMoveWeight;
	float m_flFeetCycle;

	alignas( 16 ) matrix3x4_t m_pMatrix[128];

	std::array<C_AnimationLayer, 13> m_pServerAnimOverlays;
};

class AnimationData {
public:
	void Update( );
	void CorrectVelocity(Encrypted_t<C_CSPlayer> m_player, Encrypted_t<AnimationRecord> m_record, Encrypted_t<AnimationRecord> m_previous, Encrypted_t< AnimationRecord> m_pre_previous);
	void Collect( C_CSPlayer* pPlayer );

	void UpdatePlayer( C_CSPlayer* pEntity, AnimationRecord* pRecord );
	void SimulateMovement( C_CSPlayer* pEntity, Vector& vecOrigin, Vector& vecVelocity, int& fFlags, bool bOnGround );
	void SimulateAnimations( AnimationRecord* current, AnimationRecord* previous );

	C_CSPlayer* player;

	float m_flSimulationTime;
	float m_flOldSimulationTime;
	float m_flLastNonShotPitch;

	struct 
	{
		float m_flCycle;
		float m_flPlaybackRate;
	} last_alive_loop_data;

	bool m_bWasDormant = false;
	bool m_bUpdated = false;

	bool m_bIsAlive = false;

	std::deque<AnimationRecord> m_AnimationRecord;
};

class AnimationSystem {
public:
	void FrameStage( );
	AnimationData* GetAnimationData( int index );

	bool m_bInBoneSetup[ 65 ];
private:
	std::map<int, AnimationData> m_AnimatedEntities = { };
};

extern AnimationSystem g_AnimationSystem;