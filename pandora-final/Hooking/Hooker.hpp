#include <vector>
#include <algorithm>
#include "minhook-master/include/MinHook.h"

namespace Hooked
{
	struct hook_t {
		bool enabled = false;
		void* target = nullptr;
		void* custom = nullptr;

		__forceinline void Enable( ) {
			MH_EnableHook( target );
			enabled = true;
		}

		__forceinline void Disable( ) {
			MH_DisableHook( target );
			enabled = false;
		}
	};

	class C_Hooks {
		std::vector<hook_t> m_hooks;
	public:
		C_Hooks( ) {
		}
		~C_Hooks( ) {
		}

		template < typename o, typename fn = uintptr_t >
		__forceinline o HookVirtual( void* vtable, fn custom_func, int index ) {
			return CreateHook<o, fn>( custom_func, Memory::VCall<fn>( vtable, index ) );
		}

		template < typename o, typename fn = uintptr_t >
		__forceinline o CreateHook( fn custom_func, void* o_func ) {
			o original = { };
			hook_t hook = { };
			hook.target = o_func;
			hook.custom = custom_func;
			hook.enabled = false;

			if( MH_CreateHook( o_func, custom_func, ( LPVOID* )&original ) == MH_OK ) {
				m_hooks.push_back( hook );
			}

			return original;
		}

		__forceinline void Enable( ) {
			for( auto h : m_hooks )
				h.Enable( );
		}

		__forceinline void Restore( ) {
			for( auto h : m_hooks )
				h.Disable( );
		}
	};

	inline C_Hooks HooksManager;
};