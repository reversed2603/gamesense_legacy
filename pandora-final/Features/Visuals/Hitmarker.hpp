#pragma once

#include "../../SDK/sdk.hpp"

struct impact_t {
	float x, y, z;
	float time;
};

struct hitmarker_t {
	impact_t impact;
	int damage;
	float time, alpha, distance;
};

class Hitmarker {
	void PlayerHurt( IGameEvent* pEvent );
	void BulletImpact( IGameEvent* pEvent );

	std::vector<impact_t> m_vecImpacts;
	std::vector<hitmarker_t> m_vecHitmarkers;

	bool m_bFirstMarker;
	float m_flRandomRotation;
	float m_flRandomEnlargement;
public:
	float m_flMarkerAlpha;
	Color m_uMarkerColor;

public:
	void Draw( );
	void GameEvent( IGameEvent* pEvent );
};

extern Hitmarker g_Hitmarker;