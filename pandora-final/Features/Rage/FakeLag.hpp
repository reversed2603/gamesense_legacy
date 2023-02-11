#pragma once
#include "../../SDK/sdk.hpp"

class FakeLag {
	bool ShouldFakeLag( CUserCmd* pCmd );
	int DetermineFakeLagAmount( CUserCmd* pCmd );

public:
	void HandleFakeLag( bool *bSendPacket, CUserCmd* pCmd );

	int m_iOverrideLagAmount;
	int m_iLastChokedCommands;
	int m_iLagLimit;
	int m_iAwaitingChoke;
};

extern FakeLag g_FakeLag;