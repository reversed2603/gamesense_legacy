#pragma once
#include <d3d9.h>
#include <d3dx9.h>
#pragma comment(lib, "d3dx9.lib")

class Vector2D;
class Color;
class c_sprite {
private:
	LPDIRECT3DTEXTURE9 texture;
	bool began;
	int w, h;
public:
	LPD3DXSPRITE sprite;
	template <typename t>
	void setup( IDirect3DDevice9* device, t texture_bytes, int size, int width, int height ) {
		this->w = width;
		this->h = height;
		D3DXCreateTextureFromFileInMemoryEx( device, ( void* )texture_bytes, size, width, height, 0, D3DPOOL_DEFAULT, D3DFMT_UNKNOWN, D3DPOOL_DEFAULT, D3DX_DEFAULT, D3DX_DEFAULT, D3DCOLOR_RGBA( 0, 0, 0, 255 ), NULL, NULL, &texture );
		D3DXCreateSprite( device, &sprite );
	}
	void invalidate( );
	void Draw( Vector2D pos, Color c );
};