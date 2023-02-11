#pragma once
#include "../../Utils/extern/XorStr.hpp"
#include <Windows.h>
#include <Winternl.h>
#include <TlHelp32.h>
#include <Psapi.h>
#include <string>

#include "../../SDK/variables.hpp"
#include "../Exports.h"

enum protection_status_t
{
	STATUS_SAFE,
	STATUS_UNSAFE_SYSTEM,

	STATUS_WINAPI_DEBUGGER,
	STATUS_WINAPI_REMOTE_DEBUGGER,
	STATUS_PEB_DEBUGGER,
	STATUS_THREAD_DEBUGGER,
	STATUS_THREAD_REGISTER_DEBUGGER,
	STATUS_KUSER_KERNEL_DEBUGGER,

	STATUS_PROGRAM_NOSNAPSHOT,
	STATUS_PROGRAM_EBUG,
	STATUS_PROGRAM_DBG,

	STATUS_DRIVER_NOENUM,
	STATUS_DRIVER_SANDBOXIE,
	STATUS_DRIVER_WINPCAP,
	STATUS_DRIVER_PROCESS_HACKER,
	STATUS_DRIVER_CHEAT_ENGINE,
};

static constexpr uintptr_t KUSER_SHARED_DATA = 0x7FFE0000;

#include "../../Utils/extern/lazy_importer.hpp"

class CProtection
{
	__forceinline bool is_debugger_running( )
	{
		// [junk-start]
		if( LI_FN( IsDebuggerPresent )( ) )
		{
			error_string = XorStr( "ERROR 0xD" );
			return true;
		}

		BOOL bIsDbgPresent = false;
		LI_FN( CheckRemoteDebuggerPresent )( LI_FN( GetCurrentProcess )( ), &bIsDbgPresent );

		if( bIsDbgPresent )
		{
			error_string = XorStr( "ERROR 0xLMAOC" );
			return true;
		}

		PPEB pPeb = ( PPEB )__readfsdword( 0x30 );
		if( pPeb->BeingDebugged == 1 )
		{
			error_string = XorStr( "ERROR 0x030C" );
			return true;
		}

		static PCONTEXT ctx = PCONTEXT( LI_FN( VirtualAlloc )( nullptr, sizeof( ctx ), MEM_COMMIT, PAGE_READWRITE ) );
		SecureZeroMemory( ctx, sizeof( CONTEXT ) );
		ctx->ContextFlags = CONTEXT_DEBUG_REGISTERS;

		if( LI_FN( GetThreadContext )( LI_FN( GetCurrentThread )( ), ctx ) == 0 )
		{
			error_string = XorStr( "ERROR 0x1337C" );
			return true;
		}

		if( ctx->Dr0 != 0 || ctx->Dr1 != 0 || ctx->Dr2 != 0 || ctx->Dr3 != 0 )
		{
			error_string = XorStr( "ERROR 0x69C" );
			return true;
		}

		if( *( BOOLEAN* )( KUSER_SHARED_DATA + 0x02d4 ) )
		{
			error_string = XorStr( "ERROR 0x420C" );
			return true;
		}

		return false;
		// [junk-end]
	}

	__forceinline bool is_running_forbidden_program( )
	{
		// [junk-start]
		std::string szProcesses[ ] = {
			XorStr( "ebug" ),
			XorStr( "dbg" ),
			XorStr( "idaq" ),
			XorStr( "regmon" ),
			XorStr( "nalyzer" ),
			XorStr( "ireshark" ),
			XorStr( "HookExplorer" ),
			XorStr( "PETools" ),
			XorStr( "LordPE" ),
			XorStr( "SysInspector" ),
			XorStr( "sniff_" ),
			XorStr( "procmon" ),
			XorStr( "dump" ),
			XorStr( "procexp" ),
			XorStr( "olly" ),
			XorStr( "tcpview" ),
			XorStr( "autoruns" ),
			XorStr( "ImportREC" ),
			XorStr( "joebox" ),
			XorStr( "acker" ),
			XorStr( "filemon" ),
			XorStr( "hunter" ), //pc hunter
			XorStr( "atengi" ), //cheat engine
		};

		WORD iLength = sizeof( szProcesses ) / sizeof( szProcesses[ 0 ] );

		PROCESSENTRY32 pe32;
		SecureZeroMemory( &pe32, sizeof( PROCESSENTRY32 ) );

		// We want a snapshot of processes
		HANDLE hSnapshot = LI_FN( CreateToolhelp32Snapshot )( TH32CS_SNAPPROCESS, 0 );

		// Check for a valid handle, in this case we need to check for
		// INVALID_HANDLE_VALUE instead of NULL
		if( hSnapshot == INVALID_HANDLE_VALUE )
		{
			error_string = XorStr( "ERROR 0x000C" );
			return true;
		}

		// Now we can enumerate the running process, also 
		// we can't forget to set the PROCESSENTRY32.dwSize member
		// otherwise the following functions will fail
		pe32.dwSize = sizeof( PROCESSENTRY32 );

		if( !LI_FN( Process32First )( hSnapshot, &pe32 ) )
		{
			LI_FN( CloseHandle )( hSnapshot );
			error_string = XorStr( "ERROR 0x000C" );
			return true;
		}

		do
		{
			for( int i = 0; i < iLength; i++ )
			{
				if( strstr( pe32.szExeFile, XorStr( "tracker" ) ) || strstr( pe32.szExeFile, XorStr( "Tracker" ) ) )
					continue;

				if( strstr( pe32.szExeFile, szProcesses[ i ].c_str( ) ) )
				{
					LI_FN( CloseHandle )( hSnapshot );

					error_string = XorStr( "ERR_0x" );
					error_string += std::to_string( i );
					return true;
				}
			}
		} while( LI_FN( Process32Next )( hSnapshot, &pe32 ) );

		LI_FN( CloseHandle )( hSnapshot );
		return false;
		// [junk-end]
	}

	__forceinline bool is_running_forbidden_driver( ) {
		// [junk-start]
		std::string szDrivers[ ] = {
			//XorStr( "Sbie" ), //sandboxie
			XorStr( "acker" ), //process hacker
			XorStr( "CEDRI" ), //cheatengine
		};

		WORD iLength = sizeof( szDrivers ) / sizeof( szDrivers[ 0 ] );

		LPVOID drivers[ 1024 ];
		DWORD cbNeeded;

		using GetDeviceDriverBaseNameA_t = int( __stdcall* )( LPVOID, LPSTR, DWORD );
		static auto GetDeviceDriverBaseNameA_fn = ( GetDeviceDriverBaseNameA_t )LI_FN( GetProcAddress )( GetModuleHandleA( XorStr( "kernel32.dll" ) ), "K32GetDeviceDriverBaseNameA" );

		if( LI_FN( EnumDeviceDrivers )( drivers, sizeof( drivers ), &cbNeeded ) && cbNeeded < sizeof( drivers ) ) {
			TCHAR szDriver[ 1024 ];
			int cDrivers = cbNeeded / sizeof( drivers[ 0 ] );
			for( int i = 0; i < cDrivers; i++ ) {
				if( GetDeviceDriverBaseNameA_fn( drivers[ i ], szDriver, sizeof( szDriver ) / sizeof( szDriver[ 0 ] ) ) ) {
					if( strstr( szDriver, XorStr( "tracker" ) ) || strstr( szDriver, XorStr( "Tracker" ) ) )
						continue;

					for( int j = 0; j < iLength; j++ ) {
						if( strstr( szDriver, szDrivers[ j ].c_str( ) ) )
						{
							error_string = XorStr( "ERR_" );
							error_string += szDriver;
							return true;
						}
					}
				}
			}
		}
		else {
			error_string = XorStr( "ERROR: FATAL" );
			return true;
		}

		return false;
		// [junk-end]
	}
public:
	std::string error_string;
	char* random_data;

	__declspec( noinline ) bool initial_safety_check( )
	{
		// [junk-start]
		//we ~~dont~~ do ban for these, instead we warn user and exit

		//ida
		{
			PROCESSENTRY32 pe32;
			SecureZeroMemory( &pe32, sizeof( PROCESSENTRY32 ) );

			// We want a snapshot of processes
			HANDLE hSnapshot = LI_FN( CreateToolhelp32Snapshot )( TH32CS_SNAPPROCESS, 0 );

			if( hSnapshot == INVALID_HANDLE_VALUE )
			{
				error_string = XorStr( "UNSPECIFIED SYSTEM ERROR 0x1337" );
				return false;
			}

			pe32.dwSize = sizeof( PROCESSENTRY32 );

			if( !LI_FN( Process32First )( hSnapshot, &pe32 ) )
			{
				LI_FN( CloseHandle )( hSnapshot );
				error_string = XorStr( "UNSPECIFIED SYSTEM ERROR 0x420" );
				return false;
			}

			do
			{
				if( strstr( pe32.szExeFile, XorStr( "idaq" ) ) )
				{
					LI_FN( CloseHandle )( hSnapshot );
					error_string = XorStr( "HAHIDA" );
					return false;
				}
			} while( LI_FN( Process32Next )( hSnapshot, &pe32 ) );

			LI_FN( CloseHandle )( hSnapshot );
		}

		return true;
		// [junk-end]
	}

	__forceinline bool safety_check( )
	{
		// [junk-start]
		if( is_debugger_running( ) )
			return false;

		if( is_running_forbidden_program( ) )
			return false;

		if( is_running_forbidden_driver( ) )
			return false;

		//if( retard_shit( ) || g_is_cracked )
		//	return false;

		return true;
		// [junk-end]
	}

	__forceinline uint32_t crc_rt( const uint8_t* p, size_t size ) {
		size_t s = size;
		size_t result = 0;
		const size_t prime = 31;
		for( size_t i = 0; i < s; ++i )
			result = p[ i ] + ( result * prime );

		return result;
	}

	__forceinline bool check_idk( ) {
#ifdef DEV
		return false;
#endif

		static const auto text_hash = crc_rt( ( uint8_t* )g_text_start, g_text_size );

		if( text_hash != g_text_hash ) {
			g_is_cracked = true;
			return true;
		}

		if( *( uint16_t* )g_Vars.globals.hModule == IMAGE_DOS_SIGNATURE ) {
			g_is_cracked = true;
			return true;
		}

		if( ( ( IMAGE_DOS_HEADER* )g_Vars.globals.hModule )->e_lfanew != 0 ) {
			g_is_cracked = true;
			return true;
		}

		return g_is_cracked;
	}

	__forceinline bool check_round( ) {
#ifdef DEV
		return false;
#endif

		static const auto text_hash = crc_rt( ( uint8_t* )g_text_start, g_text_size );

		if( text_hash != g_text_hash ) {
			g_is_cracked = true;
			return true;
		}

		if( *( uint16_t* )g_Vars.globals.hModule == IMAGE_DOS_SIGNATURE ) {
			g_is_cracked = true;
			return true;
		}

		if( ( ( IMAGE_DOS_HEADER* )g_Vars.globals.hModule )->e_lfanew != 0 ) {
			g_is_cracked = true;
			return true;
		}

		return g_is_cracked;
	}

	__forceinline bool check_start( ) {
#ifdef DEV
		return false;
#endif

		static const auto text_hash = crc_rt( ( uint8_t* )g_text_start, g_text_size );

		if( text_hash != g_text_hash ) {
			g_is_cracked = true;
			return true;
		}

		if( *( uint16_t* )g_Vars.globals.hModule == IMAGE_DOS_SIGNATURE ) {
			g_is_cracked = true;
			return true;
		}

		if( ( ( IMAGE_DOS_HEADER* )g_Vars.globals.hModule )->e_lfanew != 0 ) {
			g_is_cracked = true;
			return true;
		}

		return g_is_cracked;
	}

	void Run( );

};

extern CProtection g_protection;