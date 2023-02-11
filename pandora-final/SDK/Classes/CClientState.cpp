#include "CClientState.hpp"
#include "../Displacement.hpp"

int& CClientState::m_nDeltaTick( ) {
   return *( int* ) ( ( uintptr_t ) this + Engine::Displacement.CClientState.m_nDeltaTick );
}

int& CClientState::m_nLastOutgoingCommand( ) {
   return *( int* ) ( ( uintptr_t ) this + Engine::Displacement.CClientState.m_nLastOutgoingCommand );
}

int& CClientState::m_nChokedCommands( ) {
   return *( int* ) ( ( uintptr_t ) this + Engine::Displacement.CClientState.m_nChokedCommands );
}

int& CClientState::m_nLastCommandAck( ) {
   return *( int* ) ( ( uintptr_t ) this + Engine::Displacement.CClientState.m_nLastCommandAck );
}

bool& CClientState::m_bIsHLTV( ) {
   return *( bool* ) ( ( uintptr_t ) this + Engine::Displacement.CClientState.m_bIsHLTV );
}

int& CClientState::m_nMaxClients( )
{
    return *( int* )( ( uintptr_t )this + Engine::Displacement.CClientState.m_nMaxClients );
}
