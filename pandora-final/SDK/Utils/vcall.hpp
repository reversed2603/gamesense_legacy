#pragma once

namespace SDK::Memory
{
  template<typename T>
  __forceinline T VCall( const void* instance, const unsigned int index ) {
	 return ( T )( ( *( void*** )instance )[index] );
  }
}
