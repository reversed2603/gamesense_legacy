#include "../gui.h"
#include <algorithm>
#include "../../../source.hpp"

void line( Vector2D pos, bool nigger = false ) {
	for( auto i = 5; i >= 2; --i ) {
		auto offset = 5 - i;
		Render::DirectX::line( Vector2D( pos.x + offset, pos.y + offset ), Vector2D( pos.x + offset + std::clamp( i - offset, 0, 5 ), pos.y + offset ), nigger ? Color( 0, 0, 0 ) : Color( 151, 151, 151 ) );
	}
}

bool GUI::Controls::Dropdown( const std::string& name, std::vector< std::string > options, int* var_name, int max_items ) {
	Vector2D CursorPos = PopCursorPos( );
	Vector2D DrawPos = ctx->pos + CursorPos;
	Vector2D DrawSize = Vector2D( std::min( ( int )ctx->ParentSize.x - 90, 270 ), 19 );

	if( *var_name < 0 )
		*var_name = 0;
	else if( *var_name >= options.size( ) )
		*var_name = options.size( ) - 1;

	DrawPos.x += 20;

	if( !GUI::SplitStr( name, '#' )[ 0 ].empty( ) ) {
		Render::DirectX::Fonts::menu.draw_text( GUI::SplitStr( name, '#' )[ 0 ].data( ), DrawPos + Vector2D( 0, -3 ), Color( 203, 203, 203 ), 4 );

		DrawPos.y += 11;
	}
	else {
		DrawPos.y -= 3;
	}

	bool hovered = g_InputSystem.IsInBox( DrawPos, DrawSize ) && g_InputSystem.IsInBox( ctx->ParentPos, ctx->ParentSize );

	if( hovered || ctx->DropdownInfo.HashedID == GUI::Hash( name ) )
		Render::DirectX::gradient_v( DrawPos, DrawSize, { 41, 41, 41 }, { 46, 46, 46 } );
	else
		Render::DirectX::gradient_v( DrawPos, DrawSize, { 31, 31, 31 }, { 36, 36, 36 } );

	line( DrawPos + DrawSize - Vector2D( 9, 6 ) - Vector2D( 0, 6 ), true );
	line( DrawPos + DrawSize - Vector2D( 9, 6 ) + Vector2D( 0, 1 ) - Vector2D( 0, 6 ) );

	Render::DirectX::rect( DrawPos, DrawSize, { 10, 10, 10 } );
	Render::DirectX::Fonts::menu.draw_text( options[ *var_name ], DrawPos + Vector2D( 10, 4 ), { 150, 150, 150 }, 0 );

	if( ctx->FocusedID == 0 ) {
		if( hovered && InputHelper::Pressed( VK_LBUTTON ) )
			ctx->FocusedID = GUI::Hash( name );
	}
	else if( ctx->FocusedID == GUI::Hash( name ) ) {
		if( hovered ) {
			ctx->DropdownInfo.Elements = options;
			ctx->DropdownInfo.Size = DrawSize.x;
			ctx->DropdownInfo.Option = var_name;
			ctx->DropdownInfo.Pos = DrawPos + Vector2D( 0, DrawSize.y + 3 );
			ctx->DropdownInfo.HashedID = GUI::Hash( name );
		}
	}

	GUI::PushCursorPos( CursorPos + Vector2D( 0, DrawSize.y + GUI::ObjectPadding( ) + ( GUI::SplitStr( name, '#' )[ 0 ].empty( ) ? -4 : 11 ) ) );
	return 69 + GUI::Hash( XorStr( "fuck you" ) );
}
