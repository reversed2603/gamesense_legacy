#include "gui.h"
#include "../../source.hpp"

Vector2D last_cursor_pos;

Vector2D GUI::PopCursorPos( ) {
	if( ctx->CursorPosStack.empty( ) )
		return Vector2D( );

	Vector2D ret = ctx->CursorPosStack.top( );
	ctx->CursorPosStack.pop( );
	last_cursor_pos = ret;
	return ret;
}

Vector2D GUI::GetLastCursorPos( ) {
	return last_cursor_pos;
}

void GUI::PushCursorPos( const Vector2D& cursor_pos ) {
	ctx->CursorPosStack.push( cursor_pos );
}

size_t GUI::Hash( const std::string& name ) {
	return std::hash< std::string >( )( name );
}

float GUI::AnimationInterval( float interval ) {
	return ( 1.0f / interval ) * m_pGlobalVars->frametime;
}

float GUI::MapNumber( float input, float input_min, float input_max, float output_min, float output_max ) {
	return ( input - input_min ) / ( input_max - input_min ) * ( output_max - output_min ) + output_min;
}

std::vector<std::string> GUI::SplitStr( const std::string& str, char separator ) {
	std::vector<std::string> output;
	std::string::size_type prev_pos = 0, pos = 0;

	while( ( pos = str.find( separator, pos ) ) != std::string::npos ) {
		std::string substring( str.substr( prev_pos, pos - prev_pos ) );
		output.push_back( substring );

		prev_pos = pos++;
	}

	output.push_back( str.substr( prev_pos, pos - prev_pos ) );
	return output;
}
