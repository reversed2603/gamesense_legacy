#include "../hooked.hpp"

namespace Hooked
{
   void __fastcall OnSoundStarted( void* ECX, void* EDX, int guid, StartSoundParams_t& params, char const* soundname ) {
	  g_Vars.globals.szLastHookCalled = XorStr( "17" );
	  oOnSoundStarted( ECX, guid, params, soundname );
	  //if ( params.ServerFlags & SV_SND_FROMSERVER )
		 //g_ServerSounds.ProcessSound( guid, params, soundname );
   }
}