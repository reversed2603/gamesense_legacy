#define NOMINMAX
#include <Windows.h>
#include <cstdint>
#include <map>
#include <memory>
#include <stdexcept>
#include "InputSys.hpp"
#include "../source.hpp"

#include "../SDK/variables.hpp"
#include "../Utils/LogSystem.hpp"

InputSys g_InputSystem;

void* InputSys::GetMainWindow( ) const {
	return ( void* )m_hTargetWindow; 
}

Vector2D InputSys::GetMousePosition( ) {
	return m_MousePos;
}

float InputSys::GetScrollMouse( ) {
	return m_ScrollMouse;
}

void InputSys::SetScrollMouse( float scroll ) {
	m_ScrollMouse = scroll;
}

InputSys::InputSys( ) :
	m_hTargetWindow( nullptr ), m_ulOldWndProc( 0 ) {
}

InputSys::~InputSys( ) {
}

bool InputSys::Initialize( IDirect3DDevice9* pDevice ) {
	D3DDEVICE_CREATION_PARAMETERS params;

	if( FAILED( pDevice->GetCreationParameters( &params ) ) ) {
		Win32::Error( XorStr( "GetCreationParameters failed" ) );
		return false;
	}

	Interfaces::hWindow = m_hTargetWindow = params.hFocusWindow;

	m_ulOldWndProc = SetWindowLongPtr( m_hTargetWindow, GWLP_WNDPROC, ( LONG_PTR )WndProc );

	if( !m_ulOldWndProc ) {

		Win32::Error( XorStr( "SetWindowLongPtr failed" ) );
		return false;
	}

	return true;
}

void InputSys::Destroy( ) {
	if( m_ulOldWndProc )
		SetWindowLongPtr( m_hTargetWindow, GWLP_WNDPROC, m_ulOldWndProc );
	m_ulOldWndProc = 0;
}

LRESULT __stdcall InputSys::WndProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam ) {
	//auto win32input = static_cast< InputSys* >( Get( ).Xor( ) );
	if( !g_Vars.globals.m_bChatOpen && !g_Vars.globals.m_bConsoleOpen ) {
		g_InputSystem.ProcessMessage( msg, wParam, lParam );
	}

	if( msg == WM_MOUSEMOVE ) {
		g_InputSystem.m_MousePos.x = ( signed short )( lParam );
		g_InputSystem.m_MousePos.y = ( signed short )( lParam >> 16 );
	}
	else if( msg == WM_MOUSEWHEEL || msg == WM_MOUSEHWHEEL ) {
		g_InputSystem.m_ScrollMouse += ( float )GET_WHEEL_DELTA_WPARAM( wParam ) / ( float )WHEEL_DELTA;
	}

	if( g_Vars.globals.menuOpen ) {
		if( msg == WM_MOUSEWHEEL || msg == WM_MOUSEHWHEEL )
			return true;
	}

	return CallWindowProc( ( WNDPROC )g_InputSystem.m_ulOldWndProc, hWnd, msg, wParam, lParam );
}

bool InputSys::ProcessMessage( UINT uMsg, WPARAM wParam, LPARAM lParam ) {
	switch( uMsg ) {
	case WM_MBUTTONDBLCLK:
	case WM_RBUTTONDBLCLK:
	case WM_LBUTTONDBLCLK:
	case WM_XBUTTONDBLCLK:
	case WM_MBUTTONDOWN:
	case WM_RBUTTONDOWN:
	case WM_LBUTTONDOWN:
	case WM_XBUTTONDOWN:
	case WM_MBUTTONUP:
	case WM_RBUTTONUP:
	case WM_LBUTTONUP:
	case WM_XBUTTONUP:
		return ProcessMouseMessage( uMsg, wParam, lParam );
	case WM_KEYDOWN:
	case WM_KEYUP:
	case WM_SYSKEYDOWN:
	case WM_SYSKEYUP:
		return ProcessKeybdMessage( uMsg, wParam, lParam );
	default:
		return false;
	}
}

bool InputSys::ProcessMouseMessage( UINT uMsg, WPARAM wParam, LPARAM lParam ) {
	auto key = VK_LBUTTON;
	auto state = KeyState::None;

	switch( uMsg ) {
	case WM_MBUTTONDOWN:
	case WM_MBUTTONUP:
		state = uMsg == WM_MBUTTONUP ? KeyState::Up : KeyState::Down;
		key = VK_MBUTTON;
		break;
	case WM_RBUTTONDOWN:
	case WM_RBUTTONUP:
		state = uMsg == WM_RBUTTONUP ? KeyState::Up : KeyState::Down;
		key = VK_RBUTTON;
		break;
	case WM_LBUTTONDOWN:
	case WM_LBUTTONUP:
		state = uMsg == WM_LBUTTONUP ? KeyState::Up : KeyState::Down;
		key = VK_LBUTTON;
		break;
	case WM_XBUTTONDOWN:
	case WM_XBUTTONUP:
		state = uMsg == WM_XBUTTONUP ? KeyState::Up : KeyState::Down;
		key = ( HIWORD( wParam ) == XBUTTON1 ? VK_XBUTTON1 : VK_XBUTTON2 );
		break;
	default:
		return false;
	}

	if( state == KeyState::Up && m_iKeyMap[ key ] == KeyState::Down ) {
		m_iKeyMap[ key ] = KeyState::Pressed;
	}
	else
		m_iKeyMap[ key ] = state;
	return true;
}

bool InputSys::ProcessKeybdMessage( UINT uMsg, WPARAM wParam, LPARAM lParam ) {
	auto key = wParam;
	auto state = KeyState::None;

	switch( uMsg ) {
	case WM_KEYDOWN:
	case WM_SYSKEYDOWN:
		state = KeyState::Down;
		break;
	case WM_KEYUP:
	case WM_SYSKEYUP:
		state = KeyState::Up;
		break;
	default:
		return false;
	}

	if( state == KeyState::Up && m_iKeyMap[ int( key ) ] == KeyState::Down ) {
		m_iKeyMap[ int( key ) ] = KeyState::Pressed;

		auto& hotkey_callback = m_Hotkeys[ key ];

		if( hotkey_callback )
			hotkey_callback( );

	}
	else {
		m_iKeyMap[ int( key ) ] = state;
	}

	return true;
}

KeyState InputSys::GetKeyState( int vk ) {
	return m_iKeyMap[ vk ];
}

bool InputSys::IsKeyDown( int vk ) {
	if( vk <= 0 || vk > 255 )
		return false;

	return m_iKeyMap[ vk ] == KeyState::Down;
}

bool InputSys::IsInBox( Vector2D box_pos, Vector2D box_size ) {
	return (
		m_MousePos.x > box_pos.x &&
		m_MousePos.y > box_pos.y &&
		m_MousePos.x < box_pos.x + box_size.x &&
		m_MousePos.y < box_pos.y + box_size.y
		);
}

bool InputSys::WasKeyPressed( int vk ) {
	if( vk <= 0 || vk > 255 )
		return false;

	if( m_iKeyMap[ vk ] == KeyState::Pressed ) {
		m_iKeyMap[ vk ] = KeyState::Up;
		return true;
	}

	return false;
}

void InputSys::RegisterHotkey( int vk, std::function< void( void ) > f ) {
	m_Hotkeys[ vk ] = f;
}

void InputSys::RemoveHotkey( int vk ) {
	m_Hotkeys[ vk ] = nullptr;
}