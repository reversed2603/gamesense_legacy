#include "source.hpp"
#include "Utils/InputSys.hpp"
#include "Utils/extern/defs.hpp"

#include "Utils/LogSystem.hpp"

#pragma disable(warning:4099)


#include "Utils/Threading/threading.h"
#include "Utils/Threading/shared_mutex.h"
#include "SDK/displacement.hpp"
#include "SDK/variables.hpp"
#include "Utils/extern/lazy_importer.hpp"
#include "Utils/CrashHandler.hpp"
#include <thread>

#include "Features/Rage/TickbaseShift.hpp"

#include <iomanip> 
#include "Utils/extern/syscall.hpp"
#include "Loader/Security/Security.hpp"
#include "Loader/Exports.h"

#include "Utils/Config.hpp"

#include <Virtualizer/C/VirtualizerSDK.h>

static Semaphore dispatchSem;
static SharedMutex smtx;

using ThreadIDFn = int( _cdecl* )( );

ThreadIDFn AllocateThreadID;
ThreadIDFn FreeThreadID;

int AllocateThreadIDWrapper( ) {
	return AllocateThreadID( );
}

int FreeThreadIDWrapper( ) {
	return FreeThreadID( );
}

template<typename T, T& Fn>
static void AllThreadsStub( void* ) {
	dispatchSem.Post( );
	smtx.rlock( );
	smtx.runlock( );
	Fn( );
}

// TODO: Build this into the threading library
template<typename T, T& Fn>
static void DispatchToAllThreads( void* data ) {
	smtx.wlock( );

	for( size_t i = 0; i < Threading::numThreads; i++ )
		Threading::QueueJobRef( AllThreadsStub<T, Fn>, data );

	for( size_t i = 0; i < Threading::numThreads; i++ )
		dispatchSem.Wait( );

	smtx.wunlock( );

	Threading::FinishQueue( false );
}

struct DllArguments {
	HMODULE hModule;
	LPVOID lpReserved;
};

DWORD WINAPI Unload( DllArguments* pArgs ) {
	Threading::FinishQueue( );

	std::this_thread::sleep_for( std::chrono::milliseconds( 200 ) );

	Interfaces::Destroy( );

	std::this_thread::sleep_for( std::chrono::milliseconds( 200 ) );

	DispatchToAllThreads<decltype( FreeThreadIDWrapper ), FreeThreadIDWrapper>( nullptr );

	Threading::EndThreads( );


	// let's freeeeee
	FreeThreadID( );

	LI_FN( FreeLibraryAndExitThread )( pArgs->hModule, EXIT_SUCCESS );

	delete pArgs;

	return TRUE;
}

DWORD WINAPI Entry( DllArguments* pArgs ) {
	MEMORY_BASIC_INFORMATION bruh;
	VirtualQuery( pArgs->hModule, &bruh, sizeof( MEMORY_BASIC_INFORMATION ) );

	VIRTUALIZER_TIGER_WHITE_START

	g_text_start += ( uint32_t )pArgs->hModule;

#ifndef DEV
	g_Vars.globals.user_info = *( CVariables::GLOBAL::cheat_header_t* )pArgs->hModule;
	g_Vars.globals.c_login = reinterpret_cast< const char* >( pArgs->hModule );
	g_Vars.globals.hModule = pArgs->hModule;

	g_protection.Run( );

	auto some_event = CreateEvent( NULL, TRUE, FALSE, XorStr( "pandora_is_the_best_du_hurensohn" ) );

	switch( GetLastError( ) ) {
	case ERROR_ALREADY_EXISTS: {
		//HWND null = NULL;
		//LI_FN( MessageBoxA )( null, XorStr( "Good job!" ), XorStr( "Fatal user issue." ), NULL );
		LI_FN( FreeLibraryAndExitThread )( pArgs->hModule, EXIT_SUCCESS );
		return FALSE;
		break;
	}
	}
#endif

	// XDDXD
	g_Vars.menu.key.key = VK_INSERT;
	
	// xd v2 :D
	g_Vars.rage.key.cond = 3;

	g_Vars.rage_default.quick_stop_key.cond = 3;

	// let's clear the log file
	g_Log.Clear( XorStr( ".pdr" ) );

	g_protection.check_start( );

	VIRTUALIZER_TIGER_WHITE_END

	// lazy way of checking if the fucking directory exists kekw
	ConfigManager::GetConfigs( );

	// create the file if it's not there
	ConfigManager::CreateConfig( XorStr( "global_data" ) );

	// load the global data
	ConfigManager::LoadConfig( XorStr( "global_data" ), true );

	// this is only for the initial (first time use) of the cheat
	ConfigManager::SaveConfig( XorStr( "global_data" ), true );

#ifdef DEV 
	AllocConsole( );
	freopen_s( ( FILE** )stdin, XorStr( "CONIN$" ), XorStr( "r" ), stdin );
	freopen_s( ( FILE** )stdout, XorStr( "CONOUT$" ), XorStr( "w" ), stdout );
	SetConsoleTitleA( XorStr( " " ) );
#endif

	auto prior = GetPriorityClass( GetCurrentProcess( ) );
	if( prior != REALTIME_PRIORITY_CLASS && prior != HIGH_PRIORITY_CLASS )
		SetPriorityClass( GetCurrentProcess( ), HIGH_PRIORITY_CLASS );

#ifdef DEV
	g_is_cracked = false;

	g_Vars.globals.c_login = XorStr( "admin" );

	g_Vars.globals.hModule = pArgs->hModule;

	while( !GetModuleHandleA( XorStr( "serverbrowser.dll" ) ) ) {
		std::this_thread::sleep_for( std::chrono::milliseconds( 50 ) );
	}

	// kekw
	g_Vars.globals.user_info.sub_expiration -= 1605705796;
#endif // !DEV

	g_Vars.globals.user_info.sub_expiration = g_Vars.globals.user_info.sub_expiration / ( 60 * 60 * 24 );

	auto tier0 = GetModuleHandleA( XorStr( "tier0.dll" ) );

	AllocateThreadID = ( ThreadIDFn )GetProcAddress( tier0, XorStr( "AllocateThreadID" ) );
	FreeThreadID = ( ThreadIDFn )GetProcAddress( tier0, XorStr( "FreeThreadID" ) );

	Threading::InitThreads( );

	DispatchToAllThreads<decltype( AllocateThreadIDWrapper ), AllocateThreadIDWrapper>( nullptr );

	// let's also allocate our main thread :)
	AllocateThreadID( );

	if( Interfaces::Create( pArgs->lpReserved ) ) {

		m_pInputSystem->EnableInput( true );

		for( auto& child : g_Vars.m_children ) {
			child->Save( );

			auto json = child->GetJson( );
			g_Vars.m_json_default_cfg[ std::to_string( child->GetName( ) ) ] = ( json );
		}

#ifndef DEV
		while( !g_Vars.globals.hackUnload ) {
			std::this_thread::sleep_for( std::chrono::milliseconds( 200 ) );

			if( g_is_cracked )
				g_Vars.globals.hackUnload = true;
		}

		Unload( pArgs );

		return TRUE;
#endif
	}

#ifdef DEV
	while( !g_Vars.globals.hackUnload ) {
		//while( !g_Vars.globals.hackUnload ) {
		std::this_thread::sleep_for( std::chrono::milliseconds( 200 ) );
	}

	fclose( ( FILE* )stdin );
	fclose( ( FILE* )stdout );
	FreeConsole( );

	return Unload( pArgs );
#else
	return FALSE;
#endif
}

LONG WINAPI CrashHandlerWrapper( struct _EXCEPTION_POINTERS* exception ) {
	auto ret = g_CrashHandler.OnProgramCrash( exception );
	return ret;
}

BOOL APIENTRY DllMain( HMODULE hModule, DWORD dwReason, LPVOID lpReserved ) {
	if( dwReason == DLL_PROCESS_ATTACH ) {
		DisableThreadLibraryCalls( hModule );

		SetUnhandledExceptionFilter( CrashHandlerWrapper );

#if defined(BETA_MODE) || defined(DEV)
		AddVectoredExceptionHandler( 1, CrashHandlerWrapper );
#endif

		DllArguments* args = new DllArguments( );
		args->hModule = hModule;
		args->lpReserved = lpReserved;


#ifdef DEV
		HANDLE thread;
		syscall( NtCreateThreadEx )( &thread, THREAD_ALL_ACCESS, nullptr, current_process,
			nullptr, args, THREAD_CREATE_FLAGS_CREATE_SUSPENDED, NULL, NULL, NULL, nullptr );
		CONTEXT context;
		context.ContextFlags = CONTEXT_FULL;
		syscall( NtGetContextThread )( thread, &context );
		context.Eax = reinterpret_cast< uint32_t >( &Entry );
		syscall( NtSetContextThread )( thread, &context );
		syscall( NtResumeThread )( thread, nullptr );

		if( thread ) {
			strcpy( g_Vars.globals.user_info.username, XorStr( "admin" ) );
			g_Vars.globals.user_info.sub_expiration = 1640995200; // sencible date

			CloseHandle( thread );

			return TRUE;
		}
#else
		Entry( args );

		return TRUE;
#endif
	}

	return FALSE;
}
