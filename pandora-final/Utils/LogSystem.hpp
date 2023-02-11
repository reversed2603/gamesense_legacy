#pragma once
#include "../SDK/sdk.hpp"

class ILogSystem {
public:
   void Log( const char* file_fmt, const char* fmt, ... );
   void Clear( const char* file_fmt );

   ILogSystem( ) {

   }
   ~ILogSystem( ) {

   }
private:
	std::string GetTimestamp( );
};

extern ILogSystem g_Log;