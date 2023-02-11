#pragma once
#include "LagCompensation.hpp"
#include <vector>
#include <deque>

struct ShotInformation_t {
	int m_iTargetIndex;
	int m_iPredictedDamage;
	float m_flTime;
	C_LagRecord m_pRecord;

	bool m_bTapShot = false;
	bool m_bMatched = false;
	bool m_bHadPredError = false;
	bool m_bWasLBYFlick = false;

	mstudiobbox_t* m_pHitbox = nullptr;

	Vector m_vecStart;
	Vector m_vecEnd;
};

struct ShotEvents_t {
	struct player_hurt_t {
		int m_iDamage;
		int m_iHitgroup;
		int m_iTargetIndex;
	};

	std::vector<Vector> m_vecImpacts;
	std::vector<player_hurt_t> m_vecDamage;

	ShotInformation_t m_ShotData;
};

class ShotHandling {
public:
	void ProcessShots( );
	void RegisterShot( ShotInformation_t& shot );
	void GameEvent( IGameEvent* pEvent );

	bool TraceShot( ShotEvents_t* shot );

	std::vector<ShotEvents_t> m_vecShots;
	std::vector<ShotEvents_t> m_vecFilteredShots;

	bool m_bGotEvents;
};

extern ShotHandling g_ShotHandling;