#include "IMemAlloc.hpp"
#include "../sdk.hpp"

auto IMemAlloc::Alloc( int nSize ) -> void * {
  using Fn = void*( __thiscall* )( void*, int );
  return Memory::VCall< Fn >( this, 1 )( this, nSize );
}

auto IMemAlloc::Realloc( void * pMem, int nSize ) -> void * {
  using Fn = void*( __thiscall* )( void*, void*, int );
  return Memory::VCall< Fn >( this, 3 )( this, pMem, nSize );
}

auto IMemAlloc::Free( void * pMem ) -> void {
  using Fn = void( __thiscall* )( void*, void* );
  return Memory::VCall< Fn >( this, 5 )( this, pMem );
}
