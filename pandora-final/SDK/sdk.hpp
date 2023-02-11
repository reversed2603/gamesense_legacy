#pragma once

#include "Includes.hpp"

#include <d3d9.h>
#pragma comment(lib, "d3d9.lib")

#include <d3dx9.h>
#pragma comment(lib, "d3dx9.lib")

#pragma comment(lib, "Urlmon.lib")

#include <playsoundapi.h>
#include <mmeapi.h> 
#pragma comment(lib, "Winmm.lib") 

#include <stdint.h>

#include <string>
#include <sstream>
#include "Valve/vector2d.hpp"
#include "Valve/vector.hpp"
#include "Valve/vector4d.hpp"
#include "Valve/qangle.hpp"
#include "Valve/matrix.hpp"
#include "../Utils/Math.hpp"
#include "color.hpp"
#include "Definitions.hpp"
#include "variables.hpp"
#include "Classes/CStudioRender.hpp"
#include <minwindef.h>

using namespace SDK;
#define M_PI 3.1415926535f
#define TIME_TO_TICKS(dt) ((int)( 0.5f + (float)(dt) / m_pGlobalVars->interval_per_tick))
#define TICKS_TO_TIME(t) (m_pGlobalVars->interval_per_tick * float(t) )
#define ROUND_TO_TICKS( t ) ( m_pGlobalVars->interval_per_tick * TIME_TO_TICKS( t ) )

#define NUM_ENT_ENTRY_BITS         (11 + 2)
#define NUM_ENT_ENTRIES            (1 << NUM_ENT_ENTRY_BITS)
#define INVALID_EHANDLE_INDEX       0xFFFFFFFF
#define NUM_SERIAL_NUM_BITS        16 // (32 - NUM_ENT_ENTRY_BITS)
#define NUM_SERIAL_NUM_SHIFT_BITS (32 - NUM_SERIAL_NUM_BITS)
#define ENT_ENTRY_MASK             (( 1 << NUM_SERIAL_NUM_BITS) - 1)

#define MAX_COORD_FLOAT ( 16384.0f )
#define MIN_COORD_FLOAT ( -MAX_COORD_FLOAT )

#define DECLARE_POINTER_HANDLE(name) struct name##__ { int unused; }; typedef struct name##__ *name
#define MAXSTUDIOSKINS		32

#define FRUSTUM_NUMPLANES    6

#ifndef max
#define MIN(a,b)            (((a) > (b)) ? (a) : (b))
#endif

#ifndef min
#define MAX(a,b)            (((a) < (b)) ? (a) : (b))
#endif

// 
// macros
// 

#define TELEPORT_DISTANCE 4096.f

#pragma region decl_macros
#define MULTIPLAYER_BACKUP 150

#define FL_ONGROUND ( 1 << 0 )
#define FL_DUCKING ( 1 << 1 )

#define IN_ATTACK ( 1 << 0 )
#define IN_JUMP ( 1 << 1 )
#define IN_DUCK ( 1 << 2 )
#define IN_FORWARD ( 1 << 3 )
#define IN_BACK ( 1 << 4 )
#define IN_USE ( 1 << 5 )
#define IN_CANCEL ( 1 << 6 )
#define IN_LEFT ( 1 << 7 )
#define IN_RIGHT ( 1 << 8 )
#define IN_MOVELEFT ( 1 << 9 )
#define IN_MOVERIGHT ( 1 << 10 )
#define IN_ATTACK2 ( 1 << 11 )
#define IN_RUN ( 1 << 12 )
#define IN_RELOAD ( 1 << 13 )
#define IN_ALT1 ( 1 << 14 )
#define IN_ALT2 ( 1 << 15 )
#define IN_SCORE ( 1 << 16 )
#define IN_SPEED ( 1 << 17 )
#define IN_WALK ( 1 << 18 )
#define IN_ZOOM ( 1 << 19 )
#define IN_WEAPON1 ( 1 << 20 )
#define IN_WEAPON2 ( 1 << 21 )
#define IN_BULLRUSH ( 1 << 22 )
#define IN_GRENADE1 ( 1 << 23 )
#define IN_GRENADE2 ( 1 << 24 )
#pragma endregion

#pragma region masks

#define   DISPSURF_FLAG_SURFACE           (1<<0)
#define   DISPSURF_FLAG_WALKABLE          (1<<1)
#define   DISPSURF_FLAG_BUILDABLE         (1<<2)
#define   DISPSURF_FLAG_SURFPROP1         (1<<3)
#define   DISPSURF_FLAG_SURFPROP2         (1<<4)

#define   CONTENTS_EMPTY                0

#define   CONTENTS_SOLID                0x1       
#define   CONTENTS_WINDOW               0x2
#define   CONTENTS_AUX                  0x4
#define   CONTENTS_GRATE                0x8
#define   CONTENTS_SLIME                0x10
#define   CONTENTS_WATER                0x20
#define   CONTENTS_BLOCKLOS             0x40 
#define   CONTENTS_OPAQUE               0x80 
#define   LAST_VISIBLE_CONTENTS         CONTENTS_OPAQUE

#define   ALL_VISIBLE_CONTENTS            (LAST_VISIBLE_CONTENTS | (LAST_VISIBLE_CONTENTS-1))

#define   CONTENTS_TESTFOGVOLUME        0x100
#define   CONTENTS_UNUSED               0x200     
#define   CONTENTS_BLOCKLIGHT           0x400
#define   CONTENTS_TEAM1                0x800 
#define   CONTENTS_TEAM2                0x1000 
#define   CONTENTS_IGNORE_NODRAW_OPAQUE 0x2000
#define   CONTENTS_MOVEABLE             0x4000
#define   CONTENTS_AREAPORTAL           0x8000
#define   CONTENTS_PLAYERCLIP           0x10000
#define   CONTENTS_MONSTERCLIP          0x20000
#define   CONTENTS_CURRENT_0            0x40000
#define   CONTENTS_CURRENT_90           0x80000
#define   CONTENTS_CURRENT_180          0x100000
#define   CONTENTS_CURRENT_270          0x200000
#define   CONTENTS_CURRENT_UP           0x400000
#define   CONTENTS_CURRENT_DOWN         0x800000

#define   CONTENTS_ORIGIN               0x1000000 

#define   CONTENTS_MONSTER              0x2000000 
#define   CONTENTS_DEBRIS               0x4000000
#define   CONTENTS_DETAIL               0x8000000 
#define   CONTENTS_TRANSLUCENT          0x10000000
#define   CONTENTS_LADDER               0x20000000
#define   CONTENTS_HITBOX               0x40000000

#define   SURF_LIGHT                    0x0001 
#define   SURF_SKY2D                    0x0002 
#define   SURF_SKY                      0x0004 
#define   SURF_WARP                     0x0008 
#define   SURF_TRANS                    0x0010
#define   SURF_NOPORTAL                 0x0020 
#define   SURF_TRIGGER                  0x0040 
#define   SURF_NODRAW                   0x0080 

#define   SURF_HINT                     0x0100 

#define   SURF_SKIP                     0x0200   
#define   SURF_NOLIGHT                  0x0400   
#define   SURF_BUMPLIGHT                0x0800   
#define   SURF_NOSHADOWS                0x1000   
#define   SURF_NODECALS                 0x2000   
#define   SURF_NOPAINT                  SURF_NODECALS
#define   SURF_NOCHOP                   0x4000   
#define   SURF_HITBOX                   0x8000   

// -----------------------------------------------------
// spatial content masks - used for spatial queries (traceline,etc.)
// -----------------------------------------------------
#define   MASK_ALL                      (0xFFFFFFFF)
#define   MASK_SOLID                    (CONTENTS_SOLID|CONTENTS_MOVEABLE|CONTENTS_WINDOW|CONTENTS_MONSTER|CONTENTS_GRATE)
#define   MASK_PLAYERSOLID              (CONTENTS_SOLID|CONTENTS_MOVEABLE|CONTENTS_PLAYERCLIP|CONTENTS_WINDOW|CONTENTS_MONSTER|CONTENTS_GRATE)
#define   MASK_NPCSOLID                 (CONTENTS_SOLID|CONTENTS_MOVEABLE|CONTENTS_MONSTERCLIP|CONTENTS_WINDOW|CONTENTS_MONSTER|CONTENTS_GRATE)
#define   MASK_NPCFLUID                 (CONTENTS_SOLID|CONTENTS_MOVEABLE|CONTENTS_MONSTERCLIP|CONTENTS_WINDOW|CONTENTS_MONSTER)
#define   MASK_WATER                    (CONTENTS_WATER|CONTENTS_MOVEABLE|CONTENTS_SLIME)
#define   MASK_OPAQUE                   (CONTENTS_SOLID|CONTENTS_MOVEABLE|CONTENTS_OPAQUE)
#define   MASK_OPAQUE_AND_NPCS          (MASK_OPAQUE|CONTENTS_MONSTER)
#define   MASK_BLOCKLOS                 (CONTENTS_SOLID|CONTENTS_MOVEABLE|CONTENTS_BLOCKLOS)
#define   MASK_BLOCKLOS_AND_NPCS        (MASK_BLOCKLOS|CONTENTS_MONSTER)
#define   MASK_VISIBLE                  (MASK_OPAQUE|CONTENTS_IGNORE_NODRAW_OPAQUE)
#define   MASK_VISIBLE_AND_NPCS         (MASK_OPAQUE_AND_NPCS|CONTENTS_IGNORE_NODRAW_OPAQUE)
#define   MASK_SHOT                     (CONTENTS_SOLID|CONTENTS_MOVEABLE|CONTENTS_MONSTER|CONTENTS_WINDOW|CONTENTS_DEBRIS|CONTENTS_HITBOX)
#define   MASK_SHOT_BRUSHONLY           (CONTENTS_SOLID|CONTENTS_MOVEABLE|CONTENTS_WINDOW|CONTENTS_DEBRIS)
#define   MASK_SHOT_HULL                (CONTENTS_SOLID|CONTENTS_MOVEABLE|CONTENTS_MONSTER|CONTENTS_WINDOW|CONTENTS_DEBRIS|CONTENTS_GRATE)
#define   MASK_SHOT_PLAYER              ( MASK_SHOT_HULL | CONTENTS_HITBOX )
#define   MASK_SHOT_PORTAL              (CONTENTS_SOLID|CONTENTS_MOVEABLE|CONTENTS_WINDOW|CONTENTS_MONSTER)
#define   MASK_SOLID_BRUSHONLY          (CONTENTS_SOLID|CONTENTS_MOVEABLE|CONTENTS_WINDOW|CONTENTS_GRATE)
#define   MASK_PLAYERSOLID_BRUSHONLY    (CONTENTS_SOLID|CONTENTS_MOVEABLE|CONTENTS_WINDOW|CONTENTS_PLAYERCLIP|CONTENTS_GRATE)
#define   MASK_NPCSOLID_BRUSHONLY       (CONTENTS_SOLID|CONTENTS_MOVEABLE|CONTENTS_WINDOW|CONTENTS_MONSTERCLIP|CONTENTS_GRATE)
#define   MASK_NPCWORLDSTATIC           (CONTENTS_SOLID|CONTENTS_WINDOW|CONTENTS_MONSTERCLIP|CONTENTS_GRATE)
#define   MASK_NPCWORLDSTATIC_FLUID     (CONTENTS_SOLID|CONTENTS_WINDOW|CONTENTS_MONSTERCLIP)
#define   MASK_SPLITAREAPORTAL          (CONTENTS_WATER|CONTENTS_SLIME)
#define   MASK_CURRENT                  (CONTENTS_CURRENT_0|CONTENTS_CURRENT_90|CONTENTS_CURRENT_180|CONTENTS_CURRENT_270|CONTENTS_CURRENT_UP|CONTENTS_CURRENT_DOWN)
#define   MASK_DEADSOLID                (CONTENTS_SOLID|CONTENTS_PLAYERCLIP|CONTENTS_WINDOW|CONTENTS_GRATE)
#pragma endregion

#pragma region tex_groups
// These are given to FindMaterial to reference the texture groups that Show up on the 
#define TEXTURE_GROUP_LIGHTMAP						        "Lightmaps"
#define TEXTURE_GROUP_WORLD							          "World textures"
#define TEXTURE_GROUP_MODEL							          "Model textures"
#define TEXTURE_GROUP_VGUI							          "VGUI textures"
#define TEXTURE_GROUP_PARTICLE						        "Particle textures"
#define TEXTURE_GROUP_DECAL							          "Decal textures"
#define TEXTURE_GROUP_SKYBOX						          "SkyBox textures"
#define TEXTURE_GROUP_CLIENT_EFFECTS				      "ClientEffect textures"
#define TEXTURE_GROUP_OTHER							          "Other textures"
#define TEXTURE_GROUP_PRECACHED						        "Precached"
#define TEXTURE_GROUP_CUBE_MAP						        "CubeMap textures"
#define TEXTURE_GROUP_RENDER_TARGET					      "RenderTargets"
#define TEXTURE_GROUP_UNACCOUNTED					        "Unaccounted textures"
//#define TEXTURE_GROUP_STATIC_VERTEX_BUFFER		  "Static Vertex"
#define TEXTURE_GROUP_STATIC_INDEX_BUFFER			    "Static Indices"
#define TEXTURE_GROUP_STATIC_VERTEX_BUFFER_DISP		"Displacement Verts"
#define TEXTURE_GROUP_STATIC_VERTEX_BUFFER_COLOR	"Lighting Verts"
#define TEXTURE_GROUP_STATIC_VERTEX_BUFFER_WORLD	"World Verts"
#define TEXTURE_GROUP_STATIC_VERTEX_BUFFER_MODELS	"Model Verts"
#define TEXTURE_GROUP_STATIC_VERTEX_BUFFER_OTHER	"Other Verts"
#define TEXTURE_GROUP_DYNAMIC_INDEX_BUFFER			  "Dynamic Indices"
#define TEXTURE_GROUP_DYNAMIC_VERTEX_BUFFER			  "Dynamic Verts"
#define TEXTURE_GROUP_DEPTH_BUFFER					      "DepthBuffer"
#define TEXTURE_GROUP_VIEW_MODEL					        "ViewModel"
#define TEXTURE_GROUP_PIXEL_SHADERS					      "Pixel Shaders"
#define TEXTURE_GROUP_VERTEX_SHADERS				      "Vertex Shaders"
#define TEXTURE_GROUP_RENDER_TARGET_SURFACE			  "RenderTarget Surfaces"
#define TEXTURE_GROUP_MORPH_TARGETS					      "Morph Targets"
#pragma endregion

#define NOISE_DIVISIONS		128
#define MAX_BEAM_ENTS		10

#define MAX_VIS_LEAVES    32
#define MAX_AREA_STATE_BYTES        32
#define MAX_AREA_PORTAL_STATE_BYTES 24

#define STUDIO_NONE						0x00000000
#define STUDIO_RENDER					0x00000001
#define STUDIO_VIEWXFORMATTACHMENTS		0x00000002
#define STUDIO_DRAWTRANSLUCENTSUBMODELS 0x00000004
#define STUDIO_TWOPASS					0x00000008
#define STUDIO_STATIC_LIGHTING			0x00000010
#define STUDIO_WIREFRAME				0x00000020
#define STUDIO_ITEM_BLINK				0x00000040
#define STUDIO_NOSHADOWS				0x00000080
#define STUDIO_WIREFRAME_VCOLLIDE		0x00000100
#define STUDIO_NOLIGHTING_OR_CUBEMAP	0x00000200
#define STUDIO_SKIP_FLEXES				0x00000400
#define STUDIO_DONOTMODIFYSTENCILSTATE	0x00000800	// TERROR

// not a studio flag, but used to signify that the draw call should cause a depth imposter to be drawn as well.
#define STUDIO_DEPTH_ONLY				0x01000000

// Not a studio flag, but used to flag model to render using special settings for AO pre-pass
#define STUDIO_AOPREPASSTEXURE			0x02000000

// Not a studio flag, but used to flag model to keep the shadow state it starts with
#define STUDIO_KEEP_SHADOWS				0x04000000

// Not a studio flag, but used to flag model as using shadow depth material override
#define STUDIO_SSAODEPTHTEXTURE			0x08000000

// Not a studio flag, but used to flag model as a non-sorting brush model
#define STUDIO_TRANSPARENCY				0x80000000

// Not a studio flag, but used to flag model as using shadow depth material override
#define STUDIO_SHADOWDEPTHTEXTURE		0x40000000

// Not a studio flag, but used to flag model as doing custom rendering into shadow texture
#define STUDIO_SHADOWTEXTURE			0x20000000

#define STUDIO_SKIP_DECALS				0x10000000

__forceinline auto DotProduct( const Vector& a, const Vector& b ) -> float {
	return ( a[ 0 ] * b[ 0 ] + a[ 1 ] * b[ 1 ] + a[ 2 ] * b[ 2 ] );
}

// 
// indices
// 
#pragma region decl_indices
namespace Index
{
	namespace IBaseClientDLL
	{
		enum {
			GetAllClasses = 8,
			CreateMove = 22,
			FrameStageNotify = 36,
			IsChatRaised = 90,
		};
	}
	namespace ModelDraw
	{
		enum {
			DrawModelExecute = 21,
		};
	}
	namespace GameEvent
	{
		enum {
			GameEvent = 9,
		};
	}
	namespace StudioRender
	{
		enum {
			DrawModel = 29,
		};
	}
	namespace NetChannel
	{
		enum {
			Shutdown = 27,
			SendNetMsg = 40,
		};
	}
	namespace EngineClient
	{
		enum {
			GetNetChannelInfo = 78,
			IsPlayingDemo = 82,
			IsHltv = 93,
			GetScreenAspectRatio = 101,
		};
	}
	namespace BSPTreeQuery
	{
		enum {
			ListLeavesInBox = 6,
		};
	}
	namespace CClientModeShared
	{
		enum {
			CreateMove = 24,
			DoPostScreenSpaceEffects = 44,
			OverrideView = 18,
			ShouldDrawFog = 17,
			ShouldDrawParticles = 16,
		};
	}
	namespace DirectX
	{
		enum {
			EndScene = 42,
			Reset = 16,
			Present = 17,
		};
	};
	namespace IClientEntityList
	{
		enum {
			GetClientEntity = 3,
			GetClientEntityFromHandle = 4,
			GetHighestEntityIndex = 6,
		};
	}
	namespace IGameMovement
	{
		enum {
			ProcessMovement = 1,
			Reset = 2,
			StartTrackPredictionErrors = 3,
			FinishTrackPredictionErrors = 4,
			GetPlayerMins = 6,
			GetPlayerMaxs = 7,
			GetPlayerViewOffset = 8,
		};
	}
	namespace IPrediction
	{
		enum {
			Update = 3,
			CheckMovingGround = 18,
			RunCommand = 19,
			SetupMove = 20,
			FinishMove = 21,
			InPrediction = 14,
		};
	}
	namespace IMoveHelper
	{
		enum {
			SetHost = 1,
			ProccesImpacts = 4,
		};
	}
	namespace IInput
	{
		enum {
			GetUserCmd = 8,
		};
	}
	namespace IVEngineClient
	{
		enum {
			GetScreenSize = 5,
			GetPlayerInfo = 8,
			GetLocalPlayer = 12,
			Time = 14,
			GetViewAngles = 18,
			SetViewAngles = 19,
			GetMaxClients = 20,
			IsInGame = 26,
			IsConnected = 27,
			WorldToScreenMatrix = 37,
			GetNetChannelInfo = 78,
			ClientCmd_Unrestricted = 114,
		};
	}
	namespace IPanel
	{
		enum {
			GetName = 36,
			PaintTraverse = 41,
		};
	}
	namespace IEngineVGui
	{
		enum {
			Paint = 14,
		};
	}
	namespace VguiSurface
	{
		enum {
			UnLockCursor = 66,
			LockCursor = 67,
			OnScreenSizeChanged = 116,
		};
	}
	namespace MatSystem
	{
		enum {
			BeginFrame = 42,
		};
	}
}
#pragma endregion

// 
// enums
// 
#pragma region decl_enums
enum MDLCacheDataType_t {
	// Callbacks to Get called when data is loaded or unloaded for these:
	MDLCACHE_STUDIOHDR = 0,
	MDLCACHE_STUDIOHWDATA,
	MDLCACHE_VCOLLIDE,

	// Callbacks NOT called when data is loaded or unloaded for these:
	MDLCACHE_ANIMBLOCK,
	MDLCACHE_VIRTUALMODEL,
	MDLCACHE_VERTEXES,
	MDLCACHE_DECODEDANIMBLOCK
};

enum MDLCacheFlush_t {
	MDLCACHE_FLUSH_STUDIOHDR = 0x01,
	MDLCACHE_FLUSH_STUDIOHWDATA = 0x02,
	MDLCACHE_FLUSH_VCOLLIDE = 0x04,
	MDLCACHE_FLUSH_ANIMBLOCK = 0x08,
	MDLCACHE_FLUSH_VIRTUALMODEL = 0x10,
	MDLCACHE_FLUSH_AUTOPLAY = 0x20,
	MDLCACHE_FLUSH_VERTEXES = 0x40,

	MDLCACHE_FLUSH_IGNORELOCK = 0x80000000,
	MDLCACHE_FLUSH_ALL = 0xFFFFFFFF
};

enum SendPropType {
	DPT_Int = 0,
	DPT_Float,
	DPT_Vector,
	DPT_VectorXY,
	DPT_String,
	DPT_Array,
	DPT_DataTable,
	DPT_NUMSendPropTypes,
};

enum MapLoadType_t {
	MapLoad_NewGame = 0,
	MapLoad_LoadGame,
	MapLoad_Transition,
	MapLoad_Background,
};

enum ClientFrameStage_t {
	FRAME_UNDEFINED = -1,
	FRAME_START,
	FRAME_NET_UPDATE_START,
	FRAME_NET_UPDATE_POSTDATAUPDATE_START,
	FRAME_NET_UPDATE_POSTDATAUPDATE_END,
	FRAME_NET_UPDATE_END,
	FRAME_RENDER_START,
	FRAME_RENDER_END,
};
#pragma endregion

// 
// unimplemented
// 
#pragma region decl_unimplemented
struct model_t;

class SendProp;
#pragma endregion

// 
// types
// 
#pragma region decl_types
using CRC32_t = unsigned int;
using VPANEL = unsigned int;
using ModelInstanceHandle_t = unsigned short;
using ImageFormat = int;
using VertexFormat_t = int;
using MaterialPropertyTypes_t = int;
using OverrideType_t = int;
using LightCacheHandle_t = void*;
using ChangeCallback_t = void( * )( ConVar* var, const char* pOldValue, float flOldValue );
using StudioDecalHandle_t = void*;
using pixelvis_handle_t = int;
using str_32 = char[ 32 ];
using CreateClientClassFn = void* ( * )( int entnum, int serialNum );
using LeafIndex_t = unsigned short;
#pragma endregion

// 
// structs
// 
#pragma region decl_structs
struct string_t;
struct model_t;
struct mstudioanimdesc_t;
struct mstudioseqdesc_t;
struct Ray_t;
struct DrawModelInfo_t;
struct studiohwdata_t;
struct MaterialLightingState_t;
struct ColorMeshInfo_t;
struct MaterialSystem_Config_t;
struct MaterialSystemHWID_t;
struct AspectRatioInfo_t;
struct studiohwdata_t;
struct vcollide_t;
struct virtualmodel_t;
struct vertexFileHeader_t;
class CPhysCollide;
class ISpatialQuery;
class CUtlBuffer;
struct virtualmodel_t;
struct client_textmessage_t;
class CSentence;
class CAudioSource;
class SurfInfo;
struct Frustum_t;
class IAchievementMgr;
class AudioState_t;
class ISPSharedMemory;
class C_Beam;
class IMDLCache;
#include "Valve/UtlVector.hpp"
class CStudioHdr
{
public:
	mutable studiohdr_t* _m_pStudioHdr;
	mutable virtualmodel_t* m_pVModel;
	mutable uint16_t m_pStudioHdrCache;

	int m_nFrameUnlockCounter;
	int* m_pFrameUnlockCounter;
	uint16_t m_FrameUnlockCounterMutex;

	int unknown;
	CUtlVector<int>		m_boneFlags; //48
	CUtlVector<int>		m_boneParent;

	class CActivityToSequenceMapping /* final */
	{
	public:
		// A tuple of a sequence and its corresponding weight. Lists of these correspond to activities.
		struct SequenceTuple
		{
			short		seqnum;
			short		weight; // the absolute value of the weight from the sequence header
			uint16_t* pActivityModifiers;		// list of activity modifier symbols
			int			iNumActivityModifiers;
			int* iUnknown;
		};

		// The type of the hash's stored data, a composite of both key and value
		// (because that's how CUtlHash works):
		// key: an int, the activity #
		// values: an index into the m_pSequenceTuples array, a count of the
		// total sequences present for an activity, and the sum of their
		// weights.
		// Note this struct is 128-bits wide, exactly coincident to a PowerPC 
		// cache line and VMX register. Please consider very carefully the
		// performance implications before adding any additional fields to this.
		// You could probably do away with totalWeight if you really had to.
		struct HashValueType
		{
			// KEY (hashed)
			int activityIdx;

			// VALUE (not hashed)
			int startingIdx;
			int count;
			int totalWeight;

			HashValueType( int _actIdx, int _stIdx, int _ct, int _tW ) :
				activityIdx( _actIdx ), startingIdx( _stIdx ), count( _ct ), totalWeight( _tW ) {}

			// default ConstructExor (ought not to be actually used)
			HashValueType( ) : activityIdx( -1 ), startingIdx( -1 ), count( -1 ), totalWeight( -1 )
			{
				//AssertMsg(false, "Don't use default HashValueType()!");
			}


			class HashFuncs
			{
			public:
				// dummy ConstructExor (gndn)
				HashFuncs( int ) {}

				// COMPARE
				// compare two entries for uniqueness. We should never have two different
				// entries for the same activity, so we only compare the activity index;
				// this allows us to use the utlhash as a dict by ConstructExing dummy entries
				// as hash lookup keys.
				/*bool operator()( const HashValueType& lhs, const HashValueType& rhs ) const
				{
					return lhs.activityIdx == rhs.activityIdx;
				}

				// HASH
				// We only hash on the activity index; everything else is data.
				unsigned int operator()( const HashValueType& item ) const
				{
					return HashInt( item.activityIdx );
				}*/
			};
		};

		//typedef CUtlHash<HashValueType, HashValueType::HashFuncs, HashValueType::HashFuncs> ActivityToValueIdxHash;

		// These must be here because IFM does not compile/link studio.cpp (?!?)

		// ctor
		CActivityToSequenceMapping( void )
			: m_pSequenceTuples( NULL ), m_iSequenceTuplesCount( 0 ), m_ActToSeqHash( 8, 0, 0 ), m_pStudioHdr( NULL ), m_expectedVModel( NULL )
		{};

		// dtor -- not virtual because this class has no inheritors
		~CActivityToSequenceMapping( )
		{
			if( m_pSequenceTuples != NULL )
			{
				if( m_pSequenceTuples->pActivityModifiers != NULL )
				{
					delete[ ] m_pSequenceTuples->pActivityModifiers;
				}
				delete[ ] m_pSequenceTuples;
			}
		}

		/// Get the list of sequences for an activity. Returns the pointer to the
		/// first sequence tuple. Output parameters are a count of sequences present,
		/// and the total weight of all the sequences. (it would be more LHS-friendly
		/// to return these on registers, if only C++ offered more than one return 
		/// value....)
		/*const SequenceTuple* GetSequences( int forActivity, int* outSequenceCount, int* outTotalWeight );

		/// The number of sequences available for an activity.
		int NumSequencesForActivity( int forActivity );

		static CActivityToSequenceMapping* FindMapping( const CStudioHdr* pstudiohdr );
		static void ReleaseMapping( CActivityToSequenceMapping* pMap );
		static void ResetMappings( );

		//private:

			/// Allocate my internal array. (It is freed in the destructor.) Also,
			/// build the hash of activities to sequences and populate m_pSequenceTuples.
		void Initialize( const CStudioHdr* pstudiohdr );

		/// Force Initialize() to occur again, even if it has already occured.
		void Reinitialize( CStudioHdr* pstudiohdr );

		/// A more efficient version of the old SelectWeightedSequence() function in animation.cpp.
		int SelectWeightedSequence( CStudioHdr* pstudiohdr, int activity, int curSequence );

		// selects the sequence with the most matching modifiers
		int SelectWeightedSequenceFromModifiers( CStudioHdr* pstudiohdr, int activity, CUtlSymbol* pActivityModifiers, int iModifierCount, CBaseEntity* parent );*/

		// Actually a big array, into which the hash values index.
		SequenceTuple* m_pSequenceTuples;
		unsigned int m_iSequenceTuplesCount; // (size of the whole array)

		// we don't store an outer pointer because we can't initialize it at ConstructExion time
		// (warning c4355) -- there are ways around this but it's easier to just pass in a 
		// pointer to the CStudioHdr when we need it, since this class isn't supposed to 
		// export its interface outside the studio header anyway.
		// CStudioHdr * const m_pOuter;

		Vector m_ActToSeqHash;

		const studiohdr_t* m_pStudioHdr;

		// we store these so we can know if the contents of the studiohdr have changed
		// from underneath our feet (this is an emergency data integrity check)
		const void* m_expectedVModel;


		// double-check that the data I point to hasn't changed
		//bool ValidateAgainst( const CStudioHdr* __restrict pstudiohdr );
		//void SetValidation( const CStudioHdr* __restrict pstudiohdr );

		friend class CStudioHdr;
	};

	CActivityToSequenceMapping* m_pActivityToSequence;

	/*mstudioattachment_t& pAttachment( int i );
	int GetNumAttachments( );
	const studiohdr_t* GroupStudioHdr( DWORD group );
	int GetAttachmentBone( int i );*/
	inline int boneFlags( int iBone ) const { return m_boneFlags[ iBone ]; }
	inline int boneParent( int iBone ) const { return m_boneParent[ iBone ]; }
	inline int			numbones( void ) const { return _m_pStudioHdr->numbones; };
	inline mstudiobone_t* pBone( int i ) const { return _m_pStudioHdr->pBone( i ); };
	//there is more, screw the rest for now..

	inline bool IsValid( void ) { return ( _m_pStudioHdr != NULL ); };
	inline bool IsReadyForAccess( void ) const { return ( _m_pStudioHdr != NULL ); };
	//inline const studiohdr_t* GetRenderHdr( void ) const { return _m_pStudioHdr; };
	//int	GetNumPoseParameters( void ) const;
	//const mstudioposeparamdesc_t& pPoseParameter( int i );
	inline int			numikchains( ) const { return _m_pStudioHdr->flags; };
	inline int			GetNumIKChains( void ) const { return _m_pStudioHdr->numikchains; };
	//inline void*	 pIKChain( int i ) const { return _m_pStudioHdr->pIKChain( i ); };
	//mstudioseqdesc_t& pSeqdesc( int seq );
	virtualmodel_t* GetVirtualModel( ) const { return m_pVModel; };
	//inline const byte* GetBoneTableSortedByName( ) const { return _m_pStudioHdr->GetBoneTableSortedByName( ); };
	/*bool SequencesAvailable( );
	int GetNumSeq( void ) const;
	int GetActivityListVersion( );*/
};

class CStudioHdrEx {
public:
	studiohdr_t* studio_hdr;
	virtualmodel_t* virtual_model;
	char pad[ 120 ];

	int m_nPerfAnimatedBones;
	int m_nPerfUsedBones;
	int m_nPerfAnimationLayers;
};

struct hud_weapons_t {
	std::int32_t* get_weapon_count( ) {
		return reinterpret_cast< std::int32_t* >( std::uintptr_t( this ) + 0x80 );
	}
};

struct IntRect {
	int x0;
	int y0;
	int x1;
	int y1;
};

struct Vertex_t {
	Vertex_t( ) { }
	Vertex_t( const Vector2D& pos, const Vector2D& coord = Vector2D( 0, 0 ) ) {
		m_Position = pos;
		m_TexCoord = coord;
	}
	void Init( const Vector2D& pos, const Vector2D& coord = Vector2D( 0, 0 ) ) {
		m_Position = pos;
		m_TexCoord = coord;
	}

	Vector2D m_Position;
	Vector2D m_TexCoord;
};

struct DrawModelState_t {
	studiohdr_t* m_pStudioHdr;
	studiohwdata_t* m_pStudioHWData;
	void* m_pRenderable;
	const matrix3x4_t* m_pModelToWorld;
	StudioDecalHandle_t     m_decals;
	int                     m_drawFlags;
	int                     m_lod;
};

struct LightingQuery_t {
	Vector                  m_LightingOrigin;
	ModelInstanceHandle_t   m_InstanceHandle;
	bool                    m_bAmbientBoost;
};

typedef struct InputContextHandle_t__* InputContextHandle_t;

struct BeamTrail_t {
	// NOTE:  Don't add user defined fields except after these four fields.
	BeamTrail_t* next;
	float			die;
	Vector			org;
	Vector			vel;
};

struct cplane_t {
	Vector normal;
	float dist;
	uint8_t type;   // for fast side tests
	uint8_t signbits;  // signx + (signy<<1) + (signz<<1)
	uint8_t pad[ 2 ];

};
struct MaterialVideoMode_t {
	int m_Width;			// if width and height are 0 and you select 
	int m_Height;			// windowed mode, it'll use the window size
	ImageFormat m_Format;	// use ImageFormats (ignored for windowed mode)
	int m_RefreshRate;		// 0 == default (ignored for windowed mode)
};
//-----------------------------------------------------------------------------
// Enumeration interface for EnumerateLinkEntities
//-----------------------------------------------------------------------------
class IEntityEnumerator {
public:
	// This gets called with each handle
	virtual bool EnumEntity( void* pHandleEntity ) = 0;
};

class C_CSPlayer;
struct BeamInfo_t {
	//Beam
	int				m_nType;
	C_CSPlayer* m_pStartEnt;
	int				m_nStartAttachment;
	C_CSPlayer* m_pEndEnt;
	int				m_nEndAttachment;
	Vector			m_vecStart;
	Vector			m_vecEnd;
	int				m_nModelIndex;
	const char* m_pszModelName;
	int				m_nHaloIndex;
	const char* m_pszHaloName;
	float			m_flHaloScale;
	float			m_flLife;
	float			m_flWidth;
	float			m_flEndWidth;
	float			m_flFadeLength;
	float			m_flAmplitude;
	float			m_flBrightness;
	float			m_flSpeed;
	int				m_nStartFrame;
	float			m_flFrameRate;
	float			m_flRed;
	float			m_flGreen;
	float			m_flBlue;
	bool			m_bRenderable;
	int				m_nSegments;
	int				m_nFlags;
	// Rings
	Vector			m_vecCenter;
	float			m_flStartRadius;
	float			m_flEndRadius;

	BeamInfo_t( ) {
		m_nType = TE_BEAMPOINTS;
		m_nSegments = -1;
		m_pszModelName = NULL;
		m_pszHaloName = NULL;
		m_nModelIndex = -1;
		m_nHaloIndex = -1;
		m_bRenderable = true;
		m_nFlags = 0;
	}
};
struct ShaderStencilState_t {
	bool m_bEnable;
	int m_FailOp;
	int m_ZFailOp;
	int m_PassOp;
	int m_CompareFunc;
	int m_nReferenceValue;
	int m_nTestMask;
	int m_nWriteMask;
};
struct BrushSideInfo_t {
	Vector4D plane;               // The plane of the brush side
	unsigned short bevel;    // Bevel plane?
	unsigned short thin;     // Thin?
};

class CPhysCollide;


struct vcollide_t {
	unsigned short solidCount : 15;
	unsigned short isPacked : 1;
	unsigned short descSize;
	// VPhysicsSolids
	CPhysCollide** solids;
	char* pKeyValues;
	void* pUserData;
};

struct cmodel_t {
	Vector         mins, maxs;
	Vector         origin;        // for sounds or lights
	int            headnode;
	vcollide_t     vcollisionData;
};

struct csurface_t {
	const char* name;
	short          surfaceProps;
	unsigned short flags;         // BUGBUG: These are declared per surface, not per material, but this database is per-material now
};
struct ApplicationInstantCountersInfo_t {
	uint32_t m_nCpuActivityMask;
	uint32_t m_nDeferredWordsAllocated;
};
struct MaterialAdapterInfo_t {
	char m_pDriverName[ MATERIAL_ADAPTER_NAME_LENGTH ];
	unsigned int m_VendorID;
	unsigned int m_DeviceID;
	unsigned int m_SubSysID;
	unsigned int m_Revision;
	int m_nDXSupportLevel;			// This is the *preferred* dx support level
	int m_nMinDXSupportLevel;
	int m_nMaxDXSupportLevel;
	unsigned int m_nDriverVersionHigh;
	unsigned int m_nDriverVersionLow;
};
struct StaticLightingQuery_t : public LightingQuery_t {
	void* m_pRenderable;
};
struct StaticPropRenderInfo_t {
	const matrix3x4_t* pModelToWorld;
	const model_t* pModel;
	void* pRenderable;
	Vector* pLightingOrigin;
	short                   skin;
	ModelInstanceHandle_t   instance;
};

struct VPlane {
	Vector        m_Normal;
	float         m_Dist;
};

#pragma endregion

//typedefs
typedef VPlane Frustum[ FRUSTUM_NUMPLANES ];

// 
// classes
// 
#pragma region decl_classes
class IHandleEntity;
class IClientUnknown;
class ICollideable;
class IClientNetworkable;
class IClientRenderable;
class IClientEntity;
class C_BaseEntity;
class C_BaseAnimating;
class C_BaseCombatCharacter;
class C_BasePlayer;
class C_CSPlayer;
class C_BaseCombatWeapon;
class C_WeaponCSBaseGun;
class bf_write;
class bf_read;
class DVariant;
class CRecvProxyData;
class RecvProp;
class RecvTable;

class ClientClass;
class CUserCmd;
class CVerifiedUserCmd;
class CGlobalVarsBase;
class CGlobalVars;
class CMoveData;
class CGamestatsData;
class ITexture {
public:
	virtual const char* GetName( void ) const = 0;
	virtual int GetMappingWidth( ) const = 0;
	virtual int GetMappingHeight( ) const = 0;
	virtual int GetActualWidth( ) const = 0;
	virtual int GetActualHeight( ) const = 0;
	// lazy asf
};
class CEnvTonemapContorller {
public:
	bool* m_bUseCustomAutoExposureMax( ) {
		return ( bool* )( ( DWORD )this + ( DWORD )0x9D9 );
	}

	bool* m_bUseCustomAutoExposureMin( ) {
		return ( bool* )( ( DWORD )this + ( DWORD )0x9D8 );
	}

	float* m_flCustomAutoExposureMax( ) {
		return ( float* )( ( DWORD )this + ( DWORD )0x9E0 );
	}

	float* m_flCustomAutoExposureMin( ) {
		return ( float* )( ( DWORD )this + ( DWORD )0x9DC );
	}
	bool* m_bUseCustomBloomScale( ) {
		return ( bool* )( ( DWORD )this + ( DWORD )0x9DA );
	}
	float* m_flCustomBloomScale( ) {
		return ( float* )( ( DWORD )this + ( DWORD )0x9E4 );
	}
	float* m_flCustomBloomScaleMinimum( ) {
		return ( float* )( ( DWORD )this + ( DWORD )0x9E8 );
	}
	float* m_flBloomExponent( ) {
		return ( float* )( ( DWORD )this + ( DWORD )0x9EC );
	}
	float* m_flBloomSaturation( ) {
		return ( float* )( ( DWORD )this + ( DWORD )0x9F0 );
	}
	float* m_flTonemapPercentTarget( ) {
		return ( float* )( ( DWORD )this + ( DWORD )0x9F4 );
	}
	float* m_flTonemapPercentBrightPixels( ) {
		return ( float* )( ( DWORD )this + ( DWORD )0x9F8 );
	}
	float* m_flTonemapMinAvgLum( ) {
		return ( float* )( ( DWORD )this + ( DWORD )0x9FC );
	}
	float* m_flTonemapRate( ) {
		return ( float* )( ( DWORD )this + ( DWORD )0xA00 );
	}
};

class IMaterialVar {
public:
	ITexture* GetTextureValue( ) {
		return Memory::VCall< ITexture* ( __thiscall* )( void* ) >( this, 1 )( this );
	}

	void SetFloatValue( float value ) {
		Memory::VCall< void( __thiscall* )( void*, float ) >( this, 4 )( this, value );
	}

	void SetIntValue( int value ) {
		Memory::VCall< void( __thiscall* )( void*, int ) >( this, 5 )( this, value );
	}

	void SetStringValue( char const* value ) {
		Memory::VCall< void( __thiscall* )( void*, char const* ) >( this, 6 )( this, value );
	}

	void SetVecValue( float value1, float value2, float value3 ) {
		Memory::VCall< void( __thiscall* )( void*, float, float, float ) >( this, 11 )( this, value1, value2, value3 );
	}

	void SetVecComponent( const float value1, const int component ) {
		Memory::VCall< void( __thiscall* )( void*, float, int ) >( this, 26 )( this, value1, component );
	}

	void SetTextureValue( ITexture* tex ) {
		// Memory::VCall< void( __thiscall* )( void*, ITexture* ) >( this, 15 )( this, tex );
	}
};
class KeyValues;
class IMaterialProxyFactory;
class IMaterialSystemHardwareConfig;
class CShadowMgr;
class IShader;
class IMaterial;
class CStudioHdr;
class IMatRenderContext;
class DataCacheHandle_t;
class CSteamAPIContext;
class KeyValues;
class CBaseHandle;
class IMatRenderContext {
public:
	auto Release( void ) -> int {
		typedef int( __thiscall* original_fn )( void* );
		return Memory::VCall<original_fn>( this, 1 )( this );
	}

	auto SetRenderTarget( ITexture* pTexture ) -> void {
		typedef void( __thiscall* original_fn )( void*, ITexture* );
		return Memory::VCall<original_fn>( this, 6 )( this, pTexture );
	}

	void ClearBuffers( bool bClearColor, bool bClearDepth, bool bClearStencil = false ) {
		typedef void( __thiscall* original_fn )( void*, bool, bool, bool );
		return Memory::VCall<original_fn>( this, 12 )( this, bClearColor, bClearDepth, bClearStencil );
	}
	
	void Viewport( int x, int y, int width, int height ) {
		typedef void( __thiscall* original_fn )( void*, int, int, int, int );
		return Memory::VCall<original_fn>( this, 40 )( this, x, y, width, height );
	}

	void GetViewport( int& x, int& y, int& width, int& height ) {
		typedef void( __thiscall* original_fn )( void*, int&, int&, int&, int& );
		return Memory::VCall<original_fn>( this, 41 )( this, x, y, width, height );
	}

	auto DrawScreenSpaceRectangle( IMaterial* pMaterial, int destX, int destY, int width, int height, float srcTextureX0, float srcTextureY0, float srcTextureX1, float srcTextureY1, int srcTextureWidth, int srcTextureHeight, void* pClientRenderable, int nXDice, int nYDice ) -> void {
		typedef void( __thiscall* original_fn )( void*, IMaterial*, int, int, int, int, float, float, float, float, int, int, void*, int, int );
		return Memory::VCall<original_fn>( this, 114 )( this, pMaterial, destX, destY, width, height, srcTextureX0, srcTextureY0, srcTextureX1, srcTextureY1, srcTextureWidth, srcTextureHeight, pClientRenderable, nXDice, nYDice );
	}

	auto PushRenderTargetAndViewport( void ) -> void {
		typedef void( __thiscall* original_fn )( void* );
		return Memory::VCall<original_fn>( this, 119 )( this );
	}

	auto PopRenderTargetAndViewport( void ) -> void {
		typedef void( __thiscall* original_fn )( void* );
		return Memory::VCall<original_fn>( this, 120 )( this );
	}

	// TODO: BRUTEFORCE THESE INDEXES LATER NOT NOW COS I DONT CARE
	void SetStencilState( const ShaderStencilState_t& state ) {
		typedef void( __thiscall* original_fn )( void*, const ShaderStencilState_t& );
		return Memory::VCall<original_fn>( this, 127 )( this, state );
	}

	void BeginPIXEvent( unsigned long color, const char* szName ) {
		typedef void( __thiscall* original_fn )( void*, unsigned long, const char* );
		return Memory::VCall<original_fn>( this, 144 )( this, color, szName );
	}

	void ClearColor3ub( unsigned char r, unsigned char g, unsigned char b ) {
		typedef void( __thiscall* original_fn )( void*, unsigned char, unsigned char, unsigned char );
		return Memory::VCall<original_fn>( this, 78 )( this, r, g, b );
	}

	void OverrideDepthEnable( bool bEnable, bool bDepthWriteEnable, bool bDepthTestEnable = true ) {
		typedef void( __thiscall* original_fn )( void*, bool, bool, bool );
		return Memory::VCall<original_fn>( this, 80 )( this, bEnable, bDepthWriteEnable, bDepthTestEnable );
	}

};
#pragma endregion

// 
// interfaces
// 
#pragma region decl_interfaces
class IBaseClientDLL;
class IClientEntityList;
class IGameMovement;
class IPrediction;
class IMoveHelper;
class IInput;
class ICVar;
class IVEngineClient;
class IGameEventManager;
class IPanel;
class IMDLCache;
#pragma endregion


// 
// function types
// 
#pragma region decl_function_types
using CreateInterfaceFn = void* ( * )( const char*, int* );
using RecvVarProxyFn = void( * )( const CRecvProxyData*, void*, void* );
using pfnDemoCustomDataCallback = void( * )( uint8_t* pData, size_t iSize );
#pragma endregion


// 
// implementation
// 

#pragma region impl_structs
struct string_t {
public:
	const char* ToCStr( ) const;
protected:
	const char* pszValue;
};


#pragma endregion

#pragma region impl_classes


class DVariant {
public:
	union {
		float m_Float;
		int m_Int;
		const char* m_pString;
		void* m_pData;
		float m_Vector[ 3 ];
	};
	SendPropType m_Type;
};

class CRecvProxyData {
public:
	const RecvProp* m_pRecvProp;
private:
	std::uint8_t __pad[ 4 ];
public:
	DVariant m_Value;
	int m_iElement;
	int m_ObjectID;
};

class RecvProp {
public:
	const char* m_pVarName;
	SendPropType m_RecvType;
	int m_Flags;
	int m_StringBufferSize;
	bool m_bInsideArray;
	const void* m_pExtraData;
	RecvProp* m_pArrayProp;
	void* m_ArrayLengthProxy;
	RecvVarProxyFn m_ProxyFn;
	void* m_DataTableProxyFn;
	RecvTable* m_pDataTable;
	int m_Offset;
	int m_ElementStride;
	int m_nElements;
	const char* m_pParentArrayPropName;
};


enum class MotionBlurMode_t {
	MOTION_BLUR_DISABLE = 1,
	MOTION_BLUR_GAME = 2,
	MOTION_BLUR_SFM = 3
};

class CViewSetup {
public:
	int x;
	int oldX;
	int y;
	int oldY;
	int width;
	int oldWidth;
	int height;
	int oldHeight;

	bool m_bOrtho;
	float m_OrthoLeft;
	float m_OrthoTop;
	float m_OrthoRight;
	float m_OrthoBottom;

private:
	char pad1[ 0x7C ];

public:
	float fov;
	float fovViewmodel;
	Vector origin;
	QAngle angles;

	float zNear;
	float zFar;
	float zNearViewmodel;
	float zFarViewmodel;

	float m_flAspectRatio;
	float m_flNearBlurDepth;
	float m_flNearFocusDepth;
	float m_flFarFocusDepth;
	float m_flFarBlurDepth;
	float m_flNearBlurRadius;
	float m_flFarBlurRadius;
	int m_nDoFQuality;
	MotionBlurMode_t m_nMotionBlurMode;

	float m_flShutterTime;
	Vector m_vShutterOpenPosition;
	QAngle m_shutterOpenAngles;
	Vector m_vShutterClosePosition;
	QAngle m_shutterCloseAngles;

	float m_flOffCenterTop;
	float m_flOffCenterBottom;
	float m_flOffCenterLeft;
	float m_flOffCenterRight;
	
	int m_iEdgeBlur;
};

class CCycleCount {
public:
	long long			m_Int64;
};

class CFastTimer {
public:

private:
	CCycleCount	m_Duration;
	bool m_bRunning;		// Are we currently running?
};

struct DrawModelResults_t {
	int m_ActualTriCount;
	int m_TextureMemoryBytes;
	int m_NumHardwareBones;
	int m_NumBatches;
	int m_NumMaterials;
	int m_nLODUsed;
	int m_flLODMetric;
	CFastTimer m_RenderTime;
	char m_Materials[ 0x20 ];
};

struct ModelRenderInfo_t {
	Vector                  origin;
	QAngle                  angles;
	//char					pad[ 0x4 ];
	IClientRenderable* pRenderable;
	const model_t* pModel;
	const matrix3x4_t* pModelToWorld;
	const matrix3x4_t* pLightingOffset;
	const Vector* pLightingOrigin;
	int                     flags;
	int                     entity_index;
	int                     skin;
	int                     body;
	int                     hitboxset;
	ModelInstanceHandle_t   instance;

	ModelRenderInfo_t( ) {
		pModelToWorld = NULL;
		pLightingOffset = NULL;
		pLightingOrigin = NULL;
	}
};
class LightDesc_t {
public:
	LightType_t m_Type;                 //0x0000
	Vector m_Color;                     //0x0004
	Vector m_Position;                  //0x0010
	Vector m_Direction;                 //0x001C
	float m_Range;                      //0x0028
	float m_Falloff;                    //0x002C
	float m_Attenuation0;               //0x0030
	float m_Attenuation1;               //0x0034
	float m_Attenuation2;               //0x0038
	float m_Theta;                      //0x003C
	float m_Phi;                        //0x0040
	float m_ThetaDot;                   //0x0044
	float m_PhiDot;                     //0x0048
	float m_OneOverThetaDotMinusPhiDot; //0x004C
	__int32 m_Flags;                    //0x0050
	float m_RangeSquared;               //0x0054

}; //Size=0x0058

class lightpos_t {
public:
	Vector delta;  //0x0000
	float falloff; //0x000C
	float dot;     //0x0010

}; //Size=0x0014

struct MaterialLightingState_t {
	Vector m_vecAmbientCube[ 6 ]; // ambient, and lights that aren't in locallight[]
	Vector m_vecLightingOrigin; // The position from which lighting state was computed
	int m_nLocalLightCount;
	LightDesc_t m_pLocalLightDesc[ 4 ];
};
struct DrawModelInfo_t {
	studiohdr_t* m_pStudioHdr;
	studiohwdata_t* m_pHardwareData;
	StudioDecalHandle_t m_Decals;
	int m_Skin;
	int m_Body;
	int m_HitboxSet;
	IClientRenderable* m_pClientEntity;
	int m_Lod;
	ColorMeshInfo_t* m_pColorMeshes;
	bool m_bStaticLighting;
	MaterialLightingState_t m_LightingState;
};
class RecvTable {
public:
	RecvProp* m_pProps;
	int m_nProps;
	void* m_pDecoder;
	const char* m_pNetTableName;
	bool m_bInitialized;
	bool m_bInMainList;
};

class ClientClass {
public:
	void* m_pCreateFn;
	void* m_pCreateEventFn;
	const char* m_pNetworkName;
	RecvTable* m_pRecvTable;
	ClientClass* m_pNext;
	int m_ClassID;
};

enum animstate_pose_param_idx_t {
	FIRST = 0,
	LEAN_YAW = FIRST,
	SPEED,
	LADDER_SPEED,
	LADDER_YAW,
	MOVE_YAW,
	RUN,
	BODY_YAW,
	BODY_PITCH,
	DEATH_YAW,
	STAND,
	JUMP_FALL,
	AIM_BLEND_STAND_IDLE,
	AIM_BLEND_CROUCH_IDLE,
	STRAFE_DIR,
	AIM_BLEND_STAND_WALK,
	AIM_BLEND_STAND_RUN,
	AIM_BLEND_CROUCH_WALK,
	MOVE_BLEND_WALK,
	MOVE_BLEND_RUN,
	MOVE_BLEND_CROUCH_WALK,
	//STRAFE_CROSS,
	COUNT,
};

enum animstate_layer_t {
	ANIMATION_LAYER_AIMMATRIX = 0,
	ANIMATION_LAYER_WEAPON_ACTION,
	ANIMATION_LAYER_WEAPON_ACTION_RECROUCH,
	ANIMATION_LAYER_ADJUST,
	ANIMATION_LAYER_MOVEMENT_JUMP_OR_FALL,  // fixed
	ANIMATION_LAYER_MOVEMENT_LAND_OR_CLIMB, // fixed
	ANIMATION_LAYER_MOVEMENT_MOVE,          // still not fixed
	ANIMATION_LAYER_MOVEMENT_STRAFECHANGE,
	ANIMATION_LAYER_WHOLE_BODY,
	ANIMATION_LAYER_FLASHED,
	ANIMATION_LAYER_FLINCH,
	ANIMATION_LAYER_ALIVELOOP,
	ANIMATION_LAYER_LEAN,
	ANIMATION_LAYER_COUNT,
};

typedef enum
{
	ACT_INVALID = -1,			// So we have something more succint to check for than '-1'
	ACT_RESET = 0,				// Set m_Activity to this invalid value to force a reset to m_IdealActivity
	ACT_IDLE,
	ACT_TRANSITION,
	ACT_COVER,					// FIXME: obsolete? redundant with ACT_COVER_LOW?
	ACT_COVER_MED,				// FIXME: unsupported?
	ACT_COVER_LOW,				// FIXME: rename ACT_IDLE_CROUCH?
	ACT_WALK,
	ACT_WALK_AIM,
	ACT_WALK_CROUCH,
	ACT_WALK_CROUCH_AIM,
	ACT_RUN,
	ACT_RUN_AIM,
	ACT_RUN_CROUCH,
	ACT_RUN_CROUCH_AIM,
	ACT_RUN_PROTECTED,
	ACT_SCRIPT_CUSTOM_MOVE,
	ACT_RANGE_ATTACK1,
	ACT_RANGE_ATTACK2,
	ACT_RANGE_ATTACK1_LOW,		// FIXME: not used yet, crouched versions of the range attack
	ACT_RANGE_ATTACK2_LOW,		// FIXME: not used yet, crouched versions of the range attack
	ACT_DIESIMPLE,
	ACT_DIEBACKWARD,
	ACT_DIEFORWARD,
	ACT_DIEVIOLENT,
	ACT_DIERAGDOLL,
	ACT_FLY,				// Fly (and flap if appropriate)
	ACT_HOVER,
	ACT_GLIDE,
	ACT_SWIM,
	ACT_JUMP,
	ACT_HOP,				// vertical jump
	ACT_LEAP,				// long forward jump
	ACT_LAND,
	ACT_CLIMB_UP,
	ACT_CLIMB_DOWN,
	ACT_CLIMB_DISMOUNT,
	ACT_SHIPLADDER_UP,
	ACT_SHIPLADDER_DOWN,
	ACT_STRAFE_LEFT,
	ACT_STRAFE_RIGHT,
	ACT_ROLL_LEFT,			// tuck and roll, left
	ACT_ROLL_RIGHT,			// tuck and roll, right
	ACT_TURN_LEFT,			// turn quickly left (stationary)
	ACT_TURN_RIGHT,			// turn quickly right (stationary)
	ACT_CROUCH,				// FIXME: obsolete? only used be soldier (the act of crouching down from a standing position)
	ACT_CROUCHIDLE,			// FIXME: obsolete? only used be soldier (holding body in crouched position (loops))
	ACT_STAND,				// FIXME: obsolete? should be transition (the act of standing from a crouched position)
	ACT_USE,
	ACT_ALIEN_BURROW_IDLE,
	ACT_ALIEN_BURROW_OUT,

	ACT_SIGNAL1,
	ACT_SIGNAL2,
	ACT_SIGNAL3,

	ACT_SIGNAL_ADVANCE,		// Squad handsignals, specific.
	ACT_SIGNAL_FORWARD,
	ACT_SIGNAL_GROUP,
	ACT_SIGNAL_HALT,
	ACT_SIGNAL_LEFT,
	ACT_SIGNAL_RIGHT,
	ACT_SIGNAL_TAKECOVER,

	ACT_LOOKBACK_RIGHT,		// look back over shoulder without turning around.
	ACT_LOOKBACK_LEFT,
	ACT_COWER,				// FIXME: unused, should be more extreme version of crouching
	ACT_SMALL_FLINCH,		// FIXME: needed? shouldn't flinching be down with overlays?
	ACT_BIG_FLINCH,
	ACT_MELEE_ATTACK1,
	ACT_MELEE_ATTACK2,
	ACT_RELOAD,
	ACT_RELOAD_START,
	ACT_RELOAD_FINISH,
	ACT_RELOAD_LOW,
	ACT_ARM,				// pull out gun, for instance
	ACT_DISARM,				// reholster gun
	ACT_DROP_WEAPON,
	ACT_DROP_WEAPON_SHOTGUN,
	ACT_PICKUP_GROUND,		// pick up something in front of you on the ground
	ACT_PICKUP_RACK,		// pick up something from a rack or shelf in front of you.
	ACT_IDLE_ANGRY,			// FIXME: being used as an combat ready idle?  alternate idle animation in which the monster is clearly agitated. (loop)

	ACT_IDLE_RELAXED,
	ACT_IDLE_STIMULATED,
	ACT_IDLE_AGITATED,
	ACT_IDLE_STEALTH,
	ACT_IDLE_HURT,

	ACT_WALK_RELAXED,
	ACT_WALK_STIMULATED,
	ACT_WALK_AGITATED,
	ACT_WALK_STEALTH,

	ACT_RUN_RELAXED,
	ACT_RUN_STIMULATED,
	ACT_RUN_AGITATED,
	ACT_RUN_STEALTH,

	ACT_IDLE_AIM_RELAXED,
	ACT_IDLE_AIM_STIMULATED,
	ACT_IDLE_AIM_AGITATED,
	ACT_IDLE_AIM_STEALTH,

	ACT_WALK_AIM_RELAXED,
	ACT_WALK_AIM_STIMULATED,
	ACT_WALK_AIM_AGITATED,
	ACT_WALK_AIM_STEALTH,

	ACT_RUN_AIM_RELAXED,
	ACT_RUN_AIM_STIMULATED,
	ACT_RUN_AIM_AGITATED,
	ACT_RUN_AIM_STEALTH,

	ACT_CROUCHIDLE_STIMULATED,
	ACT_CROUCHIDLE_AIM_STIMULATED,
	ACT_CROUCHIDLE_AGITATED,

	ACT_WALK_HURT,			// limp  (loop)
	ACT_RUN_HURT,			// limp  (loop)
	ACT_SPECIAL_ATTACK1,	// very monster specific special attacks.
	ACT_SPECIAL_ATTACK2,
	ACT_COMBAT_IDLE,		// FIXME: unused?  agitated idle.
	ACT_WALK_SCARED,
	ACT_RUN_SCARED,
	ACT_VICTORY_DANCE,		// killed a player, do a victory dance.
	ACT_DIE_HEADSHOT,		// die, hit in head. 
	ACT_DIE_CHESTSHOT,		// die, hit in chest
	ACT_DIE_GUTSHOT,		// die, hit in gut
	ACT_DIE_BACKSHOT,		// die, hit in back
	ACT_FLINCH_HEAD,
	ACT_FLINCH_CHEST,
	ACT_FLINCH_STOMACH,
	ACT_FLINCH_LEFTARM,
	ACT_FLINCH_RIGHTARM,
	ACT_FLINCH_LEFTLEG,
	ACT_FLINCH_RIGHTLEG,
	ACT_FLINCH_PHYSICS,
	ACT_FLINCH_HEAD_BACK,
	ACT_FLINCH_HEAD_LEFT,
	ACT_FLINCH_HEAD_RIGHT,
	ACT_FLINCH_CHEST_BACK,
	ACT_FLINCH_STOMACH_BACK,
	ACT_FLINCH_CROUCH_FRONT,
	ACT_FLINCH_CROUCH_BACK,
	ACT_FLINCH_CROUCH_LEFT,
	ACT_FLINCH_CROUCH_RIGHT,

	ACT_IDLE_ON_FIRE,		// ON FIRE animations
	ACT_WALK_ON_FIRE,
	ACT_RUN_ON_FIRE,

	ACT_RAPPEL_LOOP,		// Rappel down a rope!

	ACT_180_LEFT,			// 180 degree left turn
	ACT_180_RIGHT,

	ACT_90_LEFT,			// 90 degree turns
	ACT_90_RIGHT,

	ACT_STEP_LEFT,			// Single steps
	ACT_STEP_RIGHT,
	ACT_STEP_BACK,
	ACT_STEP_FORE,

	ACT_GESTURE_RANGE_ATTACK1,
	ACT_GESTURE_RANGE_ATTACK2,
	ACT_GESTURE_MELEE_ATTACK1,
	ACT_GESTURE_MELEE_ATTACK2,
	ACT_GESTURE_RANGE_ATTACK1_LOW,	// FIXME: not used yet, crouched versions of the range attack
	ACT_GESTURE_RANGE_ATTACK2_LOW,	// FIXME: not used yet, crouched versions of the range attack

	ACT_MELEE_ATTACK_SWING_GESTURE,

	ACT_GESTURE_SMALL_FLINCH,
	ACT_GESTURE_BIG_FLINCH,
	ACT_GESTURE_FLINCH_BLAST,			// Startled by an explosion
	ACT_GESTURE_FLINCH_BLAST_SHOTGUN,
	ACT_GESTURE_FLINCH_BLAST_DAMAGED,	// Damaged by an explosion
	ACT_GESTURE_FLINCH_BLAST_DAMAGED_SHOTGUN,
	ACT_GESTURE_FLINCH_HEAD,
	ACT_GESTURE_FLINCH_CHEST,
	ACT_GESTURE_FLINCH_STOMACH,
	ACT_GESTURE_FLINCH_LEFTARM,
	ACT_GESTURE_FLINCH_RIGHTARM,
	ACT_GESTURE_FLINCH_LEFTLEG,
	ACT_GESTURE_FLINCH_RIGHTLEG,

	ACT_GESTURE_TURN_LEFT,
	ACT_GESTURE_TURN_RIGHT,
	ACT_GESTURE_TURN_LEFT45,
	ACT_GESTURE_TURN_RIGHT45,
	ACT_GESTURE_TURN_LEFT90,
	ACT_GESTURE_TURN_RIGHT90,
	ACT_GESTURE_TURN_LEFT45_FLAT,
	ACT_GESTURE_TURN_RIGHT45_FLAT,
	ACT_GESTURE_TURN_LEFT90_FLAT,
	ACT_GESTURE_TURN_RIGHT90_FLAT,

	// HALF-LIFE 1 compatability stuff goes here. Temporary!
	ACT_BARNACLE_HIT,		// barnacle tongue hits a monster
	ACT_BARNACLE_PULL,		// barnacle is lifting the monster ( loop )
	ACT_BARNACLE_CHOMP,		// barnacle latches on to the monster
	ACT_BARNACLE_CHEW,		// barnacle is holding the monster in its mouth ( loop )

	// Sometimes, you just want to set an NPC's sequence to a sequence that doesn't actually
	// have an activity. The AI will reset the NPC's sequence to whatever its IDEAL activity
	// is, though. So if you set ideal activity to DO_NOT_DISTURB, the AI will not interfere
	// with the NPC's current sequence. (SJB)
	ACT_DO_NOT_DISTURB,

	ACT_SPECIFIC_SEQUENCE,

	// viewmodel (weapon) activities
	// FIXME: move these to the specific viewmodels, no need to make global
	ACT_VM_DRAW,
	ACT_VM_HOLSTER,
	ACT_VM_IDLE,
	ACT_VM_FIDGET,
	ACT_VM_PULLBACK,
	ACT_VM_PULLBACK_HIGH,
	ACT_VM_PULLBACK_LOW,
	ACT_VM_THROW,
	ACT_VM_PULLPIN,
	ACT_VM_PRIMARYATTACK,		// fire
	ACT_VM_SECONDARYATTACK,		// alt. fire
	ACT_VM_RELOAD,
	ACT_VM_DRYFIRE,				// fire with no ammo loaded.
	ACT_VM_HITLEFT,				// bludgeon, swing to left - hit (primary attk)
	ACT_VM_HITLEFT2,			// bludgeon, swing to left - hit (secondary attk)
	ACT_VM_HITRIGHT,			// bludgeon, swing to right - hit (primary attk)
	ACT_VM_HITRIGHT2,			// bludgeon, swing to right - hit (secondary attk)
	ACT_VM_HITCENTER,			// bludgeon, swing center - hit (primary attk)
	ACT_VM_HITCENTER2,			// bludgeon, swing center - hit (secondary attk)
	ACT_VM_MISSLEFT,			// bludgeon, swing to left - miss (primary attk)
	ACT_VM_MISSLEFT2,			// bludgeon, swing to left - miss (secondary attk)
	ACT_VM_MISSRIGHT,			// bludgeon, swing to right - miss (primary attk)
	ACT_VM_MISSRIGHT2,			// bludgeon, swing to right - miss (secondary attk)
	ACT_VM_MISSCENTER,			// bludgeon, swing center - miss (primary attk)
	ACT_VM_MISSCENTER2,			// bludgeon, swing center - miss (secondary attk)
	ACT_VM_HAULBACK,			// bludgeon, haul the weapon back for a hard strike (secondary attk)
	ACT_VM_SWINGHARD,			// bludgeon, release the hard strike (secondary attk)
	ACT_VM_SWINGMISS,
	ACT_VM_SWINGHIT,
	ACT_VM_IDLE_TO_LOWERED,
	ACT_VM_IDLE_LOWERED,
	ACT_VM_LOWERED_TO_IDLE,
	ACT_VM_RECOIL1,
	ACT_VM_RECOIL2,
	ACT_VM_RECOIL3,
	ACT_VM_PICKUP,
	ACT_VM_RELEASE,

	ACT_VM_ATTACH_SILENCER,
	ACT_VM_DETACH_SILENCER,

	ACT_VM_EMPTY_FIRE,			// fire last round in magazine
	ACT_VM_EMPTY_RELOAD,        // Reload from an Empty state
	ACT_VM_EMPTY_DRAW,			// Deploy an Empty weapon
	ACT_VM_EMPTY_IDLE,			// Idle in an Empty state

//===========================
// HL2 Specific Activities
//===========================
	// SLAM	Specialty Activities
	ACT_SLAM_STICKWALL_IDLE,
	ACT_SLAM_STICKWALL_ND_IDLE,
	ACT_SLAM_STICKWALL_ATTACH,
	ACT_SLAM_STICKWALL_ATTACH2,
	ACT_SLAM_STICKWALL_ND_ATTACH,
	ACT_SLAM_STICKWALL_ND_ATTACH2,
	ACT_SLAM_STICKWALL_DETONATE,
	ACT_SLAM_STICKWALL_DETONATOR_HOLSTER,
	ACT_SLAM_STICKWALL_DRAW,
	ACT_SLAM_STICKWALL_ND_DRAW,
	ACT_SLAM_STICKWALL_TO_THROW,
	ACT_SLAM_STICKWALL_TO_THROW_ND,
	ACT_SLAM_STICKWALL_TO_TRIPMINE_ND,
	ACT_SLAM_THROW_IDLE,
	ACT_SLAM_THROW_ND_IDLE,
	ACT_SLAM_THROW_THROW,
	ACT_SLAM_THROW_THROW2,
	ACT_SLAM_THROW_THROW_ND,
	ACT_SLAM_THROW_THROW_ND2,
	ACT_SLAM_THROW_DRAW,
	ACT_SLAM_THROW_ND_DRAW,
	ACT_SLAM_THROW_TO_STICKWALL,
	ACT_SLAM_THROW_TO_STICKWALL_ND,
	ACT_SLAM_THROW_DETONATE,
	ACT_SLAM_THROW_DETONATOR_HOLSTER,
	ACT_SLAM_THROW_TO_TRIPMINE_ND,
	ACT_SLAM_TRIPMINE_IDLE,
	ACT_SLAM_TRIPMINE_DRAW,
	ACT_SLAM_TRIPMINE_ATTACH,
	ACT_SLAM_TRIPMINE_ATTACH2,
	ACT_SLAM_TRIPMINE_TO_STICKWALL_ND,
	ACT_SLAM_TRIPMINE_TO_THROW_ND,
	ACT_SLAM_DETONATOR_IDLE,
	ACT_SLAM_DETONATOR_DRAW,
	ACT_SLAM_DETONATOR_DETONATE,
	ACT_SLAM_DETONATOR_HOLSTER,
	ACT_SLAM_DETONATOR_STICKWALL_DRAW,
	ACT_SLAM_DETONATOR_THROW_DRAW,

	// Shotgun Specialty Activities
	ACT_SHOTGUN_RELOAD_START,
	ACT_SHOTGUN_RELOAD_FINISH,
	ACT_SHOTGUN_PUMP,

	// SMG2 special activities
	ACT_SMG2_IDLE2,
	ACT_SMG2_FIRE2,
	ACT_SMG2_DRAW2,
	ACT_SMG2_RELOAD2,
	ACT_SMG2_DRYFIRE2,
	ACT_SMG2_TOAUTO,
	ACT_SMG2_TOBURST,

	// Physcannon special activities
	ACT_PHYSCANNON_UPGRADE,

	// weapon override activities
	ACT_RANGE_ATTACK_AR1,
	ACT_RANGE_ATTACK_AR2,
	ACT_RANGE_ATTACK_AR2_LOW,
	ACT_RANGE_ATTACK_AR2_GRENADE,
	ACT_RANGE_ATTACK_HMG1,
	ACT_RANGE_ATTACK_ML,
	ACT_RANGE_ATTACK_SMG1,
	ACT_RANGE_ATTACK_SMG1_LOW,
	ACT_RANGE_ATTACK_SMG2,
	ACT_RANGE_ATTACK_SHOTGUN,
	ACT_RANGE_ATTACK_SHOTGUN_LOW,
	ACT_RANGE_ATTACK_PISTOL,
	ACT_RANGE_ATTACK_PISTOL_LOW,
	ACT_RANGE_ATTACK_SLAM,
	ACT_RANGE_ATTACK_TRIPWIRE,
	ACT_RANGE_ATTACK_THROW,
	ACT_RANGE_ATTACK_SNIPER_RIFLE,
	ACT_RANGE_ATTACK_RPG,
	ACT_MELEE_ATTACK_SWING,

	ACT_RANGE_AIM_LOW,
	ACT_RANGE_AIM_SMG1_LOW,
	ACT_RANGE_AIM_PISTOL_LOW,
	ACT_RANGE_AIM_AR2_LOW,

	ACT_COVER_PISTOL_LOW,
	ACT_COVER_SMG1_LOW,

	// weapon override activities
	ACT_GESTURE_RANGE_ATTACK_AR1,
	ACT_GESTURE_RANGE_ATTACK_AR2,
	ACT_GESTURE_RANGE_ATTACK_AR2_GRENADE,
	ACT_GESTURE_RANGE_ATTACK_HMG1,
	ACT_GESTURE_RANGE_ATTACK_ML,
	ACT_GESTURE_RANGE_ATTACK_SMG1,
	ACT_GESTURE_RANGE_ATTACK_SMG1_LOW,
	ACT_GESTURE_RANGE_ATTACK_SMG2,
	ACT_GESTURE_RANGE_ATTACK_SHOTGUN,
	ACT_GESTURE_RANGE_ATTACK_PISTOL,
	ACT_GESTURE_RANGE_ATTACK_PISTOL_LOW,
	ACT_GESTURE_RANGE_ATTACK_SLAM,
	ACT_GESTURE_RANGE_ATTACK_TRIPWIRE,
	ACT_GESTURE_RANGE_ATTACK_THROW,
	ACT_GESTURE_RANGE_ATTACK_SNIPER_RIFLE,
	ACT_GESTURE_MELEE_ATTACK_SWING,

	ACT_IDLE_RIFLE,
	ACT_IDLE_SMG1,
	ACT_IDLE_ANGRY_SMG1,
	ACT_IDLE_PISTOL,
	ACT_IDLE_ANGRY_PISTOL,
	ACT_IDLE_ANGRY_SHOTGUN,
	ACT_IDLE_STEALTH_PISTOL,

	ACT_IDLE_PACKAGE,
	ACT_WALK_PACKAGE,
	ACT_IDLE_SUITCASE,
	ACT_WALK_SUITCASE,

	ACT_IDLE_SMG1_RELAXED,
	ACT_IDLE_SMG1_STIMULATED,
	ACT_WALK_RIFLE_RELAXED,
	ACT_RUN_RIFLE_RELAXED,
	ACT_WALK_RIFLE_STIMULATED,
	ACT_RUN_RIFLE_STIMULATED,

	ACT_IDLE_AIM_RIFLE_STIMULATED,
	ACT_WALK_AIM_RIFLE_STIMULATED,
	ACT_RUN_AIM_RIFLE_STIMULATED,

	ACT_IDLE_SHOTGUN_RELAXED,
	ACT_IDLE_SHOTGUN_STIMULATED,
	ACT_IDLE_SHOTGUN_AGITATED,

	// Policing activities
	ACT_WALK_ANGRY,
	ACT_POLICE_HARASS1,
	ACT_POLICE_HARASS2,

	// Manned guns
	ACT_IDLE_MANNEDGUN,

	// Melee weapon
	ACT_IDLE_MELEE,
	ACT_IDLE_ANGRY_MELEE,

	// RPG activities
	ACT_IDLE_RPG_RELAXED,
	ACT_IDLE_RPG,
	ACT_IDLE_ANGRY_RPG,
	ACT_COVER_LOW_RPG,
	ACT_WALK_RPG,
	ACT_RUN_RPG,
	ACT_WALK_CROUCH_RPG,
	ACT_RUN_CROUCH_RPG,
	ACT_WALK_RPG_RELAXED,
	ACT_RUN_RPG_RELAXED,

	ACT_WALK_RIFLE,
	ACT_WALK_AIM_RIFLE,
	ACT_WALK_CROUCH_RIFLE,
	ACT_WALK_CROUCH_AIM_RIFLE,
	ACT_RUN_RIFLE,
	ACT_RUN_AIM_RIFLE,
	ACT_RUN_CROUCH_RIFLE,
	ACT_RUN_CROUCH_AIM_RIFLE,
	ACT_RUN_STEALTH_PISTOL,

	ACT_WALK_AIM_SHOTGUN,
	ACT_RUN_AIM_SHOTGUN,

	ACT_WALK_PISTOL,
	ACT_RUN_PISTOL,
	ACT_WALK_AIM_PISTOL,
	ACT_RUN_AIM_PISTOL,
	ACT_WALK_STEALTH_PISTOL,
	ACT_WALK_AIM_STEALTH_PISTOL,
	ACT_RUN_AIM_STEALTH_PISTOL,

	// Reloads
	ACT_RELOAD_PISTOL,
	ACT_RELOAD_PISTOL_LOW,
	ACT_RELOAD_SMG1,
	ACT_RELOAD_SMG1_LOW,
	ACT_RELOAD_SHOTGUN,
	ACT_RELOAD_SHOTGUN_LOW,

	ACT_GESTURE_RELOAD,
	ACT_GESTURE_RELOAD_PISTOL,
	ACT_GESTURE_RELOAD_SMG1,
	ACT_GESTURE_RELOAD_SHOTGUN,

	// Busy animations
	ACT_BUSY_LEAN_LEFT,
	ACT_BUSY_LEAN_LEFT_ENTRY,
	ACT_BUSY_LEAN_LEFT_EXIT,
	ACT_BUSY_LEAN_BACK,
	ACT_BUSY_LEAN_BACK_ENTRY,
	ACT_BUSY_LEAN_BACK_EXIT,
	ACT_BUSY_SIT_GROUND,
	ACT_BUSY_SIT_GROUND_ENTRY,
	ACT_BUSY_SIT_GROUND_EXIT,
	ACT_BUSY_SIT_CHAIR,
	ACT_BUSY_SIT_CHAIR_ENTRY,
	ACT_BUSY_SIT_CHAIR_EXIT,
	ACT_BUSY_STAND,
	ACT_BUSY_QUEUE,

	// Dodge animations
	ACT_DUCK_DODGE,

	// For NPCs being lifted/eaten by barnacles:
	// being swallowed by a barnacle
	ACT_DIE_BARNACLE_SWALLOW,
	// being lifted by a barnacle
	ACT_GESTURE_BARNACLE_STRANGLE,

	ACT_PHYSCANNON_DETACH,	// An activity to be played if we're picking this up with the physcannon
	ACT_PHYSCANNON_ANIMATE, // An activity to be played by an object being picked up with the physcannon, but has different behavior to DETACH
	ACT_PHYSCANNON_ANIMATE_PRE,	// An activity to be played by an object being picked up with the physcannon, before playing the ACT_PHYSCANNON_ANIMATE
	ACT_PHYSCANNON_ANIMATE_POST,// An activity to be played by an object being picked up with the physcannon, after playing the ACT_PHYSCANNON_ANIMATE

	ACT_DIE_FRONTSIDE,
	ACT_DIE_RIGHTSIDE,
	ACT_DIE_BACKSIDE,
	ACT_DIE_LEFTSIDE,

	ACT_DIE_CROUCH_FRONTSIDE,
	ACT_DIE_CROUCH_RIGHTSIDE,
	ACT_DIE_CROUCH_BACKSIDE,
	ACT_DIE_CROUCH_LEFTSIDE,

	ACT_OPEN_DOOR,

	// Dynamic interactions
	ACT_DI_ALYX_ZOMBIE_MELEE,
	ACT_DI_ALYX_ZOMBIE_TORSO_MELEE,
	ACT_DI_ALYX_HEADCRAB_MELEE,
	ACT_DI_ALYX_ANTLION,

	ACT_DI_ALYX_ZOMBIE_SHOTGUN64,
	ACT_DI_ALYX_ZOMBIE_SHOTGUN26,

	ACT_READINESS_RELAXED_TO_STIMULATED,
	ACT_READINESS_RELAXED_TO_STIMULATED_WALK,
	ACT_READINESS_AGITATED_TO_STIMULATED,
	ACT_READINESS_STIMULATED_TO_RELAXED,

	ACT_READINESS_PISTOL_RELAXED_TO_STIMULATED,
	ACT_READINESS_PISTOL_RELAXED_TO_STIMULATED_WALK,
	ACT_READINESS_PISTOL_AGITATED_TO_STIMULATED,
	ACT_READINESS_PISTOL_STIMULATED_TO_RELAXED,

	ACT_IDLE_CARRY,
	ACT_WALK_CARRY,

	//===========================
	// TF2 Specific Activities
	//===========================
	ACT_STARTDYING,
	ACT_DYINGLOOP,
	ACT_DYINGTODEAD,

	ACT_RIDE_MANNED_GUN,

	// All viewmodels
	ACT_VM_SPRINT_ENTER,
	ACT_VM_SPRINT_IDLE,
	ACT_VM_SPRINT_LEAVE,

	// Looping weapon firing
	ACT_FIRE_START,
	ACT_FIRE_LOOP,
	ACT_FIRE_END,

	ACT_CROUCHING_GRENADEIDLE,
	ACT_CROUCHING_GRENADEREADY,
	ACT_CROUCHING_PRIMARYATTACK,
	ACT_OVERLAY_GRENADEIDLE,
	ACT_OVERLAY_GRENADEREADY,
	ACT_OVERLAY_PRIMARYATTACK,
	ACT_OVERLAY_SHIELD_UP,
	ACT_OVERLAY_SHIELD_DOWN,
	ACT_OVERLAY_SHIELD_UP_IDLE,
	ACT_OVERLAY_SHIELD_ATTACK,
	ACT_OVERLAY_SHIELD_KNOCKBACK,
	ACT_SHIELD_UP,
	ACT_SHIELD_DOWN,
	ACT_SHIELD_UP_IDLE,
	ACT_SHIELD_ATTACK,
	ACT_SHIELD_KNOCKBACK,
	ACT_CROUCHING_SHIELD_UP,
	ACT_CROUCHING_SHIELD_DOWN,
	ACT_CROUCHING_SHIELD_UP_IDLE,
	ACT_CROUCHING_SHIELD_ATTACK,
	ACT_CROUCHING_SHIELD_KNOCKBACK,

	// turning in place
	ACT_TURNRIGHT45,
	ACT_TURNLEFT45,

	ACT_TURN,

	ACT_OBJ_ASSEMBLING,
	ACT_OBJ_DISMANTLING,
	ACT_OBJ_STARTUP,
	ACT_OBJ_RUNNING,
	ACT_OBJ_IDLE,
	ACT_OBJ_PLACING,
	ACT_OBJ_DETERIORATING,
	ACT_OBJ_UPGRADING,

	// Deploy
	ACT_DEPLOY,
	ACT_DEPLOY_IDLE,
	ACT_UNDEPLOY,

	// Crossbow
	ACT_CROSSBOW_DRAW_UNLOADED,

	// Gauss
	ACT_GAUSS_SPINUP,
	ACT_GAUSS_SPINCYCLE,

	//===========================
	// CSPort Specific Activities
	//===========================

	ACT_VM_PRIMARYATTACK_SILENCED,		// fire
	ACT_VM_RELOAD_SILENCED,
	ACT_VM_DRYFIRE_SILENCED,				// fire with no ammo loaded.
	ACT_VM_IDLE_SILENCED,
	ACT_VM_DRAW_SILENCED,
	ACT_VM_IDLE_EMPTY_LEFT,
	ACT_VM_DRYFIRE_LEFT,

	// new for CS2
	ACT_VM_IS_DRAW,
	ACT_VM_IS_HOLSTER,
	ACT_VM_IS_IDLE,
	ACT_VM_IS_PRIMARYATTACK,

	ACT_PLAYER_IDLE_FIRE,
	ACT_PLAYER_CROUCH_FIRE,
	ACT_PLAYER_CROUCH_WALK_FIRE,
	ACT_PLAYER_WALK_FIRE,
	ACT_PLAYER_RUN_FIRE,

	ACT_IDLETORUN,
	ACT_RUNTOIDLE,

	ACT_VM_DRAW_DEPLOYED,

	ACT_HL2MP_IDLE_MELEE,
	ACT_HL2MP_RUN_MELEE,
	ACT_HL2MP_IDLE_CROUCH_MELEE,
	ACT_HL2MP_WALK_CROUCH_MELEE,
	ACT_HL2MP_GESTURE_RANGE_ATTACK_MELEE,
	ACT_HL2MP_GESTURE_RELOAD_MELEE,
	ACT_HL2MP_JUMP_MELEE,

	// Portal!
	ACT_VM_FIZZLE,

	// Multiplayer
	ACT_MP_STAND_IDLE,
	ACT_MP_CROUCH_IDLE,
	ACT_MP_CROUCH_DEPLOYED_IDLE,
	ACT_MP_CROUCH_DEPLOYED,
	ACT_MP_DEPLOYED_IDLE,
	ACT_MP_RUN,
	ACT_MP_WALK,
	ACT_MP_AIRWALK,
	ACT_MP_CROUCHWALK,
	ACT_MP_SPRINT,
	ACT_MP_JUMP,
	ACT_MP_JUMP_START,
	ACT_MP_JUMP_FLOAT,
	ACT_MP_JUMP_LAND,
	ACT_MP_JUMP_IMPACT_N,
	ACT_MP_JUMP_IMPACT_E,
	ACT_MP_JUMP_IMPACT_W,
	ACT_MP_JUMP_IMPACT_S,
	ACT_MP_JUMP_IMPACT_TOP,
	ACT_MP_DOUBLEJUMP,
	ACT_MP_SWIM,
	ACT_MP_DEPLOYED,
	ACT_MP_SWIM_DEPLOYED,
	ACT_MP_VCD,

	ACT_MP_ATTACK_STAND_PRIMARYFIRE,
	ACT_MP_ATTACK_STAND_PRIMARYFIRE_DEPLOYED,
	ACT_MP_ATTACK_STAND_SECONDARYFIRE,
	ACT_MP_ATTACK_STAND_GRENADE,
	ACT_MP_ATTACK_CROUCH_PRIMARYFIRE,
	ACT_MP_ATTACK_CROUCH_PRIMARYFIRE_DEPLOYED,
	ACT_MP_ATTACK_CROUCH_SECONDARYFIRE,
	ACT_MP_ATTACK_CROUCH_GRENADE,
	ACT_MP_ATTACK_SWIM_PRIMARYFIRE,
	ACT_MP_ATTACK_SWIM_SECONDARYFIRE,
	ACT_MP_ATTACK_SWIM_GRENADE,
	ACT_MP_ATTACK_AIRWALK_PRIMARYFIRE,
	ACT_MP_ATTACK_AIRWALK_SECONDARYFIRE,
	ACT_MP_ATTACK_AIRWALK_GRENADE,
	ACT_MP_RELOAD_STAND,
	ACT_MP_RELOAD_STAND_LOOP,
	ACT_MP_RELOAD_STAND_END,
	ACT_MP_RELOAD_CROUCH,
	ACT_MP_RELOAD_CROUCH_LOOP,
	ACT_MP_RELOAD_CROUCH_END,
	ACT_MP_RELOAD_SWIM,
	ACT_MP_RELOAD_SWIM_LOOP,
	ACT_MP_RELOAD_SWIM_END,
	ACT_MP_RELOAD_AIRWALK,
	ACT_MP_RELOAD_AIRWALK_LOOP,
	ACT_MP_RELOAD_AIRWALK_END,
	ACT_MP_ATTACK_STAND_PREFIRE,
	ACT_MP_ATTACK_STAND_POSTFIRE,
	ACT_MP_ATTACK_STAND_STARTFIRE,
	ACT_MP_ATTACK_CROUCH_PREFIRE,
	ACT_MP_ATTACK_CROUCH_POSTFIRE,
	ACT_MP_ATTACK_SWIM_PREFIRE,
	ACT_MP_ATTACK_SWIM_POSTFIRE,

	// Multiplayer - Primary
	ACT_MP_STAND_PRIMARY,
	ACT_MP_CROUCH_PRIMARY,
	ACT_MP_RUN_PRIMARY,
	ACT_MP_WALK_PRIMARY,
	ACT_MP_AIRWALK_PRIMARY,
	ACT_MP_CROUCHWALK_PRIMARY,
	ACT_MP_JUMP_PRIMARY,
	ACT_MP_JUMP_START_PRIMARY,
	ACT_MP_JUMP_FLOAT_PRIMARY,
	ACT_MP_JUMP_LAND_PRIMARY,
	ACT_MP_SWIM_PRIMARY,
	ACT_MP_DEPLOYED_PRIMARY,
	ACT_MP_SWIM_DEPLOYED_PRIMARY,

	ACT_MP_ATTACK_STAND_PRIMARY,		// RUN, WALK
	ACT_MP_ATTACK_STAND_PRIMARY_DEPLOYED,
	ACT_MP_ATTACK_CROUCH_PRIMARY,		// CROUCHWALK
	ACT_MP_ATTACK_CROUCH_PRIMARY_DEPLOYED,
	ACT_MP_ATTACK_SWIM_PRIMARY,
	ACT_MP_ATTACK_AIRWALK_PRIMARY,

	ACT_MP_RELOAD_STAND_PRIMARY,		// RUN, WALK
	ACT_MP_RELOAD_STAND_PRIMARY_LOOP,
	ACT_MP_RELOAD_STAND_PRIMARY_END,
	ACT_MP_RELOAD_CROUCH_PRIMARY,		// CROUCHWALK
	ACT_MP_RELOAD_CROUCH_PRIMARY_LOOP,
	ACT_MP_RELOAD_CROUCH_PRIMARY_END,
	ACT_MP_RELOAD_SWIM_PRIMARY,
	ACT_MP_RELOAD_SWIM_PRIMARY_LOOP,
	ACT_MP_RELOAD_SWIM_PRIMARY_END,
	ACT_MP_RELOAD_AIRWALK_PRIMARY,
	ACT_MP_RELOAD_AIRWALK_PRIMARY_LOOP,
	ACT_MP_RELOAD_AIRWALK_PRIMARY_END,

	ACT_MP_ATTACK_STAND_GRENADE_PRIMARY,		// RUN, WALK
	ACT_MP_ATTACK_CROUCH_GRENADE_PRIMARY,		// CROUCHWALK
	ACT_MP_ATTACK_SWIM_GRENADE_PRIMARY,
	ACT_MP_ATTACK_AIRWALK_GRENADE_PRIMARY,


	// Secondary
	ACT_MP_STAND_SECONDARY,
	ACT_MP_CROUCH_SECONDARY,
	ACT_MP_RUN_SECONDARY,
	ACT_MP_WALK_SECONDARY,
	ACT_MP_AIRWALK_SECONDARY,
	ACT_MP_CROUCHWALK_SECONDARY,
	ACT_MP_JUMP_SECONDARY,
	ACT_MP_JUMP_START_SECONDARY,
	ACT_MP_JUMP_FLOAT_SECONDARY,
	ACT_MP_JUMP_LAND_SECONDARY,
	ACT_MP_SWIM_SECONDARY,

	ACT_MP_ATTACK_STAND_SECONDARY,		// RUN, WALK
	ACT_MP_ATTACK_CROUCH_SECONDARY,		// CROUCHWALK
	ACT_MP_ATTACK_SWIM_SECONDARY,
	ACT_MP_ATTACK_AIRWALK_SECONDARY,

	ACT_MP_RELOAD_STAND_SECONDARY,		// RUN, WALK
	ACT_MP_RELOAD_STAND_SECONDARY_LOOP,
	ACT_MP_RELOAD_STAND_SECONDARY_END,
	ACT_MP_RELOAD_CROUCH_SECONDARY,		// CROUCHWALK
	ACT_MP_RELOAD_CROUCH_SECONDARY_LOOP,
	ACT_MP_RELOAD_CROUCH_SECONDARY_END,
	ACT_MP_RELOAD_SWIM_SECONDARY,
	ACT_MP_RELOAD_SWIM_SECONDARY_LOOP,
	ACT_MP_RELOAD_SWIM_SECONDARY_END,
	ACT_MP_RELOAD_AIRWALK_SECONDARY,
	ACT_MP_RELOAD_AIRWALK_SECONDARY_LOOP,
	ACT_MP_RELOAD_AIRWALK_SECONDARY_END,

	ACT_MP_ATTACK_STAND_GRENADE_SECONDARY,		// RUN, WALK
	ACT_MP_ATTACK_CROUCH_GRENADE_SECONDARY,		// CROUCHWALK
	ACT_MP_ATTACK_SWIM_GRENADE_SECONDARY,
	ACT_MP_ATTACK_AIRWALK_GRENADE_SECONDARY,

	// Melee
	ACT_MP_STAND_MELEE,
	ACT_MP_CROUCH_MELEE,
	ACT_MP_RUN_MELEE,
	ACT_MP_WALK_MELEE,
	ACT_MP_AIRWALK_MELEE,
	ACT_MP_CROUCHWALK_MELEE,
	ACT_MP_JUMP_MELEE,
	ACT_MP_JUMP_START_MELEE,
	ACT_MP_JUMP_FLOAT_MELEE,
	ACT_MP_JUMP_LAND_MELEE,
	ACT_MP_SWIM_MELEE,

	ACT_MP_ATTACK_STAND_MELEE,		// RUN, WALK
	ACT_MP_ATTACK_STAND_MELEE_SECONDARY,
	ACT_MP_ATTACK_CROUCH_MELEE,		// CROUCHWALK
	ACT_MP_ATTACK_CROUCH_MELEE_SECONDARY,
	ACT_MP_ATTACK_SWIM_MELEE,
	ACT_MP_ATTACK_AIRWALK_MELEE,

	ACT_MP_ATTACK_STAND_GRENADE_MELEE,		// RUN, WALK
	ACT_MP_ATTACK_CROUCH_GRENADE_MELEE,		// CROUCHWALK
	ACT_MP_ATTACK_SWIM_GRENADE_MELEE,
	ACT_MP_ATTACK_AIRWALK_GRENADE_MELEE,

	// Item1
	ACT_MP_STAND_ITEM1,
	ACT_MP_CROUCH_ITEM1,
	ACT_MP_RUN_ITEM1,
	ACT_MP_WALK_ITEM1,
	ACT_MP_AIRWALK_ITEM1,
	ACT_MP_CROUCHWALK_ITEM1,
	ACT_MP_JUMP_ITEM1,
	ACT_MP_JUMP_START_ITEM1,
	ACT_MP_JUMP_FLOAT_ITEM1,
	ACT_MP_JUMP_LAND_ITEM1,
	ACT_MP_SWIM_ITEM1,

	ACT_MP_ATTACK_STAND_ITEM1,		// RUN, WALK
	ACT_MP_ATTACK_STAND_ITEM1_SECONDARY,
	ACT_MP_ATTACK_CROUCH_ITEM1,		// CROUCHWALK
	ACT_MP_ATTACK_CROUCH_ITEM1_SECONDARY,
	ACT_MP_ATTACK_SWIM_ITEM1,
	ACT_MP_ATTACK_AIRWALK_ITEM1,

	// Item2
	ACT_MP_STAND_ITEM2,
	ACT_MP_CROUCH_ITEM2,
	ACT_MP_RUN_ITEM2,
	ACT_MP_WALK_ITEM2,
	ACT_MP_AIRWALK_ITEM2,
	ACT_MP_CROUCHWALK_ITEM2,
	ACT_MP_JUMP_ITEM2,
	ACT_MP_JUMP_START_ITEM2,
	ACT_MP_JUMP_FLOAT_ITEM2,
	ACT_MP_JUMP_LAND_ITEM2,
	ACT_MP_SWIM_ITEM2,

	ACT_MP_ATTACK_STAND_ITEM2,		// RUN, WALK
	ACT_MP_ATTACK_STAND_ITEM2_SECONDARY,
	ACT_MP_ATTACK_CROUCH_ITEM2,		// CROUCHWALK
	ACT_MP_ATTACK_CROUCH_ITEM2_SECONDARY,
	ACT_MP_ATTACK_SWIM_ITEM2,
	ACT_MP_ATTACK_AIRWALK_ITEM2,

	// Flinches
	ACT_MP_GESTURE_FLINCH,
	ACT_MP_GESTURE_FLINCH_PRIMARY,
	ACT_MP_GESTURE_FLINCH_SECONDARY,
	ACT_MP_GESTURE_FLINCH_MELEE,
	ACT_MP_GESTURE_FLINCH_ITEM1,
	ACT_MP_GESTURE_FLINCH_ITEM2,

	ACT_MP_GESTURE_FLINCH_HEAD,
	ACT_MP_GESTURE_FLINCH_CHEST,
	ACT_MP_GESTURE_FLINCH_STOMACH,
	ACT_MP_GESTURE_FLINCH_LEFTARM,
	ACT_MP_GESTURE_FLINCH_RIGHTARM,
	ACT_MP_GESTURE_FLINCH_LEFTLEG,
	ACT_MP_GESTURE_FLINCH_RIGHTLEG,

	// Team Fortress specific - medic heal, medic infect, etc.....
	ACT_MP_GRENADE1_DRAW,
	ACT_MP_GRENADE1_IDLE,
	ACT_MP_GRENADE1_ATTACK,
	ACT_MP_GRENADE2_DRAW,
	ACT_MP_GRENADE2_IDLE,
	ACT_MP_GRENADE2_ATTACK,

	ACT_MP_PRIMARY_GRENADE1_DRAW,
	ACT_MP_PRIMARY_GRENADE1_IDLE,
	ACT_MP_PRIMARY_GRENADE1_ATTACK,
	ACT_MP_PRIMARY_GRENADE2_DRAW,
	ACT_MP_PRIMARY_GRENADE2_IDLE,
	ACT_MP_PRIMARY_GRENADE2_ATTACK,

	ACT_MP_SECONDARY_GRENADE1_DRAW,
	ACT_MP_SECONDARY_GRENADE1_IDLE,
	ACT_MP_SECONDARY_GRENADE1_ATTACK,
	ACT_MP_SECONDARY_GRENADE2_DRAW,
	ACT_MP_SECONDARY_GRENADE2_IDLE,
	ACT_MP_SECONDARY_GRENADE2_ATTACK,

	ACT_MP_MELEE_GRENADE1_DRAW,
	ACT_MP_MELEE_GRENADE1_IDLE,
	ACT_MP_MELEE_GRENADE1_ATTACK,
	ACT_MP_MELEE_GRENADE2_DRAW,
	ACT_MP_MELEE_GRENADE2_IDLE,
	ACT_MP_MELEE_GRENADE2_ATTACK,

	ACT_MP_ITEM1_GRENADE1_DRAW,
	ACT_MP_ITEM1_GRENADE1_IDLE,
	ACT_MP_ITEM1_GRENADE1_ATTACK,
	ACT_MP_ITEM1_GRENADE2_DRAW,
	ACT_MP_ITEM1_GRENADE2_IDLE,
	ACT_MP_ITEM1_GRENADE2_ATTACK,

	ACT_MP_ITEM2_GRENADE1_DRAW,
	ACT_MP_ITEM2_GRENADE1_IDLE,
	ACT_MP_ITEM2_GRENADE1_ATTACK,
	ACT_MP_ITEM2_GRENADE2_DRAW,
	ACT_MP_ITEM2_GRENADE2_IDLE,
	ACT_MP_ITEM2_GRENADE2_ATTACK,

	// Building
	ACT_MP_STAND_BUILDING,
	ACT_MP_CROUCH_BUILDING,
	ACT_MP_RUN_BUILDING,
	ACT_MP_WALK_BUILDING,
	ACT_MP_AIRWALK_BUILDING,
	ACT_MP_CROUCHWALK_BUILDING,
	ACT_MP_JUMP_BUILDING,
	ACT_MP_JUMP_START_BUILDING,
	ACT_MP_JUMP_FLOAT_BUILDING,
	ACT_MP_JUMP_LAND_BUILDING,
	ACT_MP_SWIM_BUILDING,

	ACT_MP_ATTACK_STAND_BUILDING,		// RUN, WALK
	ACT_MP_ATTACK_CROUCH_BUILDING,		// CROUCHWALK
	ACT_MP_ATTACK_SWIM_BUILDING,
	ACT_MP_ATTACK_AIRWALK_BUILDING,

	ACT_MP_ATTACK_STAND_GRENADE_BUILDING,		// RUN, WALK
	ACT_MP_ATTACK_CROUCH_GRENADE_BUILDING,		// CROUCHWALK
	ACT_MP_ATTACK_SWIM_GRENADE_BUILDING,
	ACT_MP_ATTACK_AIRWALK_GRENADE_BUILDING,

	ACT_MP_STAND_PDA,
	ACT_MP_CROUCH_PDA,
	ACT_MP_RUN_PDA,
	ACT_MP_WALK_PDA,
	ACT_MP_AIRWALK_PDA,
	ACT_MP_CROUCHWALK_PDA,
	ACT_MP_JUMP_PDA,
	ACT_MP_JUMP_START_PDA,
	ACT_MP_JUMP_FLOAT_PDA,
	ACT_MP_JUMP_LAND_PDA,
	ACT_MP_SWIM_PDA,

	ACT_MP_ATTACK_STAND_PDA,
	ACT_MP_ATTACK_SWIM_PDA,

	ACT_MP_GESTURE_VC_HANDMOUTH,
	ACT_MP_GESTURE_VC_FINGERPOINT,
	ACT_MP_GESTURE_VC_FISTPUMP,
	ACT_MP_GESTURE_VC_THUMBSUP,
	ACT_MP_GESTURE_VC_NODYES,
	ACT_MP_GESTURE_VC_NODNO,

	ACT_MP_GESTURE_VC_HANDMOUTH_PRIMARY,
	ACT_MP_GESTURE_VC_FINGERPOINT_PRIMARY,
	ACT_MP_GESTURE_VC_FISTPUMP_PRIMARY,
	ACT_MP_GESTURE_VC_THUMBSUP_PRIMARY,
	ACT_MP_GESTURE_VC_NODYES_PRIMARY,
	ACT_MP_GESTURE_VC_NODNO_PRIMARY,

	ACT_MP_GESTURE_VC_HANDMOUTH_SECONDARY,
	ACT_MP_GESTURE_VC_FINGERPOINT_SECONDARY,
	ACT_MP_GESTURE_VC_FISTPUMP_SECONDARY,
	ACT_MP_GESTURE_VC_THUMBSUP_SECONDARY,
	ACT_MP_GESTURE_VC_NODYES_SECONDARY,
	ACT_MP_GESTURE_VC_NODNO_SECONDARY,

	ACT_MP_GESTURE_VC_HANDMOUTH_MELEE,
	ACT_MP_GESTURE_VC_FINGERPOINT_MELEE,
	ACT_MP_GESTURE_VC_FISTPUMP_MELEE,
	ACT_MP_GESTURE_VC_THUMBSUP_MELEE,
	ACT_MP_GESTURE_VC_NODYES_MELEE,
	ACT_MP_GESTURE_VC_NODNO_MELEE,

	ACT_MP_GESTURE_VC_HANDMOUTH_ITEM1,
	ACT_MP_GESTURE_VC_FINGERPOINT_ITEM1,
	ACT_MP_GESTURE_VC_FISTPUMP_ITEM1,
	ACT_MP_GESTURE_VC_THUMBSUP_ITEM1,
	ACT_MP_GESTURE_VC_NODYES_ITEM1,
	ACT_MP_GESTURE_VC_NODNO_ITEM1,

	ACT_MP_GESTURE_VC_HANDMOUTH_ITEM2,
	ACT_MP_GESTURE_VC_FINGERPOINT_ITEM2,
	ACT_MP_GESTURE_VC_FISTPUMP_ITEM2,
	ACT_MP_GESTURE_VC_THUMBSUP_ITEM2,
	ACT_MP_GESTURE_VC_NODYES_ITEM2,
	ACT_MP_GESTURE_VC_NODNO_ITEM2,

	ACT_MP_GESTURE_VC_HANDMOUTH_BUILDING,
	ACT_MP_GESTURE_VC_FINGERPOINT_BUILDING,
	ACT_MP_GESTURE_VC_FISTPUMP_BUILDING,
	ACT_MP_GESTURE_VC_THUMBSUP_BUILDING,
	ACT_MP_GESTURE_VC_NODYES_BUILDING,
	ACT_MP_GESTURE_VC_NODNO_BUILDING,

	ACT_MP_GESTURE_VC_HANDMOUTH_PDA,
	ACT_MP_GESTURE_VC_FINGERPOINT_PDA,
	ACT_MP_GESTURE_VC_FISTPUMP_PDA,
	ACT_MP_GESTURE_VC_THUMBSUP_PDA,
	ACT_MP_GESTURE_VC_NODYES_PDA,
	ACT_MP_GESTURE_VC_NODNO_PDA,


	ACT_VM_UNUSABLE,
	ACT_VM_UNUSABLE_TO_USABLE,
	ACT_VM_USABLE_TO_UNUSABLE,

	// Specific viewmodel activities for weapon roles
	ACT_PRIMARY_VM_DRAW,
	ACT_PRIMARY_VM_HOLSTER,
	ACT_PRIMARY_VM_IDLE,
	ACT_PRIMARY_VM_PULLBACK,
	ACT_PRIMARY_VM_PRIMARYATTACK,
	ACT_PRIMARY_VM_SECONDARYATTACK,
	ACT_PRIMARY_VM_RELOAD,
	ACT_PRIMARY_VM_DRYFIRE,
	ACT_PRIMARY_VM_IDLE_TO_LOWERED,
	ACT_PRIMARY_VM_IDLE_LOWERED,
	ACT_PRIMARY_VM_LOWERED_TO_IDLE,

	ACT_SECONDARY_VM_DRAW,
	ACT_SECONDARY_VM_HOLSTER,
	ACT_SECONDARY_VM_IDLE,
	ACT_SECONDARY_VM_PULLBACK,
	ACT_SECONDARY_VM_PRIMARYATTACK,
	ACT_SECONDARY_VM_SECONDARYATTACK,
	ACT_SECONDARY_VM_RELOAD,
	ACT_SECONDARY_VM_DRYFIRE,
	ACT_SECONDARY_VM_IDLE_TO_LOWERED,
	ACT_SECONDARY_VM_IDLE_LOWERED,
	ACT_SECONDARY_VM_LOWERED_TO_IDLE,

	ACT_MELEE_VM_DRAW,
	ACT_MELEE_VM_HOLSTER,
	ACT_MELEE_VM_IDLE,
	ACT_MELEE_VM_PULLBACK,
	ACT_MELEE_VM_PRIMARYATTACK,
	ACT_MELEE_VM_SECONDARYATTACK,
	ACT_MELEE_VM_RELOAD,
	ACT_MELEE_VM_DRYFIRE,
	ACT_MELEE_VM_IDLE_TO_LOWERED,
	ACT_MELEE_VM_IDLE_LOWERED,
	ACT_MELEE_VM_LOWERED_TO_IDLE,

	ACT_PDA_VM_DRAW,
	ACT_PDA_VM_HOLSTER,
	ACT_PDA_VM_IDLE,
	ACT_PDA_VM_PULLBACK,
	ACT_PDA_VM_PRIMARYATTACK,
	ACT_PDA_VM_SECONDARYATTACK,
	ACT_PDA_VM_RELOAD,
	ACT_PDA_VM_DRYFIRE,
	ACT_PDA_VM_IDLE_TO_LOWERED,
	ACT_PDA_VM_IDLE_LOWERED,
	ACT_PDA_VM_LOWERED_TO_IDLE,

	ACT_ITEM1_VM_DRAW,
	ACT_ITEM1_VM_HOLSTER,
	ACT_ITEM1_VM_IDLE,
	ACT_ITEM1_VM_PULLBACK,
	ACT_ITEM1_VM_PRIMARYATTACK,
	ACT_ITEM1_VM_SECONDARYATTACK,
	ACT_ITEM1_VM_RELOAD,
	ACT_ITEM1_VM_DRYFIRE,
	ACT_ITEM1_VM_IDLE_TO_LOWERED,
	ACT_ITEM1_VM_IDLE_LOWERED,
	ACT_ITEM1_VM_LOWERED_TO_IDLE,

	ACT_ITEM2_VM_DRAW,
	ACT_ITEM2_VM_HOLSTER,
	ACT_ITEM2_VM_IDLE,
	ACT_ITEM2_VM_PULLBACK,
	ACT_ITEM2_VM_PRIMARYATTACK,
	ACT_ITEM2_VM_SECONDARYATTACK,
	ACT_ITEM2_VM_RELOAD,
	ACT_ITEM2_VM_DRYFIRE,
	ACT_ITEM2_VM_IDLE_TO_LOWERED,
	ACT_ITEM2_VM_IDLE_LOWERED,
	ACT_ITEM2_VM_LOWERED_TO_IDLE,

	// Infested activities
	ACT_RELOAD_SUCCEED,
	ACT_RELOAD_FAIL,
	// Autogun
	ACT_WALK_AIM_AUTOGUN,
	ACT_RUN_AIM_AUTOGUN,
	ACT_IDLE_AUTOGUN,
	ACT_IDLE_AIM_AUTOGUN,
	ACT_RELOAD_AUTOGUN,
	ACT_CROUCH_IDLE_AUTOGUN,
	ACT_RANGE_ATTACK_AUTOGUN,
	ACT_JUMP_AUTOGUN,
	// Pistol
	ACT_IDLE_AIM_PISTOL,
	// PDW
	ACT_WALK_AIM_DUAL,
	ACT_RUN_AIM_DUAL,
	ACT_IDLE_DUAL,
	ACT_IDLE_AIM_DUAL,
	ACT_RELOAD_DUAL,
	ACT_CROUCH_IDLE_DUAL,
	ACT_RANGE_ATTACK_DUAL,
	ACT_JUMP_DUAL,
	// Shotgun
	ACT_IDLE_SHOTGUN,
	ACT_IDLE_AIM_SHOTGUN,
	ACT_CROUCH_IDLE_SHOTGUN,
	ACT_JUMP_SHOTGUN,
	// Rifle
	ACT_IDLE_AIM_RIFLE,
	ACT_RELOAD_RIFLE,
	ACT_CROUCH_IDLE_RIFLE,
	ACT_RANGE_ATTACK_RIFLE,
	ACT_JUMP_RIFLE,

	// Infested General AI
	ACT_SLEEP,
	ACT_WAKE,

	// Shield Bug
	ACT_FLICK_LEFT,
	ACT_FLICK_LEFT_MIDDLE,
	ACT_FLICK_RIGHT_MIDDLE,
	ACT_FLICK_RIGHT,
	ACT_SPINAROUND,

	// Mortar Bug
	ACT_PREP_TO_FIRE,
	ACT_FIRE,
	ACT_FIRE_RECOVER,

	// Shaman
	ACT_SPRAY,

	// Boomer
	ACT_PREP_EXPLODE,
	ACT_EXPLODE,

	///******************
	///DOTA ANIMATIONS
	///******************

	ACT_DOTA_IDLE,
	ACT_DOTA_RUN,
	ACT_DOTA_ATTACK,
	ACT_DOTA_ATTACK_EVENT,
	ACT_DOTA_DIE,
	ACT_DOTA_FLINCH,
	ACT_DOTA_DISABLED,
	ACT_DOTA_CAST_ABILITY_1,
	ACT_DOTA_CAST_ABILITY_2,
	ACT_DOTA_CAST_ABILITY_3,
	ACT_DOTA_CAST_ABILITY_4,
	ACT_DOTA_OVERRIDE_ABILITY_1,
	ACT_DOTA_OVERRIDE_ABILITY_2,
	ACT_DOTA_OVERRIDE_ABILITY_3,
	ACT_DOTA_OVERRIDE_ABILITY_4,
	ACT_DOTA_CHANNEL_ABILITY_1,
	ACT_DOTA_CHANNEL_ABILITY_2,
	ACT_DOTA_CHANNEL_ABILITY_3,
	ACT_DOTA_CHANNEL_ABILITY_4,
	ACT_DOTA_CHANNEL_END_ABILITY_1,
	ACT_DOTA_CHANNEL_END_ABILITY_2,
	ACT_DOTA_CHANNEL_END_ABILITY_3,
	ACT_DOTA_CHANNEL_END_ABILITY_4,


	// Portal2
	ACT_MP_RUN_SPEEDPAINT,  // running on speed paint
	ACT_MP_LONG_FALL, // falling a LONG way
	ACT_MP_TRACTORBEAM_FLOAT, // floating in a tractor beam
	ACT_MP_DEATH_CRUSH,

	ACT_MP_RUN_SPEEDPAINT_PRIMARY, // player with portalgun running on speed paint
	ACT_MP_DROWNING_PRIMARY, // drowning while holding portalgun
	ACT_MP_LONG_FALL_PRIMARY, // falling a LONG way while holding portalgun
	ACT_MP_TRACTORBEAM_FLOAT_PRIMARY, // floating in a tractor beam while holding portalgun 
	ACT_MP_DEATH_CRUSH_PRIMARY,


	// csgo death anims that don't require direction (direction is pose-param driven for more granularity)
	ACT_DIE_STAND,
	ACT_DIE_STAND_HEADSHOT,
	ACT_DIE_CROUCH,
	ACT_DIE_CROUCH_HEADSHOT,



	// CSGO action activities
	ACT_CSGO_NULL,
	ACT_CSGO_DEFUSE,
	ACT_CSGO_DEFUSE_WITH_KIT,
	ACT_CSGO_FLASHBANG_REACTION,
	ACT_CSGO_FIRE_PRIMARY,
	ACT_CSGO_FIRE_PRIMARY_OPT_1,
	ACT_CSGO_FIRE_PRIMARY_OPT_2,
	ACT_CSGO_FIRE_SECONDARY,
	ACT_CSGO_FIRE_SECONDARY_OPT_1,
	ACT_CSGO_FIRE_SECONDARY_OPT_2,
	ACT_CSGO_RELOAD,
	ACT_CSGO_RELOAD_START,
	ACT_CSGO_RELOAD_LOOP,
	ACT_CSGO_RELOAD_END,
	ACT_CSGO_OPERATE,
	ACT_CSGO_DEPLOY,
	ACT_CSGO_CATCH,
	ACT_CSGO_SILENCER_DETACH,
	ACT_CSGO_SILENCER_ATTACH,
	ACT_CSGO_TWITCH,
	ACT_CSGO_TWITCH_BUYZONE,
	ACT_CSGO_PLANT_BOMB,
	ACT_CSGO_IDLE_TURN_BALANCEADJUST,
	ACT_CSGO_IDLE_ADJUST_STOPPEDMOVING,
	ACT_CSGO_ALIVE_LOOP,
	ACT_CSGO_FLINCH,
	ACT_CSGO_FLINCH_HEAD,
	ACT_CSGO_FLINCH_MOLOTOV,
	ACT_CSGO_JUMP,
	ACT_CSGO_FALL,
	ACT_CSGO_CLIMB_LADDER,
	ACT_CSGO_LAND_LIGHT,
	ACT_CSGO_LAND_HEAVY,
	ACT_CSGO_EXIT_LADDER_TOP,
	ACT_CSGO_EXIT_LADDER_BOTTOM,

	// this is the end of the global activities, private per-monster activities start here.
	LAST_SHARED_ACTIVITY,
} Activity;

//struct characterset_t
//{
//  char set[256];
//};

#include "Classes/IMemAlloc.hpp"

#include "Classes/IBaseClientDll.hpp"
#include "Classes/CUserCmd.hpp"
#include "Classes/IClientEntityList.hpp"
#include "Classes/IGameMovement.hpp"
#include "Classes/IPanel.hpp"
#include "Classes/IPrediction.hpp"
#include "Classes/IMoveHelper.hpp"
#include "Classes/IVEngineClient.hpp"
#include "Classes/IInput.hpp"
#include "Classes/IInputSystem.hpp"
#include "Classes/ISurface.hpp"
#include "Classes/IConVar.hpp"
#include "Classes/IGameEventManager.hpp"
#include "Classes/IVModelRender.hpp"
#include "Classes/IMaterialSystem.hpp"
#include "Classes/ISteamClient.hpp"
#include "Classes/IPhysics.hpp"
#include "Classes/WeaponInfo.hpp"
#include "Classes/GlowOutlineEffect.hpp"
#include "Classes/IVModelInfo.hpp"
#include "Classes/CClientState.hpp"
#include "Classes/IVDebugOverlay.hpp"
#include "Classes/IEngineSound.hpp"
#include "Classes/IViewRenderBeams.hpp"
#include "Classes/ILocalize.hpp"
#include "Classes/INetChannel.hpp"
#include "Classes/INetMessage.hpp"
#include "Classes/IStudioRender.hpp"
#include "Classes/CPlayerResource.hpp"
#include "Classes/ICenterPrint.hpp"
#include "Classes/IVRenderView.hpp"
#include "Classes/IClientLeafSystem.hpp"
#include "Classes/IViewRender.hpp"
#include "Classes/CCSGameRules.hpp"
#include "Classes/CFontManager.hpp"

struct MaterialSystem_SortInfo_t {
	IMaterial* material;
	int			lightmapPageID;
};
class CVerifiedUserCmd {
public:
	CUserCmd m_cmd = { };
	CRC32_t m_crc = 0u;
};
class C_CommandContext {
public:
	bool			needsprocessing;

	CUserCmd		cmd;
	int				command_number;
};
class CGlobalVarsBase {
public:
	float realtime = 0.0f;
	int framecount = 0;
	float absoluteframetime = 0.0f;
	float absoluteframestarttimestddev = 0.0f;
	float curtime = 0.0f;
	float frametime = 0.0f;
	int maxClients = 0;
	int tickcount = 0;
	float interval_per_tick = 0.0f;
	float interpolation_amount = 0.0f;
	int simTicksThisFrame = 0;
	int network_protocol = 0;
	void* pSaveData = nullptr;
	bool m_bClient = false;
	int nTimestampNetworkingBase = 0;
	int nTimestampRandomizeWindow = 0;
};

class CGlobalVars : public CGlobalVarsBase {
public:
	string_t mapname = { };
	string_t mapGroupName = { };
	int mapversion = 0;
	string_t startspot = { };
	MapLoadType_t eLoadType = MapLoad_NewGame;
	bool bMapLoadFailed = false;
	bool deathmatch = false;
	bool coop = false;
	bool teamplay = false;
	int maxEntities = 0;
	int serverCount = 0;
	void* pEdicts = nullptr;
};


class CMoveData {
public:
	bool    m_bFirstRunOfFunctions : 1;
	bool    m_bGameCodeMovedPlayer : 1;
	int     m_nPlayerHandle;        // edict index on server, client entity handle on client=
	int     m_nImpulseCommand;      // Impulse command issued.
	QAngle  m_vecViewAngles;        // Command view angles (local space)
	Vector  m_vecAbsViewAngles;     // Command view angles (world space)
	int     m_nButtons;             // Attack buttons.
	int     m_nOldButtons;          // From host_client->oldbuttons;
	float   m_flForwardMove;
	float   m_flSideMove;
	float   m_flUpMove;
	float   m_flMaxSpeed;
	float   m_flClientMaxSpeed;
	Vector  m_vecVelocity;          // edict::velocity        // Current movement direction.
	QAngle  m_vecAngles;            // edict::angles
	Vector  m_vecOldAngles;
	float   m_outStepHeight;        // how much you climbed this move
	Vector  m_outWishVel;           // This is where you tried 
	Vector  m_outJumpVel;           // This is your jump velocity
	Vector  m_vecConstraintCenter;
	float   m_flConstraintRadius;
	float   m_flConstraintWidth;
	float   m_flConstraintSpeedFactor;
	float   m_flUnknown[ 5 ];
	Vector  m_vecAbsOrigin;
	char pad[ 1000 ] = { };
};

enum SoundServerFlags {
	SV_SND_STATIC_SOUND = ( 1 << 0 ),
	SV_SND_UPDATEPOS = ( 1 << 1 ),
	SV_SND_FROMSERVER = ( 1 << 2 ),
	SV_SND_STOPSOUND = ( 1 << 3 ),
};

struct StartSoundParams_t {
	StartSoundParams_t( ) :
		staticsound( false ),
		soundsource( 0 ),
		entchannel( 0 ),
		pSfx( 0 ),
		fvol( 1.0f ),
		soundlevel( SNDLVL_NORM ),
		flags( 0 ),
		pitch( PITCH_NORM ),
		ServerFlags( 0 ),
		delay( 0.0f ),
		speakerentity( -1 ),
		suppressrecording( false ),
		initialStreamPosition( 0 ) {
		origin.Init( );
		direction.Init( );
	}

	bool staticsound;
	int soundsource;
	int entchannel;
	uintptr_t* pSfx;
	Vector origin;
	Vector direction;
	float fvol;
	soundlevel_t soundlevel;
	int flags;
	int pitch;
	float delay;
	int speakerentity;
	int unk;
	bool suppressrecording;
	int initialStreamPosition;
	int unk01;
	int unk02;
	const char* soundName;
	int unk03;
	int unk04;
	int ServerFlags; // SoundServerFlags
	int unk06;
};
#pragma endregion

#pragma region rain
// How many bits to use to encode an edict.
#define	MAX_EDICT_BITS				11			// # of bits needed to represent max edicts
// Max # of edicts in a level
#define	MAX_EDICTS					(1<<MAX_EDICT_BITS)

// Types of precipitation
enum PrecipitationType_t {
	PRECIPITATION_TYPE_RAIN = 0,
	PRECIPITATION_TYPE_SNOW,
	PRECIPITATION_TYPE_ASH,
	PRECIPITATION_TYPE_SNOWFALL,
	PRECIPITATION_TYPE_PARTICLERAIN,
	PRECIPITATION_TYPE_PARTICLEASH,
	PRECIPITATION_TYPE_PARTICLERAINSTORM,
	PRECIPITATION_TYPE_PARTICLESNOW,
	NUM_PRECIPITATION_TYPES
};

#pragma endregion

#pragma region impl_interfaces
class IMDLCache : public IAppSystem {
public:
	// Used to install callbacks for when data is loaded + unloaded
	// Returns the prior notify
	virtual void SetCacheNotify( void** pNotify ) = 0;

	// NOTE: This assumes the "GAME" path if you don't use
	// the UNC method of specifying files. This will also increment
	// the reference count of the MDL
	virtual MDLHandle_t FindMDL( const char* pMDLRelativePath ) = 0;

	// Reference counting
	virtual int AddRef( MDLHandle_t handle ) = 0;
	virtual int Release( MDLHandle_t handle ) = 0;
	virtual int GetRef( MDLHandle_t handle ) = 0;

	// Gets at the various data associated with a MDL
	virtual studiohdr_t* GetStudioHdr( MDLHandle_t handle ) = 0;
	virtual studiohwdata_t* GetHardwareData( MDLHandle_t handle ) = 0;
	virtual vcollide_t* GetVCollide( MDLHandle_t handle ) = 0;
	virtual unsigned char* GetAnimBlock( MDLHandle_t handle, int nBlock, bool preloadIfMissing ) = 0;
	virtual bool HasAnimBlockBeenPreloaded( MDLHandle_t handle, int nBlock ) = 0;
	virtual virtualmodel_t* GetVirtualModel( MDLHandle_t handle ) = 0;
	virtual int GetAutoplayList( MDLHandle_t handle, unsigned short** pOut ) = 0;
	virtual vertexFileHeader_t* GetVertexData( MDLHandle_t handle ) = 0;

	// Brings all data associated with an MDL into memory
	virtual void TouchAllData( MDLHandle_t handle ) = 0;

	// Gets/sets user data associated with the MDL
	virtual void SetUserData( MDLHandle_t handle, void* pData ) = 0;
	virtual void* GetUserData( MDLHandle_t handle ) = 0;

	// Is this MDL using the error model?
	virtual bool IsErrorModel( MDLHandle_t handle ) = 0;
	virtual bool IsOverBudget( MDLHandle_t handle ) = 0;

	// Flushes the cache, force a full discard
	virtual void Flush( MDLCacheFlush_t nFlushFlags = MDLCACHE_FLUSH_ALL ) = 0;

	// Flushes a particular model out of memory
	virtual void Flush( MDLHandle_t handle, int nFlushFlags = MDLCACHE_FLUSH_ALL ) = 0;

	// Returns the name of the model (its relative path)
	virtual const char* GetModelName( MDLHandle_t handle ) = 0;

	virtual void** GetCacheSection( MDLCacheDataType_t type ) = 0;

	// faster access when you already have the studiohdr
	virtual virtualmodel_t* GetVirtualModelFast( const studiohdr_t* pStudioHdr, MDLHandle_t handle ) = 0;

	// all cache entries that subsequently allocated or successfully checked 
	// are considered "locked" and will not be freed when additional memory is needed
	virtual void BeginLock( ) = 0;

	// reset all protected blocks to normal
	virtual void EndLock( ) = 0;

	// returns a pointer to a counter that is incremented every time the cache has been out of the locked state (EVIL)
	virtual int* GetFrameUnlockCounterPtrOLD( ) = 0;

	// Finish all pending async operations
	virtual void FinishPendingLoads( ) = 0;

	virtual vcollide_t* GetVCollideEx( MDLHandle_t handle, bool synchronousLoad = true ) = 0;
	virtual bool GetVCollideSize( MDLHandle_t handle, int* pVCollideSize ) = 0;

	virtual bool GetAsyncLoad( MDLCacheDataType_t type ) = 0;
	virtual bool SetAsyncLoad( MDLCacheDataType_t type, bool bAsync ) = 0;

	virtual void BeginMapLoad( ) = 0;
	virtual void EndMapLoad( ) = 0;
	virtual void MarkAsLoaded( MDLHandle_t handle ) = 0;

	virtual void InitPreloadData( bool rebuild ) = 0;
	virtual void ShutdownPreloadData( ) = 0;

	virtual bool IsDataLoaded( MDLHandle_t handle, MDLCacheDataType_t type ) = 0;

	virtual int* GetFrameUnlockCounterPtr( MDLCacheDataType_t type ) = 0;

	virtual studiohdr_t* LockStudioHdr( MDLHandle_t handle ) = 0;
	virtual void UnlockStudioHdr( MDLHandle_t handle ) = 0;

	virtual bool PreloadModel( MDLHandle_t handle ) = 0;

	// Hammer uses this. If a model has an error loading in GetStudioHdr, then it is flagged
	// as an error model and any further attempts to load it will just get the error model.
	// That is, until you call this function. Then it will load the correct model.
	virtual void ResetErrorModelStatus( MDLHandle_t handle ) = 0;

	virtual void MarkFrame( ) = 0;

	// Locking for things that we can lock over longer intervals than
	// resources locked by BeginLock/EndLock
	virtual void BeginCoarseLock( ) = 0;
	virtual void EndCoarseLock( ) = 0;

	virtual void ReloadVCollide( MDLHandle_t handle ) = 0;

	virtual bool ReleaseAnimBlockAllocator( ) = 0;

	virtual bool RestoreHardwareData( MDLHandle_t handle, void* pAsyncVTXControl, void* pAsyncVVDControl ) = 0;

	virtual void DisableVCollideLoad( ) = 0;
	virtual void EnableVCollideLoad( ) = 0;

	virtual void DisableFileNotFoundWarnings( ) = 0;
	virtual void EnableFileNotFoundWarnings( ) = 0;

	virtual bool ProcessPendingHardwareRestore( ) = 0;

	virtual void UnloadQueuedHardwareData( ) = 0;

	virtual void DumpDictionaryState( ) = 0;

	// ========================
	// combined models

	// This will create a combined model by the name specified in pszModelName and return back a temporary model handle.
	// May return MDLHANDLE_INVALID if the name is a duplicate of an existing model
	virtual MDLHandle_t	CreateCombinedModel( const char* pszModelName ) = 0;

	// This will create a combined model in replace of an existing model handle.  The handle should be valid to use during the entire process.
	// Returns true if the handle is acceptable to use for the combining process.
	virtual bool		CreateCombinedModel( MDLHandle_t handle ) = 0;

	// Sets the models for the combining process
	virtual bool		SetCombineModels( MDLHandle_t handle, void* vecModelsToCombine ) = 0;

	// Indicates that you are done with the setup of the combining process.  You must specify a callback function and optionally
	// a user pointer that will be supplied to you in the callback
	virtual bool		FinishCombinedModel( MDLHandle_t handle, void* pFunc, void* pUserData = NULL ) = 0;

	// Lets you query a model handle to see if it is a placeholder returned by CreateCombinedModel()
	virtual bool		IsCombinedPlaceholder( MDLHandle_t handle ) = 0;

	// Lets you query a model handle to see if it is a combined model
	virtual bool		IsCombinedModel( MDLHandle_t handle ) = 0;

	// Returns the number of models contained in the combined model.  Includes primary and all secondary models
	virtual int			GetNumCombinedSubModels( MDLHandle_t handle ) = 0;

	// Gets the file name for a sub model
	virtual void		GetCombinedSubModelFilename( MDLHandle_t handle, int nSubModelIndex, char* pszResult, int nResultSize ) = 0;

	// Returns the KV of the material set for the model.  You are responsible for calling deleteThis() on the result.
	virtual KeyValues* GetCombinedMaterialKV( MDLHandle_t handle, int nAtlasGroup = 0 ) = 0;

	// Internal engine use function to help drive the combining process.
	virtual void		UpdateCombiner( ) = 0;	// should be called from main thread after rendering has completed

	// Internal engine function to help get access to specific combined assets
	virtual void* GetCombinedInternalAsset( void* AssetType, const char* pszAssetID = NULL, int* nSize = NULL ) = 0;

	// Set Combiner Flags
	virtual void		SetCombinerFlags( unsigned nFlags ) = 0;

	// Clear Combiner Flags
	virtual void		ClearCombinerFlags( unsigned nFlags ) = 0;

	// Dump out resident combiner info
	virtual void		DebugCombinerInfo( ) = 0;
};

class CEventInfo {
public:
	short classID;
	float fire_delay;
	const void* pSendTable;
	const ClientClass* pClientClass;
	int bits;
	unsigned char* pData;
	int flags;
	char pad[28];
	CEventInfo* pNextEvent;
};
#pragma endregion

#pragma region decl_functions
void RandomSeed( unsigned int seed );
float RandomFloat( float min, float max );
int RandomInt( int min, int max );

void CRC32_Init( CRC32_t* pulCRC );
void CRC32_ProcessBuffer( CRC32_t* pulCRC, const void* p, int len );
void CRC32_Final( CRC32_t* pulCRC );
CRC32_t CRC32_GetTableEntry( unsigned int slot );

template<class T>
static T FindHudElement( const char* name ) {
	static auto pThis = *reinterpret_cast< uintptr_t** >( Engine::Displacement.Data.m_uHudElement );
	if( !pThis )
		return ( T )0;

	static auto find_hud_element = reinterpret_cast< uintptr_t( __thiscall* )( void*, const char* ) >( Engine::Displacement.Function.m_uFindHudElement );
	return ( T )find_hud_element( pThis, name );
}

extern const CRC32_t pulCRCTable[ 256 ];
#pragma endregion

#define XOR_VAL xor_val
#define ADD_VAL 0x74C59A20

#if 1
template<typename T>
class Encrypted_t {
public:
	T* pointer;

	__forceinline Encrypted_t( T* ptr ) {
		pointer = ptr;
	}

	__forceinline  T* Xor( ) const {
		return  pointer;
	}

	__forceinline  T* operator-> ( ) {
		return Xor( );
	}

	__forceinline bool IsValid( ) const {
		return pointer != nullptr;
	}
};
#else
#pragma  optimize( "", off ) 
template<typename T>
class Encrypted_t {
	__forceinline uintptr_t rotate_dec( uintptr_t c ) const {
		return c;
		//return ( ( c & 0xFFFF ) << 16 | ( c & 0xFFFF0000 ) >> 16 );
#if 0
		return ( c & 0xF ) << 28 | ( c & 0xF0000000 ) >> 28
			| ( c & 0xF0 ) << 20 | ( c & 0x0F000000 ) >> 20
			| ( c & 0xF00 ) << 12 | ( c & 0x00F00000 ) >> 12
			| ( c & 0xF000 ) << 4 | ( c & 0x000F0000 ) >> 4;
#endif
	}
public:
	uintptr_t np;
	uintptr_t xor_val;

	__forceinline Encrypted_t( T* ptr ) {
		auto p = &ptr;
		xor_val = rotate_dec( pulCRCTable[ *( ( uint8_t* )p + 1 ) + ( ( ( uintptr_t( ptr ) >> 16 ) & 7 ) << 8 ) ] );
		np = rotate_dec( rotate_dec( xor_val ) ^ ( uintptr_t( ptr ) + ADD_VAL ) );
	}

	__forceinline  T* Xor( ) const {
		return ( T* )( ( uintptr_t )( rotate_dec( np ) ^ rotate_dec( xor_val ) ) - ADD_VAL );
	}

	__forceinline  T* operator-> ( ) {
		return Xor( );
	}

	__forceinline bool IsValid( ) const {
		return ( ( uintptr_t )( rotate_dec( np ) ^ rotate_dec( xor_val ) ) - ADD_VAL ) != 0;
	}
};
#pragma  optimize( "", on )

#endif

class ActivityModifiersWrapper {
	void AddActivityModifier( const char* szName ) {
		typedef void( __thiscall* fnAddActivityModifier )( void*, const char* );
		static const auto adr = Memory::Scan( XorStr( "server.dll" ), XorStr( "55 8B EC 8B 55 08 83 EC 30" ) );
		( ( fnAddActivityModifier )adr )( this, szName );
	}

	uint32_t gap[ 0x4D ]{ 0 };
	CUtlVector< uint16_t > m_ActivityModifiers{ };
public:
	ActivityModifiersWrapper( ) = default;

	explicit ActivityModifiersWrapper( CUtlVector< uint16_t > current_modifiers ) {
		m_ActivityModifiers.RemoveAll( );
		m_ActivityModifiers.GrowVector( current_modifiers.Count( ) );

		for( auto i = 0; i < current_modifiers.Count( ); i++ )
			m_ActivityModifiers[ i ] = current_modifiers[ i ];
	}

	void AddModifier( const char* name ) {
		AddActivityModifier( name );
	}

	CUtlVector< uint16_t > get( ) {
		return m_ActivityModifiers;
	}
};

//-----------------------------------------------------------------------------
// Critical section helper code (from leaked game source)
//-----------------------------------------------------------------------------
class CMDLCacheCriticalSection
{
public:
	CMDLCacheCriticalSection( IMDLCache* pCache ) : m_pCache( pCache )
	{
		m_pCache->BeginLock( );
	}

	~CMDLCacheCriticalSection( )
	{
		m_pCache->EndLock( );
	}

private:
	IMDLCache* m_pCache;
};

class CMDLCacheCoarseCriticalSection
{
public:
	CMDLCacheCoarseCriticalSection( IMDLCache* pCache ) : m_pCache( pCache )
	{
		m_pCache->BeginCoarseLock( );
	}

	~CMDLCacheCoarseCriticalSection( )
	{
		m_pCache->EndCoarseLock( );
	}

private:
	IMDLCache* m_pCache;
};

#define MDCACHE_FINE_GRAINED 1

#if defined(MDCACHE_FINE_GRAINED)
#define MDLCACHE_CRITICAL_SECTION_( pCache ) CMDLCacheCriticalSection cacheCriticalSection(pCache)
#define MDLCACHE_COARSE_LOCK_( pCache ) ((void)(0))
#elif defined(MDLCACHE_LEVEL_LOCKED)
#define MDLCACHE_CRITICAL_SECTION_( pCache )  ((void)(0))
#define MDLCACHE_COARSE_LOCK_( pCache ) ((void)(0))
#else
#define MDLCACHE_CRITICAL_SECTION_( pCache ) ((void)(0))
#define MDLCACHE_COARSE_LOCK_( pCache ) CMDLCacheCriticalSection cacheCriticalSection(pCache)
#endif
#define MDLCACHE_CRITICAL_SECTION() MDLCACHE_CRITICAL_SECTION_(m_pMDLCache.Xor()) // Your interface ptr
#define MDLCACHE_COARSE_LOCK() MDLCACHE_COARSE_LOCK_(m_pMDLCache.Xor()) // Your interface ptr

class INetGraphPanel {
public:
	char _pad0[ 0x13208 ];
	unsigned long m_font_small;
	unsigned long m_font_proportional;
	unsigned long m_font_font;
};

struct Hit_t {
	char pad0x8[ 0x8 ];
	float x;
	float y;
	float time;
	float z;
};

class NoticeText_t {
public:
	wchar_t m_nString[ 512 ]; //0x0000 
	char pad_0x0400[ 0xC ]; //0x0400
	float set; //0x040C
	float m_flStartTime; //0x0410 
	float m_flStartTime2; //0x0414 
	float m_flLifeTimeModifier; //0x0418 
	char pad_0x041C[ 0x4 ]; //0x041C
}; //Size=0x420

class SFHudDeathNoticeAndBotStatus {
public:
	char pad_0000[ 28 ]; //0x0000
	char* m_szHudName; //0x001C
	char pad_0020[ 64 ]; //0x0020
	float m_flTime; //0x0060
	char pad_0064[ 4 ]; //0x0064
	CUtlVector< NoticeText_t > m_vecDeathNotices;
};

class CHud {
public:
	template< class T >
	T FindHudElement( const char* name ) {
		static auto FindHudElement_t = reinterpret_cast< uintptr_t( __thiscall* )( void*, const char* ) >( Memory::Scan( XorStr( "client.dll" ), XorStr( "55 8B EC 53 8B 5D 08 56 57 8B F9 33 F6 39 77 28" ) ) );

		return reinterpret_cast< T >( FindHudElement_t( this, name ) );
	}
};

#include "../Renderer/Render.hpp"

#include "../Menu/Helpers/Easing.h"
#include "../Menu/Framework/GUI.h"
#include "../Menu/Elements.h"