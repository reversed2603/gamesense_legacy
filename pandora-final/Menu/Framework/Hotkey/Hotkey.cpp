#include "../gui.h"
#include "../../../source.hpp"
#include "../../../Features/Visuals/EventLogger.hpp"

std::string GetKeyName( const int VirtualKey ) {
	auto Code = MapVirtualKeyA( VirtualKey, MAPVK_VK_TO_VSC );

	int Result;
	char Buffer[ 128 ];

	switch( VirtualKey ) {
	case VK_LEFT: case VK_UP: case VK_RIGHT: case VK_DOWN:
	case VK_RCONTROL: case VK_RMENU:
	case VK_LWIN: case VK_RWIN: case VK_APPS:
	case VK_PRIOR: case VK_NEXT:
	case VK_END: case VK_HOME:
	case VK_INSERT: case VK_DELETE:
	case VK_DIVIDE:
	case VK_NUMLOCK:
		Code |= KF_EXTENDED;
	default:
		Result = GetKeyNameTextA( Code << 16, Buffer, 128 );
	}

	if( Result == 0 || VirtualKey == VK_CAPITAL ) {
		switch( VirtualKey ) {
		case VK_XBUTTON1:
			return XorStr( "[M4]" );
		case VK_XBUTTON2:
			return XorStr( "[M5]" );
		case VK_LBUTTON:
			return XorStr( "[M1]" );
		case VK_MBUTTON:
			return XorStr( "[M3]" );
		case VK_RBUTTON:
			return XorStr( "[M2]" );
		case VK_CAPITAL:
			return XorStr( "[CAPS]" );
		default:
			return XorStr( "[-]" );
		}
	}

	auto transformer = std::string( Buffer );
	std::transform( transformer.begin( ), transformer.end( ), transformer.begin( ), ::toupper );

	return XorStr( "[" ) + transformer + XorStr( "]" );
}

std::unordered_map<size_t, float> uHoverAnimationHotkey;
std::unordered_map<size_t, float> uActiveAnimationHotkey;
void GUI::Controls::Hotkey( const std::string& name, hotkey_t* var_name, bool type_selection ) {
	Vector2D CursorPos = GetLastCursorPos( );
	Vector2D DrawPos = ctx->pos + CursorPos;

	DrawPos.x += ctx->ParentSize.x - 55;

	DrawPos.x += 8;
	DrawPos.y -= 2;

	float w = Render::DirectX::Fonts::menu_pixel.measure_size( GetKeyName( ( *var_name ).key ) ).x;
	Render::DirectX::Fonts::menu_pixel.draw_text( GetKeyName( ( *var_name ).key ), DrawPos - Vector2D( 1, 0 ) - Vector2D( w, 0 ), Color( 0, 0, 0, 75 ), 0 );
	Render::DirectX::Fonts::menu_pixel.draw_text( GetKeyName( ( *var_name ).key ), DrawPos - Vector2D( 0, 1 ) - Vector2D( w, 0 ), Color( 0, 0, 0, 75 ), 0 );
	Render::DirectX::Fonts::menu_pixel.draw_text( GetKeyName( ( *var_name ).key ), DrawPos + Vector2D( 1, 0 ) - Vector2D( w, 0 ), Color( 0, 0, 0, 75 ), 0 );
	Render::DirectX::Fonts::menu_pixel.draw_text( GetKeyName( ( *var_name ).key ), DrawPos + Vector2D( 0, 1 ) - Vector2D( w, 0 ), Color( 0, 0, 0, 75 ), 0 );

	Render::DirectX::Fonts::menu_pixel.draw_text( GetKeyName( ( *var_name ).key ), DrawPos - Vector2D( w, 0 ), ctx->FocusedID == GUI::Hash( name ) ? Color( 255, 0, 0 ) : Color( 107, 107, 107 ), 0 );
	
	bool hovered = g_InputSystem.IsInBox( DrawPos - Vector2D( w, 0 ), Vector2D( w, 8 ) ) && g_InputSystem.IsInBox( ctx->ParentPos, ctx->ParentSize );

	if( ctx->FocusedID == 0 ) {
		if( hovered && InputHelper::Released( VK_LBUTTON ) ) {
			ctx->FocusedID = GUI::Hash( name );
		}

		if( type_selection ) {
			if( hovered && InputHelper::Pressed( VK_RBUTTON ) ) {
				ctx->FocusedID = GUI::Hash( name + XorStr( "_type" ) );
			}
		}

		if( hovered && InputHelper::Pressed( VK_MBUTTON ) ) {
			if( OpenClipboard( nullptr ) ) {
				EmptyClipboard( );
				HGLOBAL clipboard_buffer = GlobalAlloc( GMEM_DDESHARE, GUI::SplitStr( name, '#' )[ 0 ].size( ) + 1 );
				char* buffer = ( char* )GlobalLock( clipboard_buffer );
				strcpy( buffer, GUI::SplitStr( name, '#' )[ 0 ].data( ) );

				GlobalUnlock( clipboard_buffer );
				SetClipboardData( CF_TEXT, clipboard_buffer );
				CloseClipboard( );

				//g_EventLog.PushEvent( XorStr( "copied name to clipboard" ), Color_f( 1.f, 1.f, 1.f ) );
			}
		}

	}
	else if( ctx->FocusedID == GUI::Hash( name + XorStr( "_type" ) ) ) {
		ctx->DropdownInfo.Elements = { XorStr( "On hotkey" ), XorStr( "Toggle" ), XorStr( "Off hotkey" ), XorStr( "Always on" ) };
		ctx->DropdownInfo.Option = &var_name->cond;
		ctx->DropdownInfo.Size = 100;
		ctx->DropdownInfo.MaxItems = 4;
		ctx->DropdownInfo.Pos = DrawPos + Vector2D( 0, 10 );
		ctx->DropdownInfo.HashedID = GUI::Hash( name + XorStr( "_type" ) );
	}
	else if( ctx->FocusedID == GUI::Hash( name ) ) {
		if( !hovered && InputHelper::Pressed( VK_LBUTTON ) ) {
			ctx->FocusedID = 0;
		}

		for( int i = 0; i < 255; i++ ) {
			if( InputHelper::Released( i ) ) {
				ctx->FocusedID = 0;
				( *var_name ).key = ( i );

				if( i == VK_ESCAPE ) {
					( *var_name ).key = ( 0 );
				}

				// either always break (which makes sense lol) or only break if i == 16 (SHIFT), but since idk if there are more keys that are multiple
				// I'll just keep it like this.
				break;
			}
		}
	}
}
