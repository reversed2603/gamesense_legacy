#include "../hooked.hpp"
#include "../../SDK/Classes/Player.hpp"

bool __fastcall Hooked::IsConnected( void ) {
   /*
   - string: "IsLoadoutAllowed"
   - follow up v8::FunctionTemplate::New function
   - inside it go to second function that is being called after "if" statement.
   - after that u need to open first function that is inside it.[ before( *( int ( ** )( void ) )( *( _DWORD* ) dword_152350E4 + 516 ) )( ); ]
   */

   g_Vars.globals.szLastHookCalled = XorStr( "13" );

   static void* is_loadout_allowed = ( void* ) ( Memory::Scan( XorStr( "client.dll" ), XorStr( "84 C0 75 04 B0 01 5F" ) ) );
   if ( is_loadout_allowed ) {
      C_CSPlayer* local = C_CSPlayer::GetLocalPlayer( );

      if ( g_Vars.misc.unlock_inventory && m_pEngine->IsInGame( ) && local && _ReturnAddress( ) == is_loadout_allowed )
         return false;
   }

   return Hooked::oIsConnected( );
}