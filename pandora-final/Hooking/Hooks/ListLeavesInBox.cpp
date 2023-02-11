#include "../hooked.hpp"
#include "../../SDK/variables.hpp"
#include "../../SDK/Classes/entity.hpp"
#include "../../SDK/Displacement.hpp"
#include <intrin.h>

class RenderableInfo_t {
public:
   IClientRenderable* m_pRenderable;
   void* m_pAlphaProperty;
   int m_EnumCount;
   int m_nRenderFrame;
   unsigned short m_FirstShadow;
   unsigned short m_LeafList;
   short m_Area;
   uint16_t m_Flags;   // 0x0016
   uint16_t m_Flags2; // 0x0018
   Vector m_vecBloatedAbsMins;
   Vector m_vecBloatedAbsMaxs;
   Vector m_vecAbsMins;
   Vector m_vecAbsMaxs;
   int pad;
};

int __fastcall Hooked::ListLeavesInBox( void* ECX, void* EDX, Vector& mins, Vector& maxs, unsigned short* pList, int listMax ) {
   g_Vars.globals.szLastHookCalled = XorStr( "15" );
   // occulusion getting updated on player movement/angle change,
   // in RecomputeRenderableLeaves ( https://github.com/pmrowla/hl2sdk-csgo/blob/master/game/client/clientleafsystem.cpp#L674 );
   // check for return in CClientLeafSystem::InsertIntoTree 
   if ( !g_Vars.esp.skip_occulusion )
	  return oListLeavesInBox( ECX, mins, maxs, pList, listMax );

   // 0x14244489  *( uint32_t* )
   if ( uintptr_t( _ReturnAddress( ) ) != Engine::Displacement.Data.m_uListLeavesInBoxReturn )
	  return oListLeavesInBox( ECX, mins, maxs, pList, listMax );

   // get current renderable info from stack ( https://github.com/pmrowla/hl2sdk-csgo/blob/master/game/client/clientleafsystem.cpp#L1470 )
   auto info = *( RenderableInfo_t * * ) ( ( uintptr_t ) _AddressOfReturnAddress( ) + 0x14 );
   if ( !info || !info->m_pRenderable )
	  return oListLeavesInBox( ECX, mins, maxs, pList, listMax ); 
   
   if ( !info->m_pRenderable->GetIClientUnknown( ) )
	  return oListLeavesInBox( ECX, mins, maxs, pList, listMax );

   // check if disabling occulusion for players ( https://github.com/pmrowla/hl2sdk-csgo/blob/master/game/client/clientleafsystem.cpp#L1491 )
   auto base_entity = info->m_pRenderable->GetIClientUnknown( )->GetBaseEntity( );
   if ( !base_entity || !base_entity->IsPlayer( ) )
	  return oListLeavesInBox( ECX, mins, maxs, pList, listMax );

   // fix render order, force translucent group ( https://www.unknowncheats.me/forum/2429206-post15.html )
   // AddRenderablesToRenderLists: https://i.imgur.com/hcg0NB5.png ( https://github.com/pmrowla/hl2sdk-csgo/blob/master/game/client/clientleafsystem.cpp#L2473 )
   info->m_Flags &= ~0x100;
   info->m_Flags2 |= 0xC0;

   // extend world space bounds to maximum ( https://github.com/pmrowla/hl2sdk-csgo/blob/master/game/client/clientleafsystem.cpp#L707 )
   static Vector map_min = Vector( MIN_COORD_FLOAT, MIN_COORD_FLOAT, MIN_COORD_FLOAT );
   static Vector map_max = Vector( MAX_COORD_FLOAT, MAX_COORD_FLOAT, MAX_COORD_FLOAT );
   auto count = oListLeavesInBox( ECX, map_min, map_max, pList, listMax );
   return count;
}
