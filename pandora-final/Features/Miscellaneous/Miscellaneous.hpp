#pragma once
#include "../../source.hpp"

class Miscellaneous {
public:
	float m_flThirdpersonTransparency;

	void ThirdPerson( );
	void Modulation( );
	void SkyBoxChanger( );
	void PreserveKillfeed( );
	void RemoveSmoke( );
	void ForceCrosshair( );
private:
	Color_f m_WallsColor = Color_f( 1.0f, 1.0f, 1.0f, 1.0f );
	Color_f m_PropsColor = Color_f( 1.0f, 1.0f, 1.0f, 1.0f );
};

extern Miscellaneous g_Misc;