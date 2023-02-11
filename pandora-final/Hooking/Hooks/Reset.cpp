#include "../Hooked.hpp"
#include "../../Renderer/Render.hpp"

HRESULT __stdcall Hooked::Reset( IDirect3DDevice9* pDevice, D3DPRESENT_PARAMETERS* pPresentationParameters ) {
	g_Vars.globals.szLastHookCalled = XorStr( "31" );

	Render::Engine::Invalidate( );
	Render::DirectX::invalidate( );
	auto ret = oReset( pDevice, pPresentationParameters );

	if( ret == D3D_OK ) {
		Render::DirectX::init( pDevice );
		Render::Engine::Initialise( );
	}

	return ret;
}