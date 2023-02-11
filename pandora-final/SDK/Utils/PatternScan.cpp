#include "PatternScan.hpp"
#include "Print.hpp"
#include "../../Loader/Exports.h"
#include <sstream>      // std::stringstream
#include <iomanip>      // std::stringstream
#include "../../Utils/extern/XorStr.hpp"

#define IS_IN_RANGE( value, max, min ) ( value >= max && value <= min )
#define GET_BITS( value ) ( IS_IN_RANGE( value, '0', '9' ) ? ( value - '0' ) : ( ( value & ( ~0x20 ) ) - 'A' + 0xA ) )
#define GET_BYTE( value ) ( GET_BITS( value[0] ) << 4 | GET_BITS( value[1] ) )

namespace SDK::Memory
{

	char* find_utf8( const char* module, const char* string ) {
		uint32_t module_start = ( uint32_t )GetModuleHandleA( module );
		const auto dos_headers = ( IMAGE_DOS_HEADER* )module_start;
		const auto nt_headers = ( IMAGE_NT_HEADERS* )( module_start + dos_headers->e_lfanew );

		uint32_t module_end = module_start + nt_headers->OptionalHeader.SizeOfImage;

		for( uint32_t current = module_start; current < module_end; current++ ) {
			if( strcmp( ( const char* )current, string ) == 0 )
				return ( char* )current;
		}

		return nullptr;
	}

	void strcpy_protected( char* dest, const char* src ) {
		unsigned long old = 0;
		int len = strlen( src );

		VirtualProtect( ( void* )dest, len, PAGE_READWRITE, &old );
		strcpy( dest, src );
		VirtualProtect( ( void* )dest, len, old, &old );
	}

	__forceinline uintptr_t PatternScan( uintptr_t mod, const char* sig, size_t size )
	{
		// Credits: MarkHC, although slightly modified by DucaRii and also documented
		static auto pattern_to_byte = [ ] ( const char* pattern )
		{
			// Prerequisites
			auto bytes = std::vector<int>{ };
			auto start = const_cast< char* >( pattern );
			auto end = const_cast< char* >( pattern ) + strlen( pattern );

			// Convert signature into corresponding bytes
			for( auto current = start; current < end; ++current )
			{
				// Is current byte a wildcard? Simply ignore that that byte later
				if( *current == '?' )
				{
					++current;

					// Check if following byte is also a wildcard
					if( *current == '?' )
						++current;

					// Dummy byte
					bytes.push_back( -1 );
				}
				else
				{
					// Convert character to byte on hexadecimal base
					bytes.push_back( strtoul( current, &current, 16 ) );
				}
			}
			return bytes;
		};


		// The region where we will search for the byte sequence
		const auto image_size = size;

		// Check if the image is faulty
		if( !image_size )
			return { };

		// Convert IDA-Style signature to a byte sequence
		auto pattern_bytes = pattern_to_byte( sig );

		const auto image_bytes = reinterpret_cast< byte* >( mod );

		const auto signature_size = pattern_bytes.size( );
		const auto signature_bytes = pattern_bytes.data( );

		// Now loop through all bytes and check if the byte sequence matches
		for( auto i = 0ul; i < image_size - signature_size; ++i )
		{
			auto byte_sequence_found = true;

			// Go through all bytes from the signature and check if it matches
			for( auto j = 0ul; j < signature_size; ++j )
			{
				if( image_bytes[ i + j ] != signature_bytes[ j ] // Bytes don't match
					&& signature_bytes[ j ] != -1 ) // Byte isn't a wildcard either, WHAT THE HECK
				{
					byte_sequence_found = false;
					break;
				}
			}

			// All good, now return the right address
			if( byte_sequence_found )
				return uintptr_t( &image_bytes[ i ] );
		}

		// Byte sequence wasn't found
		return { };
	}


	/*Credits: @hinnie on unknowncheats: https://www.unknowncheats.me/forum/counterstrike-global-offensive/375564-rtti-helper-easy-vtable-pointers.html */
	__forceinline std::string BytesToIDAPattern( byte* bytes, size_t size ) {
		std::stringstream ida_pattern;
		ida_pattern << std::hex << std::setfill( '0' );
		for( size_t i = 0; i < size; i++ )
		{
			const int32_t current_byte = bytes[ i ];
			if( current_byte != 255 )
				ida_pattern << std::setw( 2 ) << current_byte;
			else
				ida_pattern << std::setw( 1 ) << XorStr( "?" );

			if( i != size - 1 )
				ida_pattern << XorStr( " " );
		}

		return ida_pattern.str( );
	}

	/*Credits: @hinnie on unknowncheats: https://www.unknowncheats.me/forum/counterstrike-global-offensive/375564-rtti-helper-easy-vtable-pointers.html */
	__forceinline std::vector<uintptr_t> GetXRefsTo( uintptr_t address, uintptr_t start, uintptr_t size ) {
		std::vector<uintptr_t> xrefs = { };

		// Convert the address over to an IDA pattern string
		const std::string ida_pattern = BytesToIDAPattern( ( byte* )&address, 4 );

		// Get the end of the section (in our case the end of the .rdata section)
		const uintptr_t end = start + size;
		while( start && start < end )
		{
			uintptr_t xref = ( uintptr_t )PatternScan( start, ida_pattern.c_str( ), size );

			// If the xref is 0 it means that there either were no xrefs, or there are no remaining xrefs.
			// So we should break out of our loop, otherwise it will keep on trying to look for xrefs.
			if( !xref )
				break;

			// We've found an xref, save it in the vector, and add 4 to start, so it wil now search for xrefs
			// from the previously found xref untill we're at the end of the section, or there aren't any xrefs left.
			xrefs.push_back( xref );
			start = xref + 4;
		}

		return xrefs;
	}

	/*Credits: @hinnie on unknowncheats: https://www.unknowncheats.me/forum/counterstrike-global-offensive/375564-rtti-helper-easy-vtable-pointers.html */
	__forceinline bool GetSectionInfo( uintptr_t base_address, const char* section_name, uintptr_t& section_start, uintptr_t& section_size ) {
		const PIMAGE_DOS_HEADER dos_header = ( PIMAGE_DOS_HEADER )base_address;
		if( dos_header->e_magic != IMAGE_DOS_SIGNATURE )
			return false;

		const PIMAGE_NT_HEADERS32 nt_headers = ( PIMAGE_NT_HEADERS32 )( base_address + dos_header->e_lfanew );
		if( nt_headers->Signature != IMAGE_NT_SIGNATURE )
			return false;

		PIMAGE_SECTION_HEADER section_header = IMAGE_FIRST_SECTION( nt_headers );
		uint16_t number_of_sections = nt_headers->FileHeader.NumberOfSections;
		while( number_of_sections > 0 )
		{
			// If we're at the right section
			if( !strcmp( section_name, ( const char* )section_header->Name ) )
			{
				section_start = base_address + section_header->VirtualAddress;
				section_size = section_header->SizeOfRawData;
				return true;
			}

			section_header++;
			number_of_sections--;
		}

		return false;
	}

	/*Credits: @hinnie on unknowncheats: https://www.unknowncheats.me/forum/counterstrike-global-offensive/375564-rtti-helper-easy-vtable-pointers.html */
	uintptr_t* GetVirtualTablePointer( const char* ModuleName, const char* TableName ) {
		uintptr_t BaseAddress = reinterpret_cast< uintptr_t >( GetModuleHandleA( ModuleName ) );
		if( !BaseAddress )
			return nullptr;

		// Type descriptor names look like this: .?AVC_CSPlayer@@ (so: ".?AV" + TableName + "@@")
		const std::string type_descriptor_name = XorStr( ".?AV" ) + std::string( TableName ) + XorStr( "@@" );

		// Convert the string to an IDA pattern so that we can pattern scan it
		std::string IDAPattern = BytesToIDAPattern( ( byte* )type_descriptor_name.data( ), type_descriptor_name.size( ) );

		auto module = ( uintptr_t )GetModuleHandleA( ModuleName );

		uintptr_t rtti_type_descriptor = Scan( module, IDAPattern.data( ) );
		if( !rtti_type_descriptor )
			return nullptr;

		// We're doing - 0x8 here, because the location of the rtti_type_descriptor is 0x8 bytes before the string (open up client_panorama.dll in IDA and take a look) 
		rtti_type_descriptor -= 0x8;

		// We only need to get xrefs that are inside the .rdata section (there sometimes are xrefs in .text, so we have to filter them out)
		uintptr_t rdata_start = 0, rdata_size = 0;
		if( !GetSectionInfo( BaseAddress, XorStr( ".rdata" ), rdata_start, rdata_size ) )
			return nullptr;

		// Get all xrefs to the type_descriptor
		const std::vector<uintptr_t> xrefs = GetXRefsTo( rtti_type_descriptor, rdata_start, rdata_size );
		for( const uintptr_t xref : xrefs )
		{
			// xref - 0x8 = offset of this vtable in complete class (from top)
			// So if it's 0 it means it's the class we need, and not some class it inherits from (again, opening up client_panorama.dll in IDA will help you understand)
			const int32_t offset_from_class = *( int32_t* )( xref - 0x8 );
			if( offset_from_class != 0 )
				continue;

			// We've found the correct xref, the object locator is 0xC bytes before the xref. (Again, client_panorama.dll yada yada yada)
			const uintptr_t object_locator = xref - 0xC;

			// Now we need to get an xref to the object locator, as that's where the vtable is located
			{
				// Convert the object locator address to an IDA pattern
				IDAPattern = BytesToIDAPattern( ( byte* )&object_locator, 4 );

				const uintptr_t vtable_address = ( uintptr_t )PatternScan( rdata_start, IDAPattern.c_str( ), rdata_size ) + 0x4;

				// Here I'm checking for <= 4 as we're adding 0x4 to it. So if the pattern scan returns 0 we still head the fuck out
				if( vtable_address <= 4 )
					return nullptr;

				// We've now found the vtable address, however, we probably want a pointer to the vtable (which is in .text).
				// To do this, we're going to find a reference to the vtable address, and use that as pointer.

				// If you don't need a pointer to the vtable in your implementation however, just return vtable_address
				uintptr_t text_start = 0, text_size = 0;
				if( !GetSectionInfo( BaseAddress, XorStr( ".text" ), text_start, text_size ) )
					return nullptr;

				// Convert the vtable address to an IDA pattern
				IDAPattern = BytesToIDAPattern( ( byte* )&vtable_address, 4 );
				return reinterpret_cast< uintptr_t* >( PatternScan( text_start, IDAPattern.c_str( ), text_size ) );
			}
		}

		// We for some odd reason didn't find any valid xrefs
		return nullptr;
	}

	std::uintptr_t Scan( const std::string& image_name, const std::string& signature ) {
		auto image = GetModuleHandleA( image_name.c_str( ) );
		return Scan( ( std::uintptr_t )image, signature );
	}

	std::uintptr_t Scan( const std::uintptr_t image, const std::string& signature ) {
#ifndef DEV
		auto address = patterns_get_pattern( loader_hash( signature.data( ) ) );
		if( !address ) {
			MessageBoxA( 0, signature.data( ), std::to_string( image ).data( ), MB_OK );
			return 0l;
		}

		return address;
#else
		if( !image ) {
#ifdef DEV
			Win32::Error( "GetModuleHandleA failed" );
#endif
			return 0u;
		}

		auto image_base = ( std::uintptr_t )( image );
		auto image_dos_hdr = ( IMAGE_DOS_HEADER* )( image_base );

		if( image_dos_hdr->e_magic != IMAGE_DOS_SIGNATURE ) {
#ifdef DEV
			Win32::Error( "IMAGE_DOS_HEADER::e_magic is invalid" );
#endif
			return 0u;
		}

		auto image_nt_hdrs = ( IMAGE_NT_HEADERS* )( image_base + image_dos_hdr->e_lfanew );

		if( image_nt_hdrs->Signature != IMAGE_NT_SIGNATURE ) {
#ifdef DEV
			Win32::Error( "IMAGE_NT_HEADERS::Signature is invalid" );
#endif
			return 0u;
		}

		auto scan_begin = ( std::uint8_t* )( image_base );
		auto scan_end = ( std::uint8_t* )( image_base + image_nt_hdrs->OptionalHeader.SizeOfImage );

		std::uint8_t* scan_result = nullptr;
		std::uint8_t* scan_data = ( std::uint8_t* )( signature.c_str( ) );

		for( auto current = scan_begin; current < scan_end; current++ ) {
			if( *( std::uint8_t* )scan_data == '\?' || *current == GET_BYTE( scan_data ) ) {
				if( !scan_result )
					scan_result = current;

				if( !scan_data[ 2 ] )
					return ( std::uintptr_t )( scan_result );

				scan_data += ( *( std::uint16_t* )scan_data == '\?\?' || *( std::uint8_t* )scan_data != '\?' ) ? 3 : 2;

				if( !*scan_data )
					return ( std::uintptr_t )( scan_result );
			}
			else if( scan_result ) {
				current = scan_result;
				scan_data = ( std::uint8_t* )( signature.c_str( ) );
				scan_result = nullptr;
			}
		}

#ifdef DEV
		Win32::Warning( "Signature '%s' not found", signature.c_str( ) );
#endif

		return 0u;
#endif
	}
	
	unsigned int FindInDataMap( datamap_t* pMap, const char* name ) {
		while( pMap ) {
			for( int i = 0; i < pMap->dataNumFields; i++ ) {
				if( pMap->dataDesc[ i ].fieldName == NULL )
					continue;

				if( strcmp( name, pMap->dataDesc[ i ].fieldName ) == 0 )
					return pMap->dataDesc[ i ].fieldOffset[ TD_OFFSET_NORMAL ];

				if( pMap->dataDesc[ i ].fieldType == FIELD_EMBEDDED ) {
					if( pMap->dataDesc[ i ].td ) {
						unsigned int offset;

						if( ( offset = FindInDataMap( pMap->dataDesc[ i ].td, name ) ) != 0 )
							return offset;
					}
				}
			}
			pMap = pMap->baseMap;
		}

		return 0;
	}

	DWORD CallableFromRelative( DWORD nAddress )
	{
		return nAddress + *( DWORD* )( nAddress + 1 ) + 5;
	}
}