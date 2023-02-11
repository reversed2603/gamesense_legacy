#include "../../source.hpp"
#include "../Hooked.hpp"
#include "../../SDK/Displacement.hpp"
#include "../../SDK/Classes/Player.hpp"

bool __fastcall Hooked::IsHltv( IVEngineClient* EngineClient, uint32_t ) {
	static const auto return_to_accumulate_layers = Memory::Scan( XorStr( "client.dll" ), "84 C0 75 0D F6 87" );
	static const auto return_to_setup_velocity = Memory::Scan( XorStr( "client.dll" ), "84 C0 75 38 8B 0D ? ? ? ? 8B 01 8B 80" );

	if( !m_pEngine->IsInGame( ) )
		return oIsHltv( EngineClient );

	if( _ReturnAddress( ) == ( void* )return_to_setup_velocity || _ReturnAddress( ) == ( void* )return_to_accumulate_layers )
		return true;

	return oIsHltv( EngineClient );
}