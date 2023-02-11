#include <algorithm>
#include "../gui.h"
#include "../../../source.hpp"

bool GUI::Controls::Slider( const std::string& name, float* var_name, float min, float max, const std::string& display, float increment, bool precision ) {
	Vector2D CursorPos = PopCursorPos( );
	Vector2D DrawPos = ctx->pos + CursorPos;
	Vector2D DrawSize( std::min( ( int )ctx->ParentSize.x - 90, 270 ), 6 );

	DrawPos.x += 20;

	if( GUI::SplitStr( name, '#' )[ 0 ].size( ) ) {
		Render::DirectX::Fonts::menu.draw_text( GUI::SplitStr( name, '#' )[ 0 ].data( ), DrawPos + Vector2D( 0, -5 ), Color( 203, 203, 203 ), 4 );

		DrawPos.y += 11;
	}

	bool hovered = g_InputSystem.IsInBox( DrawPos, DrawSize + Vector2D( 0, 1 ) ) && g_InputSystem.IsInBox( ctx->ParentPos, ctx->ParentSize );

	if( hovered && ctx->FocusedID != GUI::Hash( name ) )
		Render::DirectX::gradient_v( DrawPos + 1, DrawSize - 1, { 57, 57, 57 }, { 73, 73, 73 } );
	else
		Render::DirectX::gradient_v( DrawPos + 1, DrawSize - 1, { 52, 52, 52 }, { 68, 68, 68 } );

	float width = GUI::MapNumber( *var_name, min, max, 0, DrawSize.x );
	Render::DirectX::gradient_v( DrawPos + 1, Vector2D( width + 1, DrawSize.y ) - 1, g_Vars.menu.ascent.ToRegularColor( ), g_Vars.menu.ascent.ToRegularColor( ) * 0.75f );

	Render::DirectX::rect( DrawPos, DrawSize, { 10, 10, 10 } );

	char buf[ 32 ];
	sprintf_s( buf, display.c_str( ), *var_name );

	Vector2D tooltip_size = Render::DirectX::Fonts::menu_bold.measure_size( buf );
	Render::DirectX::Fonts::menu_bold.draw_text( buf, DrawPos + Vector2D( width - ( tooltip_size.x / 2 ), DrawSize.y - 6 ), Color( 203, 203, 203 ), 8 );

	// - 
	if( *var_name - increment >= min && precision ) {
		Render::DirectX::line( DrawPos - Vector2D( 5, -4 ), DrawPos - Vector2D( 2, -4 ), Color( 100, 100, 100 ) );

		if( g_InputSystem.IsInBox( DrawPos - Vector2D( 8, 0 ), Vector2D( 8, 6 ) ) && InputHelper::Pressed( VK_LBUTTON ) ) {
			bool is_whole_number = ( *var_name ) == std::ceilf( ( *var_name ) );
			// goal is to take away 1, but if the number is on like 50.6 we take away .6 first
			if( !is_whole_number && ( increment == std::ceilf( increment ) ) ) {
				float delta = ( *var_name ) - std::ceil( ( *var_name ) );
				( *var_name ) = std::clamp( ( *var_name ) - ( delta + increment ), min, max );
			}
			else {
				( *var_name ) = std::clamp( ( *var_name ) - increment, min, max );
			}
		}
	}

	// +
	if( *var_name + increment <= max && precision ) {
		Render::DirectX::line( DrawPos + DrawSize + Vector2D( 3, -2 ), DrawPos + DrawSize + Vector2D( 6, -2 ), Color( 100, 100, 100 ) );
		Render::DirectX::line( DrawPos + DrawSize + Vector2D( 4, -3 ), DrawPos + DrawSize + Vector2D( 4, 0 ), Color( 100, 100, 100 ) );

		if( g_InputSystem.IsInBox( DrawPos + DrawSize - Vector2D( -1, 6 ), Vector2D( 8, 6 ) ) && InputHelper::Pressed( VK_LBUTTON ) ) {
			bool is_whole_number = ( *var_name ) == std::ceilf( ( *var_name ) );
			// goal is to add 1, but if the number is on like 50.6 we add .4 first
			if( !is_whole_number && ( increment == std::ceilf( increment ) ) ) {
				float delta = fabs( ( *var_name ) - std::ceil( ( *var_name ) ) );
				( *var_name ) = std::clamp( ( *var_name ) + ( delta ), min, max );
			}
			else {
				( *var_name ) = std::clamp( ( *var_name ) + increment, min, max );
			}
		}
	}

	if( ctx->FocusedID == 0 ) {
		if( hovered && InputHelper::Pressed( VK_LBUTTON ) )
			ctx->FocusedID = GUI::Hash( name );
	}
	else if( ctx->FocusedID == GUI::Hash( name ) ) {
		if( InputHelper::Down( VK_LBUTTON ) ) {
			float offset = std::clamp<float>( Vector2D( g_InputSystem.GetMousePosition( ) - DrawPos ).x, 0, DrawSize.x );
			*var_name = GUI::MapNumber( offset, 0, DrawSize.x, min, max );
		}
		else {
			ctx->FocusedID = 0;
		}
	}

	PushCursorPos( CursorPos + Vector2D( 0, DrawSize.y + ( GUI::SplitStr( name, '#' )[ 0 ].size( ) ? 14 : 2 ) + GUI::ObjectPadding( ) ) );
	return ctx->FocusedID == GUI::Hash( name );
}

bool GUI::Controls::Slider( const std::string& name, int* var_name, int min, int max, const std::string& display, int increment, bool precision ) {
	Vector2D CursorPos = PopCursorPos( );
	Vector2D DrawPos = ctx->pos + CursorPos;
	Vector2D DrawSize( std::min( ( int )ctx->ParentSize.x - 90, 270 ), 6 );

	DrawPos.x += 20;

	if( GUI::SplitStr( name, '#' )[ 0 ].size( ) ) {
		Render::DirectX::Fonts::menu.draw_text( GUI::SplitStr( name, '#' )[ 0 ].data( ), DrawPos + Vector2D( 0, -5 ), Color( 203, 203, 203 ), 4 );

		DrawPos.y += 11;
	}

	bool hovered = g_InputSystem.IsInBox( DrawPos, DrawSize + Vector2D( 0, 1 ) ) && g_InputSystem.IsInBox( ctx->ParentPos, ctx->ParentSize );

	if( hovered && ctx->FocusedID != GUI::Hash( name ) )
		Render::DirectX::gradient_v( DrawPos + 1, DrawSize - 1, { 57, 57, 57 }, { 73, 73, 73 } );
	else
		Render::DirectX::gradient_v( DrawPos + 1, DrawSize - 1, { 52, 52, 52 }, { 68, 68, 68 } );

	int width = GUI::MapNumber( *var_name, min, max, 0, DrawSize.x );
	Render::DirectX::gradient_v( DrawPos + 1, Vector2D( width + 1, DrawSize.y ) - 1, g_Vars.menu.ascent.ToRegularColor( ), g_Vars.menu.ascent.ToRegularColor( ) * 0.75f );

	Render::DirectX::rect( DrawPos, DrawSize, { 10, 10, 10 } );

	char buf[ 32 ];
	sprintf_s( buf, display.c_str( ), *var_name );

	Vector2D tooltip_size = Render::DirectX::Fonts::menu_bold.measure_size( buf );
	Render::DirectX::Fonts::menu_bold.draw_text( buf, DrawPos + Vector2D( width - ( tooltip_size.x / 2 ), DrawSize.y - 6 ), Color( 203, 203, 203 ), 8 );

	// - 
	if( *var_name - increment >= min && precision ) {
		Render::DirectX::line( DrawPos - Vector2D( 5, -4 ), DrawPos - Vector2D( 2, -4 ), Color( 100, 100, 100 ) );

		if( g_InputSystem.IsInBox( DrawPos - Vector2D( 8, 0 ), Vector2D( 8, 6 ) ) && InputHelper::Pressed( VK_LBUTTON ) ) {
			bool is_whole_number = ( *var_name ) == std::ceilf( ( *var_name ) );
			// goal is to take away 1, but if the number is on like 50.6 we take away .6 first
			if( !is_whole_number ) {
				int delta = ( *var_name ) - std::ceil( ( *var_name ) );
				( *var_name ) = std::clamp( ( *var_name ) - ( delta + increment ), min, max );
			}
			else {
				( *var_name ) = std::clamp( ( *var_name ) - increment, min, max );
			}
		}
	}

	// +
	if( *var_name + increment <= max && precision ) {
		Render::DirectX::line( DrawPos + DrawSize + Vector2D( 3, -2 ), DrawPos + DrawSize + Vector2D( 6, -2 ), Color( 100, 100, 100 ) );
		Render::DirectX::line( DrawPos + DrawSize + Vector2D( 4, -3 ), DrawPos + DrawSize + Vector2D( 4, 0 ), Color( 100, 100, 100 ) );

		if( g_InputSystem.IsInBox( DrawPos + DrawSize - Vector2D( -1, 6 ), Vector2D( 8, 6 ) ) && InputHelper::Pressed( VK_LBUTTON ) ) {
			bool is_whole_number = ( *var_name ) == std::ceilf( ( *var_name ) );
			// goal is to add 1, but if the number is on like 50.6 we add .4 first
			if( !is_whole_number ) {
				int delta = fabs( ( *var_name ) - std::ceil( ( *var_name ) ) );
				( *var_name ) = std::clamp( ( *var_name ) + ( delta ), min, max );
			}
			else {
				( *var_name ) = std::clamp( ( *var_name ) + increment, min, max );
			}
		}
	}

	if( ctx->FocusedID == 0 ) {
		if( hovered && InputHelper::Pressed( VK_LBUTTON ) )
			ctx->FocusedID = GUI::Hash( name );
	}
	else if( ctx->FocusedID == GUI::Hash( name ) ) {
		if( InputHelper::Down( VK_LBUTTON ) ) {
			int offset = std::floor<int>( std::clamp<int>( Vector2D( g_InputSystem.GetMousePosition( ) - DrawPos ).x, 0, DrawSize.x ) );
			*var_name = ( GUI::MapNumber( offset, 0, DrawSize.x, min, max ) );
		}
		else {
			ctx->FocusedID = 0;
		}
	}

	PushCursorPos( CursorPos + Vector2D( 0, DrawSize.y + ( GUI::SplitStr( name, '#' )[ 0 ].size( ) ? 14 : 2 ) + GUI::ObjectPadding( ) ) );
	return ctx->FocusedID == GUI::Hash( name );
}