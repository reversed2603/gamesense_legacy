#pragma once
#include "../SDK/sdk.hpp"

class ICrashHandler {
public:
  long __stdcall OnProgramCrash( struct _EXCEPTION_POINTERS* );
   
  ICrashHandler( ) {

  }
  ~ICrashHandler( ) {

  }
};

extern ICrashHandler g_CrashHandler;