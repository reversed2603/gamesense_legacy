#pragma once

namespace SDK::Win32
{

  enum class PrintType
  {
	 Trace = 0,
	 Warning,
	 Error,
  };

  void DebugPrint( PrintType print_type, const char* message );
  void DebugPrint( PrintType print_type, const wchar_t* message );

  void Trace( const char* format, ... );
  void Trace( const wchar_t* format, ... );
  void Warning( const char* format, ... );
  void Warning( const wchar_t* format, ... );
  void Error( const char* format, ... );
  void Error( const wchar_t* format, ... );

}
