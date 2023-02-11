#pragma once
#include "../Hooked.hpp"

uint32_t cookie_0 = 0;
uint32_t cookie_1 = 0;

// just some test code
void __fastcall Hooked::SetReservationCookie( void* ECX, void* EDX, int a2, int a3 ) {
   g_Vars.globals.szLastHookCalled = XorStr( "36" );

  return;
}
