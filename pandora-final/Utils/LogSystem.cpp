#include "LogSystem.hpp"
#include <fstream>
#include <iomanip>
#include <iostream>

ILogSystem g_Log;

void ILogSystem::Log( const char* file_fmt, const char* fmt, ... ) {
   if ( !fmt ) return;

   va_list va_alist;
   char logBuf[ 1024 ] = { 0 };

   va_start( va_alist, fmt );
   _vsnprintf( logBuf + strlen( logBuf ), sizeof( logBuf ) - strlen( logBuf ), fmt, va_alist );
   va_end( va_alist );

   std::ofstream file;

   file.open( std::string( XorStr( "log" ) ).append( file_fmt ), std::ios::app );

   file << GetTimestamp( ) << " " << logBuf << std::endl;
    
   file.close( );
}

void ILogSystem::Clear( const char* file_fmt )
{
    std::fstream file;

    file.open( std::string( XorStr( "log" ) ).append( file_fmt ), std::ios::out | std::ios::trunc );

    file << XorStr( "" );
    file.close( );
}

std::string ILogSystem::GetTimestamp( ) {
   std::time_t t = std::time( nullptr );
   std::tm tm;
   localtime_s( &tm, &t );
   std::locale loc( std::cout.getloc( ) );

   std::basic_stringstream<char> ss;
   ss.imbue( loc );
   ss << std::put_time( &tm, XorStr( "[%A %b %e] [%H:%M]" ) );

   return ss.str( );
}
