#include "AlternativeAttack.hpp"
#include "../../SDK/Classes/player.hpp"
#include "../../source.hpp"
#include "../../SDK/Classes/weapon.hpp"
#include "TickbaseShift.hpp"
#include "Autowall.hpp"
#include "Ragebot.hpp"
#include "Resolver.hpp"

AlternativeAttack g_AlternativeAttack;

void AlternativeAttack::RunAttacks( bool* sendPacket, Encrypted_t<CUserCmd> pCmd ) {
	if( !m_pEngine->IsInGame( ) || !g_Vars.rage.enabled )
		return;

	m_AlternativeAttackData.m_pLocalPlayer = C_CSPlayer::GetLocalPlayer( );
	if( !m_AlternativeAttackData.m_pLocalPlayer || m_AlternativeAttackData.m_pLocalPlayer->IsDead( ) )
		return;

	m_AlternativeAttackData.m_pLocalWeapon = ( C_WeaponCSBaseGun* )m_AlternativeAttackData.m_pLocalPlayer->m_hActiveWeapon( ).Get( );
	if( !m_AlternativeAttackData.m_pLocalWeapon || !m_AlternativeAttackData.m_pLocalWeapon->IsWeapon( ) )
		return;

	m_AlternativeAttackData.m_pWeaponInfo = m_AlternativeAttackData.m_pLocalWeapon->GetCSWeaponData( );
	if( !m_AlternativeAttackData.m_pWeaponInfo.IsValid( ) )
		return;

	m_AlternativeAttackData.m_pCmd = pCmd;
	if( m_AlternativeAttackData.m_pLocalPlayer->m_flNextAttack( ) > m_pGlobalVars->curtime || m_AlternativeAttackData.m_pLocalWeapon->m_flNextPrimaryAttack( ) > m_pGlobalVars->curtime )
		return;

	m_AlternativeAttackData.m_vecEyePos = m_AlternativeAttackData.m_pLocalPlayer->GetEyePosition( );
	for( int i = 1; i <= m_pGlobalVars->maxClients; i++ ) {
		C_CSPlayer* Target = ( C_CSPlayer* )m_pEntList->GetClientEntity( i );
		if( !Target
			|| Target->IsDead( )
			|| Target->IsDormant( )
			|| !Target->IsPlayer( )
			|| Target->m_iTeamNum( ) == m_AlternativeAttackData.m_pLocalPlayer->m_iTeamNum( )
			|| Target->m_bGunGameImmunity( ) )
			continue;

		auto lag_data = g_LagCompensation.GetLagData( Target->m_entIndex );
		if( !lag_data || lag_data->m_Records.empty( ) )
			continue;

		C_LagRecord* previousRecord = nullptr;
		C_LagRecord backup;
		backup.Setup( Target );
		for( auto& record : lag_data->m_Records ) {
			if( g_LagCompensation.IsRecordOutOfBounds( record, 0.2f ) || record.m_RecordFlags & ELagRecordFlags::RF_IsInvalid ) {
				continue;
			}

			if( !previousRecord
				|| previousRecord->m_vecOrigin != record.m_vecOrigin
				|| previousRecord->m_angEyeAngles.y != record.m_angEyeAngles.y
				|| previousRecord->m_vecMaxs != record.m_vecMaxs
				|| previousRecord->m_vecMins != record.m_vecMins ) {
				previousRecord = &record;

				record.Apply( Target );
				if( HandleKnifeBot( Target, sendPacket ) || HandleZeusBot( Target, sendPacket ) ) {
					m_AlternativeAttackData.m_pCmd->tick_count = TIME_TO_TICKS( record.m_flSimulationTime + g_LagCompensation.GetLerp( ) );
					break;
				}
			}
		}

		backup.Apply( Target );
	}
}

bool AlternativeAttack::HandleZeusBot( C_CSPlayer* pPlayer, bool* sendPacket ) {
	if( m_AlternativeAttackData.m_pLocalWeapon->m_iItemDefinitionIndex( ) != WEAPON_ZEUS || !g_Vars.misc.zeus_bot )
		return false;

	m_AlternativeAttackData.m_pCurrentTarget = pPlayer;

	//g_Vars.misc.zeus_bot_hitchance
	auto hdr = *( studiohdr_t** )( pPlayer->m_pStudioHdr( ) );
	if( hdr ) {
		auto hitboxSet = hdr->pHitboxSet( pPlayer->m_nHitboxSet( ) );
		if( hitboxSet ) {
			auto pStomach = hitboxSet->pHitbox( HITBOX_STOMACH );
			if( pStomach ) {
				auto vecHitboxPos = ( pStomach->bbmax + pStomach->bbmin ) * 0.5f;
				vecHitboxPos = vecHitboxPos.Transform( pPlayer->m_CachedBoneData( ).Base( )[ pStomach->bone ] );

				// run awall
				Autowall::C_FireBulletData fireData;

				Vector vecDirection = vecHitboxPos - m_AlternativeAttackData.m_vecEyePos;
				vecDirection.Normalize( );

				fireData.m_bPenetration = false;
				fireData.m_vecStart = m_AlternativeAttackData.m_vecEyePos;
				fireData.m_vecDirection = vecDirection;
				fireData.m_iHitgroup = Hitgroup_Generic;
				fireData.m_Player = m_AlternativeAttackData.m_pLocalPlayer;
				fireData.m_TargetPlayer = pPlayer;
				fireData.m_WeaponData = m_AlternativeAttackData.m_pWeaponInfo.Xor( );
				fireData.m_Weapon = m_AlternativeAttackData.m_pLocalWeapon;
				fireData.m_iPenetrationCount = 0;

				// note - alpha; 
				// ghetto, shit, but good enough for zeusbot;
				// have fun doing ragebot hitchance with this implementation
				const bool bIsAccurate = !( m_AlternativeAttackData.m_pLocalWeapon->GetInaccuracy( ) >= ( ( 100.0f - 80.f ) * 0.65f * 0.01125f ) );
				const float flDamage = Autowall::FireBullets( &fireData );
				if( flDamage >= 110.f && bIsAccurate ) {
					m_AlternativeAttackData.m_pCmd->viewangles = vecDirection.ToEulerAngles( );
					m_AlternativeAttackData.m_pCmd->buttons |= IN_ATTACK;

					*sendPacket = true;

					return true;
				}
			}
		}
	}

	return false;
}

int AlternativeAttack::GetMinimalHp( ) {
	if( m_pGlobalVars->curtime > ( m_AlternativeAttackData.m_pLocalWeapon->m_flNextPrimaryAttack( ) + 0.4f ) )
		return 34;

	return 21;
}

bool AlternativeAttack::HandleKnifeBot( C_CSPlayer* pPlayer, bool* sendPacket ) {
	if( !m_AlternativeAttackData.m_pLocalWeapon->GetCSWeaponData( ).IsValid( ) || !g_Vars.misc.knifebot )
		return false;

	if( m_AlternativeAttackData.m_pLocalWeapon->GetCSWeaponData( )->m_iWeaponType != WEAPONTYPE_KNIFE )
		return false;

	if( m_AlternativeAttackData.m_pLocalWeapon->m_iItemDefinitionIndex( ) == WEAPON_ZEUS )
		return false;

	m_AlternativeAttackData.m_pCurrentTarget = pPlayer;

	Vector vecOrigin = pPlayer->m_vecOrigin( );

	Vector vecOBBMins = pPlayer->m_Collision( )->m_vecMins;
	Vector vecOBBMaxs = pPlayer->m_Collision( )->m_vecMaxs;

	Vector vecMins = vecOBBMins + vecOrigin;
	Vector vecMaxs = vecOBBMaxs + vecOrigin;

	Vector vecEyePos = pPlayer->GetEyePosition( );

	Vector vecPos = Math::Clamp( vecMins, vecEyePos, vecMaxs );
	Vector vecDelta = vecPos - m_AlternativeAttackData.m_vecEyePos;
	vecDelta.Normalize( );

	bool bBehind = IsBehind( m_AlternativeAttackData.m_pCurrentTarget );

	int attackType = DeterminateHitType( 0, vecDelta );
	if( attackType ) {
		if( g_Vars.misc.knife_bot_type == 1 ) {
			if( attackType == 2 && m_AlternativeAttackData.m_pCurrentTarget->m_iHealth( ) <= 76 ) {
			first_attack:
				m_AlternativeAttackData.m_pCmd->viewangles = vecDelta.ToEulerAngles( );
				m_AlternativeAttackData.m_pCmd->buttons |= IN_ATTACK;

				*sendPacket = true;

				return true;
			}
		}
		else if( g_Vars.misc.knife_bot_type == 2 ) {
			if( m_AlternativeAttackData.m_pCurrentTarget->m_iHealth( ) > 46 ) {
				goto first_attack;
			}
		}
		else {
			int hp = attackType == 2 ? 76 : GetMinimalHp( );
			if( hp >= m_AlternativeAttackData.m_pCurrentTarget->m_iHealth( ) )
				goto first_attack;
		}
	}

	if( !DeterminateHitType( 1, vecDelta ) )
		return false;

	m_AlternativeAttackData.m_pCmd->viewangles = vecDelta.ToEulerAngles( );
	m_AlternativeAttackData.m_pCmd->buttons |= IN_ATTACK2;

	*sendPacket = true;

	return true;
}

int AlternativeAttack::DeterminateHitType( bool stabType, Vector eyeDelta ) {
	float minDistance = stabType ? 32.0f : 48.0f;

	Vector vecEyePos = m_AlternativeAttackData.m_vecEyePos;
	Vector vecEnd = vecEyePos + ( eyeDelta * minDistance );
	Vector vecOrigin = m_AlternativeAttackData.m_pCurrentTarget->m_vecOrigin( );

	CTraceFilter filter;
	filter.pSkip = m_AlternativeAttackData.m_pLocalPlayer;

	Ray_t ray;
	ray.Init( vecEyePos, vecEnd, Vector( -16.0f, -16.0f, -18.0f ), Vector( 16.0f, 16.0f, 18.0f ) );
	CGameTrace tr;
	m_pEngineTrace->TraceRay( ray, 0x200400B, &filter, &tr );

	if( !tr.hit_entity )
		return 0;

	if( m_AlternativeAttackData.m_pCurrentTarget ) {
		if( tr.hit_entity != m_AlternativeAttackData.m_pCurrentTarget )
			return 0;
	}
	else { // guess this only for trigger bot
		C_CSPlayer* ent = ToCSPlayer( tr.hit_entity->GetBaseEntity( ) );

		if( !ent || ent->IsDead( ) || ent->IsDormant( ) )
			return 0;

		if( ent->m_iTeamNum( ) == m_AlternativeAttackData.m_pLocalPlayer->m_iTeamNum( ) )
			return 0;
	}

	return !IsBehind( m_AlternativeAttackData.m_pCurrentTarget ) + 1;
}

bool AlternativeAttack::IsBehind( C_CSPlayer* pPlayer ) {
	Vector delta{ pPlayer->m_vecOrigin( ) - m_AlternativeAttackData.m_pLocalPlayer->GetEyePosition( ) };
	delta.z = 0.f;
	delta.Normalize( );

	Vector target;
	Math::AngleVectors( pPlayer->GetAbsAngles( ), target );
	target.z = 0.f;

	return delta.Dot( target ) > 0.475f;
}