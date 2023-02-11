#pragma once

class IMemAlloc {
public:
  auto Alloc( int nSize ) -> void*;

  auto Realloc( void* pMem, int nSize ) -> void*;

  auto Free( void* pMem ) -> void;
};