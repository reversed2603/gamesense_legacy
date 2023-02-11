#pragma once

class CFontManager {
public:
	__forceinline char*& get_language( ) {
		return *( char** )( ( uintptr_t )this + 0x28 );
	}
};