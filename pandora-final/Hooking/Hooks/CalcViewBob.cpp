#include "../../source.hpp"
#include "../Hooked.hpp"
#include "../../SDK/Displacement.hpp"
#include "../../SDK/Classes/Player.hpp"

void __fastcall Hooked::CalcViewBob( C_BasePlayer* player, void* edx, Vector& eyeOrigin ) {
	if( !g_Vars.esp.remove_bob )
		oCalcViewBob( player, eyeOrigin );
}