#pragma once
#include "../../SDK/Valve/vector2d.hpp"

namespace InputHelper {
	inline bool KeyState[ 256 ];
	inline bool PrevKeyState[ 256 ];

	inline int Scroll;
	inline Vector2D MousePos;
	inline Vector2D MouseDelta;

	void Update( );
	bool Pressed( int key );
	bool Down( int key );
	bool Released( int key );
}