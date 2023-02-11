#include "../hooked.hpp"
#include "../../SDK/Classes/Player.hpp"
#include "../../SDK/variables.hpp"

class N00000B3B {
public:
	char pad_0x0000[ 0xC4 ]; //0x0000
	Vector N00000B6D; //0x00C4
	char pad_0x00D0[ 0x770 ]; //0x00D0

}; //Size=0x0840

void __fastcall Hooked::RenderView( void* ECX, void* EDX, const CViewSetup& view, CViewSetup& hudViewSetup, int nClearFlags, int whatToDraw ) {
	g_Vars.globals.szLastHookCalled = XorStr( "30" );
	uintptr_t arg_0;

	oRenderView( ECX, view, hudViewSetup, nClearFlags, whatToDraw );
}