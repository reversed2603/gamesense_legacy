#pragma once
#include <cstdint>
#include <windows.h>

#ifndef PDR_EXPORTS
#define PDR_EXPORTS

struct netvar_t {
	uint32_t hash = 0; // for reading it in the cheat

	uint32_t offset = 0;
};

struct netvars_t {
	int count = 0;
	int magic = 0;
	netvar_t netvars[ 20000 ];
};

extern __declspec( dllexport ) netvars_t g_netvars;

inline uint32_t netvar_get_offset( uint32_t hash, const char* name = nullptr, int name2 = 0 ) {
	for( int i = 0; i < g_netvars.count; i++ ) {
		if( g_netvars.netvars[ i ].hash == hash + g_netvars.magic ) {
			return g_netvars.netvars[ i ].offset;
		}
	}
}

struct interface_t {
	uint32_t hash = 0;
	uint32_t address = 0;
};

struct interfaces_t {
	int count = 0;
	interface_t ifaces[ 100 ];
};

extern __declspec( dllexport ) interfaces_t g_interfaces;

inline uint32_t interfaces_get_interface( uint32_t hash ) {
	for( int i = 0; i < g_interfaces.count; i++ ) {
		auto iface = g_interfaces.ifaces[ i ];

		if( iface.hash == hash )
			return iface.address;
	}

	return 0;
}

struct pattern_t {
	char module[ 64 ];
	char pattern[ 128 ];
	uint32_t hash;
	uint32_t address;
};

struct patterns_t {
	int count = 100;
	pattern_t patterns[ 250 ];
};

extern __declspec( dllexport ) patterns_t g_patterns;

inline uint32_t patterns_get_pattern( uint32_t hash ) {
	for( int i = 0; i < g_patterns.count; i++ ) {
		if( g_patterns.patterns[ i ].hash == hash )
			return g_patterns.patterns[ i ].address;
	}

	return 0;
}

__forceinline uint32_t loader_hash( const char* p ) {
	size_t s = strlen( p ); // users dont have sse4 processors etc
	size_t result = 0;
	const size_t prime = 31;
	for( size_t i = 0; i < s; ++i )
		result = p[ i ] + ( result * prime );

	return result;
}

extern __declspec( dllexport ) uint32_t g_text_start;
extern __declspec( dllexport ) uint32_t g_text_size;
extern __declspec( dllexport ) uint32_t g_text_hash;

extern bool g_is_cracked;

#endif