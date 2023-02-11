#include <filesystem>
#include "Elements.h"
#include "Framework/GUI.h"
#include "../Features/Miscellaneous/KitParser.hpp"
#include "../Utils/Config.hpp"
#include "../Features/Visuals/EventLogger.hpp"
#include "../source.hpp"

#include "../Features/Scripting/Scripting.hpp"

namespace Menu {

	void DrawMenu( ) {
#if defined(LUA_SCRIPTING)
		// let's make sure everything is ready first..
		if( GUI::ctx->setup ) {
			Scripting::init( );
		}
#endif

		std::string windowName = XorStr( "pandora" );
#ifdef DEV
		windowName = XorStr( "pandora debug" );
#else
#ifdef BETA_MODE
		windowName = XorStr( "pandora beta" );
#endif
#endif
		if( GUI::Form::BeginWindow( windowName ) || GUI::ctx->setup ) {
			if( GUI::Form::BeginTab( 0, XorStr( "A" ) ) || GUI::ctx->setup ) {
				GUI::Group::BeginGroup( XorStr( "General" ), Vector2D( 50, 62 ) );
				{
					GUI::Controls::Checkbox( XorStr( "Enabled##rage" ), &g_Vars.rage.enabled );
					GUI::Controls::Hotkey( XorStr( "Enabled key##rage" ), &g_Vars.rage.key );

					std::vector<MultiItem_t> hitboxes{
						{ XorStr( "Head" ), &g_Vars.rage_default.hitbox_head },
						{ XorStr( "Neck" ), &g_Vars.rage_default.hitbox_neck },
						{ XorStr( "Chest" ), &g_Vars.rage_default.hitbox_chest },
						{ XorStr( "Stomach" ), &g_Vars.rage_default.hitbox_stomach },
						{ XorStr( "Pelvis" ), &g_Vars.rage_default.hitbox_pelvis },
						{ XorStr( "Arms" ), &g_Vars.rage_default.hitbox_arms },
						{ XorStr( "Legs" ), &g_Vars.rage_default.hitbox_legs },
						{ XorStr( "Feet" ), &g_Vars.rage_default.hitbox_feet },
					};

					//std::vector<MultiItem_t> multipoints{
					//	{ XorStr( "Head" ), &g_Vars.rage_default.multipoint_head },
					//	{ XorStr( "Upper chest" ), &g_Vars.rage_default.multipoint_upper_chest },
					//	{ XorStr( "Chest" ), &g_Vars.rage_default.multipoint_chest },
					//	{ XorStr( "Lower Chest" ), &g_Vars.rage_default.multipoint_lower_chest },
					//	{ XorStr( "Stomach" ), &g_Vars.rage_default.multipoint_stomach },
					//	{ XorStr( "Legs" ), &g_Vars.rage_default.multipoint_legs },
					//	{ XorStr( "Feet" ), &g_Vars.rage_default.multipoint_feet },
					//};

					GUI::Controls::MultiDropdown( XorStr( "Target hitbox" ), hitboxes );
					GUI::Controls::Dropdown( XorStr( "Multi-point" ), { XorStr( "Off" ), XorStr( "Low" ), XorStr( "Medium" ), XorStr( "High" ), XorStr( "Vitals" ) }, &g_Vars.rage_default.multipoint );

					if( g_Vars.rage_default.multipoint > 0 )
						GUI::Controls::Slider( XorStr( "##Point scale" ), &g_Vars.rage_default.pointscale, 1.f, 100.f, XorStr( "%.0f%%" ) );

					GUI::Controls::Slider( XorStr( "Stomach scale" ), &g_Vars.rage_default.stomach_scale, 1.f, 100.f, XorStr( "%.0f%%" ) );

					GUI::Controls::Checkbox( XorStr( "Automatic fire" ), &g_Vars.rage.auto_fire );
					GUI::Controls::Checkbox( XorStr( "Automatic penetration" ), &g_Vars.rage_default.wall_penetration );
					GUI::Controls::Checkbox( XorStr( "Silent aim" ), &g_Vars.rage.silent_aim );
					if( GUI::Controls::Checkbox( XorStr( "Minimum hit chance" ), &g_Vars.rage_default.hitchance ) ) {
						GUI::Controls::Slider( XorStr( "##sdasdsa" ), &g_Vars.rage_default.hitchance_amount, 1.f, 100.f, XorStr( "%.0f%%" ) );
					}

					auto GetDmgDisplay = [ & ] ( int dmg ) -> std::string {
						return dmg == 0 ? XorStr( "Auto" ) : dmg > 100 ? ( std::string( XorStr( "HP + " ) ).append( std::string( std::to_string( dmg - 100 ) ) ) ) : std::string( XorStr( "%d" ) );
					};

					GUI::Controls::Slider( XorStr( "Minimum damage" ), &g_Vars.rage_default.minimal_damage, 0, 125, GetDmgDisplay( g_Vars.rage_default.minimal_damage ), 1, true );
					GUI::Controls::Checkbox( XorStr( "Override AWP" ), &g_Vars.rage_default.override_awp );
					GUI::Controls::Checkbox( XorStr( "Automatic scope" ), &g_Vars.rage_default.auto_scope );
					GUI::Controls::Checkbox( XorStr( "Quick peek assist" ), &g_Vars.misc.autopeek );
					GUI::Controls::Hotkey( XorStr( "Quick peek assist key" ), &g_Vars.misc.autopeek_bind );
					GUI::Controls::Checkbox( XorStr( "Reduce aim step" ), &g_Vars.rage.aimstep );

					GUI::Controls::Checkbox( XorStr( "Log misses due to spread" ), &g_Vars.esp.event_resolver );
				}
				GUI::Group::EndGroup( );

				GUI::Group::BeginGroup( "Fake lag", Vector2D( 50, 38 ) );
				{
					GUI::Controls::Checkbox( XorStr( "Enabled##fakeKURWAlag" ), &g_Vars.rage.fake_lag );
					std::vector<MultiItem_t> fakelag_cond = {
						{ XorStr( "Standing" ), &g_Vars.rage.fake_lag_standing },
						{ XorStr( "Moving" ), &g_Vars.rage.fake_lag_moving },
						{ XorStr( "In air" ), &g_Vars.rage.fake_lag_air },
					};
					GUI::Controls::MultiDropdown( XorStr( "Triggers" ), fakelag_cond );
					GUI::Controls::Dropdown( XorStr( "##type" ), { XorStr( "Maximum" ), XorStr( "Dynamic" ), XorStr( "Fluctuate" ) }, &g_Vars.rage.fake_lag_type );
					GUI::Controls::Slider( XorStr( "Variance" ), &g_Vars.rage.fake_lag_variance, 0.0f, 100.0f, XorStr( "%.0f%%" ) );
					GUI::Controls::Slider( XorStr( "Limit" ), &g_Vars.rage.fake_lag_amount, 0, 14 );

					GUI::Controls::Checkbox( XorStr( "Fake lag while unducking" ), &g_Vars.rage.fake_lag_unduck );
				}
				GUI::Group::EndGroup( );

				GUI::Group::SetNextSize( Vector2D( -1, 200 ) );
				GUI::Group::BeginGroup( "Other", Vector2D( 50, 42 ) );
				{
					GUI::Controls::Checkbox( XorStr( "Remove spread" ), &g_Vars.rage.remove_spread, true );
					GUI::Controls::Checkbox( XorStr( "Remove recoil" ), &g_Vars.rage.remove_recoil );

					GUI::Controls::Dropdown( XorStr( "Accuracy boost" ), { XorStr( "Off" ), XorStr( "Low" ), XorStr( "Medium" ), XorStr( "High" ) }, &g_Vars.rage.accuracy_boost );

					GUI::Controls::Dropdown( XorStr( "Quick stop" ), { XorStr( "Off" ), XorStr( "On + slide" ), XorStr( "On + Duck" ) }, &g_Vars.rage_default.quick_stop );
					GUI::Controls::Hotkey( XorStr( "Quick stop key" ), &g_Vars.rage_default.quick_stop_key );

					if( GUI::Controls::Checkbox( XorStr( "Anti-aim correction" ), &g_Vars.rage.antiaim_correction ) ) {
						GUI::Controls::Checkbox( XorStr( "Anti-aim resolver" ), &g_Vars.rage.antiaim_resolver );
						GUI::Controls::Checkbox( XorStr( "Anti-aim resolver override" ), &g_Vars.rage.antiaim_resolver_override );
						GUI::Controls::Hotkey( XorStr( "Anti-aim resolver override##key" ), &g_Vars.rage.antiaim_correction_override );
					}

					GUI::Controls::Dropdown( XorStr( "Fake lag correction" ), { XorStr( "Default" ), XorStr( "Simulate" ) }, &g_Vars.rage.fakelag_correction );

					GUI::Controls::Dropdown( XorStr( "Prefer body aim" ), { XorStr( "Off" ), XorStr( "Fake angles" ), XorStr( "Always" ), XorStr( "Aggressive" ), XorStr( "High inaccuracy" ) }, &g_Vars.rage_default.prefer_body );
					GUI::Controls::Hotkey( XorStr( "Prefer body aim key##key" ), &g_Vars.rage.force_baim );

					//GUI::Controls::Label(XorStr("Double tap"));
					//GUI::Controls::Hotkey(XorStr("Double tap key##key"), &g_Vars.rage.double_tap_bind);

					GUI::Controls::Checkbox( XorStr( "Delay shot on unduck" ), &g_Vars.rage.delay_shot );
				}
				GUI::Group::EndGroup( );
				GUI::Group::PopLastSize( );

				GUI::Group::SetNextSize( Vector2D( -1, 275 ) );
				GUI::Group::BeginGroup( "Anti-aimbot", Vector2D( 50, 58 ) );
				{
					GUI::Controls::Dropdown( XorStr( "Pitch" ), { XorStr( "Off" ), XorStr( "Down" ), XorStr( "Up" ), XorStr( "Zero" ) }, &g_Vars.rage.anti_aim_base_pitch );

					GUI::Controls::Dropdown( XorStr( "Yaw" ), {
						XorStr( "Off" ), XorStr( "180" ), XorStr( "180 Jitter" ), XorStr( "Jitter" ),
						XorStr( "Spin" ), XorStr( "Sideways" ), XorStr( "Random" ), XorStr( "Static" ),
						XorStr( "180 Z" ), }, &g_Vars.rage.anti_aim_yaw );
					if( g_Vars.rage.anti_aim_yaw == 2 || g_Vars.rage.anti_aim_yaw == 3 ) {
						GUI::Controls::Slider( XorStr( "##jitter distance" ), &g_Vars.rage.anti_aim_yaw_jitter, 0, 180.f, XorStr( "%.0f°" ), 1, true );
					}
					else if( g_Vars.rage.anti_aim_yaw == 4 ) {
						GUI::Controls::Slider( XorStr( "##spin speed" ), &g_Vars.rage.anti_aim_yaw_spin_speed, 0, 10.f, XorStr( "%.0f%%" ), 1, true );
						GUI::Controls::Slider( XorStr( "##jitter distance" ), &g_Vars.rage.anti_aim_yaw_spin_direction, -180.f, 180.f, XorStr( "%.0f°" ), 1, true );
					}
					else if( g_Vars.rage.anti_aim_yaw == 5 || g_Vars.rage.anti_aim_yaw == 7 ) {
						GUI::Controls::Slider( XorStr( "##additiveyaw" ), &g_Vars.rage.anti_aim_base_yaw_additive, -180.f, 180.f, XorStr( "%.0f°" ), 1, true );
					}

					// idk what other yaws skeet had for running
					GUI::Controls::Dropdown( XorStr( "Yaw while running" ), { XorStr( "Off" ), XorStr( "180 Jitter" ) }, &g_Vars.rage.anti_aim_yaw_while_running );

					GUI::Controls::Dropdown( XorStr( "Fake yaw" ), { XorStr( "Off" ), XorStr( "Default" ), XorStr( "Local view" ), XorStr( "Opposite" ) }, &g_Vars.rage.anti_aim_fake_type );
					//GUI::Controls::Slider( XorStr( "Fake yaw strength" ), &g_Vars.rage.anti_aim_fake_yaw_strength, -180.f, 180.f, XorStr( "%.0f°" ), 1, true );

					std::vector<MultiItem_t> freestanding_cond = {
						{ XorStr( "Default" ), &g_Vars.rage.anti_aim_freestand_default },
						{ XorStr( "Running" ), &g_Vars.rage.anti_aim_freestand_running },
						{ XorStr( "Edge" ), &g_Vars.rage.anti_aim_freestand_edge },
					};
					GUI::Controls::MultiDropdown( XorStr( "Freestanding" ), freestanding_cond );
					if( g_Vars.rage.anti_aim_freestand_default )
						GUI::Controls::Slider( XorStr( "##additivefreestand" ), &g_Vars.rage.anti_aim_freestand_delta, 0, 180.f, XorStr( "%.0f°" ), 1, true );

					GUI::Controls::Dropdown( XorStr( "Edge" ), { XorStr( "Off" ), XorStr( "Static" ), XorStr( "Subtle" ) }, &g_Vars.rage.anti_aim_edge );
					if( g_Vars.rage.anti_aim_edge > 0 )
						GUI::Controls::Slider( XorStr( "##additiveedge" ), &g_Vars.rage.anti_aim_edge_delta, 0, 180.f, XorStr( "%.0f°" ), 1, true );

					GUI::Controls::Dropdown( XorStr( "Yaw base" ), { XorStr( "Local view" ), XorStr( "At crosshair" ), XorStr( "At distance" ) }, &g_Vars.rage.anti_aim_base_yaw );

					GUI::Controls::Checkbox( XorStr( "Crooked" ), &g_Vars.rage.anti_aim_crooked );
					GUI::Controls::Checkbox( XorStr( "Twist" ), &g_Vars.rage.anti_aim_twist );

					GUI::Controls::Label( XorStr( "Manual left" ) );
					GUI::Controls::Hotkey( XorStr( "Manual left key" ), &g_Vars.rage.anti_aim_left_key );

					GUI::Controls::Label( XorStr( "Manual backwards" ) );
					GUI::Controls::Hotkey( XorStr( "Manual backwards key" ), &g_Vars.rage.anti_aim_back_key );

					GUI::Controls::Label( XorStr( "Manual right" ) );
					GUI::Controls::Hotkey( XorStr( "Manual right key" ), &g_Vars.rage.anti_aim_right_key );

					GUI::Controls::Checkbox(XorStr("Break walk animation"), &g_Vars.rage.anti_aim_break_walk);

					//
					//GUI::Controls::Slider( XorStr( "test1" ), &g_Vars.rage.test2, -180, 180.f, XorStr( "%.0f°" ), 1, true );
					//GUI::Controls::Slider( XorStr( "test2" ), &g_Vars.rage.test3, -180, 180.f, XorStr( "%.0f°" ), 1, true );
					//GUI::Controls::Hotkey( XorStr( "test3" ), &g_Vars.rage.test1 );
				}
				GUI::Group::EndGroup( );
				GUI::Group::PopLastSize( );
			}

			if( GUI::Form::BeginTab( 1, XorStr( "B" ) ) || GUI::ctx->setup ) {

			}

			if( GUI::Form::BeginTab( 2, XorStr( "C" ) ) || GUI::ctx->setup ) {
				GUI::Group::SetNextSize( Vector2D( -1, 325 ) );
				GUI::Group::BeginGroup( "Player ESP", Vector2D( 50, 69 ) );
				{
					CVariables::PLAYER_VISUALS* visuals = &g_Vars.visuals_enemy;

					if( visuals ) {
						if( GUI::Controls::Checkbox( XorStr( "Teammates" ), &visuals->teammates ) );

						if( GUI::Controls::Checkbox( XorStr( "Dormant" ), &visuals->dormant ) );

						if( GUI::Controls::Checkbox( XorStr( "Bounding box" ), &visuals->box ) );
						GUI::Controls::ColorPicker( XorStr( "Box color" ), &visuals->box_color );

						GUI::Controls::Checkbox( XorStr( "Health bar" ), &visuals->health );

						if( GUI::Controls::Checkbox( XorStr( "Name ESP" ), &visuals->name ) );
						GUI::Controls::ColorPicker( XorStr( "Name color" ), &visuals->name_color );

						if( GUI::Controls::Checkbox( XorStr( "Flags" ), &visuals->flags ) );

						if( GUI::Controls::Checkbox( XorStr( "Weapon text" ), &visuals->weapon ) );
						GUI::Controls::Checkbox( XorStr( "Weapon icon" ), &visuals->weapon_icon );
						GUI::Controls::ColorPicker( XorStr( "Weapon color" ), &visuals->weapon_color );

						if( GUI::Controls::Checkbox( XorStr( "Ammo" ), &visuals->ammo ) );
						GUI::Controls::ColorPicker( XorStr( "Ammo color" ), &visuals->ammo_color );

						if( GUI::Controls::Checkbox( XorStr( "Distance" ), &visuals->distance ) );

						if( GUI::Controls::Checkbox( XorStr( "LBY timer" ), &visuals->lby_timer ) );
						GUI::Controls::ColorPicker( XorStr( "LBY Timer color" ), &visuals->lby_timer_color );

						if( GUI::Controls::Checkbox( XorStr( "Glow" ), &visuals->glow ) );
						GUI::Controls::ColorPicker( XorStr( "Glow color" ), &visuals->glow_color );

						if( GUI::Controls::Checkbox( XorStr( "Hit marker" ), &g_Vars.esp.vizualize_hitmarker ) );
						if( GUI::Controls::Checkbox( XorStr( "Hit marker sound" ), &g_Vars.misc.hitsound ) );

						GUI::Controls::Checkbox( XorStr( "Visualize sounds" ), &g_Vars.esp.visualize_sounds );
						GUI::Controls::ColorPicker( XorStr( "Visualize sounds color" ), &g_Vars.esp.visualize_sounds_color );

						GUI::Controls::Checkbox( XorStr( "Money" ), &visuals->money );

						if( GUI::Controls::Checkbox( XorStr( "Skeleton" ), &visuals->skeleton ) );
						GUI::Controls::ColorPicker( XorStr( "Skeleton color" ), &visuals->skeleton_color );

						if( GUI::Controls::Checkbox( XorStr( "Out of POV arrow" ), &visuals->view_arrows ) )
							GUI::Controls::ColorPicker( XorStr( "Out of POV arrow color" ), &visuals->view_arrows_color );
						if( visuals->view_arrows ) {
							GUI::Controls::Slider( XorStr( "Out of POV arrow distance" ), &visuals->view_arrows_distance, 10.f, 100.f, XorStr( "%.0f%%" ) );
							GUI::Controls::Slider( XorStr( "Out of POV arrow size" ), &visuals->view_arrows_size, 6.f, 24.f, XorStr( "%.0fpx" ) );
						}
					}
				}
				GUI::Group::EndGroup( );
				GUI::Group::PopLastSize( );

				GUI::Group::SetNextSize( Vector2D( -1, 150 ) );
				GUI::Group::BeginGroup( "Colored models", Vector2D( 50, 31 ) );
				{
					std::vector<std::string> materials = {
						XorStr( "Default" ), XorStr( "Solid" )
					};

					GUI::Controls::Dropdown( XorStr( "##mat" ), materials, &g_Vars.esp.chams_material );
					GUI::Controls::Checkbox( XorStr( "Player" ), &g_Vars.esp.chams_player );
					GUI::Controls::ColorPicker( XorStr( "Player color" ), &g_Vars.esp.chams_player_color );

					GUI::Controls::Checkbox( XorStr( "Player (behind wall)" ), &g_Vars.esp.chams_player_wall );
					GUI::Controls::ColorPicker( XorStr( "Player wall color" ), &g_Vars.esp.chams_player_wall_color );

					GUI::Controls::Slider( XorStr( "Reflectivity" ), &g_Vars.esp.chams_reflectivity, 0.f, 100.f, XorStr( "\n" ), 10.f, true );
					GUI::Controls::ColorPicker( XorStr( "Reflectivity color" ), &g_Vars.esp.chams_reflectivity_color );

					GUI::Controls::Slider( XorStr( "Shine" ), &g_Vars.esp.chams_shine, 0.f, 100.f, XorStr( "\n" ) );
					GUI::Controls::Slider( XorStr( "Rim" ), &g_Vars.esp.chams_rim, 0.f, 100.f, XorStr( "\n" ) );

					GUI::Controls::Checkbox( XorStr( "Show teammates" ), &g_Vars.esp.chams_teammates );
					GUI::Controls::ColorPicker( XorStr( "Show teammates color" ), &g_Vars.esp.chams_teammates_color );

					GUI::Controls::Checkbox( XorStr( "Shadow" ), &g_Vars.esp.chams_shadow );
					GUI::Controls::ColorPicker( XorStr( "Shadow color" ), &g_Vars.esp.chams_shadow_color );
				}
				GUI::Group::EndGroup( );
				GUI::Group::PopLastSize( );

				GUI::Group::SetNextSize( Vector2D( -1, 200 ) );
				GUI::Group::BeginGroup( XorStr( "Other ESP" ), Vector2D( 50, 40 ) );
				{
					GUI::Controls::Checkbox( XorStr( "Radar" ), &g_Vars.misc.ingame_radar );

					GUI::Controls::Dropdown( XorStr( "Dropped weapons" ), { XorStr( "Off" ), XorStr( "Text" ) }, &g_Vars.esp.dropped_weapons );
					GUI::Controls::ColorPicker( XorStr( "Dropped weapons color" ), &g_Vars.esp.dropped_weapons_color );

					GUI::Controls::Checkbox( XorStr( "Dropped weapon ammo" ), &g_Vars.esp.dropped_weapons_ammo );

					GUI::Controls::Checkbox( XorStr( "Grenades" ), &g_Vars.esp.grenades );
					GUI::Controls::ColorPicker( XorStr( "Grenades clr" ), &g_Vars.esp.grenades_color );

					GUI::Controls::Checkbox( XorStr( "Crosshair" ), &g_Vars.esp.force_sniper_crosshair );

					GUI::Controls::Checkbox( XorStr( "Bomb" ), &g_Vars.esp.draw_bomb );
					GUI::Controls::ColorPicker( XorStr( "Bomb clr" ), &g_Vars.esp.draw_bomb_color );

					if( GUI::Controls::Checkbox( XorStr( "Grenade trajectory" ), &g_Vars.esp.grenade_prediction ) );
					GUI::Controls::ColorPicker( XorStr( "Grenade trajectory color" ), &g_Vars.esp.grenade_prediction_color );

					GUI::Controls::Checkbox( XorStr( "Spectators" ), &g_Vars.misc.spectators );

					GUI::Controls::Checkbox( XorStr( "Penetration reticle" ), &g_Vars.esp.autowall_crosshair );
				}
				GUI::Group::EndGroup( );
				GUI::Group::PopLastSize( );

				GUI::Group::SetNextSize( Vector2D( -1, 275 ) );
				GUI::Group::BeginGroup( XorStr( "Effects" ), Vector2D( 50, 60 ) );
				{
					GUI::Controls::Checkbox( XorStr( "Remove flashbang effects" ), &g_Vars.esp.remove_flash );
					GUI::Controls::Checkbox( XorStr( "Remove smoke grenades" ), &g_Vars.esp.remove_smoke );
					GUI::Controls::Checkbox( XorStr( "Remove fog" ), &g_Vars.esp.remove_fog );

					std::vector<MultiItem_t> removals{
						{ XorStr( "Recoil shake" ), &g_Vars.esp.remove_recoil_shake },
						{ XorStr( "Recoil punch" ), &g_Vars.esp.remove_recoil_punch },
					};

					GUI::Controls::MultiDropdown( XorStr( "Visual recoil adjustment" ), removals );

					GUI::Controls::Slider( XorStr( "Transparent walls" ), &g_Vars.esp.transparent_walls, 0.f, 100.f, XorStr( "%.0f%%" ) );
					GUI::Controls::Slider( XorStr( "Transparent props" ), &g_Vars.esp.transparent_props, 0.f, 100.f, XorStr( "%.0f%%" ) );

					GUI::Controls::Dropdown( XorStr( "Brightness adjustment" ), { XorStr( "Off" ), XorStr( "Night mode" ), XorStr( "Full bright" ) }, &g_Vars.esp.brightness_adjustment );
					GUI::Controls::Checkbox( XorStr( "Remove scope overlay" ), &g_Vars.esp.remove_scope );
					GUI::Controls::Checkbox( XorStr( "Instant scope" ), &g_Vars.esp.instant_scope );
					GUI::Controls::Checkbox( XorStr( "Disable post processing" ), &g_Vars.esp.remove_post_proccesing );

					GUI::Controls::Checkbox( XorStr( "Force third person (alive)" ), &g_Vars.misc.third_person );
					GUI::Controls::Hotkey( XorStr( "Third person key" ), &g_Vars.misc.third_person_bind );
					GUI::Controls::Checkbox( XorStr( "Force third person (dead)" ), &g_Vars.misc.first_person_dead );

					GUI::Controls::Checkbox( XorStr( "Disable rendering of teammates" ), &g_Vars.esp.disable_teammate_rendering );

					GUI::Controls::Checkbox( XorStr( "Bullet tracers" ), &g_Vars.esp.bullet_beams );
					GUI::Controls::ColorPicker( XorStr( "Bullet tracers color" ), &g_Vars.esp.bullet_beams_color );

					GUI::Controls::Checkbox( XorStr( "Bullet impacts" ), &g_Vars.esp.server_impacts );
				}
				GUI::Group::EndGroup( );
				GUI::Group::PopLastSize( );
			}

			static bool on_cfg_load_gloves, on_cfg_load_knives;
			if( GUI::Form::BeginTab( 3, XorStr( "D" ) ) || GUI::ctx->setup ) {
				GUI::Group::BeginGroup( XorStr( "Miscellaneous" ), Vector2D( 50, 100 ) );
				GUI::Controls::Slider( XorStr( "Override FOV" ), &g_Vars.esp.world_fov, 60.f, 140.f, XorStr( "%.0f°" ) );
				GUI::Controls::Checkbox( XorStr( "Bunny hop" ), &g_Vars.misc.bunnyhop );
				if( GUI::Controls::Checkbox( XorStr( "Air strafe" ), &g_Vars.misc.autostrafer ) ) {
					GUI::Controls::Checkbox( XorStr( "Movement keys" ), &g_Vars.misc.autostrafer_wasd );
				}

				GUI::Controls::Dropdown( XorStr( "Air duck" ), { XorStr( "Off" ), XorStr( "Default" ) }, &g_Vars.misc.air_duck );
				GUI::Controls::Checkbox( XorStr( "Knifebot" ), &g_Vars.misc.knifebot );
				GUI::Controls::Checkbox( XorStr( "Zeusbot" ), &g_Vars.misc.zeus_bot );
				GUI::Controls::Checkbox( XorStr( "Quick stop" ), &g_Vars.misc.quickstop );
				GUI::Controls::Checkbox( XorStr( "Automatic weapons" ), &g_Vars.misc.auto_weapons );
				GUI::Controls::Checkbox( XorStr( "Jump at edge" ), &g_Vars.misc.edge_jump );
				GUI::Controls::Hotkey( XorStr( "Jump at edge key" ), &g_Vars.misc.edge_jump_key );
				GUI::Controls::Checkbox( XorStr( "Slow motion" ), &g_Vars.misc.fakewalk );
				GUI::Controls::Hotkey( XorStr( "Slow motion key" ), &g_Vars.misc.fakewalk_bind );
				if( g_Vars.misc.fakewalk ) {
					GUI::Controls::Slider( XorStr( "##Slow motion speed" ), &g_Vars.misc.slow_motion_speed, 0.f, 100.f, XorStr( "%.0f%%" ) );
				}
				GUI::Controls::Checkbox( XorStr( "Reveal competitive ranks" ), &g_Vars.misc.reveal_ranks );
				GUI::Controls::Checkbox( XorStr( "Auto-accept matchmaking" ), &g_Vars.misc.auto_accept );
				GUI::Controls::Checkbox( XorStr( "Clantag spammer" ), &g_Vars.misc.clantag_changer );

				GUI::Controls::Checkbox( XorStr( "Log weapon purchases" ), &g_Vars.esp.event_buy );
				GUI::Controls::Checkbox( XorStr( "Log damage dealt" ), &g_Vars.esp.event_dmg );


				if( GUI::Controls::Checkbox( XorStr( "Persistent kill feed" ), &g_Vars.esp.preserve_killfeed ) ) {
					// note - alpha;
					// maybe make a dropdown, with options such as:
					// "extend", "preserve" where if u have extend 
					// you can choose how long, and if u have preserve
					// just force this to FLT_MAX or smth? idk.
					g_Vars.esp.preserve_killfeed_time = 300.f;
				}

				GUI::Group::EndGroup( );

				GUI::Group::SetNextSize( Vector2D( -1, 100 ) );
				GUI::Group::BeginGroup( XorStr( "Settings" ), Vector2D( 50, 25 ) );
				{
					GUI::Controls::Label( XorStr( "Menu key" ) );
					GUI::Controls::Hotkey( XorStr( "Menu key###key" ), &g_Vars.menu.key );

					GUI::Controls::Label( XorStr( "Menu color" ) );
					GUI::Controls::ColorPicker( XorStr( "Menu color###color" ), &g_Vars.menu.ascent );

					GUI::Controls::Checkbox( XorStr( "Anti-untrusted" ), &g_Vars.misc.anti_untrusted );

					static bool f1, f2, f3;
					GUI::Controls::Checkbox( XorStr( "Hide from OBS" ), &f1 );
					GUI::Controls::Checkbox( XorStr( "Low FPS warning" ), &f2 );
				}
				GUI::Group::EndGroup( );
				GUI::Group::PopLastSize( );

				GUI::Group::SetNextSize( Vector2D( -1, 185 ) );
				GUI::Group::BeginGroup( XorStr( "Presets" ), Vector2D( 50, 25 ) );
				{
					static int selected_cfg;
					static std::vector<std::string> cfg_list;
					static bool initialise_configs = true;
					bool reinit = false;
					if( initialise_configs || ( GetTickCount( ) % 1000 ) == 0 ) {
						cfg_list = ConfigManager::GetConfigs( );
						initialise_configs = false;
						reinit = true;
					}

					static std::string config_name;
					GUI::Controls::Textbox( XorStr( "Config name" ), &config_name, 26 );
					GUI::Controls::Dropdown( XorStr( "#Config preset" ),
						( cfg_list.empty( ) ? std::vector<std::string>{XorStr( "-" )} : cfg_list ), &selected_cfg );

					if( reinit ) {
						if( selected_cfg >= cfg_list.size( ) )
							selected_cfg = cfg_list.size( ) - 1;

						if( selected_cfg < 0 )
							selected_cfg = 0;
					}

					if( !cfg_list.empty( ) ) {
						static bool confirm_save = false;
						GUI::Controls::Button( XorStr( "Load" ), [ & ] ( ) {
							if( selected_cfg <= cfg_list.size( ) && selected_cfg >= 0 ) {
								ConfigManager::ResetConfig( );

								ConfigManager::LoadConfig( cfg_list.at( selected_cfg ) );
								//g_Vars.m_global_skin_changer.m_update_skins = true;
								g_Vars.m_global_skin_changer.m_update_gloves = true;

								GUI::ctx->SliderInfo.LastChangeTime.clear( );
								GUI::ctx->SliderInfo.PreviewAnimation.clear( );
								GUI::ctx->SliderInfo.PreviousAmount.clear( );
								GUI::ctx->SliderInfo.ShouldChangeValue.clear( );
								GUI::ctx->SliderInfo.ValueTimer.clear( );
								GUI::ctx->SliderInfo.ValueAnimation.clear( );

								on_cfg_load_knives = on_cfg_load_gloves = true;

								// reset keybinds .
								for( auto& key : g_keybinds ) {
									key->enabled = false;
								}
							}
						} );

						GUI::Controls::Button( XorStr( "Save" ), [ & ] ( ) {
							if( selected_cfg <= cfg_list.size( ) && selected_cfg >= 0 ) {
								ConfigManager::SaveConfig( cfg_list.at( selected_cfg ) );

								GUI::ctx->SliderInfo.LastChangeTime.clear( );
								GUI::ctx->SliderInfo.PreviewAnimation.clear( );
								GUI::ctx->SliderInfo.PreviousAmount.clear( );
								GUI::ctx->SliderInfo.ShouldChangeValue.clear( );
								GUI::ctx->SliderInfo.ValueTimer.clear( );
								GUI::ctx->SliderInfo.ValueAnimation.clear( );
							}
						} );
					}

					GUI::Controls::Button( XorStr( "Reset" ), [ & ] ( ) {
						ConfigManager::ResetConfig( );
					} );

					GUI::Controls::Button( XorStr( "Create config" ), [ & ] ( ) {
						if( config_name.empty( ) )
							return;

						ConfigManager::CreateConfig( config_name );
						cfg_list = ConfigManager::GetConfigs( );

						config_name.clear( );
					} );

					GUI::Controls::Button( XorStr( "Import from clipboard" ), [ & ] ( ) {
						ConfigManager::ImportFromClipboard( );
					} );

					GUI::Controls::Button( XorStr( "Export to clipboard" ), [ & ] ( ) {
						ConfigManager::ExportToClipboard( );
					} );

				}
				GUI::Group::EndGroup( );
				GUI::Group::PopLastSize( );

				GUI::Group::SetNextSize( Vector2D( -1, 170 ) );
				GUI::Group::BeginGroup( XorStr( "Other" ), Vector2D( 50, 25 ) );
				{

					GUI::Controls::Button( XorStr( "Steal player name" ), [ & ] ( ) { } );
					GUI::Controls::Button( XorStr( "Unlock hidden convars" ), [ & ] ( ) {
						auto p = **reinterpret_cast< ConVar*** >( ( uint32_t )m_pCvar.Xor( ) + 0x34 );
						for( ConVar* c = p->pNext; c != nullptr; c = c->pNext ) {
							c->nFlags &= ~( 1 << 4 );
							c->nFlags &= ~( 1 << 1 );
						}
					} );

					GUI::Controls::Button( XorStr( "Unload" ), [ & ] ( ) {
						g_Vars.globals.hackUnload = true;
					} );
				}
				GUI::Group::EndGroup( );
				GUI::Group::PopLastSize( );
			}

			if( GUI::Form::BeginTab( 4, XorStr( "E" ) ) || GUI::ctx->setup ) {
				static int weapon_id;
				GUI::Group::SetNextSize( Vector2D( -1, 100 ) );
				GUI::Group::BeginGroup( XorStr( "Knife options" ), Vector2D( 50, 32 ) );
				{
					std::vector<std::string> knifes;
					GUI::Controls::Checkbox( XorStr( "Override knife##knife" ), &g_Vars.m_global_skin_changer.m_knife_changer );

					if( g_KitParser.k_knife_names.at( g_Vars.m_global_skin_changer.m_knife_vector_idx ).definition_index != g_Vars.m_global_skin_changer.m_knife_idx ) {
						auto it = std::find_if( g_KitParser.k_knife_names.begin( ), g_KitParser.k_knife_names.end( ), [ & ] ( const WeaponName_t& a ) {
							return a.definition_index == g_Vars.m_global_skin_changer.m_knife_idx;
						} );

						if( on_cfg_load_knives ) {
							if( it != g_KitParser.k_knife_names.end( ) )
								g_Vars.m_global_skin_changer.m_knife_vector_idx = std::distance( g_KitParser.k_knife_names.begin( ), it );

							on_cfg_load_knives = false;
						}
					}

					static bool init_knife_names = false;
					for( int i = 0; i < g_KitParser.k_knife_names.size( ); ++i ) {
						auto currentKnife = g_KitParser.k_knife_names[ i ];
						knifes.push_back( currentKnife.name );
					}

					static int bruh = g_Vars.m_global_skin_changer.m_knife_vector_idx;

					if( bruh != g_Vars.m_global_skin_changer.m_knife_vector_idx ) {
						g_Vars.m_global_skin_changer.m_update_gloves = true;

						bruh = g_Vars.m_global_skin_changer.m_knife_vector_idx;
					}

					if( !knifes.empty( ) ) {
						GUI::Controls::Dropdown( XorStr( "Knife models" ), knifes, &g_Vars.m_global_skin_changer.m_knife_vector_idx );
						g_Vars.m_global_skin_changer.m_knife_idx = g_KitParser.k_knife_names[ g_Vars.m_global_skin_changer.m_knife_vector_idx ].definition_index;
					}

					knifes.clear( );
				}
				GUI::Group::EndGroup( );
				GUI::Group::PopLastSize( );

				GUI::Group::SetNextSize( Vector2D( -1, 375 ) );
				GUI::Group::BeginGroup( XorStr( "Glove options" ), Vector2D( 50, 68 ) ); {
					for( int i = 0; i < g_KitParser.weapon_skins.size( ); ++i ) {
						auto whatevertheFUCK = g_KitParser.weapon_skins[ i ];

						if( g_Vars.globals.m_iWeaponIndex == whatevertheFUCK.id )
							g_Vars.globals.m_iWeaponIndexSkins = i;
					}

					static bool bOldGlove = g_Vars.m_global_skin_changer.m_glove_changer;
					static int bruh = g_Vars.m_global_skin_changer.m_gloves_vector_idx;
					GUI::Controls::Checkbox( XorStr( "Override glove model##glove" ), &g_Vars.m_global_skin_changer.m_glove_changer );

					if( g_KitParser.k_glove_names.at( g_Vars.m_global_skin_changer.m_gloves_vector_idx ).definition_index != g_Vars.m_global_skin_changer.m_gloves_idx ) {
						auto it = std::find_if( g_KitParser.k_glove_names.begin( ), g_KitParser.k_glove_names.end( ), [ & ] ( const WeaponName_t& a ) {
							return a.definition_index == g_Vars.m_global_skin_changer.m_gloves_idx;
						} );

						if( on_cfg_load_gloves ) {
							if( it != g_KitParser.k_glove_names.end( ) )
								g_Vars.m_global_skin_changer.m_gloves_vector_idx = std::distance( g_KitParser.k_glove_names.begin( ), it );
						}
					}

					static std::vector<std::string> gloves;
					for( int i = 0; i < g_KitParser.k_glove_names.size( ); ++i ) {
						auto whatevertheFUCK = g_KitParser.k_glove_names[ i ];
						gloves.push_back( whatevertheFUCK.name );
					}

					if( !gloves.empty( ) ) {
						GUI::Controls::Dropdown( XorStr( "Glove model" ), gloves, &g_Vars.m_global_skin_changer.m_gloves_vector_idx );
						g_Vars.m_global_skin_changer.m_gloves_idx = g_KitParser.k_glove_names[ g_Vars.m_global_skin_changer.m_gloves_vector_idx ].definition_index;

						static std::vector<std::string> paint_kits;

						int currentGloveID = 0;
						for( int i = 0; i < g_KitParser.weapon_skins.size( ); ++i ) {
							auto currentGlove = g_KitParser.weapon_skins[ i ];

							if( currentGlove.id == g_Vars.m_global_skin_changer.m_gloves_idx ) {
								currentGloveID = i;
							}
						}

						auto& skin_data = g_Vars.m_skin_changer;
						CVariables::skin_changer_data* skin = nullptr;
						for( size_t i = 0u; i < skin_data.Size( ); ++i ) {
							skin = skin_data[ i ];
							if( skin->m_definition_index == g_Vars.m_global_skin_changer.m_gloves_idx ) {
								break;
							}
						}

						if( currentGloveID != 0 && skin != nullptr ) {
							static int iOldKit = skin->m_paint_kit_index;

							auto& current_weapon = g_KitParser.weapon_skins[ currentGloveID ];

							for( int i = 0; i < current_weapon.m_kits.size( ); ++i ) {
								auto currentKit = current_weapon.m_kits[ i ];
								paint_kits.push_back( currentKit.name.data( ) );
							}

							if( on_cfg_load_gloves ) {
								auto& kit = current_weapon.m_kits[ skin->m_paint_kit_index ];
								if( kit.id != skin->m_paint_kit ) {
									auto it = std::find_if( current_weapon.m_kits.begin( ), current_weapon.m_kits.end( ), [ skin ] ( paint_kit& a ) {
										return a.id == skin->m_paint_kit;
									} );

									if( it != current_weapon.m_kits.end( ) )
										skin->m_paint_kit_index = std::distance( current_weapon.m_kits.begin( ), it );

									skin->m_paint_kit = current_weapon.m_kits[ skin->m_paint_kit_index ].id;
								}

								on_cfg_load_gloves = false;
							}

							GUI::Controls::Listbox( XorStr( "Glove kit" ), paint_kits, &skin->m_paint_kit_index, true, 10 );

							float flOldWear = skin->m_wear;

							if( flOldWear != skin->m_wear /*|| flOldSeed != skin->m_seed*/ ) {
								g_Vars.m_global_skin_changer.m_update_gloves = true;
								flOldWear = skin->m_wear;
								//flOldSeed = skin->m_seed;
							}

							skin->m_paint_kit = current_weapon.m_kits[ skin->m_paint_kit_index ].id;
							skin->m_enabled = true;

							if( iOldKit != skin->m_paint_kit_index ) {
								g_Vars.m_global_skin_changer.m_update_gloves = true;
								iOldKit = skin->m_paint_kit_index;
							}

							paint_kits.clear( );
						}
					}

					gloves.clear( );

					if( bruh != g_Vars.m_global_skin_changer.m_gloves_vector_idx || bOldGlove != g_Vars.m_global_skin_changer.m_glove_changer ) {
						g_Vars.m_global_skin_changer.m_update_gloves = true;

						bruh = g_Vars.m_global_skin_changer.m_gloves_vector_idx;
						bOldGlove = g_Vars.m_global_skin_changer.m_glove_changer;
					}

					GUI::Group::EndGroup( );
				}
				GUI::Group::PopLastSize( );

				GUI::Group::BeginGroup( XorStr( "Weapon skin" ), Vector2D( 50, 100 ) ); {
					g_Vars.m_global_skin_changer.m_active = true;

					auto& current_weapon = g_KitParser.weapon_skins[ /*weapon_id*/g_Vars.globals.m_iWeaponIndexSkins == -1 ? 0 : g_Vars.globals.m_iWeaponIndexSkins ];
					auto idx = g_Vars.globals.m_iWeaponIndex == -1 ? 7 : g_Vars.globals.m_iWeaponIndex;

					auto& skin_data = g_Vars.m_skin_changer;
					CVariables::skin_changer_data* skin = nullptr;
					for( size_t i = 0u; i < skin_data.Size( ); ++i ) {
						skin = skin_data[ i ];
						if( skin->m_definition_index == idx ) {
							break;
						}
					}

					if( skin ) {
						GUI::Controls::Checkbox( XorStr( "Enabled##Skins" ), &skin->m_enabled );

						GUI::Controls::Checkbox( XorStr( "StatTrak" ), &skin->m_stat_trak );

						float flOldWear = skin->m_wear;
						float flOldSeed = skin->m_seed;
						GUI::Controls::Slider( XorStr( "Quality" ), &skin->m_wear, 0.f, 100.f, XorStr( "%.0f%%" ) );
						GUI::Controls::Slider( XorStr( "Seed" ), &skin->m_seed, 0, 500, XorStr( "%d" ), 1, true );

						if( flOldWear != skin->m_wear || flOldSeed != skin->m_seed ) {
							g_Vars.m_global_skin_changer.m_update_skins = true;
							flOldWear = skin->m_wear;
							flOldSeed = skin->m_seed;
						}

						GUI::Controls::Checkbox( XorStr( "Filter by weapon" ), &skin->m_filter_paint_kits );

						if( skin->m_filter_paint_kits ) {
							auto& kit = current_weapon.m_kits[ skin->m_paint_kit_index ];
							if( kit.id != skin->m_paint_kit ) {
								auto it = std::find_if( current_weapon.m_kits.begin( ), current_weapon.m_kits.end( ), [ skin ] ( paint_kit& a ) {
									return a.id == skin->m_paint_kit;
								} );

								if( it != current_weapon.m_kits.end( ) )
									skin->m_paint_kit_index = std::distance( current_weapon.m_kits.begin( ), it );
							}
						}

						static int bruh1 = skin->m_paint_kit_index;
						static int bruh2 = skin->m_paint_kit_no_filter;

						if( skin->m_filter_paint_kits ) {
							static std::vector<std::string> paint_kits;

							for( int i = 0; i < current_weapon.m_kits.size( ); ++i ) {
								auto currentKit = current_weapon.m_kits[ i ];
								paint_kits.push_back( currentKit.name.data( ) );
							}

							if( !paint_kits.empty( ) ) {
								GUI::Controls::Listbox( XorStr( "Paint kits" ), paint_kits, &skin->m_paint_kit_index, true, 13 );
							}

							paint_kits.clear( );
						}
						else {
							if( !g_Vars.globals.m_vecPaintKits.empty( ) ) {
								GUI::Controls::Listbox( XorStr( "Paint kits" ), g_Vars.globals.m_vecPaintKits, &skin->m_paint_kit_no_filter, true, 13 );
							}
						}

						if( ( bruh1 != skin->m_paint_kit_index ) || ( bruh2 != skin->m_paint_kit_no_filter ) ) {
							//g_Vars.m_global_skin_changer.m_update_skins = true;
							g_Vars.m_global_skin_changer.m_update_gloves = true;

							bruh1 = skin->m_paint_kit_index;
							bruh2 = skin->m_paint_kit_no_filter;
						}

						skin->m_paint_kit = skin->m_filter_paint_kits ? current_weapon.m_kits[ skin->m_paint_kit_index ].id : skin->m_paint_kit_no_filter;

						//skin->m_enabled = true;
					}

					GUI::Group::EndGroup( );
				}
			}

			if( GUI::Form::BeginTab( 5, XorStr( "F" ) ) || GUI::ctx->setup ) {

			}

			GUI::Form::EndWindow( );

			// we not in setup anymore (menu code ran once)
			GUI::ctx->setup = false;
		}
	}
	void Draw( ) {
		DrawMenu( );
	}
}
