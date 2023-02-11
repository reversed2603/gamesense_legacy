#pragma once
#include <vector>
#include <map>

#include "../../SDK/variables.hpp"

struct paint_kit {
	int id;
	std::string name;
	std::string_view item_name;

	auto operator<( const paint_kit& other ) const -> bool {
		return name < other.name;
	}

	auto operator==( int _id ) const -> bool {
		return id == _id;
	}
};

enum WeaponGroup_t {
	WEAPONGROUP_PISTOL,
	WEAPONGROUP_HEAVYPISTOL,
	WEAPONGROUP_RIFLE,
	WEAPONGROUP_SNIPER,
	WEAPONGROUP_AUTOSNIPER,
	WEAPONGROUP_SUBMACHINE,
	WEAPONGROUP_HEAVY,
	WEAPONGROUP_SHOTGUN,
	WEAPONGROUP_MAX
};

class KitParser {
	enum LoadoutSlot {
		LOADOUT_POSITION_MELEE,
		LOADOUT_POSITION_C4,
		LOADOUT_POSITION_SECONDARY0,
		LOADOUT_POSITION_SECONDARY1,
		LOADOUT_POSITION_SECONDARY2,
		LOADOUT_POSITION_SECONDARY3,
		LOADOUT_POSITION_SECONDARY4,
		LOADOUT_POSITION_SECONDARY5,
		LOADOUT_POSITION_SMG0,
		LOADOUT_POSITION_SMG1,
		LOADOUT_POSITION_SMG2,
		LOADOUT_POSITION_SMG3,
		LOADOUT_POSITION_SMG4,
		LOADOUT_POSITION_SMG5,
		LOADOUT_POSITION_RIFLE0,
		LOADOUT_POSITION_RIFLE1,
		LOADOUT_POSITION_RIFLE2,
		LOADOUT_POSITION_RIFLE3,
		LOADOUT_POSITION_RIFLE4,
		LOADOUT_POSITION_RIFLE5,
		LOADOUT_POSITION_HEAVY0,
		LOADOUT_POSITION_HEAVY1,
		LOADOUT_POSITION_HEAVY2,
		LOADOUT_POSITION_HEAVY3,
		LOADOUT_POSITION_HEAVY4,
		LOADOUT_POSITION_HEAVY5,
		LOADOUT_POSITION_GRENADE0,
		LOADOUT_POSITION_GRENADE1,
		LOADOUT_POSITION_GRENADE2,
		LOADOUT_POSITION_GRENADE3,
		LOADOUT_POSITION_GRENADE4,
		LOADOUT_POSITION_GRENADE5,
		LOADOUT_POSITION_EQUIPMENT0,
		LOADOUT_POSITION_EQUIPMENT1,
		LOADOUT_POSITION_EQUIPMENT2,
		LOADOUT_POSITION_EQUIPMENT3,
		LOADOUT_POSITION_EQUIPMENT4,
		LOADOUT_POSITION_EQUIPMENT5,
		LOADOUT_POSITION_SPACER1,
		LOADOUT_POSITION_SPACER2,
		LOADOUT_POSITION_SPACER3,
		LOADOUT_POSITION_SPACER4,
		LOADOUT_POSITION_SPACER5,
		LOADOUT_POSITION_SPACER6,
		LOADOUT_POSITION_MISC0,
		LOADOUT_POSITION_MISC1,
		LOADOUT_POSITION_MISC2,
		LOADOUT_POSITION_MISC3,
		LOADOUT_POSITION_MISC4,
		LOADOUT_POSITION_MISC5,
		LOADOUT_POSITION_MISC6,
		LOADOUT_POSITION_MISC7,
		LOADOUT_POSITION_MISC8,
		LOADOUT_POSITION_MISC9,
		LOADOUT_POSITION_SPACER7,
		LOADOUT_POSITION_FLAIR0,
	};

	struct item_cstrike {
		bool glove = false;
		bool knife = false;
		int group;
		std::string_view name;
		std::string display_name;
		std::string model_name;
	};

	// Stupid MSVC requires separate constexpr constructors for any initialization
	struct Item_t {
		Item_t( ) { }

		constexpr Item_t( const char* model, const char* icon = nullptr ) :
			model( model ),
			icon( icon ) {
		}

		const char* model;
		const char* icon;
	};

	struct item_skins {
		int id;
		int group;
		std::vector< paint_kit > m_kits;
		std::string_view name;
		std::string display_name;
		std::string model_name;
		bool glove;
		bool knife;

		item_skins( ) {
			paint_kit default_kit;
			default_kit.id = 0;
			default_kit.item_name = XorStr( "#PaintKit_Default" );
			default_kit.name = XorStr( "-" );
			m_kits.push_back( default_kit );
		}

		auto
			operator==( int _id ) const -> bool {
			return id == _id;
		}
	};

public:
	std::vector< paint_kit > all_skins;
	std::vector< paint_kit > skin_kits;
	std::vector< paint_kit > glove_kits;
	std::vector< item_skins > weapon_skins;
	std::vector< WeaponName_t > k_knife_names;
	std::vector< WeaponName_t > k_glove_names;
	std::map< size_t, Item_t > k_weapon_info;
	void initialize_kits( );
};

extern KitParser g_KitParser;
