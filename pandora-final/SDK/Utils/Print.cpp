#include "print.hpp"
#include "auto.hpp"

namespace SDK::Win32
{

#ifdef DEV
   const char* m_pPrintTypes[] =
   {
	   "[Trace]",
	   "[Warning]",
	   "[Error]",
   };

   const wchar_t* m_pWidePrintTypes[] =
   {
	   L"[Trace]",
	   L"[Warning]",
	   L"[Error]",
   };
#endif

   void DebugPrint( PrintType print_type, const char* message ) {
   #ifdef DEV
	  char output[ 4096 ] = { };

	  auto type = m_pPrintTypes[ static_cast< int >( print_type ) ];
	  sprintf_s( output, "%s %s\r\n", type, message );

	  printf( output );
   #endif
}

   void DebugPrint( PrintType print_type, const wchar_t* message ) {
   #ifdef DEV
	  wchar_t output[ 4096 ] = { };

	  auto type = m_pWidePrintTypes[ static_cast< int >( print_type ) ];
	  wsprintfW( output, L"%s %s\r\n", type, message );

	  OutputDebugStringW( output );
   #endif
   }

   void Trace( const char* format, ... ) {
   #ifdef DEV
	  char message[ 2048 ] = { };

	  va_list args;
	  va_start( args, format );
	  int length = vsprintf_s( message, format, args );
	  va_end( args );

	  DebugPrint( PrintType::Trace, message );
   #endif
   }

   void Trace( const wchar_t* format, ... ) {
   #ifdef DEV
	  wchar_t message[ 2048 ] = { };

	  va_list args;
	  va_start( args, format );
	  int length = wvsprintfW( message, format, args );
	  va_end( args );

	  DebugPrint( PrintType::Trace, message );
   #endif
   }

   void Warning( const char* format, ... ) {
   #ifdef DEV
	  char message[ 2048 ] = { };

	  va_list args;
	  va_start( args, format );
	  int length = vsprintf_s( message, format, args );
	  va_end( args );

	  DebugPrint( PrintType::Warning, message );
   #endif
   }

   void Warning( const wchar_t* format, ... ) {
   #ifdef DEV
	  wchar_t message[ 2048 ] = { };

	  va_list args;
	  va_start( args, format );
	  int length = wvsprintfW( message, format, args );
	  va_end( args );

	  DebugPrint( PrintType::Warning, message );
   #endif
}

   void Error( const char* format, ... ) {
   #ifdef DEV
	  char message[ 2048 ] = { };

	  va_list args;
	  va_start( args, format );
	  int length = vsprintf_s( message, format, args );
	  va_end( args );

	  DebugPrint( PrintType::Error, message );
   #endif // !1
   }

   void Error( const wchar_t* format, ... ) {
   #ifdef DEV
	  wchar_t message[ 2048 ] = { };

	  va_list args;
	  va_start( args, format );
	  int length = wvsprintfW( message, format, args );
	  va_end( args );

	  DebugPrint( PrintType::Error, message );
   #endif // !1
   }

   }
