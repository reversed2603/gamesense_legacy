#pragma once
#include "LagCompensation.hpp"
#include <vector>
#include <deque>

enum EResolverStages {
	RESOLVE_MODE_NONE, // not currently resolving
	RESOLVE_MODE_STAND,
	RESOLVE_MODE_MOVE,
	RESOLVE_MODE_AIR,
	RESOLVE_MODE_PRED,
	RESOLVE_MODE_OVERRIDE
};

struct ResolverData_t {
	void ResetMoveData( ) {
		if( !m_vecLastMoveOrigin.IsZero( ) )
			m_vecLastMoveOrigin.Init( );

		if( m_flLastMoveBody != FLT_MAX )
			m_flLastMoveBody = FLT_MAX;

		if( m_flLastMoveAnimTime != FLT_MAX )
			m_flLastMoveAnimTime = FLT_MAX;
	}

	void ResetPredData( ) {
		if( m_flLowerBodyRealignTimer != FLT_MAX )
			m_flLowerBodyRealignTimer = FLT_MAX;

		if( m_flAnimationTime != FLT_MAX )
			m_flAnimationTime = FLT_MAX;

		if( m_flPreviousLowerBodyYaw != FLT_MAX )
			m_flPreviousLowerBodyYaw = FLT_MAX;

		if( m_bInPredictionStage != false )
			m_bInPredictionStage = false;

		if( m_iDetectedBodyUpdate != 0 )
			m_iDetectedBodyUpdate = 0;
	}

	void Reset( ) {
		m_iMissedShots = m_iMissedShotsLBY = m_iMissedShotsSpread = 0;
		m_eCurrentStage = RESOLVE_MODE_NONE;
		ResetMoveData( );
		ResetPredData( );
	}

	EResolverStages m_eCurrentStage;

	int m_iMissedShots;
	int m_iMissedShotsSpread;
	int m_iMissedShotsLBY;

	bool m_bIsOverriding = false;

	// last move data
	Vector m_vecLastMoveOrigin = { 0, 0, 0 };
	float m_flLastMoveBody = FLT_MAX;
	float m_flLastMoveAnimTime = FLT_MAX;

	// lby pred data
	float m_flPreviousLowerBodyYaw = FLT_MAX;
	int m_flLastLbyActivity = 979;
	int m_iDetectedBodyUpdate = 0;
	bool m_bInPredictionStage = false;
	float m_flLowerBodyRealignTimer = FLT_MAX;
	float m_flAnimationTime = FLT_MAX;
};

class Resolver {
	void UpdateLBYPrediction( AnimationRecord* pRecord );
	void UpdateResolverStage( AnimationRecord* pRecord );

	void ResolveStand( AnimationRecord* pRecord );
	void ResolveMove( AnimationRecord* pRecord );
	void ResolveAir( AnimationRecord* pRecord );
	void ResolvePred( AnimationRecord* pRecord );
	void ResolveBrute( AnimationRecord* pRecord );
	void ResolveOverride( AnimationRecord* pRecord );
	float ResolveAntiFreestand( AnimationRecord* pRecord );
public:
	void ResolvePlayers( AnimationRecord* pRecord );
	std::array< ResolverData_t, 65 > m_arrResolverData;
};

extern Resolver g_Resolver;