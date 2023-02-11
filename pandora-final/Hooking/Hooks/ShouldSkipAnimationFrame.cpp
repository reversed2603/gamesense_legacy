#include "../../source.hpp"
#include "../Hooked.hpp"
#include "../../SDK/Displacement.hpp"
#include "../../SDK/Classes/Player.hpp"

bool __fastcall Hooked::ShouldSkipAnimationFrame(C_BaseAnimating* ecx, uint32_t*)
{
	auto player = (C_BasePlayer*)(uintptr_t(ecx) - 4);
	auto local = C_CSPlayer::GetLocalPlayer();

	if (player && (player->EntIndex() == m_pEngine->GetLocalPlayer()
		|| !local))
		return false;

	return oShouldSkipAnimationFrame(ecx);
}