#include "KitParser.hpp"
#include "../../SDK/sdk.hpp"
#include <algorithm>
#include "../../source.hpp"
#include "../../SDK/Valve/UtlMap.hpp"
#include "../../SDK/variables.hpp"

KitParser g_KitParser;

struct StickerData_t {
	char viewmodel_geometry[ 128 ];
	char viewmodel_material[ 128 ];
	Vector worldmodel_decal_pos;
	Vector worldmodel_decal_end;
	char worldmodel_decal_bone[ 32 ];
};

struct WeaponPaintableMaterial_t {
	char Name[ 128 ];
	char OrigMat[ 128 ];
	char FolderName[ 128 ];
	int ViewmodelDim;
	int WorldDim;
	float WeaponLength;
	float UVScale;
	bool BaseTextureOverride;
	bool MirrorPattern;
};

class CCStrike15ItemSchema;
class CCStrike15ItemSystem;
class AssetInfo;
class IEconTool;
union attribute_data_union_t { bool bValue;  int iValue;  float fValue;  Vector* vValue;  void* szValue; };
struct static_attrib_t // Get a CEconItemAttributeDefinition* with m_Attributes[static_attrib_t.id] 
{
	uint16_t id;  attribute_data_union_t value;  bool force_gc_to_generate;
};

class CCStrike15ItemDefinition
{
public:
	virtual uint16_t GetDefinitionIndex( ) const = 0;
	virtual const char* GetPrefabName( ) const = 0;
	virtual const char* GetItemBaseName( ) const = 0;
	virtual const char* GetItemTypeName( ) const = 0;
	virtual const char* GetItemDesc( ) const = 0;
	virtual const char* GetInventoryImage( ) const = 0;
	virtual const char* GetBasePlayerDisplayModel( ) const = 0;
	virtual const char* GetWorldDisplayModel( ) const = 0;
	virtual const char* GetExtraWearableModel( ) const = 0;
	virtual int GetLoadoutSlot( ) const = 0;
	virtual KeyValues* GetRawDefinition( ) const = 0;
	virtual int GetHeroID( ) const = 0;
	virtual int GetRarity( ) const = 0;
	virtual CUtlVector<int>* GetItemSets( ) const = 0;
	virtual int GetBundleItemCount( ) const = 0;
	virtual void* GetBundleItem( int ) const = 0;
	virtual bool IsBaseItem( ) const = 0;
	virtual bool IsPublicItem( ) const = 0;
	virtual bool IsBundle( ) const = 0;
	virtual bool IsPackBundle( ) const = 0;
	virtual bool IsPackItem( ) const = 0;
	virtual void* BInitVisualBlockFromKV( KeyValues*, void*, void* ) = 0;
	virtual void* BInitFromKV( KeyValues*, void*, void* ) = 0;
	virtual void* BInitFromTestItemKVs( int, KeyValues*, void* ) = 0;
	virtual void* GeneratePrecacheModelStrings( bool, void* ) const = 0;
	virtual void* GeneratePrecacheSoundStrings( void* ) const = 0;
	virtual void* GeneratePrecacheEffectStrings( void* ) const = 0;
	virtual void* CopyPolymorphic( const CCStrike15ItemDefinition* ) = 0;
	virtual int GetItemTypeID( ) const = 0;
	virtual bool IsDefaultSlotItem( ) const = 0;
	virtual bool IsPreviewableInStore( ) const = 0;
	virtual int GetBundleItemPaintKitID( int ) const = 0;
	virtual const char* GetWorldDroppedModel( ) const = 0;
	virtual const char* GetHolsteredModel( ) const = 0;
	virtual const char* GetZoomInSound( ) const = 0;
	virtual const char* GetZoomOutSound( ) const = 0;
	virtual const char* GetIconDisplayModel( ) const = 0;
	virtual const char* GetBuyMenuDisplayModel( ) const = 0;
	virtual const char* GetPedestalDisplayModel( ) const = 0;
	virtual const char* GetMagazineModel( ) const = 0;
	virtual const char* GetScopeLensMaskModel( ) const = 0;
	virtual const char* GetUidModel( ) const = 0;
	virtual const char* GetStatTrakModelByType( unsigned int ) const = 0;
	virtual int GetNumSupportedStickerSlots( ) const = 0;
	virtual const char* GetStickerSlotModelBySlotIndex( unsigned int ) const = 0;
	virtual void* GetStickerSlotWorldProjectionStartBySlotIndex( unsigned int ) const = 0;
	virtual void* GetStickerSlotWorldProjectionEndBySlotIndex( unsigned int ) const = 0;
	virtual void* GetStickerWorldModelBoneParentNameBySlotIndex( unsigned int ) const = 0;
	virtual void* GetStickerSlotMaterialBySlotIndex( unsigned int ) const = 0;
	virtual const char* GetIconDefaultImage( ) const = 0;
	virtual void* GetParticleFile( ) const = 0;
	virtual void* GetParticleSnapshotFile( ) const = 0;
	virtual bool IsRecent( ) const = 0;
	virtual bool IsContentStreamable( ) const = 0;
	virtual void* IgnoreInCollectionView( ) const = 0;
	virtual void* GeneratePrecacheModelStrings( bool, void* ) = 0;

public:
	KeyValues* kv;
	uint16_t id;
	CUtlVector<uint16_t> associated_items;

private:
	int32_t pad0[ 2 ];

public:
	uint8_t min_ilevel;
	uint8_t max_ilevel;
	uint8_t item_rarity;
	uint8_t item_quality;
	uint8_t forced_item_quality;
	uint8_t default_drop_quality;
	uint8_t default_drop_quantity;
	CUtlVector<static_attrib_t> attributes;
	uint8_t popularity_seed;
	KeyValues* portraits;
	char* item_name;
	bool propername;
	char* item_type_name;
	int type_id; // crc32( item_type_name )
	char* item_description;
	int expiration_date;
	int creation_date;
	char* model_inventory;
	char* image_inventory;
	CUtlVector<const char*> image_inventory_overlay;
	int image_inventory_pos_x;
	int image_inventory_pos_y;
	int image_inventory_size_w;
	int image_inventory_size_h;
	char* model_player;

private:
	bool pad1;

public:
	bool hide_bodygroups_deployed_only;
	char* model_world;
	char* model_dropped;
	char* model_holstered;
	char* extra_wearable;

private:
	int32_t pad2[ 20 ];

public:
	CUtlVector<StickerData_t> stickers;
	char* icon_default_image;
	bool attach_to_hands;
	bool attach_to_hands_vm_only;
	bool flip_viewmodel;
	bool act_as_wearable;
	CUtlVector<int> item_sets;
	AssetInfo* visuals;

private:
	int32_t pad3;

public:
	bool allow_purchase_standalone;
	char* brass_eject_model;
	char* zoom_in_sound;
	char* zoom_out_sound;

	IEconTool* tool;

private:
	int32_t pad4[ 3 ];

public:
	int sound_material;
	bool disable_style_selector;

private:
	int32_t pad5[ 8 ];

public:
	char* particle_file;
	char* particle_snapshot;
	char* loot_list_name;

private:
	int32_t pad6[ 5 ];

public:
	CUtlVector<WeaponPaintableMaterial_t> paint_data;

	struct
	{
		QAngle* camera_angles;
		Vector* camera_offset;
		float camera_fov;
		LightDesc_t* lights[ 4 ];

	private:
		int32_t pad0;
	} *inventory_image_data;

	char* item_class;
	char* item_logname;
	char* item_iconname;
	char* name;
	bool hidden;
	bool show_in_armory;
	bool baseitem;
	bool default_slot_item;
	bool import;
	bool one_per_account_cdkey;

private:
	int32_t pad7;

public:
	char* armory_desc;
	CCStrike15ItemDefinition* armory_remapdef;
	CCStrike15ItemDefinition* store_remapdef;
	char* armory_remap;
	char* store_remap;
	char* class_token_id;
	char* slot_token_id;
	int drop_type;
	int holiday_restriction;
	int subtype;

private:
	int32_t pad8[ 4 ];

public:
	CUtlMap<unsigned int, const char*, unsigned short> alternate_icons;

private:
	int32_t pad9[ 9 ];

public:
	bool not_developer;
	bool ignore_in_collection_view;

	// This is where CCStrike15ItemDefinition begins
	int item_sub_position;
	int item_gear_slot;
	int item_gear_slot_position;
	int anim_slot;
	char* model_player_per_class[ 4 ];
	int class_usage[ 4 ];

private:
	int32_t pad10[ 2 ];
};
template < typename Key, typename Value >
struct Node_t {
	int previous_id;    //0x0000
	int next_id;        //0x0004
	void* _unknown_ptr; //0x0008
	int _unknown;       //0x000C
	Key key;            //0x0010
	Value value;        //0x0014
};

template < typename Key, typename Value >
struct Head_t {
	Node_t< Key, Value >* memory; //0x0000
	int allocation_count;         //0x0004
	int grow_size;                //0x0008
	int start_element;            //0x000C
	int next_available;           //0x0010
	int _unknown;                 //0x0014
	int last_element;             //0x0018
};                              //Size=0x001C

								// could use CUtlString but this is just easier and CUtlString isn't needed anywhere else
struct String_t {
	char* buffer;  //0x0000
	int capacity;  //0x0004
	int grow_size; //0x0008
	int length;    //0x000C
};               //Size=0x0010

struct CPaintKit {
	int id; //0x0000

	String_t name;            //0x0004
	String_t description;     //0x0014
	String_t item_name;       //0x0024
	String_t material_name;   //0x0034
	String_t image_inventory; //0x0044

	char pad_0x0054[ 0x8C ]; //0x0054
};                       //Size=0x00E0

struct AlternateIconData_t {
private:
	int32_t pad0;

public:
	String_t icon_path;
	String_t icon_path_large;

private:
	String_t pad1[ 2 ];
};

struct item_list_entry_t {
	int item;
	int paintkit;

private:
	int32_t pad0[ 5 ];
};

class CEconItemSetDefinition {
public:
	virtual ~CEconItemSetDefinition( ) { };

	char* key_name;
	char* name;
	char* unlocalized_name;
	char* set_description;
	CUtlVector< item_list_entry_t > items;
	int store_bundle;
	bool is_collection;
	bool is_hidden_set;
	CUtlVector< int > attributes;
};

void KitParser::initialize_kits( ) {
	// We need these for overriding viewmodels and icons
	g_KitParser.k_weapon_info = {
	};

	k_weapon_info[ WEAPON_KNIFE ] = { ( "models/weapons/v_knife_default_ct.mdl" ), ( "knife_default_ct" ) };
	k_weapon_info[ WEAPON_KNIFE_T ] = { ( "models/weapons/v_knife_default_t.mdl" ), ( "knife_default_t" ) };

	g_KitParser.k_knife_names = {

	};

	g_KitParser.k_glove_names = {
		//{ 0, ( "Default" ) },

	};

	const auto V_UCS2ToUTF8 = reinterpret_cast< int( * )( const wchar_t* ucs2, char* utf8, int len ) >( GetProcAddress( GetModuleHandleA( XorStr( "vstdlib.dll" ) ), XorStr( "V_UCS2ToUTF8" ) ) );
	const auto t = FIELD_OFFSET( CCStrike15ItemDefinition, model_player );

	// Search the relative calls

	// call    ItemSystem
	// push    dword ptr [esi+0Ch]
	// lea     ecx, [eax+4]
	// call    CEconItemSchema::GetPaintKitDefinition

	const auto sig_address = Memory::Scan( XorStr( "client.dll" ), XorStr( "E8 ? ? ? ? FF 76 0C 8D 48 04 E8" ) );

	// Skip the opcode, read rel32 address
	const auto item_system_offset = *reinterpret_cast< std::int32_t* >( sig_address + 1 );

	// Add the offset to the end of the instruction
	const auto item_system_fn = reinterpret_cast< CCStrike15ItemSystem * ( * )( ) >( sig_address + 5 + item_system_offset );

	// Skip VTable, first member variable of ItemSystem is ItemSchema
	const auto item_schema = reinterpret_cast< CCStrike15ItemSchema* >( std::uintptr_t( item_system_fn( ) ) + sizeof( void* ) );

	// Dump paint kits
	{
		// Skip the instructions between, skip the opcode, read rel32 address
		const auto get_paint_kit_definition_offset = *reinterpret_cast< std::int32_t* >( sig_address + 11 + 1 );

		// Add the offset to the end of the instruction
		const auto get_paint_kit_definition_fn = reinterpret_cast< CPaintKit * ( __thiscall* )( CCStrike15ItemSchema*, int ) >( sig_address + 11 + 5 + get_paint_kit_definition_offset );

		// The last offset is start_element, we need that

		// push    ebp
		// mov     ebp, esp
		// sub     esp, 0Ch
		// mov     eax, [ecx+298h]

		// Skip instructions, skip opcode, read offset
		const auto start_element_offset = *reinterpret_cast< std::intptr_t* >( std::uintptr_t( get_paint_kit_definition_fn ) + 8 + 2 );

		// Calculate head base from start_element's offset ( 0x28C )
		const auto head_offset = start_element_offset - 12;

		// 0x220 offset, so difference is 0x6C
		const auto icons_offset = head_offset - 0x6C;

		const auto items_offset = head_offset - 0x194;

		const auto items_head = reinterpret_cast< Head_t< int, CCStrike15ItemDefinition* >* >( std::uintptr_t( item_schema ) + items_offset );

		std::map< uint16_t, item_cstrike > items;

		// currently 72 items
		for( auto i = 0; i <= items_head->last_element; ++i ) {
			const auto node = items_head->memory[ i ];
			const auto item = node.value;
			if( !item || !item->name )
				continue;

			if( !item->model_player )
				continue;

			if( item->item_sub_position >= LOADOUT_POSITION_MELEE && item->item_sub_position <= LOADOUT_POSITION_HEAVY5 || item->item_sub_position == LOADOUT_POSITION_SPACER4 ) {
				if( node.key == GLOVE_T_SIDE
					|| node.key == GLOVE_CT_SIDE
					|| node.key == WEAPON_KNIFE_GHOST
					|| node.key == WEAPON_KNIFEGG
					|| node.key == WEAPON_SHIELD
					|| node.key == 42
					|| node.key == 59
					|| node.key == 74
					|| item->item_sub_position == LOADOUT_POSITION_C4 ) {
					continue;
				}

				item_cstrike def;
				def.group = item->item_sub_position;
				def.name = item->name;
				def.glove = item->item_sub_position == LOADOUT_POSITION_SPACER4;
				def.knife = item->item_sub_position == LOADOUT_POSITION_MELEE;
				def.display_name = item->item_name;
				def.model_name = item->model_player;
				items[ node.key ] = def;
			}
		}

		const auto icons_head = reinterpret_cast< Head_t< int, AlternateIconData_t >* >( std::uintptr_t( item_schema ) + *( int* )( Memory::Scan( XorStr( "client.dll" ), XorStr( "8D 9A ?? ?? ?? ?? 89 45 9C" ) ) + 2 ) + 4 );

		const auto map_head = reinterpret_cast< Head_t< int, CPaintKit* >* >( std::uintptr_t( item_schema ) + head_offset );

		skin_kits.reserve( map_head->allocation_count );

		for( auto i = 0; i <= map_head->last_element; ++i ) {
			const auto paint_kit = map_head->memory[ i ].value;

			if( paint_kit->id == 9001 )
				continue;

			const auto wide_name = m_pLocalize->Find( paint_kit->item_name.buffer + 1 );
			char name[ 256 ];
			V_UCS2ToUTF8( wide_name, name, sizeof( name ) );

			all_skins.push_back( { paint_kit->id, name, paint_kit->name.buffer } );

			if( paint_kit->id < 10000 ) {
				skin_kits.push_back( { paint_kit->id, name, paint_kit->name.buffer } );
			}
			else {
				glove_kits.push_back( { paint_kit->id, name, paint_kit->name.buffer } );
			}
		}

		const auto collections = reinterpret_cast< Head_t< int, CEconItemSetDefinition >* >( std::uintptr_t( item_schema ) + *( int* )( Memory::Scan( XorStr( "client.dll" ), XorStr( "74 43 8B 81 ?? ?? ?? ?? 89 45 FC" ) ) + 4 ) );

		for( auto i = 0; i <= collections->last_element; ++i ) {
			const auto& collect = collections->memory[ i ].value;
			if( collect.items.m_Size <= 0 )
				continue;

			for( int i = 0; i < collect.items.m_Size; ++i ) {
				const auto& entry = collect.items.m_Memory.m_pMemory[ i ];
				if( items.count( entry.item ) < 1 || items[ entry.item ].knife )
					continue;

				if( entry.item == WEAPON_KNIFE )
					continue;

				// FIXME: find makes skin parsing so fucking slow
				auto skin_it = std::find( skin_kits.begin( ), skin_kits.end( ), ( entry.paintkit ) );
				if( skin_it == skin_kits.end( ) )
					continue;

				auto tmp = *skin_it;

				auto new_item = std::find( weapon_skins.begin( ), weapon_skins.end( ), ( entry.item ) );
				if( new_item == weapon_skins.end( ) ) {
					new_item = weapon_skins.insert( weapon_skins.end( ), item_skins{ } );
				}

				new_item->m_kits.push_back( tmp );
				new_item->display_name = items[ entry.item ].display_name;
				new_item->name = items[ entry.item ].name;
				new_item->id = entry.item;
				new_item->glove = items[ entry.item ].glove;
				new_item->knife = items[ entry.item ].knife;

				switch( items[ entry.item ].group ) {
				case LOADOUT_POSITION_SECONDARY0:
				case LOADOUT_POSITION_SECONDARY1:
				case LOADOUT_POSITION_SECONDARY2:
				case LOADOUT_POSITION_SECONDARY3:
					new_item->group = WEAPONGROUP_PISTOL;
					break;

				case LOADOUT_POSITION_SECONDARY4:
				case LOADOUT_POSITION_SECONDARY5:
					new_item->group = WEAPONGROUP_HEAVYPISTOL;
					break;

				case LOADOUT_POSITION_SMG0:
				case LOADOUT_POSITION_SMG1:
				case LOADOUT_POSITION_SMG2:
				case LOADOUT_POSITION_SMG3:
				case LOADOUT_POSITION_SMG4:
				case LOADOUT_POSITION_SMG5:
					new_item->group = WEAPONGROUP_SUBMACHINE;
					break;

				case LOADOUT_POSITION_RIFLE0:
				case LOADOUT_POSITION_RIFLE1:
				case LOADOUT_POSITION_RIFLE3:
					new_item->group = WEAPONGROUP_RIFLE;
					break;

				case LOADOUT_POSITION_RIFLE2:
				case LOADOUT_POSITION_RIFLE4:
					new_item->group = WEAPONGROUP_SNIPER;
					break;

				case LOADOUT_POSITION_RIFLE5:
					new_item->group = WEAPONGROUP_AUTOSNIPER;
					break;

				case LOADOUT_POSITION_HEAVY0:
				case LOADOUT_POSITION_HEAVY1:
				case LOADOUT_POSITION_HEAVY2:
					new_item->group = WEAPONGROUP_SHOTGUN;
					break;

				case LOADOUT_POSITION_HEAVY3:
				case LOADOUT_POSITION_HEAVY4:
				case LOADOUT_POSITION_HEAVY5:
					new_item->group = WEAPONGROUP_HEAVY;
					break;

				default:
					new_item->group = -1;
					break;
				}
			}
		}

		std::sort( weapon_skins.begin( ), weapon_skins.end( ), [ ] ( const item_skins& a, const item_skins& b ) {
			return a.display_name < b.display_name;
		} );

		for( auto& info : items ) {
			if( info.first == GLOVE_T_SIDE || info.first == GLOVE_CT_SIDE ) {
				continue;
			}

			auto name = items[ size_t( info.first ) ].name;

			auto new_item = std::find( weapon_skins.begin( ), weapon_skins.end( ), info.first );
			if( new_item == weapon_skins.end( ) ) {
				new_item = weapon_skins.insert( weapon_skins.end( ), item_skins{ } );
			}

			if( items[ size_t( info.first ) ].glove || items[ size_t( info.first ) ].knife ) {
				for( int i = 0; i < icons_head->last_element; ++i ) {
					const auto& icon = icons_head->memory[ i ].value;

					auto end = icon.icon_path.buffer + icon.icon_path.length - 7;
					if( *( end + 1 ) != 'l' ) // _light
						continue;

					auto _weapon = strstr( icon.icon_path.buffer, name.data( ) );
					if( !_weapon )
						continue;

					std::string skin_str = std::string( _weapon + name.size( ) + 1, end );

					if( !items[ size_t( info.first ) ].glove ) {
						auto skin_it = std::find_if( skin_kits.begin( ), skin_kits.end( ), [ skin_str ] ( const paint_kit& a ) {
							return strcmp( skin_str.c_str( ), a.item_name.data( ) ) == 0;
						} );

						if( skin_it == skin_kits.end( ) )
							continue;

						new_item->m_kits.push_back( *skin_it );
					}
					else {
						auto skin_it = std::find_if( glove_kits.begin( ), glove_kits.end( ), [ skin_str ] ( const paint_kit& a ) {
							return strcmp( skin_str.c_str( ), a.item_name.data( ) ) == 0;
						} );

						if( skin_it == glove_kits.end( ) )
							continue;

						new_item->m_kits.push_back( *skin_it );
						glove_kits.erase( skin_it );
					}
				}
			}

			new_item->display_name = items[ size_t( info.first ) ].display_name;
			new_item->name = name;
			new_item->glove = items[ size_t( info.first ) ].glove;
			new_item->knife = items[ size_t( info.first ) ].knife;
			new_item->model_name = items[ size_t( info.first ) ].model_name;
			new_item->id = info.first;

			const auto wide_name = m_pLocalize->Find( new_item->display_name.c_str( ) );
			char buffer[ 256 ];
			V_UCS2ToUTF8( wide_name, buffer, sizeof( buffer ) );

			new_item->display_name = buffer;

			if( items[ size_t( info.first ) ].glove || items[ size_t( info.first ) ].knife ) {
				if( new_item->glove ) {
					k_glove_names.push_back( WeaponName_t( new_item->id, new_item->display_name.c_str( ) ) );
					k_weapon_info[ new_item->id ].icon = nullptr;
				}
				else {
					const char* meme = new_item->name.data( );
					auto weapon_ptr = strstr( meme, XorStr( "weapon_" ) );
					if( weapon_ptr )
						meme = weapon_ptr + sizeof( ( "weapon_" ) ) - 1;

					k_knife_names.push_back( WeaponName_t( new_item->id, new_item->display_name.c_str( ) ) );
					k_weapon_info[ new_item->id ].icon = meme;
				}

				k_weapon_info[ new_item->id ].model = new_item->model_name.c_str( );
			}
		}

		for( auto& value : weapon_skins ) {
			std::sort( value.m_kits.begin( ), value.m_kits.end( ) );
		}

		// copy array contents
		if( !skin_kits.empty( ) ) {
			std::sort( skin_kits.begin( ), skin_kits.end( ) );
			for( auto i = 0; i < skin_kits.size( ); ++i ) {
				g_Vars.globals.m_vecPaintKits.emplace_back( skin_kits[ i ].name );
			}
		}
	}

}
