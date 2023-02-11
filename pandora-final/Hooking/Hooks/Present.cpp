#include "../Hooked.hpp"
#include <intrin.h>
#include "../../Utils/InputSys.hpp"
#include "../../Features/Visuals/Hitmarker.hpp"
#include "../../Renderer/Render.hpp"
#include "../../SDK/Classes/entity.hpp"
#include "../../SDK/Classes/player.hpp"
#include "../../Utils/Config.hpp"

HRESULT __stdcall Hooked::Present( LPDIRECT3DDEVICE9 pDevice, const RECT* pSourceRect, const RECT* pDestRect, HWND hDestWindowOverride, const RGNDATA* pDirtyRegion ) {
	g_Vars.globals.szLastHookCalled = XorStr( "27" );

	if( Render::DirectX::initialized ) {
		// gay idc
		InputHelper::Update( );

		Render::DirectX::begin( );
		{
			bool aa_left_enabled = g_Vars.rage.anti_aim_left_key.enabled;
			bool aa_right_enabled = g_Vars.rage.anti_aim_right_key.enabled;
			bool aa_back_enabled = g_Vars.rage.anti_aim_back_key.enabled;

			if( !g_Vars.globals.m_bChatOpen && !g_Vars.globals.m_bConsoleOpen /*&& !g_Vars.globals.menuOpen*/ ) {

				for( auto& keybind : g_keybinds ) {
					if( !keybind )
						continue;

					// hold
					if( keybind->cond == KeyBindType::HOLD ) {
						keybind->enabled = InputHelper::Down( keybind->key );
					}

					// toggle
					else if( keybind->cond == KeyBindType::TOGGLE ) {
						if( InputHelper::Pressed( keybind->key ) )
							keybind->enabled = !keybind->enabled;
					}

					// hold off
					else if( keybind->cond == KeyBindType::HOLD_OFF ) {
						keybind->enabled = !InputHelper::Down( keybind->key );
					}

					// always on
					else if( keybind->cond == KeyBindType::ALWAYS_ON ) {
						keybind->enabled = true;
					}

				}
			}
			else {
				for( auto& keybind : g_keybinds ) {
					if( !keybind )
						continue;

					if( keybind == &g_Vars.misc.third_person_bind )
						continue;

					// hold
					if( keybind->cond == KeyBindType::HOLD ) {
						keybind->enabled = false;
					}
				}
			}

			// amazing logic, I know, thanks.
			if( aa_left_enabled && g_Vars.rage.anti_aim_left_key.enabled ) {
				if( g_Vars.rage.anti_aim_right_key.enabled || g_Vars.rage.anti_aim_back_key.enabled )
					g_Vars.rage.anti_aim_left_key.enabled = false;
			}

			if( aa_right_enabled && g_Vars.rage.anti_aim_right_key.enabled ) {
				if( g_Vars.rage.anti_aim_left_key.enabled || g_Vars.rage.anti_aim_back_key.enabled )
					g_Vars.rage.anti_aim_right_key.enabled = false;
			}

			if( aa_back_enabled && g_Vars.rage.anti_aim_back_key.enabled ) {
				if( g_Vars.rage.anti_aim_left_key.enabled || g_Vars.rage.anti_aim_right_key.enabled )
					g_Vars.rage.anti_aim_back_key.enabled = false;
			}


			{
				auto pizda = g_Vars.menu.ascent.ToRegularColor( );
				size_t hash = size_t( ( uintptr_t )g_Vars.globals.menuOpen + ( uintptr_t )g_Vars.menu.size_x + ( uintptr_t )g_Vars.menu.size_y + uintptr_t( pizda.r( ) + pizda.g( ) + pizda.b( ) + pizda.a( ) ) );
				static size_t old_hash = hash;

				if( g_InputSystem.WasKeyPressed( g_Vars.menu.key.key ) ) {
					g_Vars.globals.menuOpen = !g_Vars.globals.menuOpen;
				}

				// let's save the global_data on open/close :)
				if( old_hash != hash ) {
					ConfigManager::SaveConfig( XorStr( "global_data" ), true );
					old_hash = hash;
				}

				GUI::ctx->animation = g_Vars.globals.menuOpen ? ( GUI::ctx->animation + ( 1.0f / 0.2f ) * m_pGlobalVars->frametime )
					: ( ( GUI::ctx->animation - ( 1.0f / 0.2f ) * m_pGlobalVars->frametime ) );

				GUI::ctx->animation = std::clamp<float>( GUI::ctx->animation, 0.f, 1.0f );

				// do menu fade the chad way
				// float alpha = m_pSurface->DrawGetAlphaMultiplier( );
				// m_pSurface->DrawSetAlphaMultiplier( GUI::ctx->animation );

				Menu::Draw( );

				alpha_mod = -1.f;
				g_Vars.menu.ascent.a = 1.f;

				// restore old alpha multiplier
				// m_pSurface->DrawSetAlphaMultiplier( alpha );
			}

			g_InputSystem.SetScrollMouse( 0.f );
		}
		Render::DirectX::end( );
	}

	return oPresent( pDevice, pSourceRect, pDestRect, hDestWindowOverride, pDirtyRegion );
}