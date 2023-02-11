#pragma once
#include "../../SDK/sdk.hpp"

class AdaptiveAngle {
public:
	float m_yaw;
	float m_dist;

public:
	// ctor.
	__forceinline AdaptiveAngle(float yaw, float penalty = 0.f) {
		// set yaw.
		m_yaw = Math::AngleNormalize(yaw);

		// init distance.
		m_dist = 0.f;

		// remove penalty.
		m_dist -= penalty;
	}
};

class AntiAim {
	bool ShouldAntiAim( CUserCmd* pCmd );

	void HandlePitch( CUserCmd* pCmd );
	void HandleBaseYaws( CUserCmd* pCmd );
	void HandleYaw( CUserCmd* pCmd );
	void HandleManualAA( CUserCmd* pCmd );

	void HandleAntiAimOptions(CUserCmd* pCmd);

	C_CSPlayer* GetBestPlayer( CUserCmd* pCmd, bool distance = false );

	enum class Directions : int {
		YAW_RIGHT = -1,
		YAW_BACK,
		YAW_LEFT,
		YAW_NONE
	};

	void HandleFreestand( CUserCmd* pCmd );
	std::pair<bool, float> HandleEdge( );

	Directions m_CurrentDirection = Directions::YAW_NONE;
	float m_flLastMoveYaw;
	float m_flLastRealAngle;

	bool m_bLBYDirectionSwitch;
	bool m_bJitterUpdate;

	bool m_bEdging, m_bFreestanding;

public:
	void Prepare( bool* bSendPacket, CUserCmd* pCmd );
	void HandleAntiAim( bool* bSendPacket, bool* bFinalTick, CUserCmd* pCmd );

	QAngle angViewangle;
};

extern AntiAim g_AntiAim;