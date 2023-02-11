#include "../../source.hpp"
#include "../Hooked.hpp"
#include "../../SDK/Displacement.hpp"
#include "../../SDK/Classes/Player.hpp"

void __fastcall Hooked::UpdateClientSideAnimation( C_CSPlayer* player, void* edx ) {
	if( !player || player->IsDead( ) )
		return oUpdateClientSideAnimation( player );

	if( !g_Vars.globals.m_bUpdatingAnimations )
		return;

	oUpdateClientSideAnimation( player );
}