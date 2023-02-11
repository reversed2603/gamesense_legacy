#include "InputHelper.h"
#include "../../source.hpp"

#include <Windows.h>

void InputHelper::Update( ) {
	if( GetForegroundWindow( ) == Interfaces::hWindow ) {
		for( int i = 0; i < 256; i++ ) {
			PrevKeyState[ i ] = KeyState[ i ];
			KeyState[ i ] = GetAsyncKeyState( i );
		}
	}

	static HWND window;
	if( !window )
		window = FindWindowA( XorStr( "Valve001" ), 0 );

	POINT mouse;
	GetCursorPos( &mouse );
	ScreenToClient( window, &mouse );

	MouseDelta = MousePos - Vector2D( mouse.x, mouse.y );

	MousePos = Vector2D( mouse.x, mouse.y );
}

bool InputHelper::Down( int key ) {
	return KeyState[ key ] && PrevKeyState[ key ];
}

bool InputHelper::Pressed( int key ) {
	return KeyState[ key ] && !PrevKeyState[ key ];
}

bool InputHelper::Released( int key ) {
	return !KeyState[ key ] && PrevKeyState[ key ];
}