#include "../Hooked.hpp"
#include "../../SDK/Classes/Player.hpp"

void __stdcall Hooked::LockCursor( ) {
   g_Vars.globals.szLastHookCalled = XorStr( "16" );

   if( !m_pEngine.IsValid( ) || !m_pSurface.IsValid( ) ) {
	   if( m_pSurface.IsValid( ) ) {
		   oLockCursor( ( void* )m_pSurface.Xor( ) );
	   }

	   return;
   }

   oLockCursor( ( void* )m_pSurface.Xor( ) );

   auto pLocal = C_CSPlayer::GetLocalPlayer( );

   bool state = true;
   if( !m_pEngine->IsInGame( ) || ( pLocal != nullptr ) || GUI::ctx->typing ) {
	   if( pLocal != nullptr ) {
		   if( pLocal->IsDead( ) ) {
			   state = !g_Vars.globals.menuOpen;
		   }
		   else {
			   if( GUI::ctx->typing || !m_pEngine->IsInGame( ) ) {
				   state = !g_Vars.globals.menuOpen;
			   }
		   }
	   }
	   else {
		   if( GUI::ctx->typing || !m_pEngine->IsInGame( ) ) {
			   state = !g_Vars.globals.menuOpen;
		   }
	   } 
   }

   m_pInputSystem->EnableInput( state );

   if ( g_Vars.globals.menuOpen )
	  m_pSurface->UnlockCursor( );
}
