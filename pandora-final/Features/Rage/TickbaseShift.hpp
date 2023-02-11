#pragma once
#include "../../SDK/sdk.hpp"

class TickbaseSystem {
public:
	void Charge( );
	void UnCharge( bool forceProcess = false );

	bool CanCharge( );
	bool IncrementProcessTicks( );
	bool IsCharged( );

	void CL_Move( bool bFinalTick, float accumulated_extra_samples );

	void RunExploits( bool* bSendPacket, CUserCmd* pCmd );

	void DoRegularShifting( int iAmount );

	bool DoTheTapping( CUserCmd* pFrom );

	// charging
	bool m_bPreparedRecharge = false;
	bool m_bPrepareCharge = false;
	bool m_bCharge = false;
	bool m_bFixedCharge = false;
	bool m_bFixCharge = false;

	// is this the 2nd dt shot?
	bool m_bTapShot = false;

	// are we shifting rn?
	bool m_bInShift = false;

	// currently charged ticks
	int m_iProcessTicks;
	// ...
	int m_iMaxProcessTicks = 12;

	// used for WriteUsercmdDeltaToBuffer shifting
	int m_iShiftAmount;

	// tell cm to fix various things
	bool m_bCMFix;

	// runcommand shit
	bool m_bPreFix;
	bool m_bPostFix;

	int m_iFixAmount;
	int m_iFixCommand;

	float m_flLastExploitTime;
};

extern TickbaseSystem g_TickbaseController;