#include "../../source.hpp"
#include "../Hooked.hpp"
#include "../../SDK/Displacement.hpp"
#include "../../SDK/Classes/entity.hpp"

void __fastcall Hooked::TraceRay( void* thisptr, void*, const Ray_t& ray, unsigned int fMask, ITraceFilter* pTraceFilter, CGameTrace* pTrace ) {
	oTraceRay( thisptr, ray, fMask, pTraceFilter, pTrace );
}

void __fastcall Hooked::ClipRayCollideable( void* thisptr, void*, const Ray_t& ray, unsigned int fMask, ICollideable* pCollide, CGameTrace* pTrace )
{
	if( !thisptr || !pCollide )
		return oClipRayCollideable( thisptr, ray, fMask, pCollide, pTrace );

	//auto backup = pCollide->OBBMaxs( ).z;
	//pCollide->OBBMaxs( ).z += 5.0f;

	oClipRayCollideable( thisptr, ray, fMask, pCollide, pTrace );

	//pCollide->OBBMaxs( ).z = backup;
}