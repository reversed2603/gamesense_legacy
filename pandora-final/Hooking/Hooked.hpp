#pragma once

#include "../source.hpp"

struct vrect_t {
	int				x, y, width, height;
	vrect_t* pnext;
};

class CRecvProxyData;
struct MaterialSystemConfig_t;

namespace Hooked
{
	struct PlayerHook {
		Memory::VmtSwap clientHook;
		Memory::VmtSwap renderableHook;
		Memory::VmtSwap networkableHook;

		~PlayerHook( );
		void SetHooks( );
	};

	extern std::map< int, Hooked::PlayerHook > player_hooks;

	using CreateMoveFn = bool( __stdcall* )( float, CUserCmd* );
	inline CreateMoveFn oCreateMove;
	bool __stdcall CreateMove( float, CUserCmd* );

	using C_BaseAnimating__DrawModelFn = int( __thiscall* )( void*, int, int );
	inline C_BaseAnimating__DrawModelFn oC_BaseAnimating__DrawModel;
	int __fastcall C_BaseAnimating__DrawModel( void* ecx, void* edx, int a1, int a2 );

	using SendDatagramFn = int( __thiscall* )( INetChannel*, void* );
	inline SendDatagramFn oSendDatagram;
	int __fastcall SendDatagram( INetChannel* ecx, void* edx, void* a3 );

	using ProcessPacketFn = int( __thiscall* )( INetChannel*, void*, bool );
	inline ProcessPacketFn oProcessPacket;
	void __fastcall ProcessPacket( INetChannel* ecx, void* edx, void* a3, bool a4 );

	//void __fastcall CreateMoveInput( void* _this, int, int sequence_number, float input_sample_frametime, bool active );

	using DoPostScreenEffectsFn = int( __thiscall* )( IClientMode*, int );
	inline DoPostScreenEffectsFn oDoPostScreenEffects;
	int __stdcall DoPostScreenEffects( int a1 );

	using FrameStageNotifyFn = void( __thiscall* )( void*, ClientFrameStage_t );
	inline FrameStageNotifyFn oFrameStageNotify;
	void __fastcall FrameStageNotify( void* ecx, void* edx, ClientFrameStage_t stage );

	using RunCommandFn = void( __thiscall* )( void*, C_CSPlayer*, CUserCmd*, IMoveHelper* );
	inline RunCommandFn oRunCommand;
	void __fastcall RunCommand( void* ecx, void* edx, C_CSPlayer* player, CUserCmd* ucmd, IMoveHelper* moveHelper );

	using ProcessMovementFn = void(__thiscall*)(void*, C_BasePlayer*, CMoveData*);
	inline ProcessMovementFn oProcessMovement;
	void __fastcall ProcessMovement(void* ecx, void* edx, C_CSPlayer* basePlayer, CMoveData* m);

	using PaintTraverseFn = void( __thiscall* )( void*, unsigned int, bool, bool );
	inline PaintTraverseFn oPaintTraverse;
	void __fastcall PaintTraverse( void* ecx, void* edx, unsigned int vguiPanel, bool forceRepaint, bool allowForce );

	typedef void(__thiscall* Paint_t)(IEngineVGui*, int);
	inline Paint_t oPaint;
	void __stdcall EngineVGUI_Paint(int mode);

	using ResetFn = HRESULT( __stdcall* )( IDirect3DDevice9*, D3DPRESENT_PARAMETERS* );
	inline ResetFn oReset;
	HRESULT __stdcall Reset( IDirect3DDevice9* pDevice, D3DPRESENT_PARAMETERS* pPresentationParameters );

	using PresentFn = HRESULT( __stdcall* )( IDirect3DDevice9*, const RECT*, const RECT*, HWND, const RGNDATA* );
	inline PresentFn oPresent;
	HRESULT __stdcall Present( LPDIRECT3DDEVICE9 pDevice, const RECT* pSourceRect, const RECT* pDestRect, HWND hDestWindowOverride, const RGNDATA* pDirtyRegion );

	using LockCursorFn = void( __thiscall* )( void* );
	inline LockCursorFn oLockCursor;
	void __stdcall LockCursor( );

	using OverrideViewFn = void( __thiscall* )( void*, CViewSetup* );
	inline OverrideViewFn oOverrideView;
	void __fastcall OverrideView( void* ECX, int EDX, CViewSetup* vsView );

	using BeginFrameFn = void( __thiscall* )( void* ECX, float ft );
	inline BeginFrameFn oBeginFrame;
	void __fastcall BeginFrame( void* ECX, void* EDX, float ft );

	void __fastcall SetReservationCookie( void* ECX, void* EDX, int a2, int a3 );

	using PacketStartFn = void( __thiscall* )( void*, int, int );
	inline PacketStartFn oPacketStart;
	void __fastcall PacketStart( void* ECX, void* EDX, int incoming_sequence, int outgoing_acknowledged );

	using PacketEndFn = void( __thiscall* )( void* );
	inline PacketEndFn oPacketEnd;
	void __fastcall PacketEnd( void* ECX, void* EDX );

	using ListLeavesInBoxFn = int( __thiscall* )( void*, Vector& mins, Vector& maxs, unsigned short* pList, int listMax );
	inline ListLeavesInBoxFn oListLeavesInBox;
	int __fastcall ListLeavesInBox( void* ECX, void* EDX, Vector& mins, Vector& maxs, unsigned short* pList, int listMax );

	using SendNetMsgFn = bool( __thiscall* )( INetChannel* pNetChan, INetMessage& msg, bool bForceReliable, bool bVoice );
	inline SendNetMsgFn oSendNetMsg;
	bool __fastcall SendNetMsg( INetChannel* pNetChan, void* edx, INetMessage& msg, bool bForceReliable, bool bVoice );

	using ShutdownFn = void( __thiscall* )( INetChannel*, const char* );
	inline ShutdownFn oShutdown;
	void __fastcall Shutdown( INetChannel* pNetChan, void* EDX, const char* reason );

	using LooseFileAllowedFn = bool( __thiscall* )( void* );
	inline LooseFileAllowedFn oLooseFileAllowed;
	bool __fastcall LooseFileAllowed( void* ecx, void* edx );

	using CheckFileCRCsWithServerFn = void( __thiscall* )( void* );
	inline CheckFileCRCsWithServerFn oCheckFileCRCsWithServer;
	void __fastcall CheckFileCRCsWithServer( void* ecx, void* edx );

	using DrawModelExecuteFn = void( __thiscall* )( void*, IMatRenderContext* MatRenderContext, DrawModelState_t& DrawModelState, ModelRenderInfo_t& RenderInfo, matrix3x4_t* pCustomBoneToWorld );
	inline DrawModelExecuteFn oDrawModelExecute;
	void __fastcall DrawModelExecute( void* ECX, void* EDX, IMatRenderContext* MatRenderContext, DrawModelState_t& DrawModelState, ModelRenderInfo_t& RenderInfo, matrix3x4_t* pCustomBoneToWorld );

	IClientNetworkable* CreateCCSPlayer( int entnum, int serialNum );
	bool __fastcall SetupBones( uintptr_t ecx, void* edx, matrix3x4_t* matrix, int bone_count, int bone_mask, float time );
	void __fastcall EntityRelease( uintptr_t ecx, void* edx );

	using FnEmitSound = void( __thiscall* ) ( IEngineSound* thisptr, void* filter, int ent_index, int channel, const char* sound_entry, unsigned int sound_entry_hash,
		const char* sample, float volume, float attenuation, int seed, int flags, int pitch, const Vector* origin, const Vector* direction,
		void* vec_origins, bool update_positions, float sound_time, int speaker_entity, int test );
	inline FnEmitSound oEmitSound;
	void __fastcall EmitSound( IEngineSound* thisptr, uint32_t, void* filter, int ent_index, int channel, const char* sound_entry, unsigned int sound_entry_hash,
		const char* sample, float volume, float attenuation, int seed, int flags, int pitch, const Vector* origin, const Vector* direction,
		void* vec_origins, bool update_positions, float sound_time, int speaker_entity, int test );

	using FnGetScreenAspectRatio = float( __thiscall* )( void*, int32_t, int32_t );
	inline FnGetScreenAspectRatio oGetScreenAspectRatio;
	float __fastcall GetScreenAspectRatio( void* ECX, void* EDX, int32_t iWidth, int32_t iHeight );

	using FnInterpolateServerEntities = void( __cdecl* )( void );
	inline FnInterpolateServerEntities oInterpolateServerEntities;
	void __cdecl InterpolateServerEntities( void );

	using FnOnSoundStarted = void( __thiscall* )( void*, int, StartSoundParams_t&, char const* );
	inline FnOnSoundStarted oOnSoundStarted;
	void __fastcall OnSoundStarted( void* ECX, void* EDX, int guid, StartSoundParams_t& params, char const* soundname );

	using FnIsBoxVisible = int( __thiscall* )( void* ECX, const Vector&, const Vector& );
	inline FnIsBoxVisible oIsBoxVisible;
	int __fastcall IsBoxVisible( void* ECX, uint32_t, const Vector& mins, const Vector& maxs );

	using FnIsPlayingDemo = bool( __thiscall* )( void* ECX );
	inline FnIsPlayingDemo oIsPlayingDemo;
	bool __fastcall IsPlayingDemo( void* ECX, void* EDX );

	using FnRetrieveMessage = EGCResults( __thiscall* ) ( void* ECX, uint32_t* punMsgType, void* pubDest, uint32_t cubDest, uint32_t* pcubMsgSize );
	inline FnRetrieveMessage oRetrieveMessage;
	EGCResults __fastcall RetrieveMessage( void* ECX, void* EDX, uint32_t* punMsgType, void* pubDest, uint32_t cubDest, uint32_t* pcubMsgSize );

	using ReportHitFn = bool( __cdecl* )( Hit_t* );
	inline ReportHitFn oReportHit;
	bool __cdecl ReportHit( Hit_t* hit );

	void __vectorcall CL_Move( bool bFinalTick, float accumulated_extra_samples );
	inline decltype( &CL_Move ) oCL_Move;


	using IsUsingStaticPropDebugModeFn = bool( __cdecl* )( );
	inline IsUsingStaticPropDebugModeFn oIsUsingStaticPropDebugMode;
	bool __cdecl IsUsingStaticPropDebugMode( );

	using FnIsConnected = bool( __thiscall* ) ( void );
	inline FnIsConnected oIsConnected;
	bool __fastcall IsConnected( void );

	using FnDispatchUserMessage = bool( __thiscall* ) ( void* ECX, int msg_type, int unk1, int nBytes, bf_read& msg_data );
	inline FnDispatchUserMessage oDispatchUserMessage;
	bool __fastcall DispatchUserMessage( void* ECX, void* EDX, int msg_type, int unk1, int nBytes, bf_read& msg_data );

	using FnRenderView = void( __thiscall* ) ( void* ECX, const CViewSetup& view, CViewSetup& hudViewSetup, int nClearFlags, int whatToDraw );
	inline FnRenderView oRenderView;
	void __fastcall RenderView( void* ECX, void* EDX, const CViewSetup& view, CViewSetup& hudViewSetup, int nClearFlags, int whatToDraw );

	using ProcessTempEntitesFn = bool( __thiscall* )( void*, void* );
	inline ProcessTempEntitesFn oProcessTempEntities;
	bool __fastcall ProcessTempEntities( void* ecx, void* edx, void* msg );

	using CheckAchievementsEnabledFn = bool( __thiscall* )( void* );
	inline CheckAchievementsEnabledFn oCheckAchievementsEnabled;
	bool __fastcall CheckAchievementsEnabled( void* ecx, void* edx );

	using CL_FireEventsFn = void ( * )( void );
	inline CL_FireEventsFn CL_FireEvents;

	using FnProcessInterpolatedList = void( __cdecl* )( );
	inline FnProcessInterpolatedList oProcessInterpolatedList;
	void __cdecl ProcessInterpolatedList( );


	using FnModifyEyePosition = void( __thiscall* )( C_CSPlayer*, Vector* );
	inline FnModifyEyePosition oModifyEyePosition;
	void __fastcall ModifyEyePosition( C_CSPlayer* ecx, void* edx, Vector* eye_position );

	using FnAddBoxOverlay = void( __thiscall* )( void*, const Vector& origin, const Vector& mins, const Vector& max, QAngle const& orientation, int r, int g, int b, int a, float duration );
	inline FnAddBoxOverlay oAddBoxOverlay;
	void __fastcall AddBoxOverlay( void* ecx, void* edx, const Vector& origin, const Vector& mins, const Vector& max, QAngle const& orientation, int r, int g, int b, int a, float duration );

	using FnTraceRay = void( __thiscall* )( void*, const Ray_t&, unsigned int, ITraceFilter*, CGameTrace* );
	inline FnTraceRay oTraceRay;
	void __fastcall TraceRay( void* thisptr, void*, const Ray_t& ray, unsigned int fMask, ITraceFilter* pTraceFilter, CGameTrace* pTrace );

	using FnClipRayCollideable = void( __thiscall* )( void*, const Ray_t&, uint32_t, ICollideable*, CGameTrace* );
	inline FnClipRayCollideable oClipRayCollideable;
	void __fastcall ClipRayCollideable( void* thisptr, void*, const Ray_t& ray, unsigned int fMask, ICollideable* pCollide, CGameTrace* pTrace );

	using FnOverrideConfig = bool( __thiscall* )( IMaterialSystem*, MaterialSystemConfig_t&, bool );
	inline FnOverrideConfig oOverrideConfig;
	bool __fastcall OverrideConfig( IMaterialSystem* ecx, void* edx, MaterialSystemConfig_t& config, bool bForceUpdate );

	using FnDrawSetColor = void( __thiscall* )( void*, int, int, int, int );
	inline FnDrawSetColor oDrawSetColor;
	void __fastcall DrawSetColor( ISurface* thisptr, void* edx, int r, int g, int b, int a );

	typedef void( __thiscall* fnBuildTransformations )( C_CSPlayer*, CStudioHdr*, Vector*, Quaternion*, const matrix3x4_t&, const int32_t, BYTE* );
	inline fnBuildTransformations oBuildTransformations;
	void __fastcall BuildTransformations( C_CSPlayer* pPlayer, uint32_t, CStudioHdr* hdr, Vector* pos, Quaternion* q, const matrix3x4_t& transform, const int32_t mask, BYTE* computed );

	typedef bool(__thiscall* ShouldSkipAnimationFrame_t)(C_BaseAnimating*);
	inline ShouldSkipAnimationFrame_t oShouldSkipAnimationFrame;
	bool __fastcall ShouldSkipAnimationFrame(C_BaseAnimating* ecx, uint32_t*);

	typedef void( __thiscall* fnStandardBlendingRules )( void*, CStudioHdr*, Vector*, Quaternion*, float currentTime, int boneMask );
	inline fnStandardBlendingRules oStandardBlendingRules;
	void __fastcall StandardBlendingRules( C_CSPlayer* pPlayer, uint32_t, CStudioHdr* hdr, Vector* pos, Quaternion* q, float currentTime, int boneMask );

	typedef bool( __thiscall* fnIsRenderableInPvs )( void*, IClientRenderable* );
	inline fnIsRenderableInPvs oIsRenderableInPvs;
	bool __fastcall IsRenderableInPVS( void* ecx, void* edx, IClientRenderable* pRenderable );

	using PhysicsSimulateFn = void( __thiscall* ) ( void* ecx );
	inline PhysicsSimulateFn oPhysicsSimulate;
	void __fastcall PhysicsSimulate( void* ecx, void* edx );

	typedef void( __thiscall* fnCalcViewBob ) ( C_BasePlayer*, Vector& );
	inline fnCalcViewBob oCalcViewBob;
	void __fastcall CalcViewBob( C_BasePlayer* player, void* edx, Vector& eyeOrigin );

	typedef bool( __thiscall* fnIsHltv )( IVEngineClient* );
	inline fnIsHltv oIsHltv;
	bool __fastcall IsHltv( IVEngineClient* EngineClient, uint32_t );

	typedef void( __thiscall* fnUpdateClientSideAnimation )( C_CSPlayer* player );
	inline fnUpdateClientSideAnimation oUpdateClientSideAnimation;
	void __fastcall UpdateClientSideAnimation( C_CSPlayer* player, void* edx );

	typedef void( __thiscall* fnVertexBufferLock )( void* ecx, int max_vertex_count, bool append, void* unk );
	inline fnVertexBufferLock oVertexBufferLock;
	void __fastcall VertexBufferLock( void* ecx, void* edx, int max_vertex_count, bool append, void* unk );

	typedef void( __thiscall* fnDoProceduralFootPlant )( void* ecx, void* boneToWorld, void* pLeftFootChain, void* pRightFootChain, void* pos );
	inline fnDoProceduralFootPlant oDoProceduralFootPlant;
	void __fastcall DoProceduralFootPlant( void* ecx, void* edx, void* boneToWorld, void* pLeftFootChain, void* pRightFootChain, void* pos );

	typedef bool( __thiscall* fnWriteUsercmdDeltaToBuffer )( void*, int, void*, int, int, bool );
	inline fnWriteUsercmdDeltaToBuffer oWriteUsercmdDeltaToBuffer;
	bool __fastcall WriteUsercmdDeltaToBuffer( void* ecx, void* edx, int nSlot, void* pBuf, int nFrom, int nTo, bool newCmd );

	// Recv proxy hook
	void m_nSmokeEffectTickBegin( const CRecvProxyData* pData, void* pStruct, void* pOut );
	void m_bClientSideAnimation( const CRecvProxyData* pData, void* pStruct, void* pOut );
	void m_flVelocityModifier( const CRecvProxyData* pData, void* pStruct, void* pOut );
	void m_bNightVisionOn( const CRecvProxyData* pData, void* pStruct, void* pOut );
}
