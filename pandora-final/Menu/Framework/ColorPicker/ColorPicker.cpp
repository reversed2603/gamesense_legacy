#include "../gui.h"
#include "../../../source.hpp"

#include "../../../Features/Visuals/EventLogger.hpp"

void GUI::Controls::ColorPicker( const std::string& name, Color_f* var_name ) {
	Vector2D CursorPos = GetLastCursorPos( );
	Vector2D DrawPos = ctx->pos + CursorPos;

	DrawPos.y -= 1;
	DrawPos.x += ctx->ParentSize.x - 60;

	Render::DirectX::rect( DrawPos, Vector2D( 16, 8 ), Color( 10, 10, 10 ) );

	Render::DirectX::gradient_v( DrawPos + 1, Vector2D( 16, 8 ) - 1, var_name->ToRegularColor( ).OverrideAlpha( 255 ), var_name->ToRegularColor( ).OverrideAlpha( 255 ) * 0.9f );

	bool hovered = g_InputSystem.IsInBox( DrawPos, Vector2D( 16, 8 ) ) && g_InputSystem.IsInBox( ctx->ParentPos, ctx->ParentSize );

	if( ctx->FocusedID == 0 ) {
		if( hovered ) {
			if( InputHelper::Pressed( VK_LBUTTON ) )
				ctx->FocusedID = GUI::Hash( name );
			else if( InputHelper::Pressed( VK_RBUTTON ) )
				ctx->FocusedID = GUI::Hash( name ) + 69;
		}
	}
	else if( ctx->FocusedID == GUI::Hash( name ) ) {
		if( hovered ) {
			ctx->ColorPickerInfo.ActionCringe = 0;
			ctx->ColorPickerInfo.HashedID = GUI::Hash( name );
			ctx->ColorPickerInfo.Pos = DrawPos + Vector2D( -1, 14 );
			ctx->ColorPickerInfo.Size = 180;
			ctx->ColorPickerInfo.Value = var_name;
		}
	}
	else if( ctx->FocusedID == GUI::Hash( name ) + 69 ) {
		if( hovered ) {
			ctx->ColorPickerInfo.ActionCringe = 1;
			ctx->ColorPickerInfo.HashedID = GUI::Hash( name ) + 69;
			ctx->ColorPickerInfo.Pos = DrawPos + Vector2D( -1, 14 );
			ctx->ColorPickerInfo.Size = 100;
			ctx->ColorPickerInfo.Value = var_name;
		}
	}
}