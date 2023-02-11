#pragma once
#include "sdk.hpp"

class IStudioRender {
public:
  void* vtable;
  float fEyeShiftX;                      //0x0004
  float fEyeShiftY;                      //0x0008
  float fEyeShiftZ;                      //0x000C
  float fEyeSize;                        //0x0010
  float fEyeGlintPixelWidthLODThreshold; //0x0014
  __int32 maxDecalsPerModel;             //0x0018
  __int32 drawEntities;                  //0x001C
  __int32 skin;                          //0x0020
  __int32 fullbright;                    //0x0024
  bool bEyeMove : 1;                     // look around
  bool bSoftwareSkin : 1;
  bool bNoHardware : 1;
  bool bNoSoftware : 1;
  bool bTeeth : 1;
  bool bEyes : 1;
  bool bFlex : 1;
  bool bWireframe : 1;
  bool bDrawNormals : 1;
  bool bDrawTangentFrame : 1;
  bool bDrawZBufferedWireframe : 1;
  bool bSoftwareLighting : 1;
  bool bShowEnvCubemapOnly : 1;
  bool bWireframeDecals : 1;
  int m_nReserved[5];
  Vector m_ViewTarget;           //0x0040
  Vector m_ViewOrigin;           //0x004C
  Vector m_ViewRight;            //0x0058
  Vector m_ViewUp;               //0x0064
  Vector m_ViewPlaneNormal;      //0x0070
  Vector4D m_LightBoxColors[6];  //0x00CC
  LightDesc_t m_LocalLights[4];  //0x01E4
  __int32 m_NumLocalLights;      //0x023C
  float m_ColorMod[3];           //0x0248
  float m_AlphaMod;              //0x024C
  IMaterial* m_pForcedMaterial;  //0x0250
  __int32 m_nForcedMaterialType; //0x0254
  char pad_0x0258[0xC];          //0x0258
  __int32 unkhandle1;            //0x0264
  __int32 unkhandle2;            //0x0268
  __int32 unkhandle3;            //0x026C
  __int32 unkhandle4;            //0x0270
  char pad_0x0274[0x4];          //0x0274
  lightpos_t m_pLightPos[16];    //0x0278

  void SetColorModulation( float const* pColor ) {
	 typedef void( __thiscall * OrigFn )( void*, float const* );
	 Memory::VCall< OrigFn >( this, 27 )( this, pColor );
  }

  void DrawModel(void* pResults, DrawModelInfo_t* pInfo, matrix3x4_t* pBoneToWorld, float* flpFlexWeights, float* flpFlexDelayedWeights, Vector& vrModelOrigin, int32_t iFlags) {
	  using DrawModelFn = void(__thiscall*)(void*, void*, DrawModelInfo_t*, const matrix3x4_t*, float*, float*, Vector&, int);
	  Memory::VCall< DrawModelFn >(this, 29) (this, pResults, pInfo, pBoneToWorld, flpFlexWeights, flpFlexDelayedWeights, vrModelOrigin, iFlags);
  }

  inline void ForcedMaterialOverride( IMaterial* material, OverrideType_t nOverrideType = 0 ) {
	 m_pForcedMaterial = material;
	 m_nForcedMaterialType = nOverrideType;
  }
};

//static_assert( FIELD_OFFSET( IStudioRender, m_pForcedMaterial ) == 0x250, "Wrong m_pForcedMaterial offset" );
