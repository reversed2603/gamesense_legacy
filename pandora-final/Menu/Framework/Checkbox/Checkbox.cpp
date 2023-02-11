#include "../gui.h"
#include "../../../source.hpp"

std::unordered_map<size_t, float> uAnimation;

bool GUI::Controls::Checkbox( const std::string& name, bool* value, bool unsafe ) {
	Vector2D CursorPos = PopCursorPos( );
	Vector2D DrawPos = ctx->pos + CursorPos;
	Vector2D DrawSize = Vector2D( 8, 8 );

	Render::DirectX::rect( DrawPos, DrawSize - 1, Color( 10, 10, 10 ) );

	Render::DirectX::Fonts::menu.draw_text( GUI::SplitStr( name, '#' )[ 0 ].data( ), DrawPos + Vector2D( 20, -3 ), unsafe ? Color( 180, 180, 100 ) : Color( 200, 200, 200 ), 0 );

	bool hovered = g_InputSystem.IsInBox( DrawPos, DrawSize - 1 ) && g_InputSystem.IsInBox( ctx->ParentPos, ctx->ParentSize );
	bool hovered_text = g_InputSystem.IsInBox( DrawPos + Vector2D( 0, -3 ), Render::DirectX::Fonts::menu.measure_size( GUI::SplitStr( name, '#' )[ 0 ].data( ) ) + Vector2D( 20, 0 ) ) && g_InputSystem.IsInBox( ctx->ParentPos, ctx->ParentSize );

	if( !( *value ) ) {
		if( hovered || hovered_text )
			Render::DirectX::gradient_v( DrawPos + 1, DrawSize - 2, Color( 83, 83, 83 ), Color( 58, 58, 58 ) );
		else
			Render::DirectX::gradient_v( DrawPos + 1, DrawSize - 2, Color( 75, 75, 75 ), Color( 51, 51, 51 ) );
	}
	else {
		Render::DirectX::gradient_v( DrawPos + 1, DrawSize - 2, g_Vars.menu.ascent.ToRegularColor( ), g_Vars.menu.ascent.ToRegularColor( ) * 0.75f );
	}

	if( ctx->FocusedID == 0 ) {
		if( ( hovered || hovered_text ) && InputHelper::Pressed( VK_LBUTTON ) )
			ctx->FocusedID = GUI::Hash( name );
	}
	else if( ctx->FocusedID == GUI::Hash( name ) ) {
		if( !InputHelper::Down( VK_LBUTTON ) ) {
			ctx->FocusedID = 0;

			if( ( hovered || hovered_text ) )
				*value ^= 1;
		}
	}

	GUI::PushCursorPos( CursorPos + Vector2D( 0, DrawSize.y + GUI::ObjectPadding( ) ) );
	return *value || ctx->setup;
}
