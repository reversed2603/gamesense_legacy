#pragma once

#include "../../SDK/sdk.hpp"

class BeamEffects {
public:
	struct BeamEffectsInfo_t {
		float m_flExpTime;
		Vector m_vecStartPos;
		Vector m_vecHitPos;
		Color m_cColor;
		int m_nIndex;
		int m_nTickBase;
		bool ignore;
		bool m_bRing;
	};

	void Draw( );

	void Add( BeamEffectsInfo_t beamEffect );

	std::vector<BeamEffectsInfo_t> m_vecBeamEffects;
};

extern BeamEffects g_BeamEffects;