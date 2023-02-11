#include "../gui.h"
#include <algorithm>
#include "../../../source.hpp"

D3DVIEWPORT9 o_vp;
Vector2D gp;
Vector2D gs;
int id;

std::string name;

std::map<int, int> scrolling;

Vector2D override_size = { -1, -1 };

void GUI::Group::BeginGroup( const std::string& name, const Vector2D& size ) {
	size_t id = GUI::Hash( name );
	::name = GUI::SplitStr( name, '#' )[ 0 ].data( );
	::id = id;

	if( scrolling.find( id ) == scrolling.end( ) ) {
		scrolling.insert( { id, 0 } );
	}

	Vector2D cursor_pos = PopCursorPos( );
	Vector2D group_size;

	float width_percent = ( float )size.x / 100.f;
	float height_percent = ( float )size.y / 100.f;

	int width_available = ctx->size.x - 154;
	int height_available = ctx->size.y - 63;

	if( cursor_pos.x == 104 && width_percent == 1.0f )
		width_available += 20;

	group_size.x = width_available * width_percent;

	if( cursor_pos.y == 24 && height_percent == 1.0f )
		height_available += 20;

	group_size.y = height_available * height_percent;

	if( override_size.x != -1 )
		group_size.x = override_size.x;

	if( override_size.y != -1 )
		group_size.y = override_size.y;

	if( ( cursor_pos.y - 20 + group_size.y ) > ctx->size.y - 37 ) {
		cursor_pos.x += group_size.x + 20;
		cursor_pos.y = 24;

		PushCursorPos( cursor_pos );
		Group::BeginGroup( name, size );
	}
	else {
		Vector2D draw_pos = ctx->pos + cursor_pos;

		Render::DirectX::rect_fill( draw_pos, group_size, { 17, 17, 17 } );
		Render::DirectX::rect( draw_pos, group_size, { 10, 10, 10 } );
		Render::DirectX::rect( draw_pos + 1, group_size - 2, { 48, 48, 48 } );
		gp = draw_pos;
		gs = group_size;

		Vector2D text_size = Render::DirectX::Fonts::menu_bold.measure_size( GUI::SplitStr( name, '#' )[ 0 ].data( ) );

		Render::DirectX::rect_fill( draw_pos + Vector2D( 11, 0 ), Vector2D( 6 + text_size.x, 2 ), { 17, 17, 17 } );
		// fake group resize thingy xD

		//Render::DirectX::Fonts::menu_bold.draw_text(name, draw_pos + point(14, -5), { 203, 203, 203 }, 4);

		//Render::DirectX::rect(draw_pos, draw_pos + 10, color(255, 0, 0));

		o_vp = Render::DirectX::get_viewport( );
		D3DVIEWPORT9 meme = { draw_pos.x, draw_pos.y + 5, group_size.x, group_size.y - 7, 0.f, 1.f };
		Render::DirectX::set_viewport( meme );


		PushCursorPos( cursor_pos + Vector2D( 22, 23 + scrolling[ id ] ) );

		ctx->parent = "root." + ctx->Tabs[ ctx->ActiveTab ] + "." + name;
		ctx->NextGroupPos = cursor_pos + Vector2D( 0, group_size.y + 20 );

		ctx->ParentPos = draw_pos;
		ctx->ParentSize = group_size;
	}
}

void GUI::Group::EndGroup( ) {
	Vector2D v1 = PopCursorPos( );
	int max_height = ( int )v1.y - ( ( int )gp.y - ( int )ctx->pos.y ) - scrolling[ id ];
	int VisibleHeight = gs.y + 5;
	PushCursorPos( v1 );

	int scrollbar_height, scrollbar_pos;

	if( max_height > VisibleHeight ) {
		scrollbar_height = ( ( float )gs.y / ( float )max_height ) * gs.y;
		scrollbar_pos = std::min( std::max( ( -( float )scrolling[ id ] / ( float )max_height ) * ( float )gs.y, 2.f ), gs.y - scrollbar_height - 2.f );

		bool ScrollBarHovered = g_InputSystem.IsInBox( gp + Vector2D( gs.x - 6, scrollbar_pos ), Vector2D( 4, scrollbar_height ) );

		// dont scroll if there is a listbox that we're about to scroll in
		if( !ctx->hovered_listbox && !ctx->MultiDropdownInfo.DraggingScroll && !ctx->DropdownInfo.DraggingScroll ) {
			if( ctx->FocusedID == 0 || ctx->FocusedID == id ) {
				bool bHoveredMain = g_InputSystem.IsInBox( gp, gs - Vector2D( 6, 0 ) );
				bool bHoveredScroll = g_InputSystem.IsInBox( gp + Vector2D( gs.x - 6, scrollbar_pos ), Vector2D( 4, scrollbar_height ) );

				if( bHoveredMain || bHoveredScroll || ctx->FocusedID == id ) {
					// Ayy, I'm runnin' to the money, you know how I'm comin'
					// Monday 'til Sunday night, be thumbin', thumbin', thumbin'
					// (...)
					// I told her throw that ass back so I can bust it like a bubble
					// South Memphis nigga in this bitch, yeah, you know you in trouble
					// Ain't nuthin but a P thang, baby
					// Young iced - out nigga going crazy
					if( ctx->FocusedID != id && InputHelper::Down( VK_LBUTTON ) && bHoveredScroll ) {
						ctx->FocusedID = id;
					}
					else if( ctx->FocusedID == id ) {
						if( InputHelper::Down( VK_LBUTTON ) ) {
							const auto scale = [ ] ( int in, int bmin, int bmax, int lmin, int lmax ) {
								return float( ( lmax - lmin ) * ( in - bmin ) ) / float( bmax - bmin ) + lmin;
							};

							// i think not perfect
							auto pizdo = std::max( float( float( gs.y * ( gs.y - 12 * 2 ) )
								/ float( -max_height + ( gs.y - 12 * 2 ) ) ), 30.f );

							scrolling[ id ] += scale( InputHelper::MouseDelta.y, 0, gs.y - pizdo, 0, max_height );
						}
						else {
							ctx->FocusedID = 0;
						}
					}

					if( ctx->FocusedID != id ) {
						if( g_InputSystem.GetScrollMouse( ) > 0 ) {
							scrolling[ id ] = scrolling[ id ] + 15;
						}
						else if( g_InputSystem.GetScrollMouse( ) < 0 ) {
							scrolling[ id ] = scrolling[ id ] - 15;
						}
					}
				}

				ctx->dragging_scrollbar = ctx->FocusedID == id;

				scrolling[ id ] = std::clamp<float>( scrolling[ id ], -max_height + ( int )gs.y, 0 );
			}
		}
	}
	else {
		scrolling[ id ] = 0;
	}

	PushCursorPos( ctx->NextGroupPos );
	ctx->NextGroupPos = Vector2D( 0, 0 );
	Render::DirectX::set_viewport( o_vp );
	Render::DirectX::Fonts::menu_bold.draw_text( name, gp + Vector2D( 14, -5 ), { 203, 203, 203 }, 4 );

	if( max_height > VisibleHeight ) {
		Render::DirectX::rect_fill( gp + Vector2D( gs.x - 7, 2 ), Vector2D( 6, gs.y - 3 ), Color( 45, 45, 45 ) );

		Render::DirectX::rect_fill( gp + Vector2D( gs.x - 6, scrollbar_pos ), Vector2D( 4, scrollbar_height + 1 ),
			( g_InputSystem.IsInBox( gp + Vector2D( gs.x - 6, scrollbar_pos ), Vector2D( 4, scrollbar_height + 1 ) ) || ctx->FocusedID == id ) ? Color( 80, 80, 80 ) : Color( 65, 65, 65 ) );

		// bottom
		Vector2D GradientSize = Vector2D( gs.x - 9, 20 );
		Vector2D GradientPos = gp + Vector2D( 2, gs.y - GradientSize.y - 1 );

		// bottom arrow facing down
		if( scrolling[ id ] >= ( -max_height + ( int )gs.y ) + 10 ) {
			Render::DirectX::gradient_v( GradientPos, GradientSize, Color( 17, 17, 17, 0 ), { 17, 17, 17 } );

			Render::DirectX::rect_fill( gp + gs - Vector2D( 17, 10 ), Vector2D( 5, 1 ), Color( 0, 0, 0 ) );
			Render::DirectX::rect_fill( gp + gs - Vector2D( 17, 9 ), Vector2D( 5, 1 ), Color( 204, 204, 204 ) );
			Render::DirectX::rect_fill( gp + gs - Vector2D( 16, 8 ), Vector2D( 3, 1 ), Color( 204, 204, 204 ) );
			Render::DirectX::rect_fill( gp + gs - Vector2D( 15, 7 ), Vector2D( 1, 1 ), Color( 204, 204, 204 ) );
		}

		// top arrow facing up
		if( scrolling[ id ] <= -10 ) {
			// top
			GradientPos = gp + Vector2D( 2, 5 );
			Render::DirectX::gradient_v( GradientPos, GradientSize, { 17, 17, 17 }, Color( 17, 17, 17, 0 ) );

			Render::DirectX::rect_fill( gp + Vector2D( gs.x - 20, 7 ) + Vector2D( 5, 0 ), Vector2D( 1, 1 ), Color( 204, 204, 204 ) );
			Render::DirectX::rect_fill( gp + Vector2D( gs.x - 20, 7 ) + Vector2D( 4, 1 ), Vector2D( 3, 1 ), Color( 204, 204, 204 ) );
			Render::DirectX::rect_fill( gp + Vector2D( gs.x - 20, 7 ) + Vector2D( 3, 2 ), Vector2D( 5, 1 ), Color( 204, 204, 204 ) );
			Render::DirectX::rect_fill( gp + Vector2D( gs.x - 20, 7 ) + Vector2D( 3, 3 ), Vector2D( 5, 1 ), Color( 0, 0, 0 ) );
		}
	}
}

void GUI::Group::SetNextSize( const Vector2D& size ) {
	override_size = size;
}

void GUI::Group::PopLastSize( ) {
	override_size = { -1, -1 };
}