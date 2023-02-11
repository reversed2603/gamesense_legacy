#pragma once
#include "sdk.hpp"
#include "IAppSystem.hpp"

typedef void(*MaterialBufferReleaseFunc_t)(int nChangeFlags);	// see RestoreChangeFlags_t
typedef void(*MaterialBufferRestoreFunc_t)(int nChangeFlags);	// see RestoreChangeFlags_t
typedef void(*ModeChangeCallbackFunc_t)(void);
typedef void(*EndFrameCleanupFunc_t)(void);
typedef bool(*EndFramePriorToNextContextFunc_t)(void);
typedef void(*OnLevelShutdownFunc_t)(void *pUserData);

typedef unsigned short MaterialHandle_t;
DECLARE_POINTER_HANDLE(MaterialLock_t);

class IMaterialSystem : public IAppSystem
{
public:

	virtual CreateInterfaceFn               Init(char const* pShaderAPIDLL, IMaterialProxyFactory *pMaterialProxyFactory, CreateInterfaceFn fileSystemFactory, CreateInterfaceFn cvarFactory = NULL) = 0;
	virtual void                            SetShaderAPI(char const *pShaderAPIDLL) = 0;
	virtual void                            SetAdapter(int nAdapter, int nFlags) = 0;
	virtual void                            ModInit() = 0;
	virtual void                            ModShutdown() = 0;
	virtual void                            SetThreadMode(MaterialThreadMode_t mode, int nServiceThread = -1) = 0;
	virtual MaterialThreadMode_t            GetThreadMode() = 0;
	virtual void                            ExecuteQueued() = 0;
	virtual void                            OnDebugEvent(const char *pEvent) = 0;
	virtual IMaterialSystemHardwareConfig*  GetHardwareConfig(const char *pVersion, int *returnCode) = 0;
	virtual void                            __unknown() = 0;
	virtual bool                            UpdateConfig(bool bForceUpdate) = 0; //20
	virtual bool                            OverrideConfig(const MaterialSystem_Config_t &config, bool bForceUpdate) = 0;
	virtual const MaterialSystem_Config_t&  GetCurrentConfigForVideoCard() const = 0;
	virtual bool                            GetRecommendedConfigurationInfo(int nDXLevel, KeyValues * pKeyValues) = 0;
	virtual int                             GetDisplayAdapterCount() const = 0;
	virtual int                             GetCurrentAdapter() const = 0;
	virtual void                            GetDisplayAdapterInfo(int adapter, MaterialAdapterInfo_t& info) const = 0;
	virtual int                             GetModeCount(int adapter) const = 0;
	virtual void                            GetModeInfo(int adapter, int mode, MaterialVideoMode_t& info) const = 0;
	virtual void                            AddModeChangeCallBack(ModeChangeCallbackFunc_t func) = 0;
	virtual void                            GetDisplayMode(MaterialVideoMode_t& mode) const = 0; //30
	virtual bool                            SetMode(void* hwnd, const MaterialSystem_Config_t &config) = 0;
	virtual bool                            SupportsMSAAMode(int nMSAAMode) = 0;
	virtual const MaterialSystemHWID_t&     GetVideoCardIdentifier(void) const = 0;
	virtual void                            SpewDriverInfo() const = 0;
	virtual void                            GetBackBufferDimensions(int &width, int &height) const = 0;
	virtual ImageFormat                     GetBackBufferFormat() const = 0;
	virtual const AspectRatioInfo_t&        GetAspectRatioInfo() const = 0;
	virtual bool                            SupportsHDRMode(HDRType_t nHDRModede) = 0;
	virtual bool                            AddView(void* hwnd) = 0;
	virtual void                            RemoveView(void* hwnd) = 0; //40
	virtual void                            SetView(void* hwnd) = 0;
	virtual void                            BeginFrame(float frameTime) = 0;
	virtual void                            EndFrame() = 0;
	virtual void                            Flush(bool flushHardware = false) = 0;
	virtual unsigned int                    GetCurrentFrameCount() = 0;
	virtual void                            SwapBuffers() = 0;
	virtual void                            EvictManagedResources() = 0;
	virtual void                            ReleaseResources(void) = 0;
	virtual void                            ReacquireResources(void) = 0;
	virtual void                            AddReleaseFunc(MaterialBufferReleaseFunc_t func) = 0; //50
	virtual void                            RemoveReleaseFunc(MaterialBufferReleaseFunc_t func) = 0;
	virtual void                            AddRestoreFunc(MaterialBufferRestoreFunc_t func) = 0;
	virtual void                            RemoveRestoreFunc(MaterialBufferRestoreFunc_t func) = 0;
	virtual void                            AddEndFrameCleanupFunc(EndFrameCleanupFunc_t func) = 0;
	virtual void                            RemoveEndFrameCleanupFunc(EndFrameCleanupFunc_t func) = 0;
	virtual void                            OnLevelShutdown() = 0;
	virtual bool                            AddOnLevelShutdownFunc(OnLevelShutdownFunc_t func, void *pUserData) = 0;
	virtual bool                            RemoveOnLevelShutdownFunc(OnLevelShutdownFunc_t func, void *pUserData) = 0;
	virtual void                            OnLevelLoadingComplete() = 0;
	virtual void                            ResetTempHWMemory(bool bExitingLevel = false) = 0; //60
	virtual void                            HandleDeviceLost() = 0;
	virtual int                             ShaderCount() const = 0;
	virtual int                             GetShaders(int nFirstShader, int nMaxCount, IShader **ppShaderList) const = 0;
	virtual int                             ShaderFlagCount() const = 0;
	virtual const char*                     ShaderFlagName(int nIndex) const = 0;
	virtual void                            GetShaderFallback(const char *pShaderName, char *pFallbackShader, int nFallbackLength) = 0;
	virtual IMaterialProxyFactory*          GetMaterialProxyFactory() = 0;
	virtual void                            SetMaterialProxyFactory(IMaterialProxyFactory* pFactory) = 0;
	virtual void                            EnableEditorMaterials() = 0;
	virtual void                            EnableGBuffers() = 0; //70
	virtual void                            SetInStubMode(bool bInStubMode) = 0;
	virtual void                            DebugPrintUsedMaterials(const char *pSearchSubString, bool bVerbose) = 0;
	virtual void                            DebugPrintUsedTextures(void) = 0;
	virtual void                            ToggleSuppressMaterial(char const* pMaterialName) = 0;
	virtual void                            ToggleDebugMaterial(char const* pMaterialName) = 0;
	virtual bool                            UsingFastClipping(void) = 0;
	virtual int                             StencilBufferBits(void) = 0; //number of bits per pixel in the stencil buffer
	virtual void                            UncacheAllMaterials() = 0;
	virtual void                            UncacheUnusedMaterials(bool bRecomputeStateSnapshots = false) = 0;
	virtual void                            CacheUsedMaterials() = 0; //80
	virtual void                            ReloadTextures() = 0;
	virtual void                            ReloadMaterials(const char *pSubString = NULL) = 0;
	virtual IMaterial*                      CreateMaterial(const char *pMaterialName, KeyValues *pVMTKeyValues) = 0;
	virtual IMaterial*                      FindMaterial(char const* pMaterialName, const char *pTextureGroupName, bool complain = true, const char *pComplainPrefix = NULL) = 0;
	virtual void							unk0() = 0;
	virtual MaterialHandle_t                FirstMaterial() const = 0;
	virtual MaterialHandle_t                NextMaterial(MaterialHandle_t h) const = 0;
	virtual MaterialHandle_t                InvalidMaterial() const = 0;
	virtual IMaterial*                      GetMaterial(MaterialHandle_t h) const = 0;
	virtual int                             GetNumMaterials() const = 0;
	virtual ITexture*                       FindTexture( char const* pTextureName, const char* pTextureGroupName, bool complain = true, int nAdditionalCreationFlags = 0 ) = 0;
	virtual bool                            IsTextureLoaded(char const* pTextureName) const = 0;
	virtual ITexture*                       CreateProceduralTexture(const char	*pTextureName, const char *pTextureGroupName, int w, int h, ImageFormat fmt, int nFlags) = 0;
	virtual void                            BeginRenderTargetAllocation() = 0;
	virtual void                            EndRenderTargetAllocation() = 0; // Simulate an Alt-Tab in here, which causes a release/restore of all resources
	virtual ITexture*                       CreateRenderTargetTexture(int w, int h, RenderTargetSizeMode_t sizeMode, ImageFormat	format, MaterialRenderTargetDepth_t depth = MATERIAL_RT_DEPTH_SHARED) = 0;
	virtual ITexture*                       CreateNamedRenderTargetTextureEx(const char *pRTName, int w, int h, RenderTargetSizeMode_t sizeMode, ImageFormat format, MaterialRenderTargetDepth_t depth = MATERIAL_RT_DEPTH_SHARED, unsigned int textureFlags = TEXTUREFLAGS_CLAMPS | TEXTUREFLAGS_CLAMPT, unsigned int renderTargetFlags = 0) = 0;
	virtual ITexture*                       CreateNamedRenderTargetTexture(const char *pRTName, int w, int h, RenderTargetSizeMode_t sizeMode, ImageFormat format, MaterialRenderTargetDepth_t depth = MATERIAL_RT_DEPTH_SHARED, bool bClampTexCoords = true, bool bAutoMipMap = false) = 0;
	virtual ITexture*                       CreateNamedRenderTargetTextureEx2(const char *pRTName, int w, int h, RenderTargetSizeMode_t sizeMode, ImageFormat format, MaterialRenderTargetDepth_t depth = MATERIAL_RT_DEPTH_SHARED, unsigned int textureFlags = TEXTUREFLAGS_CLAMPS | TEXTUREFLAGS_CLAMPT, unsigned int renderTargetFlags = 0) = 0;
	virtual void				BeginLightmapAllocation() = 0;
	virtual void                unk228() = 0;
	virtual void				EndLightmapAllocation() = 0;
	virtual int 				AllocateLightmap(int width, int height,int offsetIntoLightmapPage[2],IMaterial* pMaterial) = 0;
	virtual int					AllocateWhiteLightmap(IMaterial* pMaterial) = 0;
	virtual void				UpdateLightmap(int lightmapPageID, int lightmapSize[2],	int offsetIntoLightmapPage[2],	float* pFloatImage, float* pFloatImageBump1,float* pFloatImageBump2, float* pFloatImageBump3) = 0;
	virtual int					GetNumSortIDs() = 0;
	virtual void				GetSortInfo(void* sortInfoArray) = 0;
	virtual void				GetLightmapPageSize(int lightmap, int* width, int* height) const = 0;
	virtual void				ResetMaterialLightmapPageInfo() = 0;
	virtual void				ClearBuffers(bool bClearColor, bool bClearDepth, bool bClearStencil = false) = 0;
	virtual void unk1231() = 0;
	virtual void unk1232() = 0;
	virtual void unk1233() = 0;
	virtual void unk1234() = 0;
	//virtual void unk1235() = 0;
	//virtual void unk1236() = 0;
	//virtual void unk1237() = 0;
	virtual IMatRenderContext* GetRenderContext() = 0;
};