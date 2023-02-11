#include "source.hpp"

#include "Hooking/Hooked.hpp"
#include "Utils/InputSys.hpp"

#include "SDK/Classes/PropManager.hpp"
#include "SDK/Displacement.hpp"

#include "SDK/Classes/Player.hpp"

#include "Features/Miscellaneous/GameEvent.hpp"

#include "Renderer/Render.hpp"
#include "Features/Miscellaneous/SkinChanger.hpp"
#include "Features/Miscellaneous/KitParser.hpp"
#include "Features/Visuals/Glow.hpp"

#include "Hooking/hooker.hpp"

#include "Features/Visuals/Chams.hpp"

#include "Features/Rage/EnginePrediction.hpp"
#include "Loader/Exports.h"

#include <fstream>

#include "Utils/LogSystem.hpp"

extern ClientClass* CCSPlayerClass;
extern CreateClientClassFn oCreateCCSPlayer;

bool __fastcall Hooked::CheckAchievementsEnabled( void* ecx, void* edx ) {
	return true;
}

using CHudElement__ShouldDrawFn = bool( __thiscall* )( void* );
CHudElement__ShouldDrawFn oCHudElement__ShouldDraw;

// client.dll E8 ? ? ? ? 84 C0 74 F3
bool __fastcall CHudElement__ShouldDraw( void* ecx, void* edx ) {
	return false;
}

using CHudScope__PaintFn = bool( __thiscall* )( void* );
CHudScope__PaintFn oCHudScope__Paint;

// client.dll 55 8B EC 83 E4 F8 83 EC 78 56 57 8B 3D
void __fastcall CHudScope__Paint( void* ecx, void* edx ) {

}

Encrypted_t<IBaseClientDLL> m_pClient = nullptr;
Encrypted_t<IClientEntityList> m_pEntList = nullptr;
Encrypted_t<IGameMovement> m_pGameMovement = nullptr;
Encrypted_t<IPrediction> m_pPrediction = nullptr;
Encrypted_t<IMoveHelper> m_pMoveHelper = nullptr;
Encrypted_t<IInput> m_pInput = nullptr;
Encrypted_t<CGlobalVars>  m_pGlobalVars = nullptr;
Encrypted_t<ISurface> m_pSurface = nullptr;
Encrypted_t<IVEngineClient> m_pEngine = nullptr;
Encrypted_t<IClientMode> m_pClientMode = nullptr;
Encrypted_t<ICVar> m_pCvar = nullptr;
Encrypted_t<IPanel> m_pPanel = nullptr;
Encrypted_t<IGameEventManager> m_pGameEvent = nullptr;
Encrypted_t<IVModelRender> m_pModelRender = nullptr;
Encrypted_t<IMaterialSystem> m_pMatSystem = nullptr;
Encrypted_t<ISteamClient> g_pSteamClient = nullptr;
Encrypted_t<ISteamGameCoordinator> g_pSteamGameCoordinator = nullptr;
Encrypted_t<ISteamMatchmaking> g_pSteamMatchmaking = nullptr;
Encrypted_t<ISteamUser> g_pSteamUser = nullptr;
Encrypted_t<ISteamFriends> g_pSteamFriends = nullptr;
Encrypted_t<IPhysicsSurfaceProps> m_pPhysSurface = nullptr;
Encrypted_t<IEngineTrace> m_pEngineTrace = nullptr;
Encrypted_t<CGlowObjectManager> m_pGlowObjManager = nullptr;
Encrypted_t<IVModelInfo> m_pModelInfo = nullptr;
Encrypted_t<CClientState>  m_pClientState = nullptr;
Encrypted_t<IVDebugOverlay> m_pDebugOverlay = nullptr;
Encrypted_t<IEngineSound> m_pEngineSound = nullptr;
Encrypted_t<IMemAlloc> m_pMemAlloc = nullptr;
Encrypted_t<IViewRenderBeams> m_pRenderBeams = nullptr;
Encrypted_t<ILocalize> m_pLocalize = nullptr;
Encrypted_t<IStudioRender> m_pStudioRender = nullptr;
Encrypted_t<ICenterPrint> m_pCenterPrint = nullptr;
Encrypted_t<IVRenderView> m_pRenderView = nullptr;
Encrypted_t<IClientLeafSystem> m_pClientLeafSystem = nullptr;
Encrypted_t<IMDLCache> m_pMDLCache = nullptr;
Encrypted_t<IEngineVGui> m_pEngineVGui = nullptr;
Encrypted_t<IViewRender> m_pViewRender = nullptr;
Encrypted_t<IInputSystem> m_pInputSystem = nullptr;
Encrypted_t<INetGraphPanel> m_pNetGraphPanel = nullptr;
Encrypted_t<CCSGameRules> m_pGameRules = nullptr;
Encrypted_t<CFontManager> m_pFontManager = nullptr;
Encrypted_t<CSPlayerResource*> m_pPlayerResource = nullptr;
Encrypted_t<IWeaponSystem> m_pWeaponSystem = nullptr;
Encrypted_t<SFHudDeathNoticeAndBotStatus> m_pDeathNotices = nullptr;
Encrypted_t<CHud> m_pHud = nullptr;

RecvPropHook::Shared m_pDidSmokeEffectSwap = nullptr;
RecvPropHook::Shared m_bClientSideAnimationSwap = nullptr;
RecvPropHook::Shared m_flVelocityModifierSwap = nullptr;

RecvPropHook::Shared m_bNightVisionOnSwap = nullptr;

namespace Interfaces
{
	WNDPROC oldWindowProc;
	HWND hWindow = nullptr;

	void m_bDidSmokeEffect( const CRecvProxyData* pData, void* pStruct, void* pOut ) {
		m_pDidSmokeEffectSwap->GetOriginalFunction( )( pData, pStruct, pOut );

		if( g_Vars.esp.remove_smoke )
			*( uintptr_t* )( ( uintptr_t )pOut ) = true;
	}

	typedef bool( __thiscall* fnGetBool )( void* );
	fnGetBool oNetShowfragmentsGetBool;
	bool __fastcall net_showfragments_get_bool( void* pConVar, void* edx ) {
		static auto read_sub_channel_data_ret = reinterpret_cast< uintptr_t* >( Memory::Scan( "engine.dll", "85 C0 74 12 53 FF 75 0C 68 ? ? ? ? FF 15 ? ? ? ? 83 C4 0C" ) );
		static auto check_receiving_list_ret = reinterpret_cast< uintptr_t* >( Memory::Scan( "engine.dll", "8B 1D ? ? ? ? 85 C0 74 16 FF B6" ) );

		if( !m_pEngine->IsInGame( ) || !m_pEngine->IsConnected( ) )
			return oNetShowfragmentsGetBool( pConVar );

		static uint32_t last_fragment = 0;

		if( _ReturnAddress( ) == reinterpret_cast< void* >( read_sub_channel_data_ret ) && last_fragment > 0 ) {
			const auto data = &reinterpret_cast< uint32_t* >( ( uintptr_t )m_pClientState.Xor( ) + 0x9C )[ 0x56 ];
			const auto bytes_fragments = reinterpret_cast< uint32_t* >( data )[ 0x43 ];

			if( bytes_fragments == last_fragment ) {
				auto& buffer = reinterpret_cast< uint32_t* >( data )[ 0x42 ];
				buffer = 0;
			}
		}

		if( _ReturnAddress( ) == check_receiving_list_ret ) {
			const auto data = &reinterpret_cast< uint32_t* >( ( uintptr_t )m_pClientState.Xor( ) + 0x9C )[ 0x56 ];
			const auto bytes_fragments = reinterpret_cast< uint32_t* >( data )[ 0x43 ];

			last_fragment = bytes_fragments;
		}

		return oNetShowfragmentsGetBool( pConVar );
	}

	fnGetBool oSvCheatsGetBool;
	bool __fastcall sv_cheats_get_bool( void* pConVar, void* edx ) {
		static auto ret_ard = ( uintptr_t )Memory::Scan( "client.dll", "85 C0 75 30 38 86" );
		if( reinterpret_cast< uintptr_t >( _ReturnAddress( ) ) == ret_ard )
			return true;

		return oSvCheatsGetBool( pConVar );
	}

	bool Create( void* reserved ) {
		m_pClient = ( IBaseClientDLL* )CreateInterface( XorStr( "client.dll" ), XorStr( "VClient018" ) );
		if( !m_pClient.IsValid( ) ) {
			return false;
		}

		if( !Engine::g_PropManager.Create( m_pClient.Xor( ) ) ) {
			return false;
		}

		m_pEntList = ( IClientEntityList* )CreateInterface( XorStr( "client.dll" ), XorStr( "VClientEntityList003" ) );
		if( !m_pEntList.IsValid( ) ) {
			return false;
		}

		m_pGameMovement = ( IGameMovement* )CreateInterface( XorStr( "client.dll" ), XorStr( "GameMovement001" ) );
		if( !m_pGameMovement.IsValid( ) ) {
			return false;
		}

		m_pPrediction = ( IPrediction* )CreateInterface( XorStr( "client.dll" ), XorStr( "VClientPrediction001" ) );
		if( !m_pPrediction.IsValid( ) ) {
			return false;
		}

		m_pInput = *reinterpret_cast< IInput** > ( ( *reinterpret_cast< uintptr_t** >( m_pClient.Xor( ) ) )[ 15 ] + 0x1 );
		if( !m_pInput.IsValid( ) ) {
			return false;
		}

		m_pGlobalVars = **reinterpret_cast< CGlobalVars*** > ( ( *reinterpret_cast< uintptr_t** > ( m_pClient.Xor( ) ) )[ 0 ] + 0x1B );
		if( !m_pGlobalVars.IsValid( ) ) {
			return false;
		}

		m_pWeaponSystem = *( IWeaponSystem** )( Memory::Scan( XorStr( "client.dll" ), XorStr( "8B 35 ? ? ? ? FF 10 0F B7 C0" ) ) + 2 );
		if( !m_pWeaponSystem.IsValid( ) ) {
			return false;
		}

		m_pEngine = ( IVEngineClient* )CreateInterface( XorStr( "engine.dll" ), XorStr( "VEngineClient014" ) );
		if( !m_pEngine.IsValid( ) ) {
			return false;
		}

		m_pPanel = ( IPanel* )CreateInterface( XorStr( "vgui2.dll" ), XorStr( "VGUI_Panel009" ) );
		if( !m_pPanel.IsValid( ) ) {
			return false;
		}

		m_pSurface = ( ISurface* )CreateInterface( XorStr( "vguimatsurface.dll" ), XorStr( "VGUI_Surface031" ) );
		if( !m_pSurface.IsValid( ) ) {
			return false;
		}

		m_pClientMode = **( IClientMode*** )( ( *( DWORD** )m_pClient.Xor( ) )[ 10 ] + 0x5 );
		if( !m_pClientMode.IsValid( ) ) {
			return false;
		}

		m_pCvar = ( ICVar* )CreateInterface( XorStr( "vstdlib.dll" ), XorStr( "VEngineCvar007" ) );
		if( !m_pCvar.IsValid( ) ) {
			return false;
		}

		m_pGameEvent = ( IGameEventManager* )CreateInterface( XorStr( "engine.dll" ), XorStr( "GAMEEVENTSMANAGER002" ) );
		if( !m_pGameEvent.IsValid( ) ) {
			return false;
		}

		m_pModelRender = ( IVModelRender* )CreateInterface( XorStr( "engine.dll" ), XorStr( "VEngineModel016" ) );
		if( !m_pModelRender.IsValid( ) ) {
			return false;
		}

		m_pMatSystem = ( IMaterialSystem* )CreateInterface( XorStr( "materialsystem.dll" ), XorStr( "VMaterialSystem080" ) );
		if( !m_pMatSystem.IsValid( ) ) {
			return false;
		}

		m_pPhysSurface = ( IPhysicsSurfaceProps* )CreateInterface( XorStr( "vphysics.dll" ), XorStr( "VPhysicsSurfaceProps001" ) );
		if( !m_pPhysSurface.IsValid( ) ) {
			return false;
		}

		m_pEngineTrace = ( IEngineTrace* )CreateInterface( XorStr( "engine.dll" ), XorStr( "EngineTraceClient004" ) );
		if( !m_pEngineTrace.IsValid( ) ) {
			return false;
		}

		if( !Engine::CreateDisplacement( reserved ) ) {
			return false;
		}

		m_pMoveHelper = ( IMoveHelper* )( Engine::Displacement.Data.m_uMoveHelper );
		if( !m_pMoveHelper.IsValid( ) ) {
			return false;
		}

		m_pGlowObjManager = ( CGlowObjectManager* )Engine::Displacement.Data.m_uGlowObjectManager;
		if( !m_pGlowObjManager.IsValid( ) ) {
			return false;
		}

		m_pModelInfo = ( IVModelInfo* )CreateInterface( XorStr( "engine.dll" ), XorStr( "VModelInfoClient004" ) );
		if( !m_pModelInfo.IsValid( ) ) {
			return false;
		}

		// A1 FC BC 58 10  mov eax, g_ClientState
		m_pClientState = Encrypted_t<CClientState>( **( CClientState*** )( ( *( std::uintptr_t** )m_pEngine.Xor( ) )[ 14 ] + 0x1 ) );
		if( !m_pClientState.IsValid( ) ) {
			return false;
		}

		m_pDebugOverlay = ( IVDebugOverlay* )CreateInterface( XorStr( "engine.dll" ), XorStr( "VDebugOverlay004" ) );
		if( !m_pDebugOverlay.IsValid( ) ) {
			return false;
		}

		m_pMemAlloc = *( IMemAlloc** )( GetProcAddress( GetModuleHandle( XorStr( "tier0.dll" ) ), XorStr( "g_pMemAlloc" ) ) );
		if( !m_pMemAlloc.IsValid( ) ) {
			return false;
		}

		m_pEngineSound = ( IEngineSound* )CreateInterface( XorStr( "engine.dll" ), XorStr( "IEngineSoundClient003" ) );
		if( !m_pEngineSound.IsValid( ) ) {
			return false;
		}

		m_pRenderBeams = *( IViewRenderBeams** )( Engine::Displacement.Data.m_uRenderBeams );
		if( !m_pRenderBeams.IsValid( ) ) {
			return false;
		}

		m_pLocalize = ( ILocalize* )CreateInterface( XorStr( "localize.dll" ), XorStr( "Localize_001" ) );
		if( !m_pLocalize.IsValid( ) ) {
			return false;
		}

		m_pStudioRender = ( IStudioRender* )CreateInterface( XorStr( "studiorender.dll" ), XorStr( "VStudioRender026" ) );
		if( !m_pStudioRender.IsValid( ) ) {
			return false;
		}

		m_pCenterPrint = *( ICenterPrint** )( Engine::Displacement.Data.m_uCenterPrint );
		if( !m_pCenterPrint.IsValid( ) ) {
			return false;
		}

		m_pRenderView = ( IVRenderView* )CreateInterface( XorStr( "engine.dll" ), XorStr( "VEngineRenderView014" ) );
		if( !m_pRenderView.IsValid( ) ) {
			return false;
		}

		m_pClientLeafSystem = ( IClientLeafSystem* )CreateInterface( XorStr( "client.dll" ), XorStr( "ClientLeafSystem002" ) );
		if( !m_pClientLeafSystem.IsValid( ) ) {
			return false;
		}

		m_pMDLCache = ( IMDLCache* )CreateInterface( XorStr( "datacache.dll" ), XorStr( "MDLCache004" ) );
		if( !m_pMDLCache.IsValid( ) ) {
			return false;
		}

		m_pEngineVGui = ( IEngineVGui* )CreateInterface( XorStr( "engine.dll" ), XorStr( "VEngineVGui001" ) );
		if( !m_pEngineVGui.IsValid( ) ) {
			return false;
		}

		m_pInputSystem = ( IInputSystem* )CreateInterface( XorStr( "inputsystem.dll" ), XorStr( "InputSystemVersion001" ) );
		if( !m_pInputSystem.IsValid( ) ) {
			return false;
		}

		m_pViewRender = **( IViewRender*** )( Memory::Scan( XorStr( "client.dll" ), XorStr( "FF 50 4C 8B 06 8D 4D F4" ) ) - 6 );
		if( !m_pViewRender.IsValid( ) ) {
			return false;
		}

		m_pFontManager = *( CFontManager** )( Memory::Scan( XorStr( "vguimatsurface.dll" ), XorStr( "74 1D 8B 0D ?? ?? ?? ?? 68" ) ) + 0x4 );
		if( !m_pFontManager.IsValid( ) ) {
			return false;
		}

		m_pHud = *( CHud** )( Memory::Scan( XorStr( "client.dll" ), XorStr( "B9 ? ? ? ? E8 ? ? ? ? 8B 5D 08" ) ) + 1 );
		if( !m_pHud.IsValid( ) )
			return false;

		m_pDeathNotices = m_pHud->FindHudElement< SFHudDeathNoticeAndBotStatus* >( XorStr( "SFHudDeathNoticeAndBotStatus" ) );
		if( !m_pDeathNotices.IsValid( ) )
			return false;

		Hooked::CL_FireEvents = reinterpret_cast< Hooked::CL_FireEventsFn >( Memory::Scan( XorStr( "engine.dll" ), XorStr( "55 8B EC 83 EC 08 53 8B 1D ? ? ? ? 56 57 83 BB ? ? 00 00 06" ) ) );
		if( !Hooked::CL_FireEvents ) {
			return false;
		}

		auto D3DDevice9 = **( IDirect3DDevice9*** )Engine::Displacement.Data.m_D3DDevice;
		if( !D3DDevice9 ) {
			return false;
		}

		if( !g_InputSystem.Initialize( D3DDevice9 ) ) {
			return false;
		}

		auto pSteamAPI = GetModuleHandleA( XorStr( "steam_api.dll" ) );
		g_pSteamClient = ( ( ISteamClient * ( __cdecl* )( void ) )GetProcAddress( pSteamAPI, XorStr( "SteamClient" ) ) )( );
		if( !g_pSteamClient.IsValid( ) ) {
			return false;
		}

		HSteamUser hSteamUser = reinterpret_cast< HSteamUser( __cdecl* ) ( void ) >( GetProcAddress( pSteamAPI, XorStr( "SteamAPI_GetHSteamUser" ) ) )( );
		HSteamPipe hSteamPipe = reinterpret_cast< HSteamPipe( __cdecl* ) ( void ) >( GetProcAddress( pSteamAPI, XorStr( "SteamAPI_GetHSteamPipe" ) ) )( );
		g_pSteamGameCoordinator = ( ISteamGameCoordinator* )g_pSteamClient->GetISteamGenericInterface( hSteamUser, hSteamPipe, XorStr( "SteamGameCoordinator001" ) );
		if( !g_pSteamGameCoordinator.IsValid( ) ) {
			return false;
		}

		g_GameEvent.Register( );

		g_KitParser.initialize_kits( );
		Render::DirectX::init( D3DDevice9 );
		g_SkinChanger.Create( );

		for( auto clientclass = m_pClient->GetAllClasses( );
			clientclass != nullptr;
			clientclass = clientclass->m_pNext ) {
			if( !strcmp( clientclass->m_pNetworkName, XorStr( "CCSPlayer" ) ) ) {
				CCSPlayerClass = clientclass;
				oCreateCCSPlayer = ( CreateClientClassFn )clientclass->m_pCreateFn;

				clientclass->m_pCreateFn = Hooked::CreateCCSPlayer;
				continue;
			}

			if( !strcmp( clientclass->m_pNetworkName, XorStr( "CPlayerResource" ) ) ) {
				RecvTable* pClassTable = clientclass->m_pRecvTable;

				for( int nIndex = 0; nIndex < pClassTable->m_nProps; nIndex++ ) {
					RecvProp* pProp = &pClassTable->m_pProps[ nIndex ];

					if( !pProp || strcmp( pProp->m_pVarName, XorStr( "m_iTeam" ) ) )
						continue;

					m_pPlayerResource = Encrypted_t<CSPlayerResource*>( *reinterpret_cast< CSPlayerResource*** >( std::uintptr_t( pProp->m_pDataTable->m_pProps->m_ProxyFn ) + 0x10 ) );
					break;
				}

				continue;
			}
		}

		if( m_pEngine->IsInGame( ) ) {
			for( int i = 1; i <= m_pGlobalVars->maxClients; ++i ) {
				auto entity = C_CSPlayer::GetPlayerByIndex( i );
				if( !entity || !entity->IsPlayer( ) )
					continue;

				auto& new_hook = Hooked::player_hooks[ i ];
				new_hook.clientHook.Create( entity );
				new_hook.renderableHook.Create( ( void* )( ( uintptr_t )entity + 0x4 ) );
				new_hook.networkableHook.Create( ( void* )( ( uintptr_t )entity + 0x8 ) );
				new_hook.SetHooks( );
			}
		}

		// init config system
		g_Vars.Create( );

		// setup variable stuff
		g_Vars.viewmodel_fov->fnChangeCallback.m_Size = 0;
		g_Vars.viewmodel_offset_x->fnChangeCallback.m_Size = 0;
		g_Vars.viewmodel_offset_y->fnChangeCallback.m_Size = 0;
		g_Vars.viewmodel_offset_z->fnChangeCallback.m_Size = 0;

		g_Vars.mat_ambient_light_r->fnChangeCallback.m_Size = 0;
		g_Vars.mat_ambient_light_g->fnChangeCallback.m_Size = 0;
		g_Vars.mat_ambient_light_b->fnChangeCallback.m_Size = 0;

		// disable extrapolation, i guess
		g_Vars.cl_extrapolate->SetValue( 0 );

		// setup proxy hooks
		RecvProp* prop = nullptr;

		//Engine::g_PropManager.GetProp( XorStr( "DT_SmokeGrenadeProjectile" ), XorStr( "m_bDidSmokeEffect" ), &prop );
		//m_pDidSmokeEffectSwap = std::make_shared<RecvPropHook>( prop, &Hooked::m_nSmokeEffectTickBegin );
		//
		//Engine::g_PropManager.GetProp( XorStr( "DT_BaseAnimating" ), XorStr( "bClientSideAnimation" ), &prop );
		//m_bClientSideAnimationSwap = std::make_shared<RecvPropHook>( prop, &Hooked::m_bClientSideAnimation );

		//Engine::g_PropManager.GetProp( XorStr( "DT_CSPlayer" ), XorStr( "m_flVelocityModifier" ), &prop );
		//m_flVelocityModifierSwap = std::make_shared<RecvPropHook>( prop, &Hooked::m_flVelocityModifier );

		//Engine::g_PropManager.GetProp( XorStr( "DT_CSPlayer" ), XorStr( "m_bNightVisionOn" ), &prop );
		//m_bNightVisionOnSwap = std::make_shared<RecvPropHook>( prop, &Hooked::m_bNightVisionOn );

		MH_Initialize( );

		//sv_cheats_get_bool
		oNetShowfragmentsGetBool = Hooked::HooksManager.HookVirtual<decltype( oNetShowfragmentsGetBool )>( m_pCvar->FindVar( "net_showfragments" ), &net_showfragments_get_bool, 13 );
		oSvCheatsGetBool = Hooked::HooksManager.HookVirtual<decltype( oSvCheatsGetBool )>( m_pCvar->FindVar( "sv_cheats" ), &sv_cheats_get_bool, 13 );

		// vmt hooks
		Hooked::oGetScreenAspectRatio = Hooked::HooksManager.HookVirtual<decltype( Hooked::oGetScreenAspectRatio )>( m_pEngine.Xor( ), &Hooked::GetScreenAspectRatio, Index::EngineClient::GetScreenAspectRatio );
		Hooked::oIsBoxVisible = Hooked::HooksManager.HookVirtual<decltype( Hooked::oIsBoxVisible )>( m_pEngine.Xor( ), &Hooked::IsBoxVisible, 32 );
		//Hooked::oListLeavesInBox = Hooked::HooksManager.HookVirtual<decltype( Hooked::oListLeavesInBox )>( m_pEngine.Xor( )->GetBSPTreeQuery( ), &Hooked::ListLeavesInBox, Index::BSPTreeQuery::ListLeavesInBox );
		Hooked::oIsHltv = Hooked::HooksManager.HookVirtual<decltype( Hooked::oIsHltv )>( m_pEngine.Xor( ), &Hooked::IsHltv, Index::EngineClient::IsHltv );

		//Hooked::oDispatchUserMessage = Hooked::HooksManager.HookVirtual<decltype( Hooked::oDispatchUserMessage )>( m_pClient.Xor( ), &Hooked::DispatchUserMessage, 38 );
		Hooked::oFrameStageNotify = Hooked::HooksManager.HookVirtual<decltype( Hooked::oFrameStageNotify )>( m_pClient.Xor( ), &Hooked::FrameStageNotify, Index::IBaseClientDLL::FrameStageNotify );

		Hooked::oCreateMove = Hooked::HooksManager.HookVirtual<decltype( Hooked::oCreateMove )>( m_pClientMode.Xor( ), &Hooked::CreateMove, Index::CClientModeShared::CreateMove );
		Hooked::oDoPostScreenEffects = Hooked::HooksManager.HookVirtual<decltype( Hooked::oDoPostScreenEffects )>( m_pClientMode.Xor( ), &Hooked::DoPostScreenEffects, Index::CClientModeShared::DoPostScreenSpaceEffects );
		Hooked::oOverrideView = Hooked::HooksManager.HookVirtual<decltype( Hooked::oOverrideView )>( m_pClientMode.Xor( ), &Hooked::OverrideView, Index::CClientModeShared::OverrideView );

		Hooked::oDrawSetColor = Hooked::HooksManager.HookVirtual<decltype( Hooked::oDrawSetColor )>( m_pSurface.Xor( ), &Hooked::DrawSetColor, 15 );
		Hooked::oLockCursor = Hooked::HooksManager.HookVirtual<decltype( Hooked::oLockCursor )>( m_pSurface.Xor( ), &Hooked::LockCursor, Index::VguiSurface::LockCursor );

		Hooked::oPaintTraverse = Hooked::HooksManager.HookVirtual<decltype( Hooked::oPaintTraverse )>( m_pPanel.Xor( ), &Hooked::PaintTraverse, Index::IPanel::PaintTraverse );

		Hooked::oPaint = Hooked::HooksManager.HookVirtual<decltype( Hooked::oPaint )>( m_pEngineVGui.Xor( ), &Hooked::EngineVGUI_Paint, Index::IEngineVGui::Paint );

		Hooked::oBeginFrame = Hooked::HooksManager.HookVirtual<decltype( Hooked::oBeginFrame )>( m_pMatSystem.Xor( ), &Hooked::BeginFrame, Index::MatSystem::BeginFrame );
		Hooked::oOverrideConfig = Hooked::HooksManager.HookVirtual<decltype( Hooked::oOverrideConfig )>( m_pMatSystem.Xor( ), &Hooked::OverrideConfig, 21 );

		//Hooked::oEmitSound = Hooked::HooksManager.HookVirtual<decltype( Hooked::oEmitSound )>( m_pEngineSound.Xor( ), &Hooked::EmitSound, 5 );
		Hooked::oRunCommand = Hooked::HooksManager.HookVirtual<decltype( Hooked::oRunCommand )>( m_pPrediction.Xor( ), &Hooked::RunCommand, Index::IPrediction::RunCommand );

		Hooked::oProcessMovement = Hooked::HooksManager.HookVirtual<decltype( Hooked::oProcessMovement )>( m_pGameMovement.Xor( ), &Hooked::ProcessMovement, Index::IGameMovement::ProcessMovement );

		Hooked::oClipRayCollideable = Hooked::HooksManager.HookVirtual<decltype( Hooked::oClipRayCollideable )>( m_pEngineTrace.Xor( ), &Hooked::ClipRayCollideable, 4 );
		Hooked::oTraceRay = Hooked::HooksManager.HookVirtual<decltype( Hooked::oTraceRay )>( m_pEngineTrace.Xor( ), &Hooked::TraceRay, 5 );

		Hooked::oDrawModelExecute = Hooked::HooksManager.HookVirtual<decltype( Hooked::oDrawModelExecute )>( m_pModelRender.Xor( ), &Hooked::DrawModelExecute, Index::ModelDraw::DrawModelExecute );

		Hooked::oAddBoxOverlay = Hooked::HooksManager.HookVirtual<decltype( Hooked::oAddBoxOverlay )>( m_pDebugOverlay.Xor( ), &Hooked::AddBoxOverlay, 1 );

		//Hooked::oRetrieveMessage = Hooked::HooksManager.HookVirtual<decltype( Hooked::oRetrieveMessage )>( g_pSteamGameCoordinator.Xor( ), &Hooked::RetrieveMessage, 2 );

		//Hooked::oWriteUsercmdDeltaToBuffer = Hooked::HooksManager.HookVirtual<decltype( Hooked::oWriteUsercmdDeltaToBuffer )>( m_pClient.Xor( ), &Hooked::WriteUsercmdDeltaToBuffer, 24 );

		// detours
		static auto CalcViewBobAddr = Memory::Scan( XorStr( "client.dll" ), XorStr( "55 8B EC A1 ? ? ? ? 83 EC 10 56 8B F1 B9" ) );
		Hooked::oCalcViewBob = Hooked::HooksManager.CreateHook<decltype( Hooked::oCalcViewBob ) >( &Hooked::CalcViewBob, ( void* )CalcViewBobAddr );

		static auto CheckAchievementsEnabledAddr = Memory::Scan( XorStr( "client.dll" ), XorStr( "A1 ? ? ? ? 56 8B F1 B9 ? ? ? ? FF 50 34 85 C0 0F 85" ) );
		Hooked::oCheckAchievementsEnabled = Hooked::HooksManager.CreateHook<decltype( Hooked::oCheckAchievementsEnabled ) >( &Hooked::CheckAchievementsEnabled, ( void* )CheckAchievementsEnabledAddr );

		//auto ReportHitAddr = Memory::Scan( XorStr( "client.dll" ), XorStr( "55 8B EC 8B 55 08 83 EC 1C F6 42 1C 01" ) );
		//Hooked::oReportHit = Hooked::HooksManager.CreateHook<decltype( Hooked::oReportHit ) >( &Hooked::ReportHit, ( void* )ReportHitAddr );

		auto CL_MoveAddr = Memory::Scan( XorStr( "engine.dll" ), XorStr( "55 8B EC 81 EC ?? ?? ?? ?? 53 56 57 8B 3D ?? ?? ?? ?? 8A" ) );
		Hooked::oCL_Move = Hooked::HooksManager.CreateHook<decltype( Hooked::oCL_Move ) >( &Hooked::CL_Move, ( void* )CL_MoveAddr );

		auto C_BaseAnimating__DrawModelAddr = Memory::CallableFromRelative( Memory::Scan( XorStr( "client.dll" ), XorStr( "E8 ? ? ? ? 89 44 24 20 8B 06" ) ) );
		Hooked::oC_BaseAnimating__DrawModel = Hooked::HooksManager.CreateHook<decltype( Hooked::oC_BaseAnimating__DrawModel ) >( &Hooked::C_BaseAnimating__DrawModel, ( void* )C_BaseAnimating__DrawModelAddr );

		auto SendDatagramAddr = Memory::Scan( XorStr( "engine.dll" ), XorStr( "55 8B EC 83 E4 F0 B8 ? ? ? ? E8 ? ? ? ? 56 57 8B F9 89 7C 24 18" ) );
		// Hooked::oSendDatagram = Hooked::HooksManager.CreateHook<decltype( Hooked::oSendDatagram ) >( &Hooked::SendDatagram, ( void* )SendDatagramAddr );

		// auto ProcessPacketAddr = Memory::Scan( XorStr( "engine.dll" ), XorStr( "55 8B EC 83 E4 C0 81 EC ? ? ? ? 53 56 57 8B 7D 08 8B D9" ) );;
		// Hooked::oProcessPacket = Hooked::HooksManager.CreateHook<decltype( Hooked::oProcessPacket ) >( &Hooked::ProcessPacket, ( void* )ProcessPacketAddr );

		Hooked::oModifyEyePosition = Hooked::HooksManager.CreateHook<decltype( Hooked::oModifyEyePosition ) >( &Hooked::ModifyEyePosition, ( void* )Engine::Displacement.Data.m_ModifyEyePos );

		//auto PhysicsSimulateAddr = Memory::CallableFromRelative( Memory::Scan( XorStr( "client.dll" ), XorStr( "E8 ? ? ? ? 80 BE ? ? ? ? ? 0F 84 ? ? ? ? 8B 06" ) ) );
		//Hooked::oPhysicsSimulate = Hooked::HooksManager.CreateHook<decltype( Hooked::oPhysicsSimulate ) >( &Hooked::PhysicsSimulate, ( void* )PhysicsSimulateAddr );

		//auto pFileSystem = **reinterpret_cast< void*** >( Memory::Scan( XorStr( "engine.dll" ), XorStr( "8B 0D ? ? ? ? 8D 95 ? ? ? ? 6A 00 C6" ) ) + 0x2 );
		//Hooked::oLooseFileAllowed = Hooked::HooksManager.HookVirtual<decltype( Hooked::oLooseFileAllowed )>( pFileSystem, &Hooked::LooseFileAllowed, 128 );

		//auto CheckFileCRCsWithServerAddr = reinterpret_cast< void* >( Memory::Scan( XorStr( "engine.dll" ), XorStr( "55 8B EC 81 EC ? ? ? ? 53 8B D9 89 5D F8 80" ) ) );
		//Hooked::oCheckFileCRCsWithServer = Hooked::HooksManager.CreateHook<decltype( Hooked::oCheckFileCRCsWithServer ) >( &Hooked::CheckFileCRCsWithServer, ( void* )CheckFileCRCsWithServerAddr );

		//auto StandardBlendingRulesAddr = Memory::Scan( XorStr( "client.dll" ), XorStr( "55 8B EC 83 E4 F0 B8 ? ? ? ? E8 ? ? ? ? 56 8B 75 08 57 8B F9 85 F6" ) );
		//Hooked::oStandardBlendingRules = Hooked::HooksManager.CreateHook<decltype( Hooked::oStandardBlendingRules ) >( &Hooked::StandardBlendingRules, ( void* )StandardBlendingRulesAddr );
		//
		//auto BuildTransformationsAddr = Memory::Scan( XorStr( "client.dll" ), XorStr( "55 8B EC 83 E4 F0 81 EC ? ? ? ? 56 57 8B F9 8B 0D ? ? ? ? 89 7C 24 1C" ) );
		//Hooked::oBuildTransformations = Hooked::HooksManager.CreateHook<decltype( Hooked::oBuildTransformations ) >( &Hooked::BuildTransformations, ( void* )BuildTransformationsAddr );

		auto DoProceduralFootPlantAddr = Memory::Scan( XorStr( "client.dll" ), XorStr( "55 8B EC 83 E4 F0 83 EC 78 56 8B F1 57 8B 56" ) );
		Hooked::oDoProceduralFootPlant = Hooked::HooksManager.CreateHook<decltype( Hooked::oDoProceduralFootPlant )>( &Hooked::DoProceduralFootPlant, ( void* )DoProceduralFootPlantAddr );

		auto ShouldSkipAnimationFrame = ( Memory::CallableFromRelative( Memory::Scan( XorStr( "client.dll" ), XorStr( "E8 ? ? ? ? 88 44 24 0B" ) ) ) );
		Hooked::oShouldSkipAnimationFrame = Hooked::HooksManager.CreateHook<decltype( Hooked::oShouldSkipAnimationFrame )>( &Hooked::ShouldSkipAnimationFrame, ( void* )ShouldSkipAnimationFrame );

		auto IsUsingStaticPropDebugModeAddr = Memory::CallableFromRelative( Memory::Scan( XorStr( "engine.dll" ), XorStr( "E8 ?? ?? ?? ?? 84 C0 8B 45 08" ) ) );
		Hooked::oIsUsingStaticPropDebugMode = Hooked::HooksManager.CreateHook<decltype( Hooked::oIsUsingStaticPropDebugMode ) >( &Hooked::IsUsingStaticPropDebugMode, ( void* )IsUsingStaticPropDebugModeAddr );

		static auto UpdateClientSideAnimationAddr = Memory::Scan( XorStr( "client.dll" ), XorStr( "55 8B EC 51 56 8B F1 80 BE ? ? ? ? ? 74 36" ) );
		Hooked::oUpdateClientSideAnimation = Hooked::HooksManager.CreateHook<decltype( Hooked::oUpdateClientSideAnimation ) >( &Hooked::UpdateClientSideAnimation, ( void* )UpdateClientSideAnimationAddr );

		//static auto VertexBufferLockAddr = Memory::Scan( XorStr( "shaderapidx9.dll" ), XorStr( "55 8B EC 83 E4 F8 81 EC ? ? ? ? 53 56 57" ) );
		//Hooked::oVertexBufferLock = Hooked::HooksManager.CreateHook<decltype( Hooked::oVertexBufferLock ) >( &Hooked::VertexBufferLock, ( void* )VertexBufferLockAddr );

		auto ClientStateAddr = ( void* )( uintptr_t( m_pClientState.Xor( ) ) + 0x8 );
		Hooked::oPacketStart = Hooked::HooksManager.HookVirtual<decltype( Hooked::oPacketStart )>( ClientStateAddr, &Hooked::PacketStart, 5 );
		Hooked::oPacketEnd = Hooked::HooksManager.HookVirtual<decltype( Hooked::oPacketEnd )>( ClientStateAddr, &Hooked::PacketEnd, 6 );
		Hooked::oProcessTempEntities = Hooked::HooksManager.HookVirtual<decltype( Hooked::oProcessTempEntities )>( ClientStateAddr, &Hooked::ProcessTempEntities, 36 );;

		Hooked::oSendNetMsg = Hooked::HooksManager.CreateHook<decltype( Hooked::oSendNetMsg ) >( &Hooked::SendNetMsg, ( void* )Engine::Displacement.Data.m_SendNetMsg );

		Hooked::oInterpolateServerEntities = Hooked::HooksManager.CreateHook<decltype( Hooked::oInterpolateServerEntities ) >( &Hooked::InterpolateServerEntities, ( void* )Engine::Displacement.Data.m_InterpolateServerEntities );
		Hooked::oProcessInterpolatedList = Hooked::HooksManager.CreateHook<decltype( Hooked::oProcessInterpolatedList ) >( &Hooked::ProcessInterpolatedList, ( void* )Engine::Displacement.Data.m_ProcessInterpolatedList );

		Hooked::oPresent = Hooked::HooksManager.HookVirtual<decltype( Hooked::oPresent )>( D3DDevice9, &Hooked::Present, Index::DirectX::Present );
		Hooked::oReset = Hooked::HooksManager.HookVirtual<decltype( Hooked::oReset )>( D3DDevice9, &Hooked::Reset, Index::DirectX::Reset );

		//Hooked::oIsRenderableInPvs = Hooked::HooksManager.HookVirtual<decltype( Hooked::oIsRenderableInPvs ) >( m_pClientLeafSystem.Xor( ), &Hooked::IsRenderableInPVS, 8 );

		//static auto CHudElement__ShouldDrawaddr = Memory::CallableFromRelative( Memory::Scan( XorStr( "client.dll" ), XorStr( "E8 ? ? ? ? 84 C0 74 F3" ) ) );
		//oCHudElement__ShouldDraw = Hooked::HooksManager.CreateHook<decltype( oCHudElement__ShouldDraw )>( &CHudElement__ShouldDraw, ( void* )CHudElement__ShouldDrawaddr );

		//static auto CHudScope__Paintaddr = Memory::Scan( XorStr( "client.dll" ), XorStr( "55 8B EC 83 E4 F8 83 EC 78 56 57 8B 3D" ) );
		//oCHudScope__Paint = Hooked::HooksManager.CreateHook<decltype( oCHudScope__Paint )>( &CHudScope__Paint, ( void* )CHudScope__Paintaddr );

		Hooked::HooksManager.Enable( );
		g_Vars.globals.menuOpen = true;

		return true;
	}

	void Destroy( ) {
		Hooked::HooksManager.Restore( );

		CCSPlayerClass->m_pCreateFn = oCreateCCSPlayer;
		Hooked::player_hooks.clear( );

		g_GameEvent.Shutdown( );
		g_SkinChanger.Destroy( );
		g_InputSystem.Destroy( );

		MH_Uninitialize( );

		m_pInputSystem->EnableInput( true );
	}

	void* CreateInterface( const std::string& image_name, const std::string& name ) {
#ifndef DEV
		char buf[ 128 ] = { 0 };
		wsprintfA( buf, XorStr( "%s::%s" ), image_name.data( ), name.data( ) );
		auto iface = interfaces_get_interface( loader_hash( buf ) );

		if( !iface )
			MessageBoxA( 0, buf, buf, 0 );

		return reinterpret_cast< void* >( iface );
#else
		auto image = GetModuleHandleA( image_name.c_str( ) );
		if( !image )
			return nullptr;

		auto fn = ( CreateInterfaceFn )( GetProcAddress( image, XorStr( "CreateInterface" ) ) );
		if( !fn )
			return nullptr;

		return fn( name.c_str( ), nullptr );
#endif
	}
}