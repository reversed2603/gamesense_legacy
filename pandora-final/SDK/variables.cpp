#include "variables.hpp"
#include "../source.hpp"
#include "../Features/Miscellaneous/KitParser.hpp"
#include "color.hpp"
std::vector<hotkey_t*> g_keybinds;

CVariables g_Vars;

Color_f Color_f::Black = Color_f( 0.0f, 0.0f, 0.0f, 1.0f );
Color_f Color_f::White = Color_f( 1.0f, 1.0f, 1.0f, 1.0f );
Color_f Color_f::Gray = Color_f( 0.75f, 0.75f, 0.75f, 1.0f );

Color_f Color_f::Lerp( const Color_f& dst, float t ) const {
   return Color_f( ( dst.r - r ) * t + r, ( dst.g - g ) * t + g, ( dst.b - b ) * t + b, ( dst.a - a ) * t + a );
}

void Color_f::SetColor( Color clr ) {
    r = static_cast< float >( clr.r( ) ) / 255.0f;
    g = static_cast< float >( clr.g( ) ) / 255.0f;
    b = static_cast< float >( clr.b( ) ) / 255.0f;
    a = static_cast< float >( clr.a( ) ) / 255.0f;
}

Color Color_f::ToRegularColor( ) {
    return Color( r * 255.f, g * 255.f, b * 255.f, a * 255.f );
}

void CVariables::Create( ) {
   // add weapons
   m_skin_changer.SetName( XorStr( "skin_changer" ) );
   for ( const auto& value : g_KitParser.weapon_skins ) {
	  auto idx = m_skin_changer.AddEntry( );
	  auto entry = m_skin_changer[ idx ];
	  entry->m_definition_index = value.id;
	  entry->SetName( value.name );
   }

   this->AddChild( &m_skin_changer );

   m_loaded_luas.SetName( XorStr( "lua_scripts" ) );
   this->AddChild( &m_loaded_luas );

   sv_accelerate = m_pCvar->FindVar( XorStr( "sv_accelerate" ) );
   sv_airaccelerate = m_pCvar->FindVar( XorStr( "sv_airaccelerate" ) );
   sv_gravity = m_pCvar->FindVar( XorStr( "sv_gravity" ) );
   sv_jump_impulse = m_pCvar->FindVar( XorStr( "sv_jump_impulse" ) );
   sv_penetration_type = m_pCvar->FindVar( XorStr( "sv_penetration_type" ) );

   m_yaw = m_pCvar->FindVar( XorStr( "m_yaw" ) );
   m_pitch = m_pCvar->FindVar( XorStr( "m_pitch" ) );
   sensitivity = m_pCvar->FindVar( XorStr( "sensitivity" ) );

   cl_sidespeed = m_pCvar->FindVar( XorStr( "cl_sidespeed" ) );
   cl_forwardspeed = m_pCvar->FindVar( XorStr( "cl_forwardspeed" ) );
   cl_upspeed = m_pCvar->FindVar( XorStr( "cl_upspeed" ) );
   cl_extrapolate = m_pCvar->FindVar( XorStr( "cl_extrapolate" ) );

   sv_noclipspeed = m_pCvar->FindVar( XorStr( "sv_noclipspeed" ) );

   weapon_recoil_scale = m_pCvar->FindVar( XorStr( "weapon_recoil_scale" ) );
   view_recoil_tracking = m_pCvar->FindVar( XorStr( "view_recoil_tracking" ) );

   r_jiggle_bones = m_pCvar->FindVar( XorStr( "r_jiggle_bones" ) );

   mp_friendlyfire = m_pCvar->FindVar( XorStr( "mp_friendlyfire" ) );

   sv_maxunlag = m_pCvar->FindVar( XorStr( "sv_maxunlag" ) );
   sv_minupdaterate = m_pCvar->FindVar( XorStr( "sv_minupdaterate" ) );
   sv_maxupdaterate = m_pCvar->FindVar( XorStr( "sv_maxupdaterate" ) );

   sv_client_min_interp_ratio = m_pCvar->FindVar( XorStr( "sv_client_min_interp_ratio" ) );
   sv_client_max_interp_ratio = m_pCvar->FindVar( XorStr( "sv_client_max_interp_ratio" ) );

   cl_interp_ratio = m_pCvar->FindVar( XorStr( "cl_interp_ratio" ) );
   cl_interp = m_pCvar->FindVar( XorStr( "cl_interp" ) );
   cl_updaterate = m_pCvar->FindVar( XorStr( "cl_updaterate" ) );

   game_type = m_pCvar->FindVar( XorStr( "game_type" ) );
   game_mode = m_pCvar->FindVar( XorStr( "game_mode" ) );

   ff_damage_bullet_penetration = m_pCvar->FindVar( XorStr( "ff_damage_bullet_penetration" ) );
   ff_damage_reduction_bullets = m_pCvar->FindVar( XorStr( "ff_damage_reduction_bullets" ) );

   mp_c4timer = m_pCvar->FindVar( XorStr( "mp_c4timer" ) );

   mp_damage_scale_ct_head = m_pCvar->FindVar( XorStr( "mp_damage_scale_ct_head" ) );
   mp_damage_scale_t_head = m_pCvar->FindVar( XorStr( "mp_damage_scale_t_head" ) );
   mp_damage_scale_ct_body = m_pCvar->FindVar( XorStr( "mp_damage_scale_ct_body" ) );
   mp_damage_scale_t_body = m_pCvar->FindVar( XorStr( "mp_damage_scale_t_body" ) );

   viewmodel_fov = m_pCvar->FindVar( XorStr( "viewmodel_fov" ) );
   viewmodel_offset_x = m_pCvar->FindVar( XorStr( "viewmodel_offset_x" ) );
   viewmodel_offset_y = m_pCvar->FindVar( XorStr( "viewmodel_offset_y" ) );
   viewmodel_offset_z = m_pCvar->FindVar( XorStr( "viewmodel_offset_z" ) );

   mat_ambient_light_r = m_pCvar->FindVar( XorStr( "mat_ambient_light_r" ) );
   mat_ambient_light_g = m_pCvar->FindVar( XorStr( "mat_ambient_light_g" ) );
   mat_ambient_light_b = m_pCvar->FindVar( XorStr( "mat_ambient_light_b" ) );

   sv_show_impacts = m_pCvar->FindVar( XorStr( "sv_showimpacts" ) );

   molotov_throw_detonate_time = m_pCvar->FindVar( XorStr( "molotov_throw_detonate_time" ) );
   weapon_molotov_maxdetonateslope = m_pCvar->FindVar( XorStr( "weapon_molotov_maxdetonateslope" ) );
   net_client_steamdatagram_enable_override = m_pCvar->FindVar( XorStr( "net_client_steamdatagram_enable_override" ) );
   mm_dedicated_search_maxping = m_pCvar->FindVar( XorStr( "mm_dedicated_search_maxping" ) );

   cl_csm_shadows = m_pCvar->FindVar( XorStr( "cl_csm_shadows" ) );

   r_drawmodelstatsoverlay = m_pCvar->FindVar(XorStr("r_drawmodelstatsoverlay"));
   host_limitlocal = m_pCvar->FindVar(XorStr("host_limitlocal"));

   sv_clockcorrection_msecs = m_pCvar->FindVar( XorStr( "sv_clockcorrection_msecs" ) );
   sv_max_usercmd_future_ticks = m_pCvar->FindVar( XorStr( "sv_max_usercmd_future_ticks" ) );
   sv_maxusrcmdprocessticks = m_pCvar->FindVar( XorStr( "sv_maxusrcmdprocessticks" ) );
   crosshair = m_pCvar->FindVar( XorStr( "crosshair" ) );
   engine_no_focus_sleep = m_pCvar->FindVar( XorStr( "engine_no_focus_sleep" ) );

   r_3dsky = m_pCvar->FindVar( XorStr( "r_3dsky" ) );
   r_RainRadius = m_pCvar->FindVar( XorStr( "r_RainRadius" ) );
   r_rainalpha = m_pCvar->FindVar( XorStr( "r_rainalpha" ) );
   cl_crosshair_sniper_width = m_pCvar->FindVar( XorStr( "cl_crosshair_sniper_width" ) );

   sv_friction = m_pCvar->FindVar( XorStr( "sv_friction" ) );
   sv_stopspeed = m_pCvar->FindVar( XorStr( "sv_stopspeed" ) );

   zoom_sensitivity_ratio_mouse = m_pCvar->FindVar( XorStr( "zoom_sensitivity_ratio_mouse" ) );

   /*developer = m_pCvar->FindVar( XorStr( "developer" ) );
   con_enable = m_pCvar->FindVar( XorStr( "con_enable" ) );
   con_filter_enable = m_pCvar->FindVar( XorStr( "con_filter_enable" ) );
   con_filter_text = m_pCvar->FindVar( XorStr( "con_filter_text" ) );
   con_filter_text_out = m_pCvar->FindVar( XorStr( "con_filter_text_out" ) );
   contimes = m_pCvar->FindVar( XorStr( "contimes" ) );*/
}