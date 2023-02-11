#include "sprite.h"
#include "../../../SDK/Valve/vector2d.hpp"
#include "../../../SDK/color.hpp"

void c_sprite::invalidate( ) {
	this->texture->Release( );
	delete this->texture;
	this->sprite->Release( );
	delete this->sprite;
}

void c_sprite::Draw( Vector2D pos, Color c ) {
	sprite->Begin( D3DXSPRITE_ALPHABLEND );

	D3DXVECTOR3 position;

	position.x = pos.x;
	position.y = pos.y;
	position.z = 0.01f;

	D3DXVECTOR3 scaling( 1.f, 1.f, 1.f );
	D3DXVECTOR3 spriteCentre( 100, 100, 0 );
	D3DXMATRIX matrix;

	D3DXMatrixTransformation( &matrix, NULL, 0, &scaling, &spriteCentre, NULL, &position );

	sprite->SetTransform( &matrix );

	sprite->Draw( texture, NULL, NULL, NULL, c.OverrideAlpha( 255 * alpha_mod, true ).ToARGB( ) );

	sprite->End( );
}