#pragma once
#include "../Utils/extern/FnvHash.hpp"
#include <any>
#include <variant>
#include <map>
#include <d3d9.h>
#include "../Libraries/json.h"
#include "Valve/vector.hpp"
#include "Valve/qangle.hpp"
#include "Classes/CStudioRender.hpp" 


#define USE_XOR

#include "../Utils/extern/XorStr.hpp"

class CUserCmd;

namespace KeyBindType
{
	enum {
		HOLD = 0,
		TOGGLE,
		HOLD_OFF,
		ALWAYS_ON
	};
};

struct hotkey_t {
	hotkey_t( ) { }

	int key = 0, cond = 0;
	bool enabled = false;

	void to_json( nlohmann::json& j ) {
		j = nlohmann::json{
			{ ( XorStr( "key" ) ), key },
			{ ( XorStr( "cond" ) ), cond },
		};
	}

	void from_json( nlohmann::json& j ) {
		j.at( XorStr( "key" ) ).get_to( key );
		j.at( XorStr( "cond" ) ).get_to( cond );
	}
};

extern std::vector<hotkey_t*> g_keybinds;

class Color;
class Color_f {
public:
	Color_f( ) = default;
	Color_f( float _r, float _g, float _b, float _a = 1.0f ) :
		r( _r ), g( _g ), b( _b ), a( _a ) {
	}

	Color_f( int _r, int _g, int _b, int _a = 255 ) { SetColor( _r, _g, _b, _a ); }

	Color_f Lerp( const Color_f& c, float t ) const;

	void SetColor( float _r, float _g, float _b, float _a = 1.0f ) {
		r = _r;
		g = _g;
		b = _b;
		a = _a;
	}

	void SetColor( Color clr );

	void SetColor( int _r, int _g, int _b, int _a = 255 ) {
		r = static_cast< float >( _r ) / 255.0f;
		g = static_cast< float >( _g ) / 255.0f;
		b = static_cast< float >( _b ) / 255.0f;
		a = static_cast< float >( _a ) / 255.0f;
	}

	Color_f Alpha( float alpha ) {
		return Color_f( r, g, b, alpha );
	}

	uint32_t Hex( ) const {
		union {
			uint32_t i;
			struct {
				uint8_t bytes[ 4 ];
			};
		} conv;

		conv.bytes[ 0 ] = static_cast< int >( r * 255.0f );
		conv.bytes[ 1 ] = static_cast< int >( g * 255.0f );
		conv.bytes[ 2 ] = static_cast< int >( b * 255.0f );
		conv.bytes[ 3 ] = static_cast< int >( a * 255.0f );

		return conv.i;
	};

	void Color_f::to_json( nlohmann::json& j ) {
		j = nlohmann::json{
			{ XorStr( "r" ), r },
			{ XorStr( "g" ), g },
			{ XorStr( "b" ), b },
			{ XorStr( "a" ), a }
		};
	}

	void Color_f::from_json( nlohmann::json& j ) {
		j.at( XorStr( "r" ) ).get_to( r );
		j.at( XorStr( "g" ) ).get_to( g );
		j.at( XorStr( "b" ) ).get_to( b );
		j.at( XorStr( "a" ) ).get_to( a );
	}

	bool operator==( const Color_f& clr ) const {
		return clr.r == r && clr.g == g && clr.b == b && clr.a == a;
	};

	bool operator!=( const Color_f& clr ) const {
		return clr.r != r || clr.g != g || clr.b != b || clr.a != a;
	};

	Color_f operator*( float v ) const {
		return Color_f( r * v, g * v, b * v, a );
	}

	operator uint32_t( ) const { return Hex( ); };

	operator float* ( ) { return &r; };

	float r, g, b, a;

	static Color_f Black;
	static Color_f White;
	static Color_f Gray;

	Color ToRegularColor( );
};

#pragma region Config System

class CBaseGroup {
public: // ghetto fix, for skin changer options setup
	std::map< uint32_t, std::unique_ptr< std::any > > m_options;
	uint32_t m_name;
	nlohmann::json m_json;
	nlohmann::json m_json_default_cfg;
	std::vector< CBaseGroup* > m_children;

	using AllowedTypes = std::variant< int, bool, float, std::string, Color_f >;
	template < typename T >
	using IsTypeAllowed = std::enable_if_t< std::is_constructible_v< AllowedTypes, T >, T* >;

public:
	CBaseGroup( ) = default;
	CBaseGroup( std::string name, CBaseGroup* parent = nullptr ) {
		m_name = hash_32_fnv1a( name.c_str( ) );
		if( parent )
			parent->AddChild( this );
	}

	CBaseGroup( size_t idx, CBaseGroup* parent = nullptr ) {
		m_name = hash_32_fnv1a( std::string( XorStr( "( " ) ).append( std::to_string( idx ) ).append( XorStr( " )" ) ).data( ) );
		if( parent )
			parent->AddChild( this );
	};

protected:
	template < typename T, class... Types >
	auto AddOption( const char* name, Types&& ... args ) -> T* {
		auto pair = m_options.try_emplace( hash_32_fnv1a( name ), std::make_unique< std::any >( std::make_any< T >( std::forward< Types >( args )... ) ) );

		if( typeid( T ).hash_code( ) == typeid( hotkey_t ).hash_code( ) )
			g_keybinds.push_back( reinterpret_cast< hotkey_t* >( pair.first->second.get( ) ) );

		return reinterpret_cast< T* >( pair.first->second.get( ) );
	};

	auto AddChild( CBaseGroup* group ) -> void { m_children.push_back( group ); };
public:
	template < typename T, class... Types >
	auto Add( const char* name, Types&& ... args ) -> T* {
		auto pair = m_options.try_emplace( hash_32_fnv1a( name ), std::make_unique< std::any >( std::make_any< T >( std::forward< Types >( args )... ) ) );

		if( typeid( T ).hash_code( ) == typeid( hotkey_t ).hash_code( ) )
			g_keybinds.push_back( reinterpret_cast< hotkey_t* >( pair.first->second.get( ) ) );

		return reinterpret_cast< T* >( pair.first->second.get( ) );
	}

	auto GetName( ) const -> uint32_t { return m_name; }

	auto GetJson( ) -> nlohmann::json& { return m_json; }

	auto SetName( const std::string_view& name ) -> void { m_name = hash_32_fnv1a( name.data( ) ); }

	auto Save( ) -> void {
		m_json.clear( );
		for( auto& [name_hash, opt] : m_options ) {
			std::string name = std::to_string( name_hash );

			// TODO: option class with virtual save function (is it good idea?)
			auto any = *opt.get( );
			auto hash = any.type( ).hash_code( );

			// find out, could iterate AllowedTypes
			if( typeid( int ).hash_code( ) == hash )
				m_json[ name ] = std::any_cast< int >( any );
			else if( typeid( bool ).hash_code( ) == hash )
				m_json[ name ] = std::any_cast< bool >( any );
			else if( typeid( float ).hash_code( ) == hash )
				m_json[ name ] = std::any_cast< float >( any );
			else if( typeid( std::string ).hash_code( ) == hash )
				m_json[ name ] = std::any_cast< std::string >( any );
			else if( typeid( Color_f ).hash_code( ) == hash )
				std::any_cast< Color_f >( any ).to_json( m_json[ name ] );
			else if( typeid( hotkey_t ).hash_code( ) == hash )
				std::any_cast< hotkey_t >( any ).to_json( m_json[ name ] );
		}

		for( auto& child : m_children ) {
			child->Save( );

			auto json = child->GetJson( );
			m_json[ std::to_string( child->GetName( ) ) ] = json;
		}
	}

	auto Load( nlohmann::json& js ) -> void {
		m_json.clear( );
		m_json = js;
		for( auto& [name_hash, opt] : m_options ) {
			std::string name = std::to_string( name_hash );

			// TODO: option class with virtual load function (is it good idea?)
			std::any& any = *opt.get( );
			auto hash = any.type( ).hash_code( );

			if( m_json.count( name ) <= 0 )
				continue;

			try {
				// find out, can iterate AllowedType? 
				if( typeid( int ).hash_code( ) == hash )
					std::any_cast< int& >( any ) = m_json[ name ];
				else if( typeid( bool ).hash_code( ) == hash )
					std::any_cast< bool& >( any ) = m_json[ name ];
				else if( typeid( float ).hash_code( ) == hash )
					std::any_cast< float& >( any ) = m_json[ name ];
				else if( typeid( std::string ).hash_code( ) == hash )
					std::any_cast< std::string& >( any ) = m_json[ name ];
				else if( typeid( Color_f ).hash_code( ) == hash )
					std::any_cast< Color_f& >( any ).from_json( m_json[ name ] );
				else if( typeid( hotkey_t ).hash_code( ) == hash )
					std::any_cast< hotkey_t& >( any ).from_json( m_json[ name ] );
			}
			catch( std::exception& ) {
				continue;
			}
		}

		for( auto& child : m_children ) {
			child->Load( m_json[ std::to_string( child->GetName( ) ) ] );
		}
	}
};

// TODO: std::map group
template < class ArrayImpl, class = std::enable_if_t< std::is_base_of< CBaseGroup, ArrayImpl >::value > >
class CArrayGroup : public CBaseGroup {
public:
	CArrayGroup( ) { }

	CArrayGroup( const std::string& name, size_t count = 0 ) :
		CBaseGroup( name ) {
		m_children.reserve( count );
		for( auto i = 0u; i < count; ++i ) {
			m_children.emplace_back( new ArrayImpl( i ) );
		}
	}

	auto AddEntry( ) -> size_t {
		size_t idx = m_children.size( );
		m_children.emplace_back( new ArrayImpl( idx ) );
		return idx;
	}

	auto operator[]( ptrdiff_t idx ) -> ArrayImpl* {
		return ( ArrayImpl* )m_children[ idx ];
	}

	auto Size( ) const -> size_t {
		return m_children.size( );
	};

	// update only children
	auto Save( ) -> void {
		m_json.clear( );
		for( auto& child : m_children ) {
			child->Save( );

			auto json = child->GetJson( );
			m_json[ std::to_string( child->GetName( ) ) ] = json;
		}
	}

	auto Load( nlohmann::json& js ) -> void {
		m_json.clear( );
		m_json = js;
		for( auto& child : m_children ) {
			child->Load( m_json[ std::to_string( child->GetName( ) ) ] );
		}
	}
};

#define group_begin( group_name )                                  \
                                                                   \
  class group_name : public CBaseGroup {                           \
                                                                   \
  public:                                                          \
    group_name( CBaseGroup* parent = nullptr ) :                   \
        CBaseGroup( #group_name, parent ){};                       \
    group_name( const char* name, CBaseGroup* parent = nullptr ) : \
        CBaseGroup( name, parent ){};                              \
                                                                   \
    group_name( size_t idx, CBaseGroup* parent = nullptr ) :       \
        CBaseGroup( idx, parent ) {}

#define group_end() }

#define group_end_child( group_name, var_name ) \
  }                                             \
  ;                                             \
                                                \
  group_name var_name = group_name( XorStr(#group_name), this )

#define add_child_group( group_name, var_name ) group_name var_name = group_name( XorStr(#group_name), this )
#define add_child_group_ex( group_name, var_name ) group_name var_name = group_name( XorStr(#var_name), this )
#define add_child_group_name( group_name, var_name, name ) group_name var_name = group_name( XorStr(name), this )

#define config_option( type, name, ... ) type& name = *this->AddOption< type >( XorStr(#name), __VA_ARGS__ );
#define config_option_separate( type, name, parent, ... ) type& name = *parent.AddOption< type >( XorStr(#name), __VA_ARGS__ );

#define config_keybind( name ) config_option( hotkey_t, name )
#pragma endregion

class ConVar;

struct SpreadRandom_t {
	float flRand1;
	float flRandPi1;
	float flRand2;
	float flRandPi2;
};

class CVariables : public CBaseGroup {
public:
	typedef struct _GLOBAL {
		bool menuOpen = false;
		bool hackUnload = false;

		bool m_bInBoneSetup = false;

		bool m_bInCreateMove = false;
		bool m_bInPostScreenEffects = false;

		bool m_bConsoleOpen = false;
		bool m_bChatOpen = false;

		bool m_bForceFullUpdate = false;

		bool m_bFakeWalking = false;
		bool RenderIsReady = false;
		bool m_bUpdatingAnimations;
		bool m_bShotWhileChoking = false;
		bool m_bShotAutopeek = false;

		bool m_bInsideFireRange = false;

		int m_iWeaponIndex = -1;
		int m_iWeaponIndexSkins = -1;

		// cached random values used by spread, faster than RandomFloat call every hitchance tick ( also thread safe )
		bool m_bInitRandomSeed = false;
		SpreadRandom_t SpreadRandom[ 256 ];

		std::vector<std::string> m_vecPaintKits;

		std::vector<std::pair<float, std::string>> m_vecDevMessages;

		std::vector<__int64> m_vecWhitelistedIds = {
			76561198191038805, // nico (http://steamcommunity.com/profiles/76561198191038805)
			76561198326344021, // alpha (http://steamcommunity.com/profiles/76561198326344021)
			76561199006975792, // chase (https://steamcommunity.com/profiles/76561199006975792)
			76561199049103876 // eddy (https://steamcommunity.com/profiles/76561199049103876)
		};

		struct cheat_header_t {
			char username[ 32 ];
			uint64_t sub_expiration;
			uint32_t access_token;
		};

		cheat_header_t user_info;
		std::string c_login;

		HINSTANCE hModule;

		std::string szLastHookCalled = XorStr( "null" );
		std::vector<std::string> m_vecHitsounds;

		float m_flLastVelocityModifier = FLT_MAX;

	} GLOBAL, * PGLOBAL;

	group_begin( MENU );
	config_option( int, m_count, 0 );
	config_option( int, m_selected, 0 );
	config_option( bool, expand_side, true );
	config_option( bool, lock_layout, false );

	config_option( Color_f, ascent, Color_f( 0.647058824f, 0.57254902f, 0.952941176f, 1.0f ) );
	config_keybind( key );
	config_option( bool, watermark, true );
	config_option( float, size_x, 610.f );
	config_option( float, size_y, 415.f );
	group_end( );

#pragma region Skin Changer
	group_begin( skin_changer_data );
	config_option( bool, m_enabled, false );
	config_option( bool, m_filter_paint_kits, true );
	config_option( bool, m_custom, false );
	config_option( int, m_paint_kit, 0 );
	config_option( int, m_paint_kit_no_filter, 0 );
	config_option( int, m_seed, 0 );
	config_option( bool, m_stat_trak, false );
	config_option( float, m_wear, 100.f );
	config_option( std::string, m_custom_name, "" );

	// run-time data only
	//friend class Interfaces::feature::SkinChanger;
	//friend class OptionStorage;
	int m_paint_kit_index = 0;
	uint16_t m_definition_index = 0;
	bool m_executed = false;

	group_end( );

	group_begin( skin_changer_global_data );
	config_option( bool, m_active, false );

	config_option( bool, m_knife_changer, false );
	config_option( int, m_knife_idx, 0 );
	int m_knife_vector_idx = 0;

	config_option( bool, m_glove_changer, false );
	config_option( int, m_gloves_idx, 0 );
	int m_gloves_vector_idx = 0;

	bool m_update_gloves = false;
	bool m_update_skins = false;

	group_end( );
#pragma endregion

#pragma region Lua Scripts
	group_begin( lua_scripts_data );

	config_option( bool, m_load, false );
	config_option( std::string, m_script_name, XorStr( "" ) );

	group_end( );
#pragma endregion

#pragma region Rage general group
	group_begin( RAGE_GENERAL );
	config_option( bool, enabled, false );
	config_keybind( key );

	config_option( bool, team_check, false );

	config_option( bool, silent_aim, false );
	config_option( bool, delay_shot_unduck, false );
	config_option( bool, auto_fire, false );
	config_option( bool, exploit, false );
	config_option( bool, double_tap_teleport, false );
	config_option( bool, double_tap_recovery, false );
	config_option( bool, double_tap_recharge_threat, false );
	config_option( bool, double_tap_adaptive, false );
	config_option( bool, double_tap_duck, false );
	config_option( int, double_tap_reserve, 2 );
	config_option( int, double_tap_type, 0 );

	config_option( bool, fake_lag, false );

	// conditions
	config_option( bool, fake_lag_standing, false );
	config_option( bool, fake_lag_moving, false );
	config_option( bool, fake_lag_air, false );
	config_option( bool, fake_lag_unduck, false );

	config_option( int, fake_lag_amount, 7 );
	config_option( float, fake_lag_variance, 7 );
	config_option( int, fake_lag_type, 0 );

	config_option( bool, anti_aim_crooked, false );
	config_option( bool, anti_aim_twist, false );
	config_option( bool, anti_aim_break_walk, false );

	config_option( int, anti_aim_base_pitch, 0 );

	config_option( int, anti_aim_yaw, 0 );
	config_option( int, anti_aim_yaw_while_running, 0 );
	config_option( float, anti_aim_yaw_jitter, 0.f );
	config_option( float, anti_aim_yaw_spin_speed, 0.f );
	config_option( float, anti_aim_yaw_spin_direction, 0.f );

	config_option( int, anti_aim_base_yaw, 0 );
	config_option( float, anti_aim_base_yaw_additive, 0.f );

	config_option( bool, anti_aim_freestand_default, false );
	config_option( bool, anti_aim_freestand_running, false );
	config_option( bool, anti_aim_freestand_edge, false );
	config_option( float, anti_aim_freestand_delta, 105.f );

	config_option( int, anti_aim_edge, 0 );
	config_option( float, anti_aim_edge_delta, 105.f );

	config_option( int, anti_aim_fake_type, 0 );

	config_keybind( anti_aim_left_key );
	config_keybind( anti_aim_right_key );
	config_keybind( anti_aim_back_key );
	
	config_keybind( test1 );
	config_option( float, test2, 105.f );
	config_option( float, test3, 105.f );

	config_keybind( min_damage_override_key );
	config_keybind( force_baim );

	config_keybind( double_tap_bind );
	config_option( float, double_tap_hitchance, 20.f );
	config_keybind( hide_shots_bind );

	config_keybind( force_recharge );

	config_option( bool, remove_spread, false );
	config_option( bool, remove_recoil, false );
	config_option( int, accuracy_boost, 0 );

	config_option( bool, antiaim_correction, false );
	config_option( bool, antiaim_resolver, false );
	config_option( bool, antiaim_resolver_override, false );
	config_option( bool, aimstep, false );
	config_option( bool, delay_shot, false );
	config_keybind( antiaim_correction_override );

	config_option( int, fakelag_correction, 0 );

	group_end( );

	group_begin( RAGE );
	config_option( bool, override_default_config, false );

	config_option( bool, hitbox_head, false );
	config_option( bool, hitbox_neck, false );
	config_option( bool, hitbox_chest, false );
	config_option( bool, hitbox_stomach, false );
	config_option( bool, hitbox_pelvis, false );
	config_option( bool, hitbox_arms, false );
	config_option( bool, hitbox_legs, false );
	config_option( bool, hitbox_feet, false );

	config_option( int, prefer_body, 0 );

	config_option( int, multipoint, 0 );

	//config_option( bool, multipoint_head, false );
	//config_option( bool, multipoint_upper_chest, false );
	//config_option( bool, multipoint_chest, false );
	//config_option( bool, multipoint_lower_chest, false );
	//config_option( bool, multipoint_stomach, false );
	//config_option( bool, multipoint_legs, false );
	//config_option( bool, multipoint_feet, false );

	config_option( bool, hitchance, false );
	config_option( float, hitchance_amount, 45.f );

	config_option( float, stomach_scale, 55.f );
	config_option( float, pointscale, 55.f );

	config_option( bool, override_awp, false );

	config_option( int, minimal_damage, 30 );
	config_option( int, minimal_damage_override, 5 );
	config_option( bool, wall_penetration, false );
	config_option( int, wall_penetration_damage, 20 );

	config_option( bool, scale_damage_on_hp, false );

	config_option( bool, auto_scope, false );
	config_option( int, auto_scope_type, 0 );

	config_option( int, quick_stop, 0 );
	config_keybind( quick_stop_key );

	group_end( );
#pragma endregion

#pragma region Antiaim group

	group_begin( ANTIAIM );


	group_end( );
#pragma endregion

#pragma region Fakelag group
	group_begin( FAKELAG );

	group_end( );
#pragma endregion

#pragma region ESP group
	// TODO: separate visuals from esp
	group_begin( ESP );


	// chams
	config_option( int, chams_material, false );

	config_option( bool, chams_player, false );
	config_option( Color_f, chams_player_color, Color_f( 0.58823529411f, 0.78431372549f, 0.23529411764f, 1.0f ) );
	config_option( bool, chams_player_wall, false );
	config_option( Color_f, chams_player_wall_color, Color_f( 0.23529411764f, 0.47058823529f, 0.70588235294f, 0.69803921568f ) );

	config_option( float, chams_reflectivity, 67.f );
	config_option( Color_f, chams_reflectivity_color, Color_f( 1.0f, 0.69803921568f, 0.f, 1.0f ) );

	config_option( float, chams_shine, 10.f );
	config_option( float, chams_rim, 10.f );

	config_option( bool, chams_teammates, false );
	config_option( bool, chams_shadow, false );
	config_option( Color_f, chams_shadow_color, Color_f( 0.f, 0.f, 0.f, 0.47058823529f ) );
	config_option( Color_f, chams_teammates_color, Color_f( 0.f, 0.f, 0.f, 0.47058823529f ) );

	config_option( bool, remove_scope, false );
	config_option( bool, instant_scope, false );
	config_option( bool, remove_recoil_shake, false );
	config_option( bool, remove_recoil_punch, false );
	config_option( bool, remove_flash, false );
	config_option( bool, remove_smoke, false );
	config_option( bool, remove_fog, false );
	config_option( bool, remove_sleeves, false );
	config_option( bool, remove_hands, false );
	config_option( bool, remove_bob, false );

	config_option( float, transparent_walls, 100.f );
	config_option( float, transparent_props, 100.f );

	config_option( bool, preserve_killfeed, false );
	config_option( float, preserve_killfeed_time, 1.0f );

	config_option( bool, draw_bomb, false );
	config_option( Color_f, draw_bomb_color, Color_f( 1.0f, 1.0f, 1.0f, 1.0f ) );

	config_option( bool, autowall_crosshair, false );
	config_option( float, autowall_crosshair_height, 0.f );

	config_option( bool, skip_occulusion, false );

	config_option( bool, nades, false );
	config_option( Color_f, nades_text_color, Color_f( 1.0f, 1.0f, 1.0f, 1.0f ) );

	config_option( int, dropped_weapons, 0 );
	config_option( bool, dropped_weapons_ammo, false );
	config_option( Color_f, dropped_weapons_color, Color_f( 1.0f, 1.0f, 1.0f, 0.78431372549f ) );
	config_option( Color_f, dropped_weapons_ammo_color, Color_f( 1.0f, 1.0f, 1.0f, 1.0f ) );

	config_option( bool, extended_esp, false );

	config_option( bool, fov_crosshair, false );
	config_option( Color_f, fov_crosshair_color, Color_f( 1.0f, 1.0f, 1.0f, 1.0f ) );

	config_option( bool, force_sniper_crosshair, false );

	config_option( bool, event_bomb, false );
	config_option( bool, event_harm, false );
	config_option( bool, event_dmg, false );
	config_option( bool, event_buy, false );
	config_option( bool, event_resolver, false );
	config_option( bool, event_misc, false );
	config_option( bool, event_console, false );
	config_option( bool, event_fire, false );

	config_option( bool, vizualize_hitmarker, false );
	config_option( bool, visualize_hitmarker_world, false );
	config_option( bool, visualize_damage, false );

	config_option( bool, zeus_distance, false );
	config_option( Color_f, zeus_distance_color, Color_f( 1.0f, 1.0f, 1.0f, 1.0f ) );

	config_option( float, world_fov, 90.f );

	config_option( bool, blur_in_scoped, false );
	config_option( float, blur_in_scoped_value, 50.f );

	config_option( bool, aspect_ratio, false );
	config_option( float, aspect_ratio_value, 1.5f );

	config_option( bool, remove_blur_effect, false );
	config_option( bool, remove_post_proccesing, false );

	config_option( bool, server_impacts, false );
	config_option( bool, disable_teammate_rendering, false );

	config_option( bool, bullet_beams, false );
	config_option( Color_f, bullet_beams_color, Color_f( 0.58823529411f, 0.50980392156f, 1.0f, 1.0f ) );

	config_option( bool, visualize_sounds, false );
	config_option( Color_f, visualize_sounds_color, Color_f( 0.98039215686f, 0.24705882352f, 0.24705882352f, 1.0f ) );

	config_option( int, brightness_adjustment, 0 );

	config_option( bool, indicator_side, false );
	config_option( bool, indicator_exploits, false );
	config_option( bool, indicator_aimbot, false );
	config_option( bool, indicator_fake_duck, false );
	config_option( Color_f, indicator_color, Color_f( 1.0f, 1.0f, 1.0f, 1.0f ) );

	config_option( bool, fog_effect, false );
	config_option( bool, fog_blind, false );
	config_option( float, fog_density, 0.f );
	config_option( float, fog_hdr_scale, 0.f );
	config_option( int, fog_distance, 1000 );
	config_option( Color_f, fog_color, Color_f( 1.0f, 1.0f, 1.0f, 1.0f ) );
	config_option( Color_f, fog_color_secondary, Color_f( 1.0f, 1.0f, 1.0f, 1.0f ) );

	config_option( bool, ambient_ligtning, false );
	config_option( Color_f, ambient_ligtning_color, Color_f( 0.06666666666, 0.06666666666, 0.090196, 1.0f ) );

	config_option( bool, grenade_prediction, false );
	config_option( Color_f, grenade_prediction_color, Color_f( 0.0f, 0.6f, 1.0f, 1.0f ) );

	config_option( bool, grenades, false );
	config_option( Color_f, grenades_color, Color_f( 1.0f, 1.0f, 1.0f, 0.8f ) );
	group_end( );
#pragma endregion

	group_begin( PLAYER_VISUALS );

	config_option( bool, teammates, false );

	config_option( bool, box, false );
	config_option( Color_f, box_color, Color_f( 1.0f, 1.0f, 1.0f, 0.50980392156f ) );

	config_option( bool, health, false );
	config_option( bool, health_color_override, false );
	config_option( Color_f, health_color, Color_f( 1.0f, 1.0f, 1.0f ) );

	config_option( bool, name, false );
	config_option( Color_f, name_color, Color_f( 1.0f, 1.0f, 1.0f, 0.78431372549f ) );

	config_option( bool, flags, false );

	config_option( bool, lby_timer, false );
	config_option( Color_f, lby_timer_color, Color_f( 1.0f, 0.f, 1.0f ) );

	config_option( bool, money, false );

	config_option( bool, skeleton, false );
	config_option( Color_f, skeleton_color, Color_f( 1.0f, 1.f, 0.66666666666f ) );

	config_option( bool, ammo, false );
	config_option( Color_f, ammo_color, Color_f( 0.31372549019f, 0.54901960784f, 0.78431372549f, 0.90196078431f ) );

	config_option( bool, distance, false );

	config_option( bool, weapon, false );
	config_option( bool, weapon_icon, false );
	config_option( Color_f, weapon_color, Color_f( 1.0f, 1.0f, 1.0f ) );

	config_option( bool, view_arrows, false );
	config_option( Color_f, view_arrows_color, Color_f( 1.0f, 1.0f, 1.0f ) );

	config_option( float, view_arrows_distance, 50.f );
	config_option( float, view_arrows_size, 16.f );

	config_option( bool, glow, false );
	config_option( Color_f, glow_color, Color_f( 0.70588235294f, 0.23529411764f, 0.47058823529f, 0.6f ) );

	config_option( bool, dormant, false );

	group_end( );

#pragma region LUA group
	group_begin( LUA );

	config_option( bool, test, false );

	group_end( );

#pragma endregion

#pragma region MISC group
	group_begin( MISC );

	config_option( bool, active, false );
	config_option( bool, bunnyhop, false );
	config_option( bool, autostrafer, false );
	config_option( bool, autostrafer_wasd, false );
	config_option( bool, move_exploit, false );
	config_keybind( move_exploit_key );

	config_option( bool, fastduck, false );
	config_option( bool, minijump, false );
	config_option( bool, quickstop, false );
	config_option( bool, accurate_walk, false );
	config_option( bool, fakewalk, false );
	config_keybind( fakewalk_bind );
	config_option( float, slow_motion_speed, 100.f );

	config_option( bool, third_person, false );
	config_option( bool, first_person_dead, false );
	config_keybind( third_person_bind );

	config_option( float, viewmodel_fov, 68.f );
	config_option( bool, viewmodel_change, false );
	config_option( float, viewmodel_x, 2.f );
	config_option( float, viewmodel_y, 2.f );
	config_option( float, viewmodel_z, -2.f );

	config_option( bool, hitsound, false );
	config_option( int, hitsound_type, 0 );
	config_option( int, hitsound_custom, 0 );
	config_option( float, hitsound_volume, 100.f );

	config_option( bool, clantag_changer, false );
	config_option( int, air_duck, 0 );
	config_option( bool, knifebot, false );
	config_option( bool, zeusbot, false );
	config_option( bool, blockbot, false );
	config_keybind( blockbot_key );
	config_option( bool, auto_weapons, false );
	config_option( bool, edge_jump, false );
	config_keybind( edge_jump_key );
	config_option( bool, reveal_ranks, false );
	config_option( bool, auto_accept, true );

	config_option( bool, autobuy_enabled, false );
	config_option( int, autobuy_first_weapon, 0 );
	config_option( int, autobuy_second_weapon, 0 );
	config_option( bool, autobuy_flashbang, false );
	config_option( bool, autobuy_smokegreanade, false );
	config_option( bool, autobuy_molotovgrenade, false );
	config_option( bool, autobuy_hegrenade, false );
	config_option( bool, autobuy_decoy, false );
	config_option( bool, autobuy_armor, false );
	config_option( bool, autobuy_zeus, false );
	config_option( bool, autobuy_defusekit, false );

	config_option( bool, knife_bot, false );
	config_option( int, knife_bot_type, 0 );

	config_option( bool, zeus_bot, false );
	config_option( float, zeus_bot_hitchance, 80.f );

	config_option( bool, anti_untrusted, true );

	config_option( bool, autopeek, false );
	config_option( bool, autopeek_visualise, false );
	config_keybind( autopeek_bind );
	config_option( Color_f, autopeek_color, Color_f( 1.0f, 1.0f, 1.0f, 0.5f ) );

	config_option( bool, money_revealer, false );

	config_option( bool, unlock_inventory, false );
	config_option( bool, auto_release_grenade, false );

	config_option( bool, ingame_radar, false );
	config_option( bool, spectators, false );

#if defined(BETA_MODE) || defined(DEV)
	config_option( bool, undercover_log, false );
	config_option( bool, undercover_fire_log, false );
	config_option( bool, undercover_watermark, false );

#if defined(DEV)
	config_option( int, what_developer_is_this, 0 );
#endif

#endif

	group_end( );

#pragma endregion

	add_child_group_ex( MENU, menu );
	add_child_group_ex( RAGE_GENERAL, rage );

	RAGE* rage_option( );
	RAGE* current_rage_option;

	add_child_group_ex( RAGE, rage_default );
	add_child_group_ex( RAGE, rage_pistols );
	add_child_group_ex( RAGE, rage_heavypistols );
	add_child_group_ex( RAGE, rage_rifles );
	add_child_group_ex( RAGE, rage_awp );
	add_child_group_ex( RAGE, rage_scout );
	add_child_group_ex( RAGE, rage_autosnipers );
	add_child_group_ex( RAGE, rage_smgs );
	add_child_group_ex( RAGE, rage_heavys );
	add_child_group_ex( RAGE, rage_shotguns );

	add_child_group_ex( ANTIAIM, antiaim );
	//add_child_group_ex( ANTIAIM_STATE, antiaim_move );
	//add_child_group_ex( ANTIAIM_STATE, antiaim_air );
	add_child_group_ex( FAKELAG, fakelag );
	add_child_group_ex( ESP, esp );
	add_child_group_ex( PLAYER_VISUALS, visuals_enemy );
	add_child_group_ex( MISC, misc );
	add_child_group_ex( skin_changer_global_data, m_global_skin_changer );

	CArrayGroup<skin_changer_data> m_skin_changer;
	CArrayGroup<lua_scripts_data> m_loaded_luas;

	add_child_group_ex( LUA, lua );

	GLOBAL globals;

	// convars
	ConVar* sv_accelerate;
	ConVar* sv_airaccelerate;
	ConVar* sv_gravity;
	ConVar* sv_jump_impulse;
	ConVar* sv_penetration_type;

	ConVar* m_yaw;
	ConVar* m_pitch;
	ConVar* sensitivity;

	ConVar* cl_sidespeed;
	ConVar* cl_forwardspeed;
	ConVar* cl_upspeed;
	ConVar* cl_extrapolate;

	ConVar* sv_noclipspeed;

	ConVar* weapon_recoil_scale;
	ConVar* view_recoil_tracking;

	ConVar* r_jiggle_bones;

	ConVar* mp_friendlyfire;

	ConVar* sv_maxunlag;
	ConVar* sv_minupdaterate;
	ConVar* sv_maxupdaterate;

	ConVar* sv_client_min_interp_ratio;
	ConVar* sv_client_max_interp_ratio;

	ConVar* cl_interp_ratio;
	ConVar* cl_interp;
	ConVar* cl_updaterate;

	ConVar* game_type;
	ConVar* game_mode;

	ConVar* ff_damage_bullet_penetration;
	ConVar* ff_damage_reduction_bullets;

	ConVar* mp_c4timer;

	ConVar* mp_damage_scale_ct_head;
	ConVar* mp_damage_scale_t_head;
	ConVar* mp_damage_scale_ct_body;
	ConVar* mp_damage_scale_t_body;

	ConVar* viewmodel_fov;
	ConVar* viewmodel_offset_x;
	ConVar* viewmodel_offset_y;
	ConVar* viewmodel_offset_z;

	ConVar* mat_ambient_light_r;
	ConVar* mat_ambient_light_g;
	ConVar* mat_ambient_light_b;

	ConVar* sv_show_impacts;

	ConVar* molotov_throw_detonate_time;
	ConVar* weapon_molotov_maxdetonateslope;
	ConVar* net_client_steamdatagram_enable_override;
	ConVar* mm_dedicated_search_maxping;
	ConVar* cl_csm_shadows;

	ConVar* developer;
	ConVar* con_enable;
	ConVar* con_filter_enable;
	ConVar* con_filter_text;
	ConVar* con_filter_text_out;
	ConVar* contimes;
	ConVar* crosshair;

	ConVar* r_drawmodelstatsoverlay;

	ConVar* sv_clockcorrection_msecs;
	ConVar* sv_max_usercmd_future_ticks;
	ConVar* sv_maxusrcmdprocessticks;
	ConVar* engine_no_focus_sleep;
	ConVar* r_3dsky;

	ConVar* host_limitlocal;

	ConVar* r_RainRadius;
	ConVar* r_rainalpha;
	ConVar* cl_crosshair_sniper_width;
	ConVar* sv_friction;
	ConVar* sv_stopspeed;

	ConVar* zoom_sensitivity_ratio_mouse;

	void Create( );
};

struct WeaponName_t {
	constexpr WeaponName_t( int32_t definition_index, const char* name ) :
		definition_index( definition_index ),
		name( name ) {
	}

	int32_t definition_index = 0;
	const char* name = nullptr;
};

extern CVariables g_Vars;
