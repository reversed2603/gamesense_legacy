#pragma once
#include "../../SDK/sdk.hpp"

class IGrenadePrediction {
public:
	void Tick( int buttons );
	void View( );
	void Paint( );

	IGrenadePrediction( ) { };
	~IGrenadePrediction( ) {
	}

private:
	void Setup( C_BasePlayer* pl, Vector& vecSrc, Vector& vecThrow, const QAngle& angEyeAngles );
	void Simulate( QAngle& Angles, C_BasePlayer* pLocal );
	int Step( Vector& vecSrc, Vector& vecThrow, int tick, float interval );
	bool CheckDetonate( const Vector& vecThrow, const CGameTrace& tr, int tick, float interval );
	void TraceHull( Vector& src, Vector& end, CGameTrace& tr );
	void AddGravityMove( Vector& move, Vector& vel, float frametime, bool onground );
	void PushEntity( Vector& src, const Vector& move, CGameTrace& tr );
	void ResolveFlyCollisionCustom( CGameTrace& tr, Vector& vecVelocity, float interval );
	int PhysicsClipVelocity( const Vector& in, const Vector& normal, Vector& out, float overbounce );

	bool mouseDown = false;
	int classID = 0;
	float flThrowVelocity = 0.f;
	float flThrowStrength = 0.f;
	std::vector<Vector> vecPath;
	std::vector<std::pair<Vector, Color>> vecBounces;
	std::vector< C_BaseEntity* > vecIgnoredEntities;
};

extern IGrenadePrediction g_GrenadePrediction;