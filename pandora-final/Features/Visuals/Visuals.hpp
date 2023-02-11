#pragma once
#include "../../SDK/Classes/Player.hpp"

class ExtendedVisuals {
private:
	void NormalizeSound( C_CSPlayer* player, SndInfo_t& sound );
	bool ValidateSound( SndInfo_t& sound );

	CUtlVector<SndInfo_t> m_vecSoundBuffer;
public:
	struct SoundPlayer {
		void Override( SndInfo_t& sound ) {
			m_iIndex = sound.m_nSoundSource;
			m_iReceiveTime = GetTickCount( );
		}

		int m_iIndex = 0;
		int m_iReceiveTime = 0;
		Vector m_vecOrigin = Vector( 0, 0, 0 );
		Vector m_vecLastOrigin = Vector( 0, 0, 0 );

		int m_nFlags = 0;
		Vector m_vecAbsOrigin = Vector( 0, 0, 0 );
		bool m_bValidSound = false;
	};

	struct SpectatorDormancy_t {
		void Reset( ) {
			m_bOverride = false;
		}

		bool m_bOverride = false;
	};

	std::array<SpectatorDormancy_t, 65> m_arrSpectatorPlayers;

	void Adjust( );
	void Restore( );

	std::array<SoundPlayer, 65> m_arrSoundPlayers;
	std::vector<SoundPlayer> m_vecRestorePlayers;

	std::array<std::pair<short, CCSWeaponInfo*>, 65> m_arrWeaponInfos;
};

extern ExtendedVisuals g_ExtendedVisuals;


class Visuals {
	struct Box_t {
		int x, y, w, h;
	};

	// word to your mother she's sucking on the tip of my penis rn
	std::map<int, char> m_WeaponIconsMap = {
		{ WEAPON_DEAGLE, 'A' },
		{ WEAPON_ELITE, 'B' },
		{ WEAPON_FIVESEVEN, 'C' },
		{ WEAPON_GLOCK, 'D' },
		{ WEAPON_P2000, 'E' },
		{ WEAPON_P250, 'F' },
		{ WEAPON_USPS, 'G' },
		{ WEAPON_TEC9, 'H' },
		{ WEAPON_CZ75, 'I' },
		{ WEAPON_REVOLVER, 'J' },
		{ WEAPON_MAC10, 'K' },
		{ WEAPON_UMP45, 'L' },
		{ WEAPON_BIZON, 'M' },
		{ WEAPON_MP7, 'N' },
		{ WEAPON_MP5SD, 'L' },
		{ WEAPON_MP9, 'O' },
		{ WEAPON_P90, 'P' },
		{ WEAPON_GALIL, 'Q' },
		{ WEAPON_FAMAS, 'R' },
		{ WEAPON_M4A4, 'S' },
		{ WEAPON_M4A1S, 'T' },
		{ WEAPON_AUG, 'U' },
		{ WEAPON_SG553, 'V' },
		{ WEAPON_AK47, 'W' },
		{ WEAPON_G3SG1, 'X' },
		{ WEAPON_SCAR20, 'Y' },
		{ WEAPON_AWP, 'Z' },
		{ WEAPON_SSG08, 'a' },
		{ WEAPON_XM1014, 'b' },
		{ WEAPON_SAWEDOFF, 'c' },
		{ WEAPON_MAG7, 'd' },
		{ WEAPON_NOVA, 'e' },
		{ WEAPON_NEGEV, 'f' },
		{ WEAPON_M249, 'g' },
		{ WEAPON_ZEUS, 'h' },
		{ WEAPON_KNIFE_T, 'i' },
		{ WEAPON_KNIFE, 'j' },
		{ WEAPON_KNIFE_FALCHION, '0' },
		{ WEAPON_KNIFE_BAYONET, '1' },
		{ WEAPON_KNIFE_FLIP, '2' },
		{ WEAPON_KNIFE_GUT, '3' },
		{ WEAPON_KNIFE_KARAMBIT, '4' },
		{ WEAPON_KNIFE_M9_BAYONET, '5' },
		{ WEAPON_KNIFE_TACTICAL, '6' },
		{ WEAPON_KNIFE_SURVIVAL_BOWIE, '7' },
		{ WEAPON_KNIFE_BUTTERFLY, '8' },
		{ WEAPON_KNIFE_URSUS, 'j' },
		{ WEAPON_KNIFE_GYPSY_JACKKNIFE, 'j' },
		{ WEAPON_KNIFE_STILETTO, 'j' },
		{ WEAPON_KNIFE_WIDOWMAKER, 'j' },
		{ WEAPON_FLASHBANG, 'k' },
		{ WEAPON_HEGRENADE, 'l' },
		{ WEAPON_SMOKEGRENADE, 'm' },
		{ WEAPON_MOLOTOV, 'n' },
		{ WEAPON_DECOY, 'o' },
		{ WEAPON_INC, 'p' },
		{ WEAPON_C4, 'q' },
	};

	// prototypes
	std::string GetWeaponIcon( const int id );
	bool SetupBoundingBox( C_CSPlayer* player, Box_t& box );
	bool IsValidPlayer( C_CSPlayer* entity );
	bool IsValidEntity( C_BaseEntity* entity );
	void HandlePlayerVisuals( C_CSPlayer* entity );
	void HandleWorldVisuals( C_BaseEntity* entity );
	Color DetermineVisualsColor( Color regular, C_CSPlayer* entity );

	void PenetrationCrosshair( );

	// actual visuals
	void RenderBox( const Box_t& box, C_CSPlayer* entity );
	void RenderName( const Box_t& box, C_CSPlayer* entity );
	void RenderHealth( const Box_t& box, C_CSPlayer* entity );
	void RenderBottomInfo( const Box_t& box, C_CSPlayer* entity );
	void RenderSideInfo( const Box_t& box, C_CSPlayer* entity );
	void RenderArrows( C_BaseEntity* entity );

	void Spectators( );

	void RenderSkeleton( C_CSPlayer* entity );

	void RenderDroppedWeapons( C_BaseEntity* entity );
	void RenderNades( C_BaseEntity* entity );

	std::string GetBombSite( C_PlantedC4* entity );

	void RenderObjectives( C_BaseEntity* entity );

	CVariables::PLAYER_VISUALS* visuals_config;

public:
	void AddDebugMessage( std::string msg );

	void Draw( );
	void DrawWatermark( );

	std::array<float, 65> animated_lby;
	std::array<float, 65> player_fading_alpha;
	std::array<int, 65> player_nade_damage;
	
	// todo: move
	std::vector<std::string> debug_messages;
	std::vector<std::string> debug_messages_sane;
};

extern Visuals g_Visuals;