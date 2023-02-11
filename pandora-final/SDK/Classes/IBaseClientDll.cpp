#include "../sdk.hpp"

ClientClass* IBaseClientDLL::GetAllClasses()
{
	using Fn = ClientClass * (__thiscall*)(void*);
	return Memory::VCall<Fn>(this, Index::IBaseClientDLL::GetAllClasses)(this);
}

bool IBaseClientDLL::IsChatRaised( ) {
   using Fn = bool( __thiscall* )( void* );
   return Memory::VCall<Fn>( this, 90 )( this );
}
