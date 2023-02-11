#pragma once

#include <d3d9.h>
#include <string>

enum font_flags {
	font_centered = ( 1 << 1 ),
	font_dropshadow = ( 1 << 2 ),
	font_outline = ( 1 << 3 ),
};

class font {
private:
	void Draw( std::string text, Vector2D pos, Color c, int flags ) {
		RECT rect;
		SetRect( &rect, pos.x, pos.y, pos.x, pos.y );
		p_font->DrawTextA( NULL, text.data( ), -1, &rect, ( ( flags & font_centered ) ? DT_CENTER : DT_LEFT ) | DT_NOCLIP, D3DCOLOR_RGBA( c.r( ), c.g( ), c.b( ), c.OverrideAlpha( 255 * alpha_mod, true ).a( ) ) );
	}
	std::string name;
	int height;
	unsigned int weight;
	bool anti_aliased = false;
public:
	font( std::string name, int height, unsigned int weight, bool anti_aliased ) {
		this->name = name;
		this->height = height;
		this->weight = weight;
		this->anti_aliased = anti_aliased;
	}
	font( std::string name, int height, unsigned int weight ) {
		this->name = name;
		this->height = height;
		this->weight = weight;
		this->anti_aliased = false;
	}

	IDirect3DDevice9* device;
	LPD3DXFONT p_font;
	void setup( IDirect3DDevice9* p_device ) {
		this->device = p_device;
	}

	void init( ) {
		D3DXCreateFontA( device, height, 0, weight, 0, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS, anti_aliased ? CLEARTYPE_NATURAL_QUALITY : NONANTIALIASED_QUALITY, DEFAULT_PITCH, name.data( ), &p_font );
	}

	void invalidate( ) {
		p_font->Release( );
	}

	void draw_text( std::string text, Vector2D pos, Color c, int flags ) {
		if ( flags & font_dropshadow ) {
			Draw( text, pos + 1, Color( 0, 0, 0, c.OverrideAlpha( 255 * alpha_mod, true ).a( ) ), flags );

		}

		if ( flags & font_outline ) {
			// bleh
			for ( int x = -1; x <= 1; x++ ) for ( int y = -1; y <= 1; y++ ) Draw( text, Vector2D( pos.x + x, pos.y + y ), Color( 0, 0, 0, c.OverrideAlpha( 255 * alpha_mod, true ).a( ) ), flags );
		}

		Draw( text, pos, c.OverrideAlpha( 255 * alpha_mod, true ), flags );
	}

	Vector2D measure_size( const std::string& text ) {
		RECT rect;
		p_font->DrawTextA( 0, text.c_str( ), text.size( ), &rect, DT_CALCRECT, 0 );
		return Vector2D( rect.right - rect.left, rect.bottom - rect.top );
	}
};