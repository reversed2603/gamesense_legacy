#include "../hooked.hpp"

int __fastcall Hooked::IsBoxVisible( void* ECX, uint32_t, const Vector& mins, const Vector& maxs ) {
   g_Vars.globals.szLastHookCalled = XorStr( "12" );
   static const auto ret = XorStr( "\x85\xC0\x74\x2D\x83\x7D\x10\x00\x75\x1C" );

   if ( !memcmp( _ReturnAddress( ), ret, 10 ) )
	  return 1;

   return Hooked::oIsBoxVisible(ECX, mins, maxs );
}