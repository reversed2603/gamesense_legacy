#include "../gui.h"

bool GUI::Controls::Label( const std::string& name, bool unsafe ) {
	Vector2D CursorPos = PopCursorPos( );
	Vector2D DrawPos = ctx->pos + CursorPos;

	DrawPos.x += 20;
	Render::DirectX::Fonts::menu.draw_text( GUI::SplitStr( name, '#' )[ 0 ].data( ), DrawPos - Vector2D( 0, 5 ), unsafe ? Color( 180, 180, 100 ) : Color( 200, 200, 200 ), 4 );

	GUI::PushCursorPos( CursorPos + Vector2D( 0, 8 + GUI::ObjectPadding( ) ) );
	return ( name.size( ) > 0 );
}
