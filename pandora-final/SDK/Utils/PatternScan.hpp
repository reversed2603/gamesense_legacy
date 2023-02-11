#pragma once

#include "auto.hpp"
#include "../Classes/DataMap.hpp"

#include <functional>
#include <thread>
#include <chrono>

#define SDK_concat(x, y) x##y
#define SDK_concatiate(x, y) SDK_concat(x, y)
#define SDK_pad(size)                                         \
private:                                                          \
  std::uint8_t SDK_concatiate(__pad, __COUNTER__)[size] = {}; \
                                                                  \
public:

namespace SDK::Memory
{
	char* find_utf8( const char* module, const char* string );
	void strcpy_protected( char* dest, const char* src );
	std::uintptr_t Scan( const std::string& image_name, const std::string& signature );
	std::uintptr_t Scan( const std::uintptr_t image, const std::string& signature );
	uintptr_t* GetVirtualTablePointer( const char* ModuleName, const char* TableName );
	unsigned int FindInDataMap( datamap_t* pMap, const char* name );
	// we have like 5 lambdas for this in random places
	DWORD CallableFromRelative( DWORD nAddress );

	inline void ThreadSleep( DWORD msecs ) {
		std::this_thread::sleep_for( std::chrono::milliseconds( msecs ) );
	}

	template <typename T>
	inline bool ResolveSymbol( T* out, const std::function<T( )>& fn )
	{
		unsigned int i = 0;
		while( true )
		{
			*out = fn( );
			if( *out )
			{
				break;
			}

			i++;

			//10 seconds
			if( i > 200 )
			{
				return false;
			}

			ThreadSleep( 50 );
		}

		return true;
	}
}
