#include "../GUI.h"
#include "../../../source.hpp"

#pragma warning(disable : 4018)

std::map< int, float > iSlider;

std::map< int, std::string > szText;

std::map<size_t, float> LastShiftHoldTime;

/// FIXING THE SCROLLBAR OF THIS IS A TODO LMFAO
bool GUI::Controls::Listbox( const std::string& id, std::vector<std::string> elements, int* option, bool bSearchBar, int iSizeInElements ) {
	Vector2D CursorPos = GUI::PopCursorPos( );
	Vector2D DrawPos = ctx->pos + CursorPos;

	std::map< size_t, std::vector<std::string> > vecFoundElements;
	std::map< int, int > iPos;

	if( iSlider.find( GUI::Hash( id ) ) == iSlider.end( ) ) {
		iSlider.insert( { GUI::Hash( id ), 0.f } );
	}

	if( LastShiftHoldTime.find( GUI::Hash( id ) ) == LastShiftHoldTime.end( ) ) {
		LastShiftHoldTime.insert( { GUI::Hash( id ), 0.f } );
	}

	if( vecFoundElements.find( GUI::Hash( id ) ) == vecFoundElements.end( ) ) {
		vecFoundElements.insert( { GUI::Hash( id ), { "" } } );
	}

	if( szText.find( GUI::Hash( id ) ) == szText.end( ) ) {
		szText.insert( { GUI::Hash( id ), "" } );
	}

	if( iPos.find( GUI::Hash( id ) ) == iPos.end( ) ) {
		iPos.insert( { GUI::Hash( id ), -20 } );
	}

	DrawPos.x += 20;
	if( bSearchBar ) {
		DrawPos.y += 19;
	}

	int iWidth = std::min( ( int )ctx->ParentSize.x - 90, 270 );

	const int kurwa = 20;

	bool bHoveredSearch = g_InputSystem.IsInBox( DrawPos - Vector2D( 0, 18 ), Vector2D( iWidth, 19 ) ) && g_InputSystem.IsInBox( ctx->ParentPos, ctx->ParentSize ) && bSearchBar;
	bool bHovered = g_InputSystem.IsInBox( DrawPos, Vector2D( iWidth, kurwa * iSizeInElements ) ) && g_InputSystem.IsInBox( ctx->ParentPos, ctx->ParentSize );

	Render::DirectX::rect_fill( DrawPos, Vector2D( iWidth, kurwa * iSizeInElements ), Color( 35, 35, 35 ) );

	D3DVIEWPORT9 clip = { DrawPos.x, DrawPos.y + 1, iWidth, ( kurwa * iSizeInElements ) - 2, 0.f, 1.0f };
	Render::DirectX::set_scissor_rect( clip );

	bool bDrawBottomArrow = false;
	bool bDrawTopArrow = false;
	for( int i = 0; i < elements.size( ); i++ ) {
		std::string transformed_elements( elements.at( i ) );
		std::transform( transformed_elements.begin( ), transformed_elements.end( ), transformed_elements.begin( ), ::tolower );

		std::string transformed_text( szText[ GUI::Hash( id ) ] );
		std::transform( transformed_text.begin( ), transformed_text.end( ), transformed_text.begin( ), ::tolower );

		if( ( transformed_elements.find( transformed_text ) == std::string::npos ) && transformed_text.size( ) > 0 ) {
			continue;
		}
		else {
			vecFoundElements[ GUI::Hash( id ) ].push_back( elements.at( i ) );
			iPos[ GUI::Hash( id ) ] += kurwa;
		}

		bool scrollbar = vecFoundElements[ GUI::Hash( id ) ].size( ) > iSizeInElements;

		Vector2D OptionPos = DrawPos + Vector2D( 0, iPos[ GUI::Hash( id ) ] + iSlider[ GUI::Hash( id ) ] );
		Vector2D OptionSize = Vector2D( iWidth - ( scrollbar ? 6 : 0 ), kurwa );

		// add and subtract 20 for tolerance - we do this cos the "scroll" is a float
		// and not an int (for the epic smooth easing effect) - just clip the elements,
		// drawing 2 more elements wont affect performance that much.
		bool bInBoundsTop = OptionPos.y >= ( DrawPos.y - kurwa );
		bool bInBoundsBottom = OptionPos.y + OptionSize.y <= ( DrawPos.y + kurwa ) + kurwa * ( iSizeInElements - 1 );

		bool bIsInBounds = bInBoundsTop && bInBoundsBottom;

		if( !bInBoundsBottom ) { bDrawBottomArrow = true; };
		if( !bInBoundsTop ) { bDrawTopArrow = true; };

		if( bHovered && bIsInBounds ) {
			if( g_InputSystem.IsInBox( OptionPos, OptionSize - Vector2D( 6, 0 ) ) && !bHoveredSearch && g_InputSystem.IsInBox( ctx->ParentPos, ctx->ParentSize ) && ctx->FocusedID == 0 ) {
				Render::DirectX::rect_fill( OptionPos, OptionSize, Color( 45, 45, 45 ) );

				if( InputHelper::Pressed( VK_LBUTTON ) ) {
					*option = i;
				}
			}
		}

		if( bIsInBounds ) {
			if( *option != i ) {
				Render::DirectX::Fonts::menu.draw_text( elements[ i ].c_str( ), OptionPos + Vector2D( 10, 4 ), Color( 200, 200, 200 ), 0 );
			}
			else {
				Render::DirectX::rect_fill( OptionPos, OptionSize, Color( 26, 26, 26 ) );
				Render::DirectX::Fonts::menu.draw_text( elements[ i ].c_str( ), OptionPos + Vector2D( 10, 4 ), g_Vars.menu.ascent.ToRegularColor( ).OverrideAlpha( 255 * ctx->animation ), 0 );
			}
		}
	}
	Render::DirectX::reset_scissor_rect( );

	int scrollbar_height = -1;
	int scrollbar_pos = -1;

	if( vecFoundElements[ GUI::Hash( id ) ].size( ) > iSizeInElements ) {
		scrollbar_height = ( ( float )( kurwa * ( iSizeInElements + 1 ) ) / ( float )( kurwa * ( vecFoundElements[ GUI::Hash( id ) ].size( ) + 1 ) ) ) * ( kurwa * ( iSizeInElements + 1 ) );
		scrollbar_pos = std::min( std::max( ( -( float )iSlider[ GUI::Hash( id ) ] / ( float )( kurwa * vecFoundElements[ GUI::Hash( id ) ].size( ) ) ) * ( float )( kurwa * iSizeInElements ), 2.f ), ( kurwa * iSizeInElements ) - scrollbar_height - 2.f );
	}

	// listbox
	if( ( bHovered || ctx->FocusedID == GUI::Hash( id + "s" ) ) && vecFoundElements[ GUI::Hash( id ) ].size( ) > iSizeInElements && ( scrollbar_height != -1 && scrollbar_pos != -1 ) ) {
		if( ctx->FocusedID == 0 ) {
			if( g_InputSystem.GetScrollMouse( ) > 0 ) {
				iSlider[ GUI::Hash( id ) ] = iSlider[ GUI::Hash( id ) ] + kurwa;
			}
			else if( g_InputSystem.GetScrollMouse( ) < 0 ) {
				iSlider[ GUI::Hash( id ) ] = iSlider[ GUI::Hash( id ) ] - kurwa;
			}
		}

		bool bHoveredScroll = g_InputSystem.IsInBox( DrawPos + Vector2D( iWidth - 5, scrollbar_pos + 1 ), Vector2D( 4, scrollbar_height ) );

		static float prev_mouse = 0.f;

		const auto diff = prev_mouse - g_InputSystem.GetMousePosition( ).y;

		if( ctx->FocusedID == 0 ) {
			if( bHoveredScroll && InputHelper::Down( VK_LBUTTON ) ) {
				ctx->FocusedID = GUI::Hash( id + "s" );
			}
		}
		else if( ctx->FocusedID == GUI::Hash( id + "s" ) ) {
			if( InputHelper::Down( VK_LBUTTON ) ) {
				//iSlider[ GUI::Hash( id ) ] += ( diff / 2 ) * ( fabs( ( kurwa * vecFoundElements[ GUI::Hash( id ) ].size( ) ) - scrollbar_height ) / iSizeInElements );

				const auto scale = [ ] ( int in, int bmin, int bmax, int lmin, int lmax ) {
					return float( ( lmax - lmin ) * ( in - bmin ) ) / float( bmax - bmin ) + lmin;
				};

				// i think not perfect
				auto gs = kurwa * iSizeInElements;
				auto pizdo = std::max( float( float( gs * ( gs - 12 * 2 ) )
					/ float( ( kurwa * vecFoundElements[ GUI::Hash( id ) ].size( ) ) + ( gs - 12 * 2 ) ) ), 30.f );

				iSlider[ GUI::Hash( id ) ] += scale( InputHelper::MouseDelta.y, 0, gs - pizdo, 0, ( kurwa * vecFoundElements[ GUI::Hash( id ) ].size( ) ) );
			}
			else {
				prev_mouse = 0.f;
				ctx->FocusedID = 0;
			}
		}

		prev_mouse = g_InputSystem.GetMousePosition( ).y;

		ctx->hovered_listbox = true;
		iSlider[ GUI::Hash( id ) ] = std::clamp<int>( iSlider[ GUI::Hash( id ) ], ( ( kurwa * ( vecFoundElements[ GUI::Hash( id ) ].size( ) - 1 ) ) * -1 ) + ( int )( kurwa * iSizeInElements ), 0 );
	}
	else {
		ctx->hovered_listbox = false;
	}

	if( vecFoundElements[ GUI::Hash( id ) ].size( ) > iSizeInElements && ( scrollbar_height != -1 && scrollbar_pos != -1 ) ) {
		Render::DirectX::rect_fill( DrawPos + Vector2D( iWidth - 6, 1 ), Vector2D( 6, ( kurwa * iSizeInElements ) - 1 ), Color( 45, 45, 45 ) );
		Render::DirectX::rect_fill( DrawPos + Vector2D( iWidth - 5, scrollbar_pos + 1 ), Vector2D( 4, scrollbar_height ), ( g_InputSystem.IsInBox( DrawPos + Vector2D( iWidth - 5, scrollbar_pos + 1 ), Vector2D( 4, scrollbar_height ) ) || ctx->FocusedID == GUI::Hash( id + "s" ) ) ? Color( 80, 80, 80 ) : Color( 65, 65, 65 ) );
	}

	if( bDrawBottomArrow && ( vecFoundElements[ GUI::Hash( id ) ].size( ) > iSizeInElements ) && vecFoundElements[ GUI::Hash( id ) ].size( ) > iSizeInElements && ( scrollbar_height != -1 && scrollbar_pos != -1 ) ) {
		Render::DirectX::rect_fill( DrawPos + Vector2D( iWidth, ( kurwa * iSizeInElements ) - 2 ) - Vector2D( 17, 10 ), Vector2D( 5, 1 ), Color( 0, 0, 0 ) );
		Render::DirectX::rect_fill( DrawPos + Vector2D( iWidth, ( kurwa * iSizeInElements ) - 2 ) - Vector2D( 17, 9 ), Vector2D( 5, 1 ), Color( 204, 204, 204 ) );
		Render::DirectX::rect_fill( DrawPos + Vector2D( iWidth, ( kurwa * iSizeInElements ) - 2 ) - Vector2D( 16, 8 ), Vector2D( 3, 1 ), Color( 204, 204, 204 ) );
		Render::DirectX::rect_fill( DrawPos + Vector2D( iWidth, ( kurwa * iSizeInElements ) - 2 ) - Vector2D( 15, 7 ), Vector2D( 1, 1 ), Color( 204, 204, 204 ) );
	}

	// top arrow facing up
	if( bDrawTopArrow && iSlider[ GUI::Hash( id ) ] <= -20 && vecFoundElements[ GUI::Hash( id ) ].size( ) > iSizeInElements && ( scrollbar_height != -1 && scrollbar_pos != -1 ) ) {
		Render::DirectX::rect_fill( DrawPos + Vector2D( iWidth - 20, 7 ) + Vector2D( 5, 0 ), Vector2D( 1, 1 ), Color( 204, 204, 204 ) );
		Render::DirectX::rect_fill( DrawPos + Vector2D( iWidth - 20, 7 ) + Vector2D( 4, 1 ), Vector2D( 3, 1 ), Color( 204, 204, 204 ) );
		Render::DirectX::rect_fill( DrawPos + Vector2D( iWidth - 20, 7 ) + Vector2D( 3, 2 ), Vector2D( 5, 1 ), Color( 204, 204, 204 ) );
		Render::DirectX::rect_fill( DrawPos + Vector2D( iWidth - 20, 7 ) + Vector2D( 3, 3 ), Vector2D( 5, 1 ), Color( 0, 0, 0 ) );
	}

	// search
	if( bSearchBar ) {
		Render::DirectX::rect_fill( DrawPos - Vector2D( 0, 18 ), Vector2D( iWidth, 19 ), Color( 25, 25, 25, 255 * ctx->animation ) );

		if( ctx->FocusedID == 0 ) {
			if( ctx->typing )
				ctx->typing = false;

			if( bHoveredSearch && g_InputSystem.IsKeyDown( VK_LBUTTON ) )
				ctx->FocusedID = GUI::Hash( id + XorStr( "search" ) );
		}
		else if( ctx->FocusedID == GUI::Hash( id + XorStr( "search" ) ) ) {
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
							szText[ GUI::Hash( id ) ] += ( ( g_InputSystem.IsKeyDown( VK_SHIFT ) ) || GetKeyState( VK_CAPITAL ) ) ? i : i + 32;
							iSlider[ GUI::Hash( id ) ] = 0;
						}
						else if( i >= '0' && i <= '9' || i == ' ' ) {
							szText[ GUI::Hash( id ) ] += i;
							iSlider[ GUI::Hash( id ) ] = 0;
						}
					}
					else {
						szText[ GUI::Hash( id ) ] = szText[ GUI::Hash( id ) ].substr( 0, szText[ GUI::Hash( id ) ].size( ) - 1 );
						iSlider[ GUI::Hash( id ) ] = 0;
					}
				}

				// left control + backspace = delete every fucking thing
				if( InputHelper::Down( VK_LCONTROL ) && GetAsyncKeyState( VK_BACK ) ) {
					szText[ GUI::Hash( id ) ] = szText[ GUI::Hash( id ) ].substr( 0, szText[ GUI::Hash( id ) ].size( ) - 1 );
					iSlider[ GUI::Hash( id ) ] = 0;
				}
			}

			// if we arent holding backspace store the time
			if( !GetAsyncKeyState( VK_BACK ) ) {
				LastShiftHoldTime[ GUI::Hash( id ) ] = m_pGlobalVars->curtime;
			}

			// the delta between the current time and last "non held shift" time exceeds 500ms,
			// this means that we've been holding shift for 500ms. let's erase.
			if( m_pGlobalVars->curtime - LastShiftHoldTime[ GUI::Hash( id ) ] > 0.5f ) {
				szText[ GUI::Hash( id ) ] = szText[ GUI::Hash( id ) ].substr( 0, szText[ GUI::Hash( id ) ].size( ) - 1 );
				iSlider[ GUI::Hash( id ) ] = 0;
			}
		}

		D3DVIEWPORT9 clip = { DrawPos.x, DrawPos.y - kurwa, Vector2D( iWidth, kurwa ).x, Vector2D( iWidth, kurwa ).y - 2, 0.f, 1.0f };
		Render::DirectX::set_scissor_rect( clip ); {
			Render::DirectX::Fonts::menu.draw_text( ( szText[ GUI::Hash( id ) ] + ( ctx->FocusedID == GUI::Hash( id + XorStr( "search" ) ) ? '_' : ' ' ) ).c_str( ), DrawPos - Vector2D( -4, 15 ), Color( 200, 200, 200 ), 0 );
		}
		Render::DirectX::reset_scissor_rect( );
	}

	if( bSearchBar ) {
		Render::DirectX::rect( DrawPos - Vector2D( 0, 18 ), Vector2D( iWidth, kurwa * iSizeInElements ) + Vector2D( 0, 18 ), Color( 10, 10, 10 ) );
		Render::DirectX::rect( DrawPos - Vector2D( 0, 18 ), Vector2D( iWidth, 19 ), Color( 10, 10, 10 ) );
		Render::DirectX::rect( DrawPos - Vector2D( 0, 18 ) + 1, Vector2D( iWidth, 19 ) - 2, Color( 50, 50, 50 ) );
		Render::DirectX::rect( DrawPos - Vector2D( 0, 18 ) + 2, Vector2D( iWidth, 19 ) - 4, Color( 16, 16, 16 ) );
	}
	else {
		Render::DirectX::rect( DrawPos, Vector2D( iWidth, kurwa * iSizeInElements ), Color( 10, 10, 10 ) );
	}

	GUI::PushCursorPos( CursorPos + Vector2D( 0, ( kurwa * iSizeInElements ) + ( bSearchBar ? ( GUI::ObjectPadding( ) + kurwa ) : GUI::ObjectPadding( ) ) ) );
	return ctx->FocusedID == GUI::Hash( id );
}

#if defined(DEV) || defined(BETA_MODE) || defined(DEBUG_MODE) || defined(LUA_SCRIPTING)
// HAHAHHAHAHAHAHAHHAHAH
bool GUI::Controls::Luabox( const std::string& id, std::vector<std::string> elements, int* option, bool bSearchBar, int iSizeInElements ) {
	Vector2D CursorPos = GUI::PopCursorPos( );
	Vector2D DrawPos = ctx->pos + CursorPos;

	GUI::PushCursorPos( CursorPos + Vector2D( 0, ( 20 * iSizeInElements ) + ( bSearchBar ? ( GUI::ObjectPadding( ) + 20 ) : GUI::ObjectPadding( ) ) ) );
	return ctx->FocusedID == GUI::Hash( id );
}
#endif