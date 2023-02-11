#pragma once
#include "../sdk.hpp"
#include "PropManager.hpp"

class CSPlayerResource
{
public:
  int GetPlayerPing(int idx);
  int GetPlayerAssists(int idx);
  int GetPlayerKills(int idx);
  int GetPlayerDeaths( int idx );
  Vector& m_bombsiteCenterA( );
  Vector& m_bombsiteCenterB( );
};