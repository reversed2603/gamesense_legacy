#pragma once

class ICenterPrint {
public:
  virtual void* Create( unsigned long long unknown ) = 0;
  virtual int   Destroy( ) = 0;
  virtual int	 SetTextColor( int, int, int, int ) = 0;
  virtual void  Print( char16_t* ) = 0;
  virtual int	 PrintW( wchar_t* ) = 0;
  virtual void  ColorPrint( int, int, int, int, char16_t * ) = 0;
  virtual int	 ColorPrintW( int, int, int, int, wchar_t* ) = 0;
  virtual int	 Clear( ) = 0;
};