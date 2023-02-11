#pragma once

#include "../../../SDK/Classes/Player.hpp"
#include "../../../SDK/Classes/PropManager.hpp"
#include "../../../source.hpp"

namespace Wrappers::Entity {

	class NetVarType {
	public:
		NetVarType( const char* table, const char* prop, C_CSPlayer* player ) {
			if( !C_CSPlayer::GetLocalPlayer( ) ) {
				m_player = nullptr;
			}
			else {
				m_player = player;
			}

			m_table = table;
			m_prop = prop;

			if( m_table && m_prop ) {
				auto offset = Engine::g_PropManager.GetOffset( m_table, m_prop );
				if( !offset )
					offset = -1;

				m_offset = offset;
			}
		}

		int get_offset( ) {
			return m_offset;
		}

		int get_int( ) {
			if( !m_player || !m_table || !m_prop )
				return -1;

			auto offset = get_offset( );

			if( !offset )
				return -1;

			return *reinterpret_cast< int* >( uintptr_t( m_player ) + offset );
		}

		void set_int( int input ) {
			if( !m_player || !m_table || !m_prop )
				return;

			auto offset = get_offset( );

			if( !offset )
				return;

			*reinterpret_cast< int* >( uintptr_t( m_player ) + offset ) = input;
		}

		bool get_bool( ) {
			if( !m_player || !m_table || !m_prop )
				return false;

			auto offset = get_offset( );

			if( !offset )
				return false;

			return *reinterpret_cast< bool* >( uintptr_t( m_player ) + offset );
		}

		void set_bool( bool input ) {
			if( !m_player || !m_table || !m_prop )
				return;

			auto offset = get_offset( );

			if( !offset )
				return;

			*reinterpret_cast< bool* >( uintptr_t( m_player ) + offset ) = input;
		}

		float get_float( ) {
			if( !m_player || !m_table || !m_prop )
				return false;

			auto offset = get_offset( );

			if( !offset )
				return false;

			return *reinterpret_cast< float* >( uintptr_t( m_player ) + offset );

		}

		void set_float( float input ) {
			if( !m_player || !m_table || !m_prop )
				return;

			auto offset = get_offset( );

			if( !offset )
				return;

			*reinterpret_cast< float* >( uintptr_t( m_player ) + offset ) = input;
		}

		const char* get_string( ) {
			if( !m_player || !m_table || !m_prop )
				return XorStr( "#error" );

			auto offset = get_offset( );

			if( !offset )
				return XorStr( "#error" );

			return reinterpret_cast< const char* >( uintptr_t( m_player ) + offset );
		}

		Vector get_vector( ) {
			if( !m_player || !m_table || !m_prop )
				return Vector( );

			auto offset = get_offset( );

			if( !offset )
				return Vector( );

			return *reinterpret_cast< Vector* >( uintptr_t( m_player ) + offset );
		}

		void set_vector( Vector input ) {
			if( !m_player || !m_table || !m_prop )
				return;

			auto offset = get_offset( );

			if( !offset )
				return;

			*reinterpret_cast< Vector* >( uintptr_t( m_player ) + offset ) = input;
		}

		Vector2D get_vector2d( ) {
			if( !m_player || !m_table || !m_prop )
				return Vector2D( );

			auto offset = get_offset( );

			if( !offset )
				return Vector2D( );

			return *reinterpret_cast< Vector2D* >( uintptr_t( m_player ) + offset );
		}

		void set_vector2d( Vector2D input ) {
			if( !m_player || !m_table || !m_prop )
				return;

			auto offset = get_offset( );

			if( !offset )
				return;

			*reinterpret_cast< Vector2D* >( uintptr_t( m_player ) + offset ) = input;
		}

		CBaseHandle get_handle( ) {
			if( !m_player || !m_table || !m_prop )
				return NULL;

			auto offset = get_offset( );

			if( !offset )
				return NULL;

			return *reinterpret_cast< CBaseHandle* >( uintptr_t( m_player ) + offset );
		}

	private:
		C_CSPlayer* m_player;
		const char* m_table;
		const char* m_prop;
		int m_offset;
	};

	class CEntity {
	public:
		// so, we need this in order to be able to use the entity inside this class..
		// the entity will be constructed upon calling GetClientEntity from entitylist
		CEntity( C_CSPlayer* player ) {
			m_player = player;
		}

		// just for testing.
		int index( ) {
			if( !this || !C_CSPlayer::GetLocalPlayer( ) || !m_player )
				return -1;

			return m_player->m_entIndex;
		}

		bool dormant( ) {
			if( !this || !C_CSPlayer::GetLocalPlayer( ) || !m_player )
				return true;

			return m_player->IsDormant( );
		}

		Vector origin( ) {
			if( !this || !C_CSPlayer::GetLocalPlayer( ) || !m_player )
				return { };

			return m_player->GetAbsOrigin( );
		}

		// I tried to handle this with one function, dynamic
		// but yeah, fuck that lol
		NetVarType GetVar( const char* table, const char* prop ) {
			return NetVarType( table, prop, m_player );
		}

		int GetClassID( ) {
			if( !C_CSPlayer::GetLocalPlayer( ) || !m_player )
				return -1;

			auto clientClass = m_player->GetClientClass( );
			if( !clientClass )
				return -1;

			return clientClass->m_ClassID;
		}

		//CEntity GetLocalPlayer( ) {
		//	return CEntity( C_CSPlayer::GetLocalPlayer( ) );
		//}

		operator C_CSPlayer* ( ) {
			return m_player;
		}

	private:
		C_CSPlayer* m_player;
	};



}