#pragma once

class CClientState
{
public:
  int& m_nDeltaTick( );
  int& m_nLastOutgoingCommand( );
  int& m_nChokedCommands( );
  int& m_nLastCommandAck( );
  bool& m_bIsHLTV( );
  int& m_nMaxClients( );
};
