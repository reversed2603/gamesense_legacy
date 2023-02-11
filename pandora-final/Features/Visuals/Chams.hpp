#pragma once
#include "../../SDK/sdk.hpp"


enum ChamsMaterials {
	MATERIAL_REGULAR = 0,
	MATERIAL_FLAT,
};

class Chams {
public:
	void OnDrawModelExecute( void* ECX, IMatRenderContext* MatRenderContext, DrawModelState_t& DrawModelState, ModelRenderInfo_t& RenderInfo, matrix3x4_t* pCustomBoneToWorld );
	bool HandlePlayerChams( C_CSPlayer* player, std::function<void()> oFn );
	bool GetCustomMatrix( C_CSPlayer* player, matrix3x4_t* out, bool bBacktrack = false );

	Chams( ) { };
	~Chams( ) { };

private:
	bool CreateMaterials( );
	void OverrideMaterial( bool xqz, const Color_f& color, IMaterial* pOverrideMaterial = nullptr );
	bool PassesSanityChecks( C_CSPlayer* pLocal, C_CSPlayer* pEntity, ModelRenderInfo_t& RenderInfo );

	bool UsingGlowEffect( int nIndex );

	bool m_bInit = false;
	IMaterial* m_matRegular = nullptr;
	IMaterial* m_matFlat = nullptr;

	matrix3x4_t *m_pOverrideMatrix = nullptr;
};

extern Chams g_Chams;

