#include "../hooked.hpp"

bool __fastcall Hooked::IsPlayingDemo( void* ECX, void* EDX ) {
   g_Vars.globals.szLastHookCalled = XorStr( "14" );

   if ( !ECX )
      return Hooked::oIsPlayingDemo( ECX );

   if ( !m_pEngine->IsInGame( ) || !m_pEngine->IsConnected( ) )
      return Hooked::oIsPlayingDemo( ECX );

   if ( g_Vars.misc.money_revealer ) {
	  if ( *static_cast< uintptr_t* >( _ReturnAddress( ) ) == 0x0975C084
		   && **reinterpret_cast< uintptr_t * * >( uintptr_t( _AddressOfReturnAddress( ) ) + 4 ) == 0x0C75C084 )
		 return true;
   } 

   return Hooked::oIsPlayingDemo( ECX );
}
