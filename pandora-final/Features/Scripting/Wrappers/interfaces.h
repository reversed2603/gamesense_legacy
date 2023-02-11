#pragma once

#include "entity.h"

namespace Wrappers::Interfaces {
	namespace EntityList {
		Wrappers::Entity::CEntity GetClientEntity( int entnum ) {
			// return and construct the entity class.
			return Wrappers::Entity::CEntity( reinterpret_cast<C_CSPlayer*>( ::m_pEntList->GetClientEntity( entnum ) ) );
		}

		Wrappers::Entity::CEntity GetClientEntityFromHandle( Wrappers::Entity::NetVarType netvar ) {
			return Wrappers::Entity::CEntity( reinterpret_cast< C_CSPlayer* >( ::m_pEntList->GetClientEntityFromHandle( netvar.get_handle( ) ) ) );
		}

		int GetHighestEntityIndex( ) {
			return ::m_pEntList->GetHighestEntityIndex( );
		}

		std::vector<int> GetAll( const char* name ) {
			std::vector<int> temp;

			for( int i = 1; i <= ::m_pEntList->GetHighestEntityIndex( ); ++i ) {
				auto player = reinterpret_cast<C_CSPlayer*>( ::m_pEntList->GetClientEntity( i ) );
				if( !player )
					continue;

				if( player->is( name ) ) {
					temp.push_back( player->m_entIndex );
				}
			}

			return temp;
		}
	}

	namespace ConVar {
		::ConVar* FindVar( const char* var_name ) {
			return ::m_pCvar->FindVar( var_name );
		}
	}

	namespace GlobalVars {
		float curtime( ) {
			return ::m_pGlobalVars->curtime;
		}

		float realtime( ) {
			return ::m_pGlobalVars->realtime;
		}

		float frametime( ) {
			return ::m_pGlobalVars->frametime;
		}

		int framecount( ) {
			return ::m_pGlobalVars->framecount;
		}

		int maxClients( ) {
			return ::m_pGlobalVars->maxClients;
		}

		int interval_per_tick( ) {
			return ::m_pGlobalVars->interval_per_tick;
		}

		int choked_commands( ) {
			if( !::m_pClientState.IsValid( ) )
				return -1;

			return ::m_pClientState->m_nChokedCommands( );
		}

		
	}

	namespace Engine {
		int GetPlayerForUserID( int userID ) {
			return ::m_pEngine->GetPlayerForUserID( userID );
		}

		bool IsInGame( ) {
			return ::m_pEngine->IsInGame( );
		}

		int GetLocalPlayer( ) {
			return ::m_pEngine->GetLocalPlayer( );
		}

		player_info_t GetPlayerInfo( int ent_num ) {
			player_info_t xd;
			::m_pEngine->GetPlayerInfo( ent_num, &xd );
			return xd;
		}

		QAngle GetViewAngles( ) {
			QAngle va;
			::m_pEngine->GetViewAngles( va );

			return va;
		}

		void SetViewAngles( QAngle va ) {
			::m_pEngine->SetViewAngles( va );
		}

		void ExecuteClientCmd( const char* szCmdString ) {
			::m_pEngine->ClientCmd( szCmdString );
		}

		bool IsConnected( ) {
			return ::m_pEngine->IsConnected( );
		}
	}
}