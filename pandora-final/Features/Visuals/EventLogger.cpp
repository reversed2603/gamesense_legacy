#include "EventLogger.hpp"
#include "../../source.hpp"
#include "../../Renderer/Render.hpp"

class NotifyText {
public:
	std::string m_text;
	Color		m_color;
	float		m_time;
	float		m_width;
	float		m_height_offset;

public:
	__forceinline NotifyText( const std::string& text, Color color, float time ) : m_text{ text }, m_color{ color }, m_time{ time }, m_width{ 0.f }, m_height_offset{ 0.f }{}
};

std::deque< std::shared_ptr< NotifyText > > m_notify_text;

EventLogger g_EventLog;

void EventLogger::Main( ) {
	int		x{ 8 }, y{ 5 }, size{ Render::Engine::console.m_size.m_height + 1 };
	Color	color;
	float	left;

	// update lifetimes.
	for( size_t i{ }; i < m_notify_text.size( ); ++i ) {
		auto notify = m_notify_text[ i ];

		notify->m_time -= m_pGlobalVars->frametime;

		if( notify->m_time <= 0.f ) {
			m_notify_text.erase( m_notify_text.begin( ) + i );
			continue;
		}
	}

	// we have nothing to draw.
	if( m_notify_text.empty( ) )
		return;

	// clear more than 6 entries.
	if( m_notify_text.size( ) > 6 )
		m_notify_text.pop_front( );

	// iterate entries.
	for( size_t i{ }; i < m_notify_text.size( ); ++i ) {
		auto notify = m_notify_text[ i ];

		if( notify->m_text.find( XorStr( "%%%%" ) ) != std::string::npos )
			notify->m_text.erase( notify->m_text.find( XorStr( "%" ) ), 3 );

		left = notify->m_time;
		color = notify->m_color;

		if( left < .5f ) {
			float f = left;
			f = Math::Clamp( f, 0.f, .5f );

			f /= .5f;

			notify->m_width = ( int )( f * 255.f );

			if( i == 0 && f < 0.2f )
				y -= size * ( 1.f - f / 0.2f );
		}

		else
			notify->m_width = 255;

		Render::Engine::console.string( x, y, Color( 255, 255, 255, notify->m_width ), notify->m_text );
		y += size;
	}
}

void EventLogger::PushEvent( std::string msg, Color_f clr, bool visualise, std::string prefix ) {
	if( visualise ) {
		m_notify_text.push_back( std::make_shared< NotifyText >( msg, clr.ToRegularColor( ), 5.f ) );
	}

	m_pCvar->ConsoleColorPrintf( Color( 150, 200, 60 ), XorStr( "[gamesense] " ) );

	m_pCvar->ConsoleColorPrintf( Color( 220, 220, 220 ), std::string( msg + XorStr( "\n" ) ).c_str( ) );
}
