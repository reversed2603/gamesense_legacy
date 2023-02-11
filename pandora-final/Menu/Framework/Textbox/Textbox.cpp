#include <algorithm>
#include "../gui.h"
#include "../../../source.hpp"

std::map< int, std::string > _szText;
std::map<size_t, float> _LastShiftHoldTime;

bool GUI::Controls::Textbox( const std::string& name, std::string* input, int max_text ) {
	Vector2D CursorPos = PopCursorPos( );
	Vector2D DrawPos = ctx->pos + CursorPos;

	Vector2D DrawSize( std::min( ( int )ctx->ParentSize.x - 90, 270 ), 18 );

	if( _szText.find( GUI::Hash( name ) ) == _szText.end( ) ) {
		_szText.insert( { GUI::Hash( name ), "" } );
	}

	if( _LastShiftHoldTime.find( GUI::Hash( name ) ) == _LastShiftHoldTime.end( ) ) {
		_LastShiftHoldTime.insert( { GUI::Hash( name ), 0.f } );
	}

	DrawPos.x += 20;

	Render::DirectX::Fonts::menu.draw_text( name, DrawPos + Vector2D( 1, -3 ), Color( 203, 203, 203 ), 4 );

	DrawPos.y += 11;

	int iWidth = DrawSize.x;
	bool bHoveredSearch = g_InputSystem.IsInBox( DrawPos, Vector2D( iWidth, DrawSize.y ) ) && g_InputSystem.IsInBox( ctx->ParentPos, ctx->ParentSize );

	// search
	Render::DirectX::rect_fill( DrawPos, Vector2D( iWidth + 1, DrawSize.y ), Color( 25, 25, 25, 255 * ctx->animation ) );

	if( ctx->FocusedID == 0 ) {
		if( ctx->typing )
			ctx->typing = false;

		if( bHoveredSearch && g_InputSystem.IsKeyDown( VK_LBUTTON ) )
			ctx->FocusedID = GUI::Hash( name );
	}
	else if( ctx->FocusedID == GUI::Hash( name ) ) {
		if( !ctx->typing )
			ctx->typing = true;

		if( g_InputSystem.WasKeyPressed( VK_LBUTTON ) && !bHoveredSearch ) {
			ctx->FocusedID = 0;
		}

		for( int i = 0; i < 255; i++ ) {
			if( InputHelper::Pressed( i ) ) {
				if( i != VK_BACK ) {
					if( i == VK_ESCAPE || i == VK_RETURN ) {
						ctx->FocusedID = 0;
					}
					else if( i >= 'A' && i <= 'Z' ) {
						_szText[ GUI::Hash( name ) ] += ( ( g_InputSystem.IsKeyDown( VK_SHIFT ) ) || GetKeyState( VK_CAPITAL ) ) ? i : i + 32;
					}
					else if( i >= '0' && i <= '9' || i == ' ' ) {
						_szText[ GUI::Hash( name ) ] += i;
					}
				}
				else {
					_szText[ GUI::Hash( name ) ] = _szText[ GUI::Hash( name ) ].substr( 0, _szText[ GUI::Hash( name ) ].size( ) - 1 );
				}
			}

			// left control + backspace = delete every fucking thing
			if( InputHelper::Down( VK_LCONTROL ) && GetAsyncKeyState( VK_BACK ) ) {
				_szText[ GUI::Hash( name ) ] = _szText[ GUI::Hash( name ) ].substr( 0, _szText[ GUI::Hash( name ) ].size( ) - 1 );
			}
		}

		// if we arent holding backspace store the time
		if( !GetAsyncKeyState( VK_BACK ) ) {
			_LastShiftHoldTime[ GUI::Hash( name ) ] = m_pGlobalVars->curtime;
		}

		// the delta between the current time and last "non held shift" time exceeds 500ms,
		// this means that we've been holding shift for 500ms. let's erase.
		if( m_pGlobalVars->curtime - _LastShiftHoldTime[ GUI::Hash( name ) ] > 0.5f ) {
			_szText[ GUI::Hash( name ) ] = _szText[ GUI::Hash( name ) ].substr( 0, _szText[ GUI::Hash( name ) ].size( ) - 1 );
		}
	}

	D3DVIEWPORT9 clip = { DrawPos.x, DrawPos.y + 1, Vector2D( iWidth, DrawSize.y ).x, Vector2D( iWidth, DrawSize.y ).y - 2, 0.f, 1.0f };

	Render::DirectX::rect( DrawPos, Vector2D( iWidth, DrawSize.y ), Color( 10, 10, 10 ) );
	Render::DirectX::rect( DrawPos + 1, Vector2D( iWidth, DrawSize.y ) - 2, Color( 60, 60, 60 ) );
	Render::DirectX::rect( DrawPos + 2, Vector2D( iWidth, DrawSize.y ) - 4, Color( 16, 16, 16 ) );

	_szText[ GUI::Hash( name ) ] = _szText[ GUI::Hash( name ) ].substr( 0, max_text );
	*input = _szText[ GUI::Hash( name ) ];

	Render::DirectX::set_scissor_rect( clip ); {
		Render::DirectX::Fonts::menu.draw_text( ( _szText[ GUI::Hash( name ) ] + (ctx->FocusedID == GUI::Hash(name) ? '_' : ' ') ).c_str( ), DrawPos + Vector2D( 5, 3 ), Color( 200, 200, 200 ), 0 );
	}
	Render::DirectX::reset_scissor_rect( );

	PushCursorPos( CursorPos + Vector2D( 0, DrawSize.y + Render::DirectX::Fonts::menu.measure_size( name ).y + GUI::ObjectPadding( ) ) );
	return ctx->FocusedID == GUI::Hash( name );
}