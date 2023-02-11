#include "CrashHandler.hpp"
#include "LogSystem.hpp"
#include <winternl.h>

ICrashHandler g_CrashHandler;

long __stdcall ICrashHandler::OnProgramCrash( struct _EXCEPTION_POINTERS* ExceptionInfo ) {
	struct _LDR_DATA_TABLE_ENTRY_ { // nice undocumented struct nn
		PVOID Reserved1[ 2 ];
		LIST_ENTRY InMemoryOrderLinks;
		PVOID Reserved2[ 2 ];
		PVOID DllBase;
		PVOID Reserved3[ 2 ];
		UNICODE_STRING FullDllName;
		UNICODE_STRING BaseDllName;
	};

	auto peb = reinterpret_cast< PEB* >( reinterpret_cast< TEB* >( __readfsdword( 0x18 ) )->ProcessEnvironmentBlock );

	auto module_list = &peb->Ldr->InMemoryOrderModuleList;

	for( auto i = module_list->Flink; i != module_list; i = i->Flink ) {
		auto entry = CONTAINING_RECORD( i, _LDR_DATA_TABLE_ENTRY_, InMemoryOrderLinks );
		if( !entry )
			continue;

		auto dos = reinterpret_cast< IMAGE_DOS_HEADER* >( entry->DllBase );
		auto nt = reinterpret_cast< IMAGE_NT_HEADERS* >( ( uintptr_t )entry->DllBase + dos->e_lfanew );

		if( ( uintptr_t )ExceptionInfo->ExceptionRecord->ExceptionAddress >= ( uintptr_t )entry->DllBase &&
			( uintptr_t )ExceptionInfo->ExceptionRecord->ExceptionAddress <= ( uintptr_t )entry->DllBase + nt->OptionalHeader.SizeOfImage ) {
			g_Log.Log( XorStr( ".pdr" ), XorStr( "crash: %ws + 0x%08X" ), entry->BaseDllName.Buffer, ( uintptr_t )ExceptionInfo->ExceptionRecord->ExceptionAddress - ( uintptr_t )entry->DllBase );
			
			return EXCEPTION_CONTINUE_SEARCH;
		}
	}

	g_Log.Log( XorStr( ".pdr" ), XorStr( "crash: hack + 0x%08X | nignog %s" ), ( uintptr_t )ExceptionInfo->ExceptionRecord->ExceptionAddress - ( uintptr_t )g_Vars.globals.hModule, g_Vars.globals.szLastHookCalled.c_str( ) );

	return EXCEPTION_CONTINUE_SEARCH;
}
