#include "../../source.hpp"
#include "../Hooked.hpp"
#include "../../SDK/Displacement.hpp"

void __fastcall Hooked::VertexBufferLock( void* ecx, void* edx, int max_vertex_count, bool append, void* unk ) {
	g_Vars.globals.szLastHookCalled = XorStr( "69vBl" );

	oVertexBufferLock( ecx, std::min( 32767, max_vertex_count ), append, unk );
}