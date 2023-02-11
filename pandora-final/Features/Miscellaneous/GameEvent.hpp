#pragma once
#include "../../SDK/sdk.hpp"

class GameEvent : public IGameEventListener
{
public:
  void Register( );
  void Shutdown( );

public: // IGameEventListener
	virtual void FireGameEvent( IGameEvent* event );
	virtual int  GetEventDebugID( void );
};

extern GameEvent g_GameEvent;
