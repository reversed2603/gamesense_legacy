#include "../sdk.hpp"

void IGameMovement::ProcessMovement( C_BasePlayer* pPlayer, CMoveData* pMove ) {
  using Fn = void( __thiscall* )( void*, C_BasePlayer*, CMoveData* );
  return Memory::VCall<Fn>( this, Index::IGameMovement::ProcessMovement )( this, pPlayer, pMove );
}

void IGameMovement::Reset( ) {
  using Fn = void( __thiscall* )( void* );
  return Memory::VCall<Fn>( this, Index::IGameMovement::Reset )( this );
}

void IGameMovement::StartTrackPredictionErrors( C_BasePlayer* pPlayer ) {
  using Fn = void( __thiscall* )( void*, C_BasePlayer* );
  return Memory::VCall<Fn>( this, Index::IGameMovement::StartTrackPredictionErrors )( this, pPlayer );
}

void IGameMovement::FinishTrackPredictionErrors( C_BasePlayer* pPlayer ) {
  using Fn = void( __thiscall* )( void*, C_BasePlayer* );
  return Memory::VCall<Fn>( this, Index::IGameMovement::FinishTrackPredictionErrors )( this, pPlayer );
}

Vector IGameMovement::GetPlayerMins( bool bDucked ) {
  return Memory::VCall< const Vector&( __thiscall* )( void*, bool ) >( this, Index::IGameMovement::GetPlayerMins )( this, bDucked );
}

Vector IGameMovement::GetPlayerMaxs( bool bDucked ) {
  return Memory::VCall< const Vector&( __thiscall* )( void*, bool ) >( this, Index::IGameMovement::GetPlayerMaxs )( this, bDucked );
}

Vector IGameMovement::GetPlayerViewOffset( bool bDucked ) {
  return Memory::VCall< const Vector&( __thiscall* )( void*, bool ) >( this, Index::IGameMovement::GetPlayerViewOffset )( this, bDucked );
}
