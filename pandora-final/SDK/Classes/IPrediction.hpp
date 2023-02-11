#pragma once

class IPrediction
{
public:
	void Update( int startframe, bool validframe, int incoming_acknowledged, int outgoing_command );
	void SetupMove( C_BasePlayer* player, CUserCmd* ucmd, IMoveHelper* pHelper, CMoveData* move );
	void FinishMove( C_BasePlayer* player, CUserCmd* ucmd, CMoveData* move );
	int CheckMovingGround( C_BasePlayer* player, double unk );
	void SetLocalViewAngles( const QAngle& ang );
	int& m_nCommandsPredicted( );
	bool InPrediction( );
	CGlobalVarsBase* GetUnpredictedGlobals( );

	void ForceRepredict( bool had_errors = false )
	{
		m_previous_startframe = -1;
		m_commands_predicted = 0;

		if( had_errors )
			m_previous_ack_had_errors = 1;
	}

	// s/o nixer1337 for full proper struct
	// - LED
	char pad00[ 8 ];                            // 0x0
	bool m_in_prediction;                     // 0x8
	bool m_old_cl_predict_value;              // 0x9
	bool m_engine_paused;                     // 0xA
	int m_previous_startframe;               // 0xC
	int m_incoming_packet_number;             // 0x10
	float m_last_server_world_time_stamp;     // 0x14
	bool m_is_first_time_predicted;           // 0x18
	int m_commands_predicted;                 // 0x1C
	int m_server_commands_acknowledged;       // 0x20
	bool m_previous_ack_had_errors;           // 0x24
};
