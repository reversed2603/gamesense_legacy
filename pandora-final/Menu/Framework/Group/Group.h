#pragma once

namespace GUI::Group {
	void BeginGroup( const std::string& name, const Vector2D& size );
	void EndGroup( );

	void SetNextSize( const Vector2D& size );
	void PopLastSize( );
}
