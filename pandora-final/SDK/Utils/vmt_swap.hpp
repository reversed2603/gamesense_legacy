#pragma once
#include <memory>
#include <stdint.h>

namespace SDK::Memory
{

	class VmtSwap
	{
	public:
		using Shared = std::shared_ptr<VmtSwap>;
	public:
		VmtSwap( const void* instance = nullptr ) {
			if( instance )
				Create( instance );
		}

	public:
		~VmtSwap( ) {
			Destroy( );
		}

	public:
		bool Create( const void* instance );
		void Destroy( );
		void Replace( );
		void Restore( );
		void Hook( const void* hooked, const std::uint32_t index );

		template<typename T>
		T VCall( const std::size_t index ) {
			return ( T )( m_pRestore[ index ] );
		}
	private:
		std::uintptr_t** m_ppInstance = nullptr;
		std::uintptr_t* m_pRestore = nullptr;
		std::unique_ptr<std::uintptr_t[ ]> m_pReplace = nullptr;
		std::uint32_t m_nSize = 0u;
	};

}
