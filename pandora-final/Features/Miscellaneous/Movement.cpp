#include "Movement.hpp"
#include "../../SDK/variables.hpp"
#include "../../SDK/Classes/Player.hpp"
#include "../../SDK/Classes/weapon.hpp"

#include "../Rage/AntiAim.hpp"
#include "../Rage/FakeLag.hpp"
#include "../Rage/EnginePrediction.hpp"
#include "../Rage/ServerAnimations.hpp"
#include "../Rage/AlternativeAttack.hpp"

Movement g_Movement;

void Movement::FixMovement( CUserCmd* cmd, QAngle wishangle ) {
	auto pLocal = C_CSPlayer::GetLocalPlayer( );
	if( !pLocal ) {
		return;
	}

	//
	// sorry but aimware shit is inaccurate. so i used la supre meme
	// - LED

	Vector  move, dir;
	float   delta, len;
	QAngle   move_angle;

	// roll nospread fix.
	if( !( pLocal->m_fFlags( ) & FL_ONGROUND ) && cmd->viewangles.z != 0.f )
		cmd->sidemove = 0.f;

	// convert movement to vector.
	move = { cmd->forwardmove, cmd->sidemove, 0.f };

	// get move length and ensure we're using a unit vector ( vector with length of 1 ).
	len = move.Normalize( );
	if( !len )
		return;

	// convert move to an angle.
	Math::VectorAngles( move, move_angle );

	// calculate yaw delta.
	delta = ( cmd->viewangles.y - wishangle.y );

	// accumulate yaw delta.
	move_angle.y += delta;

	// calculate our new move direction.
	// dir = move_angle_forward * move_length
	Math::AngleVectors( move_angle, dir );

	// scale to og movement.
	dir *= len;

	// fix ladder and noclip.
	if( pLocal->m_MoveType( ) == MOVETYPE_LADDER ) {
		if( cmd->viewangles.x >= 45.f && wishangle.x < 45.f && std::abs( delta ) <= 65.f )
			dir.x = -dir.x;

		cmd->forwardmove = dir.x;
		cmd->sidemove = dir.y;
	}
	else {
		if( cmd->viewangles.x < -90.f || cmd->viewangles.x > 90.f )
			dir.x = -dir.x;

		cmd->forwardmove = dir.x;
		cmd->sidemove = dir.y;
	}

	// CorrectMoveButtons( cmd, pLocal->m_MoveType( ) == MOVETYPE_LADDER );
}

void Movement::CorrectMoveButtons( CUserCmd* cmd, bool ladder )
{
	// strip old flags.
	cmd->buttons &= ~( IN_FORWARD | IN_BACK | IN_MOVELEFT | IN_MOVERIGHT );

	// set new button flags.
	if( cmd->forwardmove > ( ladder ? 200.f : 0.f ) )
		cmd->buttons |= IN_FORWARD;

	else if( cmd->forwardmove < ( ladder ? -200.f : 0.f ) )
		cmd->buttons |= IN_BACK;

	if( cmd->sidemove > ( ladder ? 200.f : 0.f ) )
		cmd->buttons |= IN_MOVERIGHT;

	else if( cmd->sidemove < ( ladder ? -200.f : 0.f ) )
		cmd->buttons |= IN_MOVELEFT;
}

void Movement::InstantStop( CUserCmd* cmd ) {
	const auto pLocal = C_CSPlayer::GetLocalPlayer( );
	if( !pLocal )
		return;

	float maxSpeed = pLocal->GetMaxSpeed( );
	Vector velocity = pLocal->m_vecVelocity( );
	velocity.z = 0.0f;

	float speed = velocity.Length2D( );

	if( speed < 18.f )
	{
		cmd->forwardmove = 0;
		cmd->sidemove = 0;
		return;
	}

	QAngle angle;
	Math::VectorAngles( velocity * -1.f, angle );

	// fix direction by factoring in where we are looking.
	angle.y = g_Movement.m_vecOriginalCmdAngles.y - angle.y;

	Vector direction;
	Math::AngleVectors( angle, direction );

	auto stop = direction * speed;

	cmd->forwardmove = stop.x;
	cmd->sidemove = stop.y;

	/*static int playerSurfaceFrictionOffset = SDK::Memory::FindInDataMap( pLocal->GetPredDescMap( ), XorStr( "m_surfaceFriction" ) );
	float playerSurfaceFriction = *( float* )( uintptr_t( pLocal ) + playerSurfaceFrictionOffset );
	float max_accelspeed = g_Vars.sv_accelerate->GetFloat( ) * m_pGlobalVars->interval_per_tick * maxSpeed * playerSurfaceFriction;
	if( speed - max_accelspeed <= -1.f ) {
		cmd->forwardmove = speed / max_accelspeed;
	}
	else {
		cmd->forwardmove = g_Vars.cl_forwardspeed->GetFloat( );
	}

	cmd->sidemove = 0.0f;

	QAngle angMoveDir = m_vecMovementAngles;

	float flDirection = atan2( velocity.y, velocity.x );
	angMoveDir.yaw = std::remainderf( ToDegrees( flDirection ) + 180.0f, 360.0f );
	g_Movement.RotateMovement( cmd, angMoveDir, cmd->viewangles );*/
}

void Movement::PrePrediction( CUserCmd* cmd, bool* bSendPacket ) {
	const auto pLocal = C_CSPlayer::GetLocalPlayer( );
	if( !pLocal )
		return;

	if( pLocal->IsDead( ) )
		return;

	// bunny hop
	if( g_Vars.misc.bunnyhop ) {
		// holding space
		if( cmd->buttons & IN_JUMP ) {
			// valid movetype
			if( pLocal->m_MoveType( ) != MOVETYPE_LADDER &&
				pLocal->m_MoveType( ) != MOVETYPE_NOCLIP )
			{
				// not on ground
				if( !( pLocal->m_fFlags( ) & FL_ONGROUND ) ) {
					// remove jump only when in air
					// will get added back the tick we're on ground
					cmd->buttons &= ~IN_JUMP;
				}
			}
		}
	}

	// auto strafers
	if( g_Vars.misc.autostrafer ) {
		// make sure we're not on ground
		if( !( pLocal->m_fFlags( ) & FL_ONGROUND ) ) {
			// valid movetype
			if( pLocal->m_MoveType( ) == MOVETYPE_WALK ) {
				// do the "strafes"
				static float flSideSwap = 1.0f;
				flSideSwap = -flSideSwap;

				// save velocity
				Vector vecVelocity = pLocal->m_vecVelocity( );
				vecVelocity.z = 0.0f;

				// save speed, compute the ideal strafe
				const float flSpeed = vecVelocity.Length2D( );
				const float flIdealStrafe = Math::Clamp( ToDegrees( atan( 15.f / flSpeed ) ), 0.0f, 90.0f );

				// wasd strafer
				if( g_Vars.misc.autostrafer_wasd && ( cmd->forwardmove != 0.0f || cmd->sidemove != 0.0f ) ) {
					enum EDirections {
						FORWARDS = 0,
						BACKWARDS = 180,
						LEFT = 90,
						RIGHT = -90,
						BACK_LEFT = 135,
						BACK_RIGHT = -135
					};

					float flWishDirection{ };

					const bool bHoldingW = cmd->buttons & IN_FORWARD;
					const bool bHoldingA = cmd->buttons & IN_MOVELEFT;
					const bool bHoldingS = cmd->buttons & IN_BACK;
					const bool bHoldingD = cmd->buttons & IN_MOVERIGHT;

					if( bHoldingW ) {
						if( bHoldingA ) {
							flWishDirection += ( EDirections::LEFT / 2 );
						}
						else if( bHoldingD ) {
							flWishDirection += ( EDirections::RIGHT / 2 );
						}
						else {
							flWishDirection += EDirections::FORWARDS;
						}
					}
					else if( bHoldingS ) {
						if( bHoldingA ) {
							flWishDirection += EDirections::BACK_LEFT;
						}
						else if( bHoldingD ) {
							flWishDirection += EDirections::BACK_RIGHT;
						}
						else {
							flWishDirection += EDirections::BACKWARDS;
						}

						cmd->forwardmove = 0.f;
					}
					else if( bHoldingA ) {
						flWishDirection += EDirections::LEFT;
					}
					else if( bHoldingD ) {
						flWishDirection += EDirections::RIGHT;
					}

					m_vecMovementAngles.yaw += std::remainderf( flWishDirection, 360.f );
					cmd->sidemove = 0.f;
				}

				cmd->forwardmove = 0.f;

				// cba commenting after here
				static float flOldYaw = 0.f;
				const float flYawDelta = std::remainderf( m_vecMovementAngles.yaw - flOldYaw, 360.0f );
				const float flAbsYawDelta = abs( flYawDelta );
				flOldYaw = m_vecMovementAngles.yaw;

				if( flAbsYawDelta <= flIdealStrafe || flAbsYawDelta >= 30.f ) {
					auto angVelocityDirection = vecVelocity.ToEulerAngles( );
					auto flVelocityDelta = std::remainderf( m_vecMovementAngles.yaw - angVelocityDirection.yaw, 360.0f );

					if( flVelocityDelta <= flIdealStrafe || flSpeed <= 15.f ) {
						if( -( flIdealStrafe ) <= flVelocityDelta || flSpeed <= 15.f ) {
							m_vecMovementAngles.yaw += flSideSwap * flIdealStrafe;
							cmd->sidemove = g_Vars.cl_sidespeed->GetFloat( ) * flSideSwap;
						}
						else {
							m_vecMovementAngles.yaw = angVelocityDirection.yaw - flIdealStrafe;
							cmd->sidemove = g_Vars.cl_sidespeed->GetFloat( );
						}
					}
					else {
						m_vecMovementAngles.yaw = angVelocityDirection.yaw + flIdealStrafe;
						cmd->sidemove = -g_Vars.cl_sidespeed->GetFloat( );
					}
				}
				else if( flYawDelta > 0.0f ) {
					cmd->sidemove = -g_Vars.cl_sidespeed->GetFloat( );
				}
				else if( flYawDelta < 0.0f ) {
					cmd->sidemove = g_Vars.cl_sidespeed->GetFloat( );
				}

				//m_vecMovementAngles.Normalize( );
				FixMovement( cmd, m_vecMovementAngles );
			}
		}
	}

	if( g_Vars.misc.air_duck ) {
		if( !( pLocal->m_fFlags( ) & FL_ONGROUND ) ) {
			cmd->buttons |= IN_DUCK;
		}
	}

	if( ( g_Vars.misc.quickstop && !g_Vars.globals.m_bShotAutopeek && pLocal->m_fFlags( ) & FL_ONGROUND && !( cmd->buttons & IN_JUMP ) && pLocal->m_vecVelocity( ).Length( ) > 1.0f ) ) {
		if( cmd->forwardmove == cmd->sidemove && cmd->sidemove == 0.0f )
			InstantStop( cmd );
	}

	if (g_Vars.rage.anti_aim_break_walk
		&& pLocal->m_hActiveWeapon() != -1
		&& ((C_WeaponCSBaseGun*)pLocal->m_hActiveWeapon().Get())->IsGun() // mega shitcode
		&& ((C_WeaponCSBaseGun*)pLocal->m_hActiveWeapon().Get())->m_iItemDefinitionIndex() != WEAPON_ZEUS // mega shitcode
		&& m_pClientState->m_nChokedCommands() == 0
		&& pLocal->m_fFlags() & FL_ONGROUND
		&& !(cmd->buttons & IN_JUMP)
		&& pLocal->m_vecVelocity().Length() >= 1.f
		&& !(cmd->buttons & IN_ATTACK || cmd->buttons & IN_ATTACK2)
		&& !g_Vars.rage.double_tap_bind.enabled
		&& g_Vars.rage.fake_lag
		&& g_Vars.rage.fake_lag_amount > 1)
	{
		InstantStop(cmd);
		//cmd->buttons |= IN_DUCK;
	}

	g_Movement.FakeWalk( bSendPacket, cmd );
	g_AntiAim.Prepare( bSendPacket, cmd );
	g_AlternativeAttack.RunAttacks( bSendPacket, cmd );
}

void Movement::InPrediction( CUserCmd* cmd ) {
	AutoPeek( cmd );
}

void Movement::PostPrediction( CUserCmd* cmd ) {

	// fix movement after all movement code has ran
	g_Movement.FixMovement( cmd, m_vecOriginalCmdAngles );
}

void Movement::AutoPeek( CUserCmd* cmd ) {
	const auto pLocal = C_CSPlayer::GetLocalPlayer( );
	if( !pLocal )
		return;

	if( pLocal->IsDead( ) )
		return;

	if( g_Vars.misc.autopeek && g_Vars.misc.autopeek_bind.enabled ) {
		if( ( pLocal->m_fFlags( ) & FL_ONGROUND ) ) {
			if( m_vecAutoPeekPos.IsZero( ) ) {
				m_vecAutoPeekPos = pLocal->GetAbsOrigin( );
			}
		}
	}
	else {
		m_vecAutoPeekPos = Vector( );
	}

	bool bPeek = true;
	if( g_Vars.misc.autopeek && g_Vars.globals.m_bShotAutopeek && !m_vecAutoPeekPos.IsZero( ) ) {
		cmd->buttons &= ~IN_JUMP;

		bPeek = false;

		auto delta = m_vecAutoPeekPos - pLocal->GetAbsOrigin( );
		/*m_vecMovementAngles = delta.ToEulerAngles( );

		cmd->forwardmove = g_Vars.cl_forwardspeed->GetFloat( );
		cmd->sidemove = 0.0f;*/
		auto angle = Math::CalcAngle( pLocal->GetAbsOrigin( ), m_vecAutoPeekPos );

		// fix direction by factoring in where we are looking.
		angle.y = m_vecMovementAngles.y - angle.y;

		Vector direction;
		Math::AngleVectors( angle, direction );

		auto stop = direction * -450.f;

		cmd->forwardmove = stop.x;
		cmd->sidemove = stop.y;

		if( delta.Length2D( ) <= std::fmaxf( 11.f, pLocal->m_vecVelocity( ).Length2D( ) * m_pGlobalVars->interval_per_tick ) ) {
			bPeek = true;
			g_Vars.globals.m_bShotAutopeek = false;
		}
	}
	else {
		g_Vars.globals.m_bShotAutopeek = false;
	}
}

void Movement::FakeWalk( bool* bSendPacket, CUserCmd* cmd ) {
	g_Vars.globals.m_bFakeWalking = false;

	const auto pLocal = C_CSPlayer::GetLocalPlayer( );
	if( !pLocal )
		return;

	if( pLocal->IsDead( ) )
		return;

	if( !g_Vars.misc.fakewalk )
		return;

	static bool was_moving_until_fakewalk = false;

	if( !g_Vars.misc.fakewalk_bind.enabled ) {
		was_moving_until_fakewalk = pLocal->m_vecVelocity( ).Length2D( ) >= 0.1f;
		return;
	}

	if( !( pLocal->m_fFlags( ) & FL_ONGROUND ) )
		return;

	if( was_moving_until_fakewalk ) {
		if( pLocal->m_vecVelocity( ).Length2D( ) >= 0.1f ) {
			InstantStop( cmd );
		}
		else
			was_moving_until_fakewalk = false;

		return;
	}

	cmd->buttons &= ~IN_SPEED;

	const float flServerTime = TICKS_TO_TIME( pLocal->m_nTickBase( ) );
	int nTicksToUpdate = TIME_TO_TICKS( g_ServerAnimations.m_uServerAnimations.m_flLowerBodyRealignTimer - flServerTime ) - 1;

	const Vector vecMove = { cmd->forwardmove, cmd->sidemove, cmd->upmove };

	int nTicksToStop;
	for( nTicksToStop = 0; nTicksToStop < 15; ++nTicksToStop ) {
		if( pLocal->m_vecVelocity( ).Length2D( ) < 0.1f )
			break;

		InstantStop( cmd );

		g_Prediction.Begin( cmd );
	}

	cmd->forwardmove = vecMove.x;
	cmd->sidemove = vecMove.y;
	cmd->upmove = vecMove.z;

	if( nTicksToStop > 0 )
		g_Prediction.GetData( )->m_RestoreData.Apply( pLocal );

	// XDDD!!
	//const bool is_mrx = strstr( m_pEngine->GetNetChannelInfo( )->GetAddress( ), XorStr( "178.32.80.148" ) );

	const int nFakewalkSpeed = int( g_Vars.misc.slow_motion_speed * 0.14f );
	int nMaxTicks = nFakewalkSpeed;
	const int nTicksLeft = nMaxTicks - m_pClientState->m_nChokedCommands( );

	if( m_pClientState->m_nChokedCommands( ) < nMaxTicks || nTicksToStop )
		*bSendPacket = 0;

	if( nTicksToStop > nTicksLeft || !m_pClientState->m_nChokedCommands( ) || *bSendPacket ) {
		InstantStop( cmd );
	}

	// frog champ
	g_Vars.globals.m_bFakeWalking = true;
}

void Movement::MovementControl( CUserCmd* cmd, float velocity ) {
	const auto pLocal = C_CSPlayer::GetLocalPlayer( );
	if( !pLocal )
		return;

	/*if( velocity <= 0.52f )
		cmd->buttons |= IN_SPEED;
	else*/
		cmd->buttons &= ~IN_SPEED;

	float movement_speed = std::sqrtf( cmd->forwardmove * cmd->forwardmove + cmd->sidemove * cmd->sidemove );
	if( movement_speed > 28.f ) {
		if( movement_speed > velocity ) {
			float mov_speed = pLocal->m_vecVelocity( ).Length2D( );
			/*if( ( velocity + 1.0f ) <= mov_speed ) {
				cmd->forwardmove = 0.0f;
				cmd->sidemove = 0.0f;
			}
			else {*/
				float forward_ratio = cmd->forwardmove / movement_speed;
				float side_ratio = cmd->sidemove / movement_speed;

				cmd->forwardmove = forward_ratio * std::min( movement_speed, velocity );
				cmd->sidemove = side_ratio * std::min( movement_speed, velocity );

				if (pLocal->m_fFlags() & 6) {
					cmd->forwardmove = cmd->forwardmove / (((pLocal->m_flDuckAmount() * 0.34f) + 1.0f) - pLocal->m_flDuckAmount());
					cmd->sidemove = cmd->sidemove / (((pLocal->m_flDuckAmount() * 0.34f) + 1.0f) - pLocal->m_flDuckAmount());
				}
			//}
		}
	}
}