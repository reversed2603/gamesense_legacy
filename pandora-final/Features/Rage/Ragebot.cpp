#include "../../source.hpp"

#include "Ragebot.hpp"
#include "../Rage/EnginePrediction.hpp"
#include "../../SDK/Classes/Player.hpp"
#include "../../SDK/Classes/weapon.hpp"
#include "../../SDK/Valve/CBaseHandle.hpp"
#include "../../Utils/InputSys.hpp"
#include "../../Renderer/Render.hpp"
#include "../../Utils/Threading/threading.h"
#include <algorithm>
#include <atomic>
#include <thread>
#include "../../SDK/Displacement.hpp"
#include "Resolver.hpp"
#include "../Visuals/EventLogger.hpp"
#include "Resolver.hpp"
#include "ShotHandling.hpp"
#include "TickbaseShift.hpp"
#include "../visuals/visuals.hpp"
#include "../Visuals/Chams.hpp"

#include "../Scripting/Scripting.hpp"
#include "../Miscellaneous/Movement.hpp"

#include <sstream>

#include "../../Utils/Threading/shared_mutex.h"

Ragebot g_Ragebot;

#define MT_AWALL

static SharedMutex smtx;

CVariables::RAGE* Ragebot::GetRageSettings( ) {
	if( !m_AimbotInfo.m_pWeapon || !m_AimbotInfo.m_pWeaponInfo )
		return nullptr;

	CVariables::RAGE* current = nullptr;

	auto id = m_AimbotInfo.m_pWeapon->m_iItemDefinitionIndex( );

	// for now, I might do make aimbot also run on zeus
	if( id == WEAPON_ZEUS )
		return false;

	current = &g_Vars.rage_default;

	/*switch( m_AimbotInfo.m_pWeaponInfo->m_iWeaponType ) {
	case WEAPONTYPE_PISTOL:
		if( id == WEAPON_DEAGLE || id == WEAPON_REVOLVER )
			current = &g_Vars.rage_heavypistols;
		else
			current = &g_Vars.rage_pistols;
		break;
	case WEAPONTYPE_SUBMACHINEGUN:
		current = &g_Vars.rage_smgs;
		break;
	case WEAPONTYPE_RIFLE:
		current = &g_Vars.rage_rifles;
		break;
	case WEAPONTYPE_SHOTGUN:
		current = &g_Vars.rage_shotguns;
		break;
	case WEAPONTYPE_SNIPER_RIFLE:
		if( id == WEAPON_G3SG1 || id == WEAPON_SCAR20 )
			current = &g_Vars.rage_autosnipers;
		else
			current = ( id == WEAPON_AWP ) ? &g_Vars.rage_awp : &g_Vars.rage_scout;
		break;
	case WEAPONTYPE_MACHINEGUN:
		current = &g_Vars.rage_heavys;
		break;
	default:
		current = &g_Vars.rage_default;
		break;
	}*/

	if( !current )
		return nullptr;

	if( !current->override_default_config ) {
		current = &g_Vars.rage_default;
	}

	return current;
}

float Ragebot::CalculateDamageOnPoint( C_LagRecord* pRecord, AimTarget_t& pTarget, Vector vecPoint, bool bCalculatePoint, Hitboxes hitbox ) {
	if( !pTarget.m_pEntity || !pTarget.m_pHitboxSet )
		return -1.f;

	if( !pRecord || pRecord->m_RecordFlags & ELagRecordFlags::RF_IsInvalid )
		return -1.f;

	auto pMatrix = pRecord->m_pMatrix;
	if( !pMatrix )
		return -1.f;

	if( pMatrix ) {
		const auto pHitbox = pTarget.m_pHitboxSet->pHitbox( hitbox );
		if( pHitbox ) {
			Vector vecFinalPoint = bCalculatePoint ? pMatrix[ pHitbox->bone ].at( hitbox ) : vecPoint;
			Vector vecDirection = vecFinalPoint - m_AimbotInfo.m_vecEyePosition;
			vecDirection.Normalize( );

			AimPoint_t point{ };
			point.m_pFireBulletData = new Autowall::C_FireBulletData( );
			point.m_pTarget = &pTarget;
			if( point.m_pFireBulletData != nullptr ) {
				point.m_pHitbox = pHitbox;
				point.m_vecPosition = vecFinalPoint;

				point.m_pFireBulletData->m_vecStart = m_AimbotInfo.m_vecEyePosition;
				point.m_pFireBulletData->m_vecPos = point.m_vecPosition;
				point.m_pFireBulletData->m_vecDirection = vecDirection;
				point.m_pFireBulletData->m_iHitgroup = point.m_pHitbox->group;

				point.m_pFireBulletData->m_Player = m_AimbotInfo.m_pLocal;
				point.m_pFireBulletData->m_TargetPlayer = pTarget.m_pEntity;
				point.m_pFireBulletData->m_Weapon = m_AimbotInfo.m_pWeapon;
				point.m_pFireBulletData->m_WeaponData = m_AimbotInfo.m_pWeaponInfo;

				point.m_pFireBulletData->m_bPenetration = m_AimbotInfo.m_pSettings->wall_penetration;

				pRecord->Apply( pRecord->player );

				RunAwall( &point );

				if( point.m_pFireBulletData != nullptr ) {
					float flDamage = point.m_pFireBulletData->m_flCurrentDamage;

					// let's better delete the allocated memory (LMAO)
					delete point.m_pFireBulletData;

					// let's set it to null..
					point.m_pFireBulletData = nullptr;

					return flDamage;
				}

			}
		}
	}

	return -1.f;
}

bool Ragebot::AutoStop( Encrypted_t<CUserCmd> cmd ) {

	const auto pLocal = C_CSPlayer::GetLocalPlayer( );
	m_AimbotInfo.m_bDidStop = false;

	if( !pLocal )
		return true;

	if( pLocal->IsDead( ) )
		return true;

	auto pWeapon = reinterpret_cast< C_WeaponCSBaseGun* >( pLocal->m_hActiveWeapon( ).Get( ) );
	if( !pWeapon )
		return true;

	if( !m_AimbotInfo.m_pSettings )
		return true;

	if( !m_AimbotInfo.m_pSettings->quick_stop )
		return true;

	if( !m_AimbotInfo.m_bShouldStop )
		return true;

	if( /*!m_AimbotInfo.m_pSettings->auto_stop_air &&*/ !( pLocal->m_fFlags( ) & FL_ONGROUND ) )
		return true;

	if( g_Vars.globals.m_bInsideFireRange )
		return true;

	// don't retry if we already tried to stop this tick..
	//if( m_AimbotInfo.m_bDidStop )
	//	return true;

	if( !cmd.IsValid( ) || !pWeapon )
		return true;

	Vector vecVelocity = pLocal->m_vecVelocity( );
	vecVelocity.z = 0.0f;

	float vecVelocityLength = vecVelocity.Length2D( );

	if( g_Vars.rage_default.quick_stop == 2 ) {
		cmd->buttons |= IN_DUCK;
	}

	float maxSpeed = ( pWeapon->GetMaxSpeed( ) * 0.33f ) - 1.f;

	if( g_Vars.rage.anti_aim_break_walk && m_pClientState->m_nChokedCommands( ) == 0 && !g_Vars.globals.m_bFakeWalking && !g_Vars.rage.double_tap_bind.enabled && g_Vars.rage.fake_lag )
		maxSpeed *= 0.1f;

	if( vecVelocityLength <= maxSpeed ) {
		g_Movement.MovementControl( m_AimbotInfo.m_pCmd, maxSpeed );
	}
	else {
		if( vecVelocityLength > 15.f ) {
			QAngle angle;
			Math::VectorAngles( vecVelocity * -1.f, angle );

			// fix direction by factoring in where we are looking.
			angle.y = g_Movement.m_vecOriginalCmdAngles.y - angle.y;

			Vector direction;
			Math::AngleVectors( angle, direction );

			auto stop = direction * vecVelocityLength;

			cmd->forwardmove = stop.x;
			cmd->sidemove = stop.y;
		}
		else {
			cmd->forwardmove = 0.f;
			cmd->sidemove = 0.f;
		}
	}

	m_AimbotInfo.m_bShouldStop = false;

	m_AimbotInfo.m_bDidStop = true;
	return false;
}

bool Ragebot::AutoScope( ) {
	if( !m_AimbotInfo.m_pSettings->auto_scope )
		return false;

	if( !m_AimbotInfo.m_pCmd )
		return false;

	if( m_AimbotInfo.m_pWeaponInfo->m_iWeaponType != WEAPONTYPE_SNIPER_RIFLE )
		return false;

	if( m_AimbotInfo.m_pWeapon->m_zoomLevel( ) >= 1 )
		return false;

	// remove attack (so we can properly scope)
	m_AimbotInfo.m_pCmd->buttons &= ~IN_ATTACK;

	// set in_attack2 (scope nibba)
	m_AimbotInfo.m_pCmd->buttons |= IN_ATTACK2;

	return true;
}

int Ragebot::GetMinimalDamage( AimPoint_t* pPoint ) {
	if( !pPoint )
		return 420;

	if( !pPoint->m_pTarget )
		return 1337;

	if( !pPoint->m_pTarget->m_pEntity || !pPoint->m_pTarget->m_pRecord )
		return 420;

	if( !pPoint->m_pTarget->m_pRecord->player )
		return 1337420;

	const int nPlayerHP = pPoint->m_pTarget->m_pEntity->m_iHealth( );
	auto GetHPDamage = [ & ] ( int nDamage ) {
		return nDamage > 100 ? ( nPlayerHP + ( nDamage - 100 ) ) : nDamage;
	};

	bool bAutomatic = m_AimbotInfo.m_pSettings->minimal_damage == 0;
	int nDamage = std::min( nPlayerHP, GetHPDamage( m_AimbotInfo.m_pSettings->minimal_damage ) );

	if( bAutomatic ) {
		nDamage = pPoint->m_bPenetrated ? 15 : 35;

		if( m_AimbotInfo.m_pWeapon->m_iItemDefinitionIndex( ) == WEAPON_AWP ||
			m_AimbotInfo.m_pWeapon->m_iItemDefinitionIndex( ) == WEAPON_SSG08 )
		{
			if( pPoint->m_bPenetrated ) {
				nDamage = nPlayerHP / 2;
			}
			else {
				nDamage = nPlayerHP + 1;
			}
		}
	}

	return nDamage;
}

C_LagRecord* Ragebot::FindPriorityBacktrackRecord( AimTarget_t* pTarget, int accuracy_boost )
{
	auto pLagData = g_LagCompensation.GetLagData( pTarget->m_pEntity->m_entIndex );
	if( !pLagData || pLagData->m_Records.empty( ) )
		return { };

	C_LagRecord* pBestRecord = nullptr;

	for( auto it = pLagData->m_Records.begin( ); it != pLagData->m_Records.end( ); it = next( it ) )
	{
		if( ( it->m_RecordFlags & ELagRecordFlags::RF_IsResolved ) == 0 )
			continue;

		// if it's a valid lby flick record, shoot it !!!
		if( g_LagCompensation.IsRecordOutOfBounds( *it ) )
			continue;

		pBestRecord = &( *it );
	}

	return pBestRecord;
}

C_LagRecord* Ragebot::GetBestLagRecord( AimTarget_t* pTarget ) {
	auto pLagData = g_LagCompensation.GetLagData( pTarget->m_pEntity->m_entIndex );
	if( !pLagData || pLagData->m_Records.empty( ) )
		return { };

	C_LagRecord* pBestRecord = nullptr;

	auto GetRecord = [ & ] ( bool bLast = false ) -> C_LagRecord* {
		if( bLast ) {
			for( auto it = pLagData->m_Records.rbegin( ); it != pLagData->m_Records.rend( ); it = next( it ) ) {
				if( !g_LagCompensation.IsRecordOutOfBounds( *it ) ) {
					return &( *it );
				}
			}
		}
		else {
			for( auto it = pLagData->m_Records.begin( ); it != pLagData->m_Records.end( ); it = next( it ) ) {
				if( !g_LagCompensation.IsRecordOutOfBounds( *it ) ) {
					return &( *it );
				}
			}
		}

		return { };
	};

	// lets hope two of those functions wont eat extra fps...
	// -LED
	C_LagRecord* pLatestRecord = GetRecord( false );
	C_LagRecord* pOldestRecord = GetRecord( true );

	// no accuracy boost, return latest record only (aka no backtrack)
	if( g_Vars.rage.accuracy_boost == 0 )
		return pLatestRecord;

	auto prior = FindPriorityBacktrackRecord( pTarget, g_Vars.rage.accuracy_boost );

	if( prior && prior->player == pTarget->m_pEntity )
		return prior;

	pBestRecord = pLatestRecord;

	// low accuracy boost, return latest and priority
	if( g_Vars.rage.accuracy_boost == 1 )
		return pLatestRecord;

	int best_damage = 0;

	// loop through all the records
	for( int i = 0; i < pLagData->m_Records.size( ); i++ )
	{
		auto it = &pLagData->m_Records[ i ];

		if( g_Vars.rage.accuracy_boost == 2 )
		{
			if( i > int( pLagData->m_Records.size( ) / 2 ) )
				break;
		}

		// if it's a valid resolved record, shoot it !!!
		if( g_LagCompensation.IsRecordOutOfBounds( *it ) )
			continue;

		const int& damage = CalculateDamageOnPoint( it, *pTarget, Vector( ), true );

		if( damage < 1.f )
			continue;

		if( damage <= best_damage && ( it->m_RecordFlags & ELagRecordFlags::RF_IsResolved ) == 0 || ( std::fmaxf( 0, ( damage - 10 ) ) <= best_damage && ( it->m_RecordFlags & ELagRecordFlags::RF_IsResolved ) != 0 ) )
			continue;

		pBestRecord = &( *it );
		best_damage = damage;
	}

	return pBestRecord;
}

std::vector<Hitboxes> Ragebot::GetHitboxes( ) {
	std::vector<Hitboxes> vecHitboxes{ };

	if( g_Vars.rage.force_baim.enabled ) {
		if( m_AimbotInfo.m_pSettings->hitbox_chest ) {
			vecHitboxes.push_back( Hitboxes::HITBOX_UPPER_CHEST );
			vecHitboxes.push_back( Hitboxes::HITBOX_CHEST );
			vecHitboxes.push_back( Hitboxes::HITBOX_LOWER_CHEST );
		}

		// maybe stomach and pelvis one option?
		if( m_AimbotInfo.m_pSettings->hitbox_stomach ) {
			vecHitboxes.push_back( Hitboxes::HITBOX_STOMACH );
		}
		if( m_AimbotInfo.m_pSettings->hitbox_pelvis ) {
			vecHitboxes.push_back( Hitboxes::HITBOX_PELVIS );
		}
	}
	else {

		// maybe stomach and pelvis one option?
		if( m_AimbotInfo.m_pSettings->hitbox_stomach ) {
			vecHitboxes.push_back( Hitboxes::HITBOX_STOMACH );
		}

		if( m_AimbotInfo.m_pSettings->hitbox_chest ) {
			vecHitboxes.push_back( Hitboxes::HITBOX_UPPER_CHEST );
			vecHitboxes.push_back( Hitboxes::HITBOX_CHEST );
			vecHitboxes.push_back( Hitboxes::HITBOX_LOWER_CHEST );
		}

		if( m_AimbotInfo.m_pSettings->hitbox_head ) {
			vecHitboxes.push_back( Hitboxes::HITBOX_HEAD );
		}

		if( m_AimbotInfo.m_pSettings->hitbox_neck ) {
			vecHitboxes.push_back( Hitboxes::HITBOX_NECK );
			//vecHitboxes.push_back( Hitboxes::HITBOX_LOWER_NECK );
		}

		if( m_AimbotInfo.m_pSettings->hitbox_pelvis ) {
			vecHitboxes.push_back( Hitboxes::HITBOX_PELVIS );
		}

		if( m_AimbotInfo.m_pSettings->hitbox_arms ) {
			vecHitboxes.push_back( Hitboxes::HITBOX_LEFT_HAND );
			vecHitboxes.push_back( Hitboxes::HITBOX_RIGHT_HAND );

			vecHitboxes.push_back( Hitboxes::HITBOX_LEFT_UPPER_ARM );
			vecHitboxes.push_back( Hitboxes::HITBOX_RIGHT_UPPER_ARM );

			vecHitboxes.push_back( Hitboxes::HITBOX_LEFT_FOREARM );
			vecHitboxes.push_back( Hitboxes::HITBOX_RIGHT_FOREARM );
		}

		if( m_AimbotInfo.m_pSettings->hitbox_legs ) {
			vecHitboxes.push_back( Hitboxes::HITBOX_LEFT_CALF );
			vecHitboxes.push_back( Hitboxes::HITBOX_RIGHT_CALF );

			vecHitboxes.push_back( Hitboxes::HITBOX_LEFT_THIGH );
			vecHitboxes.push_back( Hitboxes::HITBOX_RIGHT_THIGH );
		}

		if( m_AimbotInfo.m_pSettings->hitbox_feet ) {
			vecHitboxes.push_back( Hitboxes::HITBOX_LEFT_FOOT );
			vecHitboxes.push_back( Hitboxes::HITBOX_RIGHT_FOOT );
		}
	}

	return vecHitboxes;
}

void Ragebot::RunAwall( AimPoint_t* pPoint ) {
	if( !m_pEngine.IsValid( ) )
		return;

	if( !m_pEngine->IsInGame( ) || !m_pEngine->IsConnected( ) )
		return;

	const auto pLocal = C_CSPlayer::GetLocalPlayer( );
	if( !pLocal )
		return;

	if( pLocal->IsDead( ) )
		return;

	if( !pPoint )
		return;

	if( pPoint->m_pFireBulletData == nullptr )
		return;

	if( !pPoint->m_pTarget )
		return;

	if( !pPoint->m_pTarget->m_pEntity )
		return;

	if( !pPoint->m_pTarget->m_pRecord
		|| !pPoint->m_pTarget->m_pRecord->player )
		return;

	if( !pPoint->m_pTarget->m_pEntity->m_CachedBoneData( ).m_Memory.m_pMemory )
		return;

	smtx.rlock( );
	smtx.runlock( );

	// runawall
	Autowall::FireBullets( pPoint->m_pFireBulletData );
}

bool Ragebot::RunHitscan( std::vector<Hitboxes> hitboxesToScan ) {
	auto vecHitboxes = hitboxesToScan.empty( ) ? GetHitboxes( ) : hitboxesToScan;
	if( vecHitboxes.empty( ) )
		return false;

	auto vecPlayers = FindTargets( );
	if( vecPlayers.empty( ) )
		return false;

	static std::vector<AimTarget_t> vecTargets{ };

	if( !vecTargets.empty( ) )
		vecTargets.clear( );

	for( auto player : vecPlayers ) {
		// somehow got null?
		if( !player ) {
			g_Visuals.AddDebugMessage( std::string( XorStr( __FUNCTION__ ) ).append( XorStr( " -> L" ) + std::to_string( __LINE__ ) ) );
			continue;
		}

		if( !player->IsPlayer( ) ) {
			g_Visuals.AddDebugMessage( std::string( XorStr( __FUNCTION__ ) ).append( XorStr( " -> L" ) + std::to_string( __LINE__ ) ) );
			continue;
		}

		// setup a new target
		AimTarget_t target{ };
		target.m_pEntity = player;

		// setup hitbox set
		if( auto pModel = player->GetModel( ); pModel ) {
			const auto pHdr = m_pModelInfo->GetStudiomodel( pModel );
			if( pHdr ) {
				target.m_pHitboxSet = pHdr->pHitboxSet( player->m_nHitboxSet( ) );
			}
		}

		// fack.
		if( !target.m_pHitboxSet ) {
			g_Visuals.AddDebugMessage( std::string( XorStr( __FUNCTION__ ) ).append( XorStr( " -> L" ) + std::to_string( __LINE__ ) ) );
			continue;
		}

		// get the best lagrecord
		target.m_pRecord = GetBestLagRecord( &target );

		// we couldn't find a proper record?
		// let's not even bother
		if( !target.m_pRecord || !target.m_pRecord->player || target.m_pRecord->player == nullptr ) {
			g_Visuals.AddDebugMessage( std::string( XorStr( __FUNCTION__ ) ).append( XorStr( " -> L" ) + std::to_string( __LINE__ ) ) );
			continue;
		}

		for( auto hitbox : vecHitboxes ) {
			AddPoints( &target, hitbox );
		}

		vecTargets.push_back( target );
	}

	// run awall on points
	for( auto& target : vecTargets ) {
		for( auto& point : target.m_vecAimPoints ) {
			// allocate new firebulletdata so we can use it correctly
			// in the threading
			point.m_pFireBulletData = new Autowall::C_FireBulletData( );

			// setup firebulletdata
			if( point.m_pFireBulletData != nullptr ) {
				point.m_pFireBulletData->m_vecStart = m_AimbotInfo.m_vecEyePosition;
				point.m_pFireBulletData->m_vecPos = point.m_vecPosition;

				Vector vecDirection = ( point.m_pFireBulletData->m_vecPos - point.m_pFireBulletData->m_vecStart );
				vecDirection.Normalize( );

				point.m_pFireBulletData->m_vecDirection = vecDirection;
				point.m_pFireBulletData->m_iHitgroup = point.m_pHitbox->group;

				point.m_pFireBulletData->m_Player = m_AimbotInfo.m_pLocal;
				point.m_pFireBulletData->m_TargetPlayer = target.m_pEntity;
				point.m_pFireBulletData->m_Weapon = m_AimbotInfo.m_pWeapon;
				point.m_pFireBulletData->m_WeaponData = m_AimbotInfo.m_pWeaponInfo;

				point.m_pFireBulletData->m_bPenetration = m_AimbotInfo.m_pSettings->wall_penetration;

				target.m_pRecord->Apply( target.m_pEntity );

				// queue awall
#ifdef MT_AWALL
				Threading::QueueJobRef( RunAwall, &point );
#else
				RunAwall( &point );
#endif
			}
		}
	}

#ifdef MT_AWALL
	// finish queue (let threads run their jobs)
	Threading::FinishQueue( );
#endif

	// now filter points
	for( auto& target : vecTargets ) {

		// static so it only initializes once /shrug
		std::vector<AimPoint_t> m_vecFinalPoints{ };

		for( auto& point : target.m_vecAimPoints ) {
			if( point.m_pFireBulletData == nullptr )
				continue;

			// suddenly we did not hit head
			//if( point.m_pHitbox->group == Hitgroup_Head && point.m_pFireBulletData->m_iHitgroup != Hitgroup_Head )
			//	continue;

			if( point.m_pHitbox->group != point.m_pFireBulletData->m_iHitgroup )
				continue;

			// more stuff to be filled/added here
			// such as haslethalpoint,...
			if( point.m_pFireBulletData->m_flCurrentDamage >= GetMinimalDamage( &point ) ) {
				// store and convert damage from float to int..
				point.m_iDamage = static_cast< int >( point.m_pFireBulletData->m_flCurrentDamage );
				point.m_bPenetrated = point.m_pFireBulletData->m_iPenetrationCount < 4;
				point.m_iTraceHitgroup = point.m_pFireBulletData->m_iHitgroup;
				point.m_bBody = ( point.m_pFireBulletData->m_iHitgroup == Hitgroup_Stomach || point.m_pFireBulletData->m_iHitgroup == Hitgroup_Chest ) || ( point.m_iHitbox != 0 && point.m_iHitbox != 1 ); // means hitscan didnt find points in stomach
				point.m_bHead = point.m_pFireBulletData->m_iHitgroup == Hitgroup_Head;
				point.m_bLethal = point.m_iDamage >= point.m_pTarget->m_pEntity->m_iHealth( ) && point.m_bBody;
				point.m_bResolved = point.m_pTarget->m_pRecord->m_RecordFlags & ELagRecordFlags::RF_IsResolved;

				// just to make sure :)
				point.m_pTarget = &target;

				// let's better delete the allocated memory (LMAO)
				delete point.m_pFireBulletData;

				// let's set it to null..
				point.m_pFireBulletData = nullptr;

				m_vecFinalPoints.push_back( point );
			}
		}

		// override the points
		if( !m_vecFinalPoints.empty( ) ) {
			target.m_vecAimPoints = m_vecFinalPoints;

			// emplace target with valid points
			m_AimbotInfo.m_vecAimData.emplace_back( target );
		}

	}

	vecPlayers.clear( );
	vecHitboxes.clear( );

	// no valid aim data? fuck dude
	if( m_AimbotInfo.m_vecAimData.empty( ) ) {
		g_Visuals.AddDebugMessage( std::string( XorStr( __FUNCTION__ ) ).append( XorStr( " -> L" ) + std::to_string( __LINE__ ) ) );
		return false;
	}

	return true;
}

bool Ragebot::FinishAimbot( ) {
	// find best target
	AimTarget_t bestTarget;

	// if there is only one entry anyways, no need to iterate
	if( m_AimbotInfo.m_vecAimData.size( ) == 1 ) {
		bestTarget = m_AimbotInfo.m_vecAimData.at( 0 );
	}
	else {
		for( auto& data : m_AimbotInfo.m_vecAimData ) {
			// no target yet? we just take this one first.
			if( !bestTarget.m_pEntity ) {
				g_Visuals.AddDebugMessage( std::string( XorStr( __FUNCTION__ ) ).append( XorStr( " -> L" ) + std::to_string( __LINE__ ) ) );

				bestTarget = data;
				continue;
			}

			// is the current guys health lower than best? we try to kill it.
			if( data.m_pEntity->m_iHealth( ) < bestTarget.m_pEntity->m_iHealth( ) ) {
				bestTarget = data;
			}
			else {
				// get damage of all points
				int currentDamage{ }, bestDamage{ };

				for( auto& point : data.m_vecAimPoints ) {
					currentDamage += point.m_iDamage;
				}
				for( auto& point : bestTarget.m_vecAimPoints ) {
					bestDamage += point.m_iDamage;
				}

				// current damage higher?
				if( currentDamage > bestDamage ) {
					bestTarget = data;
				}
			}

		}
	}

	// apparently we were not able to find a good target..
	if( !bestTarget.m_pEntity ) {
		g_Visuals.AddDebugMessage( std::string( XorStr( __FUNCTION__ ) ).append( XorStr( " -> L" ) + std::to_string( __LINE__ ) ) );
		return false;
	}

	AimPoint_t bestPoint{ };
	bestPoint.m_bFirstPoint = true;

	float flMaxBodyDamage = Autowall::ScaleDamage( bestTarget.m_pEntity, m_AimbotInfo.m_pWeaponInfo->m_iWeaponDamage, m_AimbotInfo.m_pWeaponInfo->m_flArmorRatio, Hitgroup_Stomach );

	// main aimbot logic (prefer body, safepoint, etc.)
	for( auto& point : bestTarget.m_vecAimPoints ) {
		// if it's the first point we take it
		if( bestPoint.m_bFirstPoint ) {
			bestPoint = point;
			continue;
		}

		if( bestTarget.m_pEntity != nullptr ) {
			if( int( flMaxBodyDamage ) >= bestTarget.m_pEntity->m_iHealth( ) ) {
				if( !point.m_bBody ) {
					continue;
				}

				// if body and we got enough damage to one hit
				// we take for now
				bestPoint = point;

				// if this is lethal, we want it..
				if( bestPoint.m_bLethal )
					break;
			}
		}

		// do basic forced conditions first such as prefer/force baim if lethal,
		// and prefer/force headaim if resolved

		// lethal and center hitbox?
		if( point.m_bLethal && point.m_bCenter ) {
			bestPoint = point;
			// ok, we done here
			break;
		}

		// don't run this on always prefer baim :p
		if( g_Vars.rage_default.prefer_body != 2 ) {
			player_info_t info;

			bool bResolved = point.m_bResolved;
			bool bGotPlayerInfo = m_pEngine->GetPlayerInfo( bestTarget.m_pEntity->EntIndex( ), &info );

			// note - michal;
			// ignore this on bots, because if people want to test around how the body-aim 
			// conditions function, then we don't want the aimbot perma shooting head due
			// to bots always being set to the resolved flag xd
			if( bGotPlayerInfo && info.fakeplayer ) {
				bResolved = false;
			}

			// head and resolved? yeah, take it
			if( point.m_bHead && bResolved ) {
				bestPoint = point;
				// ok, we done here
				break;
			}
		}

		// now once those points have been sorted we can do our baim conditions

		// Fake angles
		if( g_Vars.rage_default.prefer_body == 1 ) {
			// enemy isn't resolved
			if( !point.m_bResolved ) {
				// we found a body point which we can hit the center of
				// let's take it
				if( point.m_bBody && point.m_bCenter ) {
					bestPoint = point;
					break;
				}

				// we found a body point, awesome, let's keep it but keep looking
				// maybe there will be a body point with higher damage
				if( point.m_bBody ) {
					bestPoint = point;
				}
			}
		}
		// Always
		else if( g_Vars.rage_default.prefer_body == 2 ) {
			// we found body point, important to keep note of it and keep looking
			if( point.m_bBody ) {
				bestPoint = point;
			}

			// we found a (better) body point which we can hit the center of
			if( point.m_bBody && point.m_bCenter ) {
				bestPoint = point;
				break;
			}
		}
		// Aggressive
		else if( g_Vars.rage_default.prefer_body == 3 ) {
			// 2 shots will be enough to kill him in the body
			if( int( flMaxBodyDamage ) >= ( bestTarget.m_pEntity->m_iHealth( ) * 0.5f ) ) {
				// we found body point, important to keep note of it and keep looking
				if( point.m_bBody ) {
					bestPoint = point;
				}

				// we found a (better) body point which we can hit the center of
				if( point.m_bBody && point.m_bCenter ) {
					bestPoint = point;
					break;
				}
			}
		}
		// High inaccuracy
		else if( g_Vars.rage_default.prefer_body == 4 ) {
			const bool bDucked = m_AimbotInfo.m_pLocal->m_fFlags( ) & FL_DUCKING;
			const bool bSniper = m_AimbotInfo.m_pWeaponInfo->m_iWeaponType == WEAPONTYPE_SNIPER_RIFLE;
			const auto fnRoundAccuracy = [ ] ( const float accuracy ) { return roundf( accuracy * 1000.f ) / 1000.f; };

			const float flRoundedInaccuracy = fnRoundAccuracy( g_Prediction.GetInaccuracy( ) );

			const bool bAccurate = flRoundedInaccuracy == (
				bDucked ? fnRoundAccuracy( bSniper ? m_AimbotInfo.m_pWeaponInfo->m_flInaccuracyCrouchAlt : m_AimbotInfo.m_pWeaponInfo->m_flInaccuracyCrouch ) :
				fnRoundAccuracy( bSniper ? m_AimbotInfo.m_pWeaponInfo->m_flInaccuracyStandAlt : m_AimbotInfo.m_pWeaponInfo->m_flInaccuracyStand ) );

			// we're inaccurate
			if( !bAccurate || g_Resolver.m_arrResolverData[ bestTarget.m_pEntity->EntIndex( ) ].m_iMissedShotsSpread >= 2 ) {
				// we found body point, important to keep note of it and keep looking
				if( point.m_bBody ) {
					bestPoint = point;
				}

				// we found a (better) body point which we can hit the center of
				if( point.m_bBody && point.m_bCenter ) {
					bestPoint = point;
					break;
				}
			}
		}

		// damage higher and we don't already have a lethal point AND this point isn't head?
		if( point.m_iDamage > bestPoint.m_iDamage && !bestPoint.m_bLethal && !point.m_bHead ) {
			bestPoint = point;
		}
	}

	bool bBetweenShots = m_AimbotInfo.m_pWeapon->m_iItemDefinitionIndex( ) != WEAPON_SSG08 && m_AimbotInfo.m_pWeapon->m_iItemDefinitionIndex( ) != WEAPON_AWP;

	bool noAimbot = !m_AimbotInfo.m_pLocal->CanShoot( ) || g_Vars.globals.m_bShotWhileChoking;
	if( noAimbot ) {
		/*if( bBetweenShots && TICKS_TO_TIME( m_AimbotInfo.m_pLocal->m_nTickBase( ) ) >= m_AimbotInfo.m_pLocal->m_flNextAttack( ) ) {
			AutoStop( true );
		}*/

		g_Visuals.AddDebugMessage( std::string( XorStr( __FUNCTION__ ) ).append( XorStr( " -> L" ) + std::to_string( __LINE__ ) ) );
		return false;
	}

	if( !bestPoint.m_pTarget || !bestPoint.m_pTarget->m_pEntity ) {
		g_Visuals.AddDebugMessage( std::string( XorStr( __FUNCTION__ ) ).append( XorStr( " -> L" ) + std::to_string( __LINE__ ) ) );
		return false;
	}

	// always stop in the current tick
	//AutoStop( );
	m_AimbotInfo.m_bShouldStop = true;
	AutoScope( );

	float flHitchance{ };
	flHitchance = m_AimbotInfo.m_pSettings->hitchance_amount;

	// by 6 missed shots due to spread we'll have upped the hitchance by 35
	// increments by "5.83333333333" every missed spread shot 
	//flHitchance += 35.f * ( float( g_Resolver.m_arrResolverData.at( bestPoint.m_pTarget->m_pEntity->EntIndex( ) ).m_iMissedShotsSpread % 10 ) / 10.f );

	// scale the shit
	flHitchance *= 0.01f;

	const bool bHitchanced = RunHitchance( &bestPoint, flHitchance );

	if( bHitchanced ) {
		AimAtPoint( &bestPoint );
	}

	/*bool ret = false;
	if( AutoStop( ) ) {
		ret = true;
	}*/

	//if( ret ) {
	//	g_Visuals.AddDebugMessage( std::string( XorStr( __FUNCTION__ ) ).append( XorStr( " -> L" ) + std::to_string( __LINE__ ) ) );
	//}


	if( bHitchanced && m_AimbotInfo.m_pCmd->buttons & IN_ATTACK ) {
		PostAimbot( &bestPoint );
		last_shot_command = m_AimbotInfo.m_pCmd->command_number;
	}

	return true;
}

bool Ragebot::RunHitchance( AimPoint_t* pPoint, float chance ) {
	if( chance <= 0.0f )
		return true;

	if( !pPoint )
		return false;

	if( !pPoint->m_pTarget )
		return false;

	if( !pPoint->m_pTarget->m_pRecord
		|| !pPoint->m_pTarget->m_pRecord->player )
		return false;

	//if( ( m_AimbotInfo.m_vecEyePosition - pPoint->m_vecPosition ).Length( ) > m_AimbotInfo.m_pWeaponInfo->m_flWeaponRange )
	//	return false;

	Vector forward = pPoint->m_vecPosition - m_AimbotInfo.m_vecEyePosition;
	forward.Normalize( );

	Vector right, up;
	forward.GetVectors( right, up );

	if( !pPoint->m_pHitbox )
		return false;

	const auto pMatrix = pPoint->m_pTarget->m_pRecord->m_pMatrix;
	if( !pMatrix )
		return false;

	const bool bCapsule = pPoint->m_pHitbox->m_flRadius != -1.0f;

	const float flInaccuracy = g_Prediction.GetInaccuracy( );
	const float flSpread = g_Prediction.GetSpread( );

	auto is_special_weapon = m_AimbotInfo.m_pWeapon->m_iItemDefinitionIndex() == 9
		|| m_AimbotInfo.m_pWeapon->m_iItemDefinitionIndex() == 11
		|| m_AimbotInfo.m_pWeapon->m_iItemDefinitionIndex() == 38
		|| m_AimbotInfo.m_pWeapon->m_iItemDefinitionIndex() == 40;

	if (is_special_weapon && flInaccuracy <= g_Prediction.m_flPerfectAccuracy)
		return true;

	const Vector vecMin = pPoint->m_pHitbox->bbmin.Transform( pMatrix[ pPoint->m_pHitbox->bone ] );
	const Vector vecMax = pPoint->m_pHitbox->bbmax.Transform( pMatrix[ pPoint->m_pHitbox->bone ] );

	constexpr int maxIntersections = 255;

	int hits_needed = float( maxIntersections ) * ( float( chance ) / 100.f );

	int bHitchanceHits = 0;
	for( int i = 0; i < maxIntersections; ++i ) {
		float flRand1 = g_Vars.globals.SpreadRandom[ i ].flRand1;
		float flRandPi1 = g_Vars.globals.SpreadRandom[ i ].flRandPi1;
		float flRand2 = g_Vars.globals.SpreadRandom[ i ].flRand2;
		float flRandPi2 = g_Vars.globals.SpreadRandom[ i ].flRandPi2;

		const float flRandInaccuracy = flRand1 * flInaccuracy;
		const float flRandSpread = flRand2 * flSpread;

		float flRandPi1Cos, flRandPi1Sin;
		DirectX::XMScalarSinCos( &flRandPi1Sin, &flRandPi1Cos, flRandPi1 );

		float flRandPi2Cos, flRandPi2Sin;
		DirectX::XMScalarSinCos( &flRandPi2Sin, &flRandPi2Cos, flRandPi2 );

		float spread_x = flRandPi1Cos * flRandInaccuracy + flRandPi2Cos * flRandSpread;
		float spread_y = flRandPi1Sin * flRandInaccuracy + flRandPi2Sin * flRandSpread;

		Vector vecDirection;
		vecDirection.x = forward.x + ( spread_x * right.x ) + ( spread_y * up.x );
		vecDirection.y = forward.y + ( spread_x * right.y ) + ( spread_y * up.y );
		vecDirection.z = forward.z + ( spread_x * right.z ) + ( spread_y * up.z );
		vecDirection.Normalize( );

		Vector vecEnd = m_AimbotInfo.m_vecEyePosition + ( vecDirection * 8192.f );

		auto bHit = bCapsule ? Math::IntersectSegmentToSegment( m_AimbotInfo.m_vecEyePosition, vecEnd, vecMin, vecMax, pPoint->m_pHitbox->m_flRadius ) : Math::IntersectionBoundingBox( m_AimbotInfo.m_vecEyePosition, pPoint->m_pHitbox, vecDirection, pMatrix[ pPoint->m_pHitbox->bone ] );

		if( bHit ) {
			++bHitchanceHits;
		}

		//m_pDebugOverlay->AddLineOverlay(m_AimbotInfo.m_vecEyePosition, vecEnd, 255, bHit ? 255 : 0, 0, true, 2.f * m_pGlobalVars->interval_per_tick);

		// abort if we can no longer reach hitchance.
		if( float( maxIntersections - i + bHitchanceHits ) < hits_needed ) {
			pPoint->m_iHitchance = 0;

			return false;
		}
	}

	// calculate hitchance.
	float calculatedHitchance{ };
	if( bHitchanceHits ) {
		calculatedHitchance = float( bHitchanceHits ) / float( maxIntersections );
	}
	else {
		calculatedHitchance = 0.f;
	}

	pPoint->m_iHitchance = int( calculatedHitchance * 100.f );

	if( calculatedHitchance >= chance ) 
	{
		int boostHits{ }, traceHits{ };
	
		std::vector<AimPoint_t> vecComputedPoints{ };
	
		for( int i = 1; i <= 6; i++ ) {
			for( int j = 0; j < 8; ++j ) {
				float flCalculatedSpread = ( flSpread + flInaccuracy ) * float( float( i ) / 6.f );
	
				float flDirCos, flDirSin;
				DirectX::XMScalarSinCos( &flDirCos, &flDirSin, DirectX::XM_2PI * float( float( j ) / 8.0f ) );
	
				// calculate spread
				Vector2D vecSpread;
				vecSpread.x = flDirCos * flCalculatedSpread;
				vecSpread.y = flDirSin * flCalculatedSpread;
	
				// calculate direction
				Vector vecDirection;
				vecDirection.x = ( forward.x + vecSpread.x * right.x + vecSpread.y * up.x );
				vecDirection.y = ( forward.y + vecSpread.x * right.y + vecSpread.y * up.y );
				vecDirection.z = ( forward.z + vecSpread.x * right.z + vecSpread.y * up.z );
				vecDirection.Normalize( );
	
				// calculate end point
				Vector vecEnd = m_AimbotInfo.m_vecEyePosition + vecDirection * 8196.f;
	
				auto bHit = bCapsule ? Math::IntersectSegmentToSegment(m_AimbotInfo.m_vecEyePosition, vecEnd, vecMin, vecMax, pPoint->m_pHitbox->m_flRadius) : Math::IntersectionBoundingBox(m_AimbotInfo.m_vecEyePosition, pPoint->m_pHitbox, vecDirection, pMatrix[pPoint->m_pHitbox->bone]);
				if( bHit ) {
					boostHits++;
	
					AimPoint_t point{ };
					point.m_pHitbox = pPoint->m_pHitbox;
					point.m_pTarget = pPoint->m_pTarget;
					point.m_vecPosition = vecEnd;
					point.m_pFireBulletData = new Autowall::C_FireBulletData( );
					if( point.m_pFireBulletData != nullptr ) {
						point.m_pFireBulletData->m_vecStart = m_AimbotInfo.m_vecEyePosition;
						point.m_pFireBulletData->m_vecPos = point.m_vecPosition;
	
						Vector vecDirection = ( point.m_pFireBulletData->m_vecPos - point.m_pFireBulletData->m_vecStart );
						vecDirection.Normalize( );
	
						point.m_pFireBulletData->m_vecDirection = vecDirection;
						point.m_pFireBulletData->m_iHitgroup = point.m_pHitbox->group;
	
						point.m_pFireBulletData->m_Player = m_AimbotInfo.m_pLocal;
						point.m_pFireBulletData->m_TargetPlayer = point.m_pTarget->m_pEntity;
						point.m_pFireBulletData->m_Weapon = m_AimbotInfo.m_pWeapon;
						point.m_pFireBulletData->m_WeaponData = m_AimbotInfo.m_pWeaponInfo;
	
						point.m_pFireBulletData->m_bPenetration = m_AimbotInfo.m_pSettings->wall_penetration;
	
						// has to be done like this, pointer to it needs to be kept alive
						vecComputedPoints.push_back( point );
					}
				}
			}
		}
	
		for( auto& point : vecComputedPoints ) {
			if( point.m_pFireBulletData == nullptr || !point.m_pTarget || !point.m_pTarget->m_pEntity || !point.m_pTarget->m_pRecord->player )
				continue;
	
#ifdef MT_AWALL
			Threading::QueueJobRef( RunAwall, &point );
#else
			RunAwall( &point );
#endif
		}
	
#ifdef MT_AWALL
		Threading::FinishQueue( );
#endif
	
		for( auto& point : vecComputedPoints ) {
			if( point.m_pFireBulletData == nullptr )
				continue;
	
			if( point.m_pFireBulletData->m_flCurrentDamage > 1.f ) {
				++traceHits;
			}
	
			// let's better delete the allocated memory (LMAO)
			delete point.m_pFireBulletData;
	
			// let's set it to null..
			point.m_pFireBulletData = nullptr;
		}
	
		if( !traceHits )
			return false;
	
		if( !vecComputedPoints.empty( ) )
			vecComputedPoints.clear( );
	
		const float scaledHits = traceHits / 48.f;
	
		return scaledHits >= chance;
	}

	return false;//calculatedHitchance >= chance;
}

std::vector<C_CSPlayer*> Ragebot::FindTargets( ) {
	std::vector<C_CSPlayer*> vecPlayers{ };

	for( int i = 1; i <= m_pGlobalVars->maxClients; ++i ) {
		const auto player = C_CSPlayer::GetPlayerByIndex( i );
		if( !player )
			continue;

		if( player->IsDead( ) || player->m_bGunGameImmunity( ) || player->IsDormant( ) )
			continue;

		if( player->IsTeammate( m_AimbotInfo.m_pLocal ) )
			continue;

		vecPlayers.push_back( player );
	}

	return vecPlayers;
}

void Ragebot::AddPoint( AimTarget_t* pTarget, mstudiobbox_t* pHitbox, Vector vecPosition, int iHitbox, bool center ) {
	if( !pTarget )
		return;

	if( !pHitbox )
		return;

	AimPoint_t aimPoint{ };
	aimPoint.m_pHitbox = pHitbox;
	aimPoint.m_vecPosition = vecPosition;
	aimPoint.m_pTarget = pTarget;
	aimPoint.m_bCenter = center;
	aimPoint.m_iHitbox = iHitbox;

	pTarget->m_vecAimPoints.push_back( aimPoint );
}

void Ragebot::AddPoints( AimTarget_t* pTarget, Hitboxes hitbox ) {
	if( !pTarget )
		return;

	const auto pMatrix = pTarget->m_pRecord->m_pMatrix;
	if( !pMatrix )
		return;

	const auto pHitbox = pTarget->m_pHitboxSet->pHitbox( hitbox );
	if( !pHitbox )
		return;

	Vector vecCenter = ( pHitbox->bbmax + pHitbox->bbmin ) * 0.5f;
	AddPoint( pTarget, pHitbox, vecCenter.Transform( pMatrix[ pHitbox->bone ] ), hitbox, true );

	float pointScale = m_AimbotInfo.m_pSettings->pointscale;
	if( pointScale <= 0.0f )
		return;

	// yup
	pointScale *= 0.01f;

	bool bHead = false,
		bStomach = false,
		bChest = false,
		bLegs = false,
		bFeet = false;

	// low 
	if( g_Vars.rage_default.multipoint == 1 ) {
		bHead = bStomach = true;
	}
	// medium 
	else if( g_Vars.rage_default.multipoint == 2 ) {
		bHead = bStomach = bChest = bLegs = true;
	}
	// high
	else if( g_Vars.rage_default.multipoint == 3 ) {
		bHead = bStomach = bChest = bLegs = bFeet = true;
	}
	// vitals
	else if( g_Vars.rage_default.multipoint == 4 ) {
		bHead = bStomach = bFeet = true;
	}

	const bool bCapsule = pHitbox->m_flRadius != -1.0f;
	if( bCapsule ) {
		float radiusScaled = pHitbox->m_flRadius * pointScale;

		// head multipoints
		if( hitbox == Hitboxes::HITBOX_HEAD && bHead ) {
			Vector vecRight{ pHitbox->bbmax.x, pHitbox->bbmax.y, pHitbox->bbmax.z + ( pHitbox->m_flRadius * 0.5f ) };
			AddPoint( pTarget, pHitbox, vecRight.Transform( pMatrix[ pHitbox->bone ] ), hitbox );

			Vector vecLeft{ pHitbox->bbmax.x, pHitbox->bbmax.y, pHitbox->bbmax.z - ( pHitbox->m_flRadius * 0.5f ) };
			AddPoint( pTarget, pHitbox, vecLeft.Transform( pMatrix[ pHitbox->bone ] ), hitbox );

			// top/back 45 deg.
			constexpr float rotation = 0.70710678f;

			pointScale = std::clamp( pointScale, 0.1f, 0.82f );
			radiusScaled = pHitbox->m_flRadius * pointScale;

			Vector vecTopBack{ pHitbox->bbmax.x + ( rotation * radiusScaled ), pHitbox->bbmax.y + ( -rotation * radiusScaled ), pHitbox->bbmax.z };
			AddPoint( pTarget, pHitbox, vecTopBack.Transform( pMatrix[ pHitbox->bone ] ), hitbox );

			pointScale = std::clamp( pointScale, 0.1f, 0.7f );
			radiusScaled = pHitbox->m_flRadius * pointScale;

			Vector vecBack{ vecCenter.x, pHitbox->bbmax.y - radiusScaled, vecCenter.z };
			AddPoint( pTarget, pHitbox, vecBack.Transform( pMatrix[ pHitbox->bone ] ), hitbox );
		}
		else if( hitbox == Hitboxes::HITBOX_STOMACH && bStomach ) {
			// apparently stomach hitbox is also fucked
			Vector vecRight{ pHitbox->bbmax.x, pHitbox->bbmax.y, pHitbox->bbmax.z + ( pHitbox->m_flRadius * 1.5f ) };
			AddPoint( pTarget, pHitbox, vecRight.Transform( pMatrix[ pHitbox->bone ] ), hitbox );

			Vector vecLeft{ pHitbox->bbmax.x, pHitbox->bbmax.y, pHitbox->bbmax.z - ( pHitbox->m_flRadius * 0.5f ) };
			AddPoint( pTarget, pHitbox, vecLeft.Transform( pMatrix[ pHitbox->bone ] ), hitbox );

			pointScale = std::clamp( pointScale, 0.1f, 0.7f );
			radiusScaled = pHitbox->m_flRadius * pointScale;

			Vector vecBack{ vecCenter.x, pHitbox->bbmax.y - radiusScaled, vecCenter.z };
			AddPoint( pTarget, pHitbox, vecBack.Transform( pMatrix[ pHitbox->bone ] ), hitbox );
		}
		else if( hitbox == Hitboxes::HITBOX_LOWER_CHEST || hitbox == Hitboxes::HITBOX_CHEST && bChest ) {
			pointScale = std::clamp( pointScale, 0.1f, 0.75f );
			radiusScaled = pHitbox->m_flRadius * pointScale;

			Vector vecBack{ vecCenter.x, pHitbox->bbmax.y - radiusScaled, vecCenter.z };
			AddPoint( pTarget, pHitbox, vecBack.Transform( pMatrix[ pHitbox->bone ] ), hitbox );
		}

		if( bLegs ) {
			if( hitbox == Hitboxes::HITBOX_RIGHT_THIGH || hitbox == Hitboxes::HITBOX_LEFT_THIGH ) {
				Vector vecHalfBottom{ pHitbox->bbmax.x - ( pHitbox->m_flRadius * ( 0.5f * pointScale ) ), pHitbox->bbmax.y, pHitbox->bbmax.z };
				AddPoint( pTarget, pHitbox, vecHalfBottom.Transform( pMatrix[ pHitbox->bone ] ), hitbox );
			}
			else if( hitbox == Hitboxes::HITBOX_RIGHT_CALF || hitbox == Hitboxes::HITBOX_LEFT_CALF ) {
				Vector vecHalfTop{ pHitbox->bbmax.x - ( pHitbox->m_flRadius * ( 0.5f * pointScale ) ), pHitbox->bbmax.y, pHitbox->bbmax.z };
				AddPoint( pTarget, pHitbox, vecHalfTop.Transform( pMatrix[ pHitbox->bone ] ), hitbox );
			}
		}
	}
	else {
		// feet multipoints (this shit is so fucked seriously)
		if( bFeet ) {
			if( hitbox == Hitboxes::HITBOX_RIGHT_FOOT || hitbox == Hitboxes::HITBOX_LEFT_FOOT ) {
				pointScale = std::clamp( pointScale, 0.1f, 0.80f );

				if( hitbox == Hitboxes::HITBOX_LEFT_FOOT ) {
					Vector vecHeel{ vecCenter.x + ( ( pHitbox->bbmax.x - vecCenter.x ) * pointScale ), vecCenter.y, vecCenter.z };
					AddPoint( pTarget, pHitbox, vecHeel.Transform( pMatrix[ pHitbox->bone ] ), hitbox );
				}

				if( hitbox == Hitboxes::HITBOX_RIGHT_FOOT ) {
					Vector vecHeel{ vecCenter.x + ( ( pHitbox->bbmin.x - vecCenter.x ) * pointScale ), vecCenter.y, vecCenter.z };
					AddPoint( pTarget, pHitbox, vecHeel.Transform( pMatrix[ pHitbox->bone ] ), hitbox );
				}
			}
		}
	}
}

bool Ragebot::AimAtPoint( AimPoint_t* pPoint ) {
	if( !pPoint ) {
		g_Visuals.AddDebugMessage( std::string( XorStr( __FUNCTION__ ) ).append( XorStr( " -> L" ) + std::to_string( __LINE__ ) ) );
		return false;
	}

	if( !pPoint->m_pTarget ) {
		g_Visuals.AddDebugMessage( std::string( XorStr( __FUNCTION__ ) ).append( XorStr( " -> L" ) + std::to_string( __LINE__ ) ) );
		return false;
	}

	if( g_Vars.rage.delay_shot ) {
		bool bInUnduck = false;

		// we're ducking
		if( m_AimbotInfo.m_pLocal->m_flDuckAmount( ) > 0.f ) {
			// we're not fully ducked
			if( m_AimbotInfo.m_pLocal->m_flDuckAmount( ) < 1.0f ) {
				static float flPreviousDuck = m_AimbotInfo.m_pLocal->m_flDuckAmount( );

				// duck amount is changing, and current duck amount is lower than last duck amount
				if( flPreviousDuck != m_AimbotInfo.m_pLocal->m_flDuckAmount( ) ) {
					if( m_AimbotInfo.m_pLocal->m_flDuckAmount( ) < flPreviousDuck ) {
						bInUnduck = true;
						//m_pCvar->ConsoleColorPrintf( Color::White( ), "Unducking; %.2f - %.2f\n", flPreviousDuck, m_AimbotInfo.m_pLocal->m_flDuckAmount( ) );
					}
					flPreviousDuck = m_AimbotInfo.m_pLocal->m_flDuckAmount( );
				}
			}
		}

		if( bInUnduck ) {
			if( m_AimbotInfo.m_pLocal->m_flDuckAmount( ) >= 0.15f ) {
				//m_pCvar->ConsoleColorPrintf( Color::White( ), "Returning at; %.2f\n", m_AimbotInfo.m_pLocal->m_flDuckAmount( ) );
				return false;
			}
		}
	}

	// remove use flag from usercmd
	m_AimbotInfo.m_pCmd->buttons &= ~IN_USE;

	Vector delta = pPoint->m_vecPosition - m_AimbotInfo.m_vecEyePosition;
	delta.Normalize( );

	QAngle aimAngles;
	Math::VectorAngles( delta, aimAngles );

	if( !g_Vars.rage.silent_aim ) {
		m_pEngine->SetViewAngles( aimAngles );
	}

	m_AimbotInfo.m_pCmd->tick_count = TIME_TO_TICKS( pPoint->m_pTarget->m_pRecord->m_flSimulationTime + g_LagCompensation.GetLerp( ) );
	m_AimbotInfo.m_pCmd->viewangles = aimAngles;

	auto no_spread = [ & ] ( ) {
		auto s1 = RandomFloat( 0.f, 1.f ), s2 = RandomFloat( 0.f, 2.f * M_PI ), s3 = RandomFloat( 0.f, 1.f ), s4 = RandomFloat( 0.f, 2.f * M_PI );

		if( m_AimbotInfo.m_pWeapon->m_iItemDefinitionIndex( ) == WEAPON_REVOLVER && m_AimbotInfo.m_pCmd->buttons & IN_ATTACK2 ) {
			s1 = 1.f - s1 * s1;
			s3 = 1.f - s3 * s3;
		}
		else if( m_AimbotInfo.m_pWeapon->m_iItemDefinitionIndex( ) == WEAPON_NEGEV && m_AimbotInfo.m_pWeapon->m_flRecoilIndex( ) < 3.f ) {
			for( auto i = 3; i > m_AimbotInfo.m_pWeapon->m_flRecoilIndex( ); i-- ) {
				s1 *= s1;
				s3 *= s3;
			}

			s1 = 1.f - s1;
			s3 = 1.f - s3;
		}

		auto random_spread = s1 * g_Prediction.GetSpread( );
		auto cone = s3 * g_Prediction.GetInaccuracy( );

		auto spread = Vector( sin( s2 ) * random_spread + sin( s4 ) * cone, cos( s2 ) * random_spread + cos( s4 ) * cone, 0.f );

		m_AimbotInfo.m_pCmd->viewangles.x += RAD2DEG( atan( spread.Length2D( ) ) );
		m_AimbotInfo.m_pCmd->viewangles.z = -RAD2DEG( atan2( spread.y, spread.x ) );
	};

	if( g_Vars.rage.remove_spread )
		no_spread( );

	// norecoil lmfao.
	if( g_Vars.rage.remove_recoil )
		m_AimbotInfo.m_pCmd->viewangles -= m_AimbotInfo.m_pLocal->m_aimPunchAngle( ) * g_Vars.weapon_recoil_scale->GetFloat( );

	if( g_Vars.rage.auto_fire ) {

		bool can_shoot = /*!g_Vars.rage.silent_aim || */m_pClientState->m_nChokedCommands( ) < 14 || g_Vars.rage.double_tap_bind.enabled;

		if (!!g_Vars.rage.double_tap_bind.enabled) {
			if (!g_Vars.globals.m_bFakeWalking) {
				*m_AimbotInfo.m_pSendPacket = !g_Vars.rage.silent_aim;
			}
			else
				can_shoot = !*m_AimbotInfo.m_pSendPacket;
		}

		g_TickbaseController.m_flLastExploitTime = m_pGlobalVars->realtime;

		if( can_shoot )
			m_AimbotInfo.m_pCmd->buttons |= IN_ATTACK;
	}

	return true;
}

void Ragebot::SetupEyePosition( ) {
	if( !m_AimbotInfo.m_pLocal )
		return;

	auto animState = m_AimbotInfo.m_pLocal->m_PlayerAnimState( );
	if( !animState )
		return;

	// backup animation shit
	const auto animStateBackup = *m_AimbotInfo.m_pLocal->m_PlayerAnimState( );
	float poses[ 20 ];
	C_AnimationLayer layers[ 13 ];
	matrix3x4_t matrix[ 128 ];
	std::memcpy( layers, m_AimbotInfo.m_pLocal->m_AnimOverlay( ).Base( ), sizeof( layers ) );
	std::memcpy( poses, m_AimbotInfo.m_pLocal->m_flPoseParameter( ), sizeof( poses ) );
	std::memcpy( matrix, m_AimbotInfo.m_pLocal->m_CachedBoneData( ).Base( ), m_AimbotInfo.m_pLocal->m_CachedBoneData( ).Count( ) * sizeof( matrix3x4_t ) );

	// allow reanimating
	if( animState->m_nLastUpdateFrame == m_pGlobalVars->framecount )
		animState->m_nLastUpdateFrame = m_pGlobalVars->framecount - 1;

	const float PitchPosBackup = *( float* )( uintptr_t( m_AimbotInfo.m_pLocal ) + Engine::Displacement.DT_BaseAnimating.m_flPoseParameter + 48 );
	*( float* )( uintptr_t( m_AimbotInfo.m_pLocal ) + Engine::Displacement.DT_BaseAnimating.m_flPoseParameter + 48 ) = 0.5f;

	m_AimbotInfo.m_pLocal->SetupBones( nullptr, 128, BONE_USED_BY_ANYTHING, m_AimbotInfo.m_pLocal->m_flSimulationTime( ) );

	*( float* )( uintptr_t( m_AimbotInfo.m_pLocal ) + Engine::Displacement.DT_BaseAnimating.m_flPoseParameter + 48 ) = PitchPosBackup;

	// setup eyepos.
	m_AimbotInfo.m_vecEyePosition = m_AimbotInfo.m_pLocal->GetEyePosition( );

	std::memcpy( m_AimbotInfo.m_pLocal->m_CachedBoneData( ).Base( ), matrix, m_AimbotInfo.m_pLocal->m_CachedBoneData( ).Count( ) * sizeof( matrix3x4_t ) );

	// restore animation shit
	std::memcpy( m_AimbotInfo.m_pLocal->m_AnimOverlay( ).Base( ), layers, sizeof( layers ) );
	std::memcpy( m_AimbotInfo.m_pLocal->m_flPoseParameter( ), poses, sizeof( poses ) );
	*m_AimbotInfo.m_pLocal->m_PlayerAnimState( ) = animStateBackup;
}

void Ragebot::CockRevolver( ) {
	if( !g_Vars.misc.auto_weapons )
		return;

	if( m_AimbotInfo.m_pWeapon->m_iItemDefinitionIndex( ) != WEAPON_REVOLVER )
		return;

	if( !( m_AimbotInfo.m_pCmd->buttons & IN_RELOAD ) && m_AimbotInfo.m_pWeapon->m_iClip1( ) ) {
		static float cockTime = 0.f;

		const float curtime = m_AimbotInfo.m_pLocal->m_nTickBase( ) * m_pGlobalVars->interval_per_tick;

		m_AimbotInfo.m_pCmd->buttons &= ~IN_ATTACK2;

		if( m_AimbotInfo.m_pLocal->CanShoot( true ) ) {
			if( cockTime <= curtime ) {
				if( m_AimbotInfo.m_pWeapon->m_flNextSecondaryAttack( ) <= curtime ) {
					cockTime = curtime + 0.234375f;
				}
				else {
					m_AimbotInfo.m_pCmd->buttons |= IN_ATTACK2;
				}
			}
			else {
				m_AimbotInfo.m_pCmd->buttons |= IN_ATTACK;
			}
		}
		else {
			cockTime = curtime + 0.234375f;
			m_AimbotInfo.m_pCmd->buttons &= ~IN_ATTACK;
		}
	}
}


std::string hitbox_to_string( int h )
{
	switch( h )
	{
	case 0:
		return "head";
		break;
	case 1:
		return "neck";
		break;
	case HITBOX_RIGHT_FOOT:
		return "right foot";
		break;
	case HITBOX_LEFT_FOOT:
		return "left foot";
		break;
	case HITBOX_RIGHT_HAND:
		return "right hand";
		break;
	case HITBOX_LEFT_HAND:
		return "left hand";
		break;
	default:
		return "body";
		break;
	}
}


void Ragebot::PostAimbot( AimPoint_t* pPoint ) {
	if( !pPoint )
		return;

	if( !pPoint->m_pTarget )
		return;

	if( !pPoint->m_pTarget->m_pEntity )
		return;

	if( !pPoint->m_pTarget->m_pHitboxSet )
		return;

	if( !pPoint->m_pTarget->m_pRecord
		|| !pPoint->m_pTarget->m_pRecord->player || pPoint->m_pTarget->m_pRecord->player == nullptr )
		return;

	// if we got here, let's increment the current shotid
	++m_AimbotInfo.m_iShotID;

	player_info_t info;
	if( m_pEngine->GetPlayerInfo( pPoint->m_pTarget->m_pEntity->EntIndex( ), &info ) ) {
		auto FixedStrLength = [ ] ( std::string str ) -> std::string {
			if( ( int )str[ 0 ] > 255 )
				return XorStr( "" );

			if( str.size( ) < 15 )
				return str;

			std::string result;
			for( size_t i = 0; i < 15u; i++ )
				result.push_back( str.at( i ) );
			return result;
		};

		auto HitgroupToString = [ ] ( int hitgroup ) -> std::string {
			switch( hitgroup ) {
			case Hitgroup_Generic:
				return XorStr( "generic" );
			case Hitgroup_Head:
				return XorStr( "head" );
			case Hitgroup_Chest:
				return XorStr( "chest" );
			case Hitgroup_Stomach:
				return XorStr( "stomach" );
			case Hitgroup_LeftArm:
				return XorStr( "left arm" );
			case Hitgroup_RightArm:
				return XorStr( "right arm" );
			case Hitgroup_LeftLeg:
				return XorStr( "left leg" );
			case Hitgroup_RightLeg:
				return XorStr( "right leg" );
			case Hitgroup_Neck:
				return XorStr( "neck" );
			}
			return XorStr( "generic" );
		};

		std::stringstream msg{ };

		std::vector<std::string> flags{ };
		if( pPoint->m_bPenetrated ) {
			flags.push_back( XorStr( "PEN" ) );
		}
		if( pPoint->m_bCenter ) {
			flags.push_back( XorStr( "CTR" ) );
		}
		if( pPoint->m_bLethal ) {
			flags.push_back( XorStr( "L" ) );
		}

		std::string buffer{ };
		if( !flags.empty( ) ) {
			for( auto n = 0; n < flags.size( ); ++n ) {
				auto& it = flags.at( n );

				buffer.append( it );
				if( n != flags.size( ) - 1 && flags.size( ) > 1 )
					buffer.append( XorStr( ":" ) );
			}
		}
		else {
			buffer = XorStr( "none" );
		}

		const int nHistoryTicks = TIME_TO_TICKS( pPoint->m_pTarget->m_pEntity->m_flSimulationTime( ) - pPoint->m_pTarget->m_pRecord->m_flSimulationTime );

		//if( pPoint->m_pTarget->m_pRecord->m_pMatrix )
		//	pPoint->m_pTarget->m_pEntity->DrawHitboxMatrix( pPoint->m_pTarget->m_pRecord->m_pMatrix, pPoint->m_pHitbox );

#if 0
		auto hdr = m_pModelInfo->GetStudiomodel( pPoint->m_pTarget->m_pEntity->GetModel( ) );
		if( hdr ) {
			auto hitboxSet = hdr->pHitboxSet( pPoint->m_pTarget->m_pEntity->m_nHitboxSet( ) );
			if( hitboxSet ) {
				for( int i = 0; i < hitboxSet->numhitboxes; ++i ) {
					auto hitbox = hitboxSet->pHitbox( i );
					if( !hitbox )
						continue;

					if( hitbox->m_flRadius <= 0.f )
						continue;

					auto min = hitbox->bbmin.Transform( pPoint->m_pTarget->m_pRecord->m_pMatrix[ hitbox->bone ] );
					auto max = hitbox->bbmax.Transform( pPoint->m_pTarget->m_pRecord->m_pMatrix[ hitbox->bone ] );

					m_pDebugOverlay->AddCapsuleOverlay( min, max, hitbox->m_flRadius, 255, 255, 255, 255,
						m_pCvar->FindVar( XorStr( "sv_showlagcompensation_duration" ) )->GetFloat( ) );
				}
			}
		}
	}
#endif
	ShotInformation_t shot;
	shot.m_iPredictedDamage = pPoint->m_iDamage;
	shot.m_flTime = m_pGlobalVars->realtime;
	shot.m_iTargetIndex = pPoint->m_pTarget->m_pEntity->EntIndex( );
	shot.m_bTapShot = g_TickbaseController.m_bTapShot;
	shot.m_vecStart = m_AimbotInfo.m_vecEyePosition;
	shot.m_pHitbox = pPoint->m_pHitbox;
	shot.m_pRecord = *pPoint->m_pTarget->m_pRecord;
	shot.m_vecEnd = pPoint->m_vecPosition;
	shot.m_bWasLBYFlick = pPoint->m_pTarget->m_pRecord->m_RecordFlags & ELagRecordFlags::RF_LBYFlicked;
	shot.m_bHadPredError = nHistoryTicks < 0; // happened a few times, always had a pred error then
	g_ShotHandling.RegisterShot( shot );

	std::string new_nick( info.szName ); new_nick.resize( 15 );

	C_AnimationLayer* layer_3 = &pPoint->m_pTarget->m_pEntity->m_AnimOverlay( )[ 3 ];
	if( layer_3 ) {
		int layer_3_activity = pPoint->m_pTarget->m_pEntity->GetSequenceActivity( layer_3->m_nSequence );

		// fired shot at name in hitbox (hitbox id), r:[choked ticks|backtrack_ticks|onshot='s' lbyflick ='lby' fakewalk='fw' nothing='?'|velocity]:[inair='air' duck='duck' neither='?']
		m_pCvar->ConsoleColorPrintf( Color::White( ), "fired shot at %s in %s (%i) for %i dmg | R:[%i|%i|%s|%.1f]:[%s]:[%i]\n",
			new_nick.c_str( ),
			hitbox_to_string( pPoint->m_iHitbox ).c_str( ),
			pPoint->m_iHitbox,
			pPoint->m_iDamage,
			pPoint->m_pTarget->m_pRecord->m_iLaggedTicks,
			m_pGlobalVars->tickcount - TIME_TO_TICKS( pPoint->m_pTarget->m_pRecord->m_flSimulationTime ),
			( pPoint->m_pTarget->m_pRecord->m_RecordFlags & ELagRecordFlags::RF_Shot ) != 0 ? "S" : ( shot.m_bWasLBYFlick ? "LBY" : ( ( pPoint->m_pTarget->m_pRecord->m_RecordFlags & ELagRecordFlags::RF_FakeWalking ) != 0 ? "FW" : "?" ) ),
			pPoint->m_pTarget->m_pRecord->m_vecVelocity.Length( ),
			( pPoint->m_pTarget->m_pRecord->m_iFlags & FL_ONGROUND ) == 0 ? "AIR" : ( ( pPoint->m_pTarget->m_pRecord->m_iFlags & 6 ) != 0 ? "DUCK" : "?" ),
			layer_3_activity
		);
	}
}
}

bool Ragebot::RunInternal( ) {
	// early autostop here

	if( RunHitscan( ) ) {
		return FinishAimbot( );
	}

	g_Visuals.AddDebugMessage( std::string( XorStr( __FUNCTION__ ) ).append( XorStr( " -> L" ) + std::to_string( __LINE__ ) ) );
	return false;
}

bool Ragebot::Run( bool* bSendPacket, CUserCmd* pCmd ) {
	if( !g_Vars.rage.enabled || !g_Vars.rage.key.enabled ) {
		return false;
	}

	if( !g_Vars.globals.m_bInitRandomSeed ) {
		return false;
	}

	const auto pLocal = C_CSPlayer::GetLocalPlayer( );
	if( !pLocal )
		return false;

	if( pLocal->IsDead( ) ) {
		return false;
	}

	auto pWeapon = reinterpret_cast< C_WeaponCSBaseGun* >( pLocal->m_hActiveWeapon( ).Get( ) );
	if( !pWeapon ) {
		return false;
	}

	auto pWeaponInfo = pWeapon->GetCSWeaponData( );
	if( !pWeaponInfo.IsValid( ) ) {
		return false;
	}

	if( pWeapon->m_iItemDefinitionIndex( ) != WEAPON_ZEUS ) {
		if( pWeaponInfo->m_iWeaponType == WEAPONTYPE_KNIFE || pWeaponInfo->m_iWeaponType == WEAPONTYPE_GRENADE || pWeaponInfo->m_iWeaponType == WEAPONTYPE_C4 ) {
			return false;
		}
	}

	// setup aimbot info variables
	m_AimbotInfo.m_pLocal = pLocal;
	m_AimbotInfo.m_pWeaponInfo = pWeaponInfo.Xor( );
	m_AimbotInfo.m_pWeapon = pWeapon;
	m_AimbotInfo.m_pCmd = pCmd;
	m_AimbotInfo.m_pSendPacket = bSendPacket;

	if (last_shot_command == m_AimbotInfo.m_pCmd->command_number)
		return false;

	// setup eyepos
	SetupEyePosition( );

	m_AimbotInfo.m_pSettings = GetRageSettings( );
	if( !m_AimbotInfo.m_pSettings )
		return false;

	if( g_Vars.rage.silent_aim && !g_Vars.rage.double_tap_bind.enabled )
	{
		if( m_pClientState->m_nChokedCommands( ) >= 13 )
			return false;

		if( *bSendPacket && m_pClientState->m_nChokedCommands( ) > 2 ) //has a reason
			return false;
	}

	// cock the volverr
	CockRevolver( );

	// reset data from last tick
	m_AimbotInfo.m_bDidStop = false;

	if( !m_AimbotInfo.m_vecAimData.empty( ) )
		m_AimbotInfo.m_vecAimData.clear( );

	auto bRet = RunInternal( );

	if( g_Visuals.debug_messages.size( ) )
		g_Visuals.debug_messages_sane = std::move( g_Visuals.debug_messages );

	g_Visuals.debug_messages.clear( );
	return bRet;
}