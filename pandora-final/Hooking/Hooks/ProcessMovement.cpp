#include "../Hooked.hpp"
#include "../../Features/Rage/EnginePrediction.hpp"
#include "../../SDK/Classes/weapon.hpp"
#include "../../SDK/Valve/CBaseHandle.hpp"
#include "../../SDK/Classes/Player.hpp"
#include "../../SDK/Displacement.hpp"
#include <deque>
#include "../../Features/Rage/TickbaseShift.hpp"

void __fastcall Hooked::ProcessMovement(void* ecx, void* edx, C_CSPlayer* basePlayer, CMoveData* m)
{
	auto moveData = Encrypted_t(m);

	if (moveData.IsValid())
		moveData->m_bGameCodeMovedPlayer = false;

	oProcessMovement(ecx, basePlayer, moveData.Xor());
}