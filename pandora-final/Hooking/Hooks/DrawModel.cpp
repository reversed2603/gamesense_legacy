#include "../hooked.hpp"
#include "../../Features/Visuals/Chams.hpp"
#include "../../SDK/Classes/Player.hpp"
//-----------------------------------------------------------------------------
// Forward declarations
//-----------------------------------------------------------------------------
struct matrix3x4_t;

void __fastcall Hooked::DrawModelExecute( void* ECX, void* EDX, IMatRenderContext* MatRenderContext, DrawModelState_t& DrawModelState, ModelRenderInfo_t& RenderInfo, matrix3x4_t* pCustomBoneToWorld ) {
	g_Vars.globals.szLastHookCalled = XorStr( "6" );
	if( !MatRenderContext || !pCustomBoneToWorld || !g_Vars.globals.RenderIsReady || !ECX )
		return oDrawModelExecute( ECX, MatRenderContext, DrawModelState, RenderInfo, pCustomBoneToWorld );

	C_CSPlayer* local = C_CSPlayer::GetLocalPlayer( );
	if( !local )
		return oDrawModelExecute( ECX, MatRenderContext, DrawModelState, RenderInfo, pCustomBoneToWorld );

	// fuck a nigga named shadow
	if( strstr( RenderInfo.pModel->szName, XorStr( "shadow" ) ) != nullptr )
		return;

	static std::vector< const char* > smoke_mats = {
		XorStr( "particle/vistasmokev1/vistasmokev1_fire" ),
		XorStr( "particle/vistasmokev1/vistasmokev1_smokegrenade" ),
		XorStr( "particle/vistasmokev1/vistasmokev1_emods" ),
		XorStr( "particle/vistasmokev1/vistasmokev1_emods_impactdust" ),
	};

	for( auto mats : smoke_mats ) {
		static IMaterial* mat = m_pMatSystem->FindMaterial( mats, XorStr( TEXTURE_GROUP_OTHER ) );
		mat->SetMaterialVarFlag( MATERIAL_VAR_NO_DRAW, g_Vars.esp.remove_smoke );
	}

	g_Chams.OnDrawModelExecute( ECX, MatRenderContext, DrawModelState, RenderInfo, pCustomBoneToWorld );
}
