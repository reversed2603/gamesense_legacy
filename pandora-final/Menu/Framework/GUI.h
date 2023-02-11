#pragma once
#include <stack>

#include "../../Utils/InputSys.hpp"
#include "../../SDK/sdk.hpp"
#include "../Helpers/InputHelper.h"

/* Prototypes */
struct MultiItem_t {
	std::string name;
	bool* value;
};

/* Context structs */
struct DropdownInfo_t {
	size_t HashedID;

	std::map<size_t, float> uScroll;

	std::vector<std::string> Elements;
	int* Option;

	int MaxItems;

	bool out_anim;
	bool DraggingScroll;

	float Size = 0.f;
	Vector2D Pos = { 0, 0 };
};

struct MultiDropdownInfo_t {
	size_t HashedID;

	std::map<size_t, float> uScroll;

	std::vector<MultiItem_t> Elements;

	int MaxItems;
	bool DraggingScroll;

	float Size = 0.f;
	Vector2D Pos = { 0, 0 };
};

struct HotkeyInfo_t {
	size_t id;
	bool open = false;

	Vector2D Pos = { 0, 0 };
};

struct ColorPickerInfo_t {
	size_t HashedID;

	Color_f* Value;

	bool PickingAlpha = false;
	bool PickingColor = false;
	bool PickingHue = false;

	bool CopyingColor = false;
	bool PastingColor = false;

	int ActionCringe;

	int Size = 0.f;
	Vector2D Pos = { 0, 0 };
};

struct SliderInfo_t {
	std::map<size_t, float> ValueAnimation;
	std::map<size_t, float> ValueTimer;

	std::map<size_t, float> PreviewAnimation;
	std::map<size_t, float> PreviousAmount;

	std::map<size_t, float> LastChangeTime;

	std::map<size_t, std::pair<float, bool>> ShouldChangeValue;
};

/* "Object" structs */
struct MenuContext_t {
	Vector2D pos = { 100, 100 };

	// 660, 550
	Vector2D size = { 648, 538 };

	Vector2D NextGroupPos;
	std::stack< Vector2D > CursorPosStack;

	std::vector< std::string > Tabs;
	int ActiveTab = 0;

	DropdownInfo_t DropdownInfo;
	MultiDropdownInfo_t MultiDropdownInfo;
	ColorPickerInfo_t ColorPickerInfo;
	HotkeyInfo_t HotkeyInfo;
	SliderInfo_t SliderInfo;

	Vector2D ParentPos;
	Vector2D ParentSize;

	std::string parent;
	size_t FocusedID = 0;

	std::string CurrentTab;
	std::string CurrentSubTab;
	std::string CurrentGroup;
	std::string CurrentWeaponGroup;

	bool dragging = false;
	bool resizing = false;
	//bool setup = true;
	bool setup = false;
	bool typing = false;
	bool hovered_listbox = false;
	bool dragging_scrollbar = false;

	float animation = 0.f;
};

namespace GUI {
	inline MenuContext_t* ctx = new MenuContext_t( );

	// modify this if you'd like to change the spacing between each object
	inline int ObjectPadding( ) {
		constexpr int spacing{ 10 };

		return spacing /*+ 1*/;
	};

	Vector2D PopCursorPos( );
	Vector2D GetLastCursorPos( );
	void PushCursorPos( const Vector2D& cursor_pos );

	size_t Hash( const std::string& name );

	float AnimationInterval( float interval = 0.035f );

	template < typename T = float >
	inline float Approach( float a, float b, float multiplier ) {
		return ( a + static_cast< T >( multiplier * ( b - a ) ) );
	}

	float MapNumber( float input, float input_min, float input_max, float output_min, float output_max );
	std::vector<std::string> SplitStr( const std::string& str, char separator );
}

#include "form/form.h"
#include "group/group.h"
#include "checkbox/checkbox.h"
#include "button/button.h"
#include "label/label.h"
#include "slider/slider.h"
#include "dropdown/dropdown.h"
#include "colorpicker/colorpicker.h"
#include "hotkey/hotkey.h"
#include "multidropdown/multidropdown.h"
#include "Listbox/Listbox.h"
#include "Textbox/Textbox.h"