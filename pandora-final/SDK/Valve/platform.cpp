#include "platform.hpp"
#include "../source.hpp"

void* MemAlloc_Alloc( int size ) {
  return m_pMemAlloc->Alloc( size );
}

void *MemAlloc_Realloc( void *pMemBlock, int size ) {
  return m_pMemAlloc->Realloc( pMemBlock, size );
}

void MemAlloc_Free( void* pMemBlock ) {
  m_pMemAlloc->Free( pMemBlock );
}
