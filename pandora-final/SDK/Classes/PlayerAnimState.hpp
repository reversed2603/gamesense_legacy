#pragma once
#include "../sdk.hpp"

class C_CSPlayer;
class C_WeaponCSBaseGun;

struct animstate_pose_param_cache_t {
	bool valid = false;
	int index = -1;
	const char* name;
	void SetValue( C_CSPlayer* player, float flValue );
};

struct procedural_foot_t
{
	Vector m_vecPosAnim;
	Vector m_vecPosAnimLast;
	Vector m_vecPosPlant;
	Vector m_vecPlantVel;
	float m_flLockAmount;
	float m_flLastPlantTime;

	procedural_foot_t( )
	{
		m_vecPosAnim.Init( );
		m_vecPosAnimLast.Init( );
		m_vecPosPlant.Init( );
		m_vecPlantVel.Init( );
		m_flLockAmount = 0;
		m_flLastPlantTime = 0.f;// gpGlobals->curtime;
	}

	void Init( Vector vecNew )
	{
		m_vecPosAnim = vecNew;
		m_vecPosAnimLast = vecNew;
		m_vecPosPlant = vecNew;
		m_vecPlantVel.Init( );
		m_flLockAmount = 0;
		m_flLastPlantTime = 0.f;// gpGlobals->curtime;
	}
};

struct aimmatrix_transition_t
{
	float	m_flDurationStateHasBeenValid;
	float	m_flDurationStateHasBeenInValid;
	float	m_flHowLongToWaitUntilTransitionCanBlendIn;
	float	m_flHowLongToWaitUntilTransitionCanBlendOut;
	float	m_flBlendValue;

	void UpdateTransitionState( bool bStateShouldBeValid, float flTimeInterval, float flSpeed )
	{
		if( bStateShouldBeValid )
		{
			m_flDurationStateHasBeenInValid = 0;
			m_flDurationStateHasBeenValid += flTimeInterval;
			if( m_flDurationStateHasBeenValid >= m_flHowLongToWaitUntilTransitionCanBlendIn )
			{
				m_flBlendValue = Math::Approach( 1, m_flBlendValue, flSpeed );
			}
		}
		else
		{
			m_flDurationStateHasBeenValid = 0;
			m_flDurationStateHasBeenInValid += flTimeInterval;
			if( m_flDurationStateHasBeenInValid >= m_flHowLongToWaitUntilTransitionCanBlendOut )
			{
				m_flBlendValue = Math::Approach( 0, m_flBlendValue, flSpeed );
			}
		}
	}

	void Init( void )
	{
		m_flDurationStateHasBeenValid = 0;
		m_flDurationStateHasBeenInValid = 0;
		m_flHowLongToWaitUntilTransitionCanBlendIn = 0.3f;
		m_flHowLongToWaitUntilTransitionCanBlendOut = 0.3f;
		m_flBlendValue = 0;
	}

	aimmatrix_transition_t( )
	{
		Init( );
	}
};

typedef const int* animlayerpreset;

// class created thanks to cs:go source leak :-)
class CCSGOPlayerAnimState {
public:
	animlayerpreset			m_pLayerOrderPreset;

	bool					m_bFirstRunSinceInit;

	bool					m_bFirstFootPlantSinceInit;
	int						m_iLastUpdateFrame;

	float					m_flEyePositionSmoothLerp;

	float					m_flStrafeChangeWeightSmoothFalloff;

	aimmatrix_transition_t	m_tStandWalkAim;
	aimmatrix_transition_t	m_tStandRunAim;
	aimmatrix_transition_t	m_tCrouchWalkAim;

	int						m_cachedModelIndex;

	float					m_flStepHeightLeft;
	float					m_flStepHeightRight;

	void* m_pWeaponLastBoneSetup;

	C_CSPlayer* m_pPlayer;
	void* m_pWeapon;
	void* m_pWeaponLast;

	float					m_flLastUpdateTime;
	int						m_nLastUpdateFrame;
	float					m_flLastUpdateIncrement;

	float					m_flEyeYaw;
	float					m_flEyePitch;
	float					m_flFootYaw;
	float					m_flFootYawLast;
	float					m_flMoveYaw;
	float					m_flMoveYawIdeal;
	float					m_flMoveYawCurrentToIdeal;
	float					m_flTimeToAlignLowerBody;

	float					m_flPrimaryCycle;
	float					m_flMoveWeight;
	float					m_flMoveWeightSmoothed;
	float					m_flAnimDuckAmount;
	float					m_flDuckAdditional;
	float					m_flRecrouchWeight;

	Vector					m_vecPositionCurrent;
	Vector					m_vecPositionLast;

	Vector					m_vecVelocity;
	Vector					m_vecVelocityNormalized;
	Vector					m_vecVelocityNormalizedNonZero;
	float					m_flVelocityLengthXY;
	float					m_flVelocityLengthZ;

	float					m_flSpeedAsPortionOfRunTopSpeed;
	float					m_flSpeedAsPortionOfWalkTopSpeed;
	float					m_flSpeedAsPortionOfCrouchTopSpeed;

	float					m_flDurationMoving;
	float					m_flDurationStill;

	bool					m_bOnGround;

	bool					m_bLanding;
	float					m_flJumpToFall;
	float					m_flDurationInAir;
	float					m_flLeftGroundHeight;
	float					m_flLandAnimMultiplier;

	float					m_flWalkToRunTransition;

	bool					m_bLandedOnGroundThisFrame;
	bool					m_bLeftTheGroundThisFrame;
	float					m_flInAirSmoothValue;

	bool					m_bOnLadder;
	float					m_flLadderWeight;
	float					m_flLadderSpeed;

	bool					m_bWalkToRunTransitionState;

	bool					m_bDefuseStarted;
	bool					m_bPlantAnimStarted;
	bool					m_bTwitchAnimStarted;
	bool					m_bAdjustStarted;

	CUtlVector< uint16_t >	m_ActivityModifiers;

	float					m_flNextTwitchTime;

	float					m_flTimeOfLastKnownInjury;

	float					m_flLastVelocityTestTime;
	Vector					m_vecVelocityLast;
	Vector					m_vecTargetAcceleration;
	Vector					m_vecAcceleration;
	float					m_flAccelerationWeight;

	float					m_flAimMatrixTransition;
	float					m_flAimMatrixTransitionDelay;

	bool					m_bFlashed;

	float					m_flStrafeChangeWeight;
	float					m_flStrafeChangeTargetWeight;
	float					m_flStrafeChangeCycle;
	int						m_nStrafeSequence;
	bool					m_bStrafeChanging;
	float					m_flDurationStrafing;

	float					m_flFootLerp;

	bool					m_bFeetCrossed;

	bool					m_bPlayerIsAccelerating;

	animstate_pose_param_cache_t m_tPoseParamMappings[ 20 ];

	float					m_flDurationMoveWeightIsTooHigh;
	float					m_flStaticApproachSpeed;

	int						m_nPreviousMoveState;
	float					m_flStutterStep;

	float					m_flActionWeightBiasRemainder;

	procedural_foot_t		m_footLeft;
	procedural_foot_t		m_footRight;

	float					m_flCameraSmoothHeight;
	bool					m_bSmoothHeightValid;
	float					m_flLastTimeVelocityOverTen;

	float					m_flAimYawMin;
	float					m_flAimYawMax;
	float					m_flAimPitchMin;
	float					m_flAimPitchMax;

	//float					m_flMoveWalkWeight;
	//float					m_flMoveCrouchWalkWeight;
	//float					m_flMoveRunWeight;

	int						m_nAnimstateModelVersion;

	void Reset( );
	void Update( QAngle angles );
	float GetMaxFraction( );
	float GetDesyncDelta( bool useMinYaw = false );
	int GetWeightedSequenceFromActivity( int activity );
	const char* GetWeaponPrefix( );
};//Size=0x344
