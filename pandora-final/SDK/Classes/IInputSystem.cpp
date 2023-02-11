#include "IInputSystem.hpp"
#include "../sdk.hpp"

// ayysense kek
void IInputSystem::EnableInput( bool state ) {
	using original_fn = void( __thiscall* )( IInputSystem*, bool );
	return ( *( original_fn** )this )[ 11 ]( this, state );
}