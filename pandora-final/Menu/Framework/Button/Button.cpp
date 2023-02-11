#include "../gui.h"
#include <algorithm>
#include "../../../source.hpp"

void GUI::Controls::Button( const std::string& name, std::function< void( ) > callback, bool use_unique_id ) {
	Vector2D CursorPos = PopCursorPos( );
	Vector2D DrawPos = ctx->pos + CursorPos;
	Vector2D DrawSize = Vector2D( std::min( ( int )ctx->ParentSize.x - 90, 270 ), 24 );

	DrawPos.x += 20;

	bool hovered = g_InputSystem.IsInBox( DrawPos, DrawSize ) && g_InputSystem.IsInBox( ctx->ParentPos, ctx->ParentSize );

	if( ctx->FocusedID == GUI::Hash( name ) ) {
		Render::DirectX::gradient_v( DrawPos, DrawSize, { 30, 30, 30 }, { 20, 20, 20 } );
	}
	else {
		if( hovered )
			Render::DirectX::gradient_v( DrawPos, DrawSize, { 45, 45, 45 }, { 35, 35, 35 } );
		else
			Render::DirectX::gradient_v( DrawPos, DrawSize, { 35, 35, 35 }, { 25, 25, 25 } );
	}

	Render::DirectX::rect( DrawPos, DrawSize, { 10, 10, 10 } );
	Render::DirectX::rect( DrawPos + 1, DrawSize - 2, { 50, 50, 50 } );

	Vector2D text_size = Render::DirectX::Fonts::menu_bold.measure_size( GUI::SplitStr( name, '#' )[ 0 ].data( ) );
	Render::DirectX::Fonts::menu_bold.draw_text( GUI::SplitStr( name, '#' )[ 0 ].data( ), DrawPos + ( ( DrawSize / 2 ) - ( text_size / 2 ) ), { 220, 220, 220 }, 4 );

	if( ctx->FocusedID == 0 ) {
		if( hovered && InputHelper::Pressed( VK_LBUTTON ) ) {
			ctx->FocusedID = GUI::Hash( name );
		}
	}
	else if( ctx->FocusedID == GUI::Hash( name ) ) {
		if( !InputHelper::Down( VK_LBUTTON ) ) {
			ctx->FocusedID = 0;

			if( hovered ) {
				callback( );
			}
		}
	}

	GUI::PushCursorPos( CursorPos + Vector2D( 0, DrawSize.y + GUI::ObjectPadding( ) ) );
}
