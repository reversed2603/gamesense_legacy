#pragma once
class IGameMovement
{
public:
	void ProcessMovement( C_BasePlayer* pPlayer, CMoveData* pMove );
	void Reset( );
	void StartTrackPredictionErrors( C_BasePlayer* pPlayer );
	void FinishTrackPredictionErrors( C_BasePlayer* pPlayer );
	Vector GetPlayerMins( bool bDucked );
	Vector GetPlayerMaxs( bool bDucked );
	Vector GetPlayerViewOffset( bool bDucked );
};
