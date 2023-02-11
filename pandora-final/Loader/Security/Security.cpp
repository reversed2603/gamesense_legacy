#include "Security.hpp"
#include <Windows.h>
#include <TlHelp32.h>
#include <thread>

#include "../../Utils/extern/FnvHash.hpp"
#include "../../Utils/extern/syscall.hpp"
#include "../../Utils/LogSystem.hpp"

#include <Virtualizer/C/VirtualizerSDK.h>

CProtection g_protection;

/* hey btw, here we could also implement heartbeat ! */
void TestThread( ) {
	VIRTUALIZER_TIGER_WHITE_START
	int count = 0;

	// I hope this can't cause any false-positives, I only noticed 1 which was caused by nvidia, LOL!
	do {
		if( !g_protection.safety_check( ) ) {
			++count;

			g_Log.Log( XorStr( ".pdr" ), g_protection.error_string.c_str( ) );
		}

		if( count >= 3 || g_is_cracked ) {
			g_Vars.globals.hackUnload = true;
		}

		std::this_thread::sleep_for( std::chrono::seconds( 1 ) );
	} while( !g_Vars.globals.hackUnload );

	VIRTUALIZER_TIGER_WHITE_END
}

void CProtection::Run( ) {
	HANDLE thread;
	syscall( NtCreateThreadEx )( &thread, THREAD_ALL_ACCESS, nullptr, current_process,
		nullptr, nullptr, THREAD_CREATE_FLAGS_CREATE_SUSPENDED | THREAD_CREATE_FLAGS_HIDE_FROM_DEBUGGER, NULL, NULL, NULL, nullptr );
	CONTEXT context;
	context.ContextFlags = CONTEXT_FULL;
	syscall( NtGetContextThread )( thread, &context );
	context.Eax = reinterpret_cast< uint32_t >( &TestThread );
	syscall( NtSetContextThread )( thread, &context );
	syscall( NtResumeThread )( thread, nullptr );

	if( thread ) {
		LI_FN( CloseHandle )( thread );
	}
}
