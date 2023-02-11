#pragma once

enum FontFlags_t {
   FONTFLAG_NONE,
   FONTFLAG_ITALIC = 0x001,
   FONTFLAG_UNDERLINE = 0x002,
   FONTFLAG_STRIKEOUT = 0x004,
   FONTFLAG_SYMBOL = 0x008,
   FONTFLAG_ANTIALIAS = 0x010,
   FONTFLAG_GAUSSIANBLUR = 0x020,
   FONTFLAG_ROTARY = 0x040,
   FONTFLAG_DROPSHADOW = 0x080,
   FONTFLAG_ADDITIVE = 0x100,
   FONTFLAG_OUTLINE = 0x200,
};
enum {
   TE_BEAMPOINTS = 0x00,		// beam effect between two points
   TE_SPRITE = 0x01,	// additive sprite, plays 1 cycle
   TE_BEAMDISK = 0x02,	// disk that expands to max radius over lifetime
   TE_BEAMCYLINDER = 0x03,		// cylinder that expands to max radius over lifetime
   TE_BEAMFOLLOW = 0x04,		// create a line of decaying beam segments until entity stops moving
   TE_BEAMRING = 0x05,		// connect a beam ring to two entities
   TE_BEAMSPLINE = 0x06,
   TE_BEAMRINGPOINT = 0x07,
   TE_BEAMLASER = 0x08,		// Fades according to viewpoint
   TE_BEAMTESLA = 0x09,
};
enum {
	STUDIORENDER_DRAW_ENTIRE_MODEL = 0,
	STUDIORENDER_DRAW_OPAQUE_ONLY = 0x01,
	STUDIORENDER_DRAW_TRANSLUCENT_ONLY = 0x02,
	STUDIORENDER_DRAW_GROUP_MASK = 0x03,

	STUDIORENDER_DRAW_NO_FLEXES = 0x04,
	STUDIORENDER_DRAW_STATIC_LIGHTING = 0x08,

	STUDIORENDER_DRAW_ACCURATETIME = 0x10, // Use accurate timing when drawing the model.
	STUDIORENDER_DRAW_NO_SHADOWS = 0x20,
	STUDIORENDER_DRAW_GET_PERF_STATS = 0x40,

	STUDIORENDER_DRAW_WIREFRAME = 0x80,

	STUDIORENDER_DRAW_ITEM_BLINK = 0x100,

	STUDIORENDER_SHADOWDEPTHTEXTURE = 0x200
};
enum {
   FBEAM_STARTENTITY = 0x00000001,
   FBEAM_ENDENTITY = 0x00000002,
   FBEAM_FADEIN = 0x00000004,
   FBEAM_FADEOUT = 0x00000008,
   FBEAM_SINENOISE = 0x00000010,
   FBEAM_SOLID = 0x00000020,
   FBEAM_SHADEIN = 0x00000040,
   FBEAM_SHADEOUT = 0x00000080,
   FBEAM_ONLYNOISEONCE = 0x00000100,		// Only calculate our noise once
   FBEAM_NOTILE = 0x00000200,
   FBEAM_USE_HITBOXES = 0x00000400,		// Attachment indices represent hitbox indices instead when this is set.
   FBEAM_STARTVISIBLE = 0x00000800,		// Has this client actually seen this beam's start entity yet?
   FBEAM_ENDVISIBLE = 0x00001000,		// Has this client actually seen this beam's end entity yet?
   FBEAM_ISACTIVE = 0x00002000,
   FBEAM_FOREVER = 0x00004000,
   FBEAM_HALOBEAM = 0x00008000,		// When drawing a beam with a halo, don't ignore the segments and endwidth
   FBEAM_REVERSED = 0x00010000,
   NUM_BEAM_FLAGS = 17	// KEEP THIS UPDATED!
};
enum RenderableTranslucencyType_t {
   RENDERABLE_IS_OPAQUE = 0,
   RENDERABLE_IS_TRANSLUCENT,
   RENDERABLE_IS_TWO_PASS,    // has both translucent and opaque sub-partsa
};

enum ItemDefinitionIndex {
   ITEM_NONE = 0,
   WEAPON_DEAGLE,
   WEAPON_ELITE,
   WEAPON_FIVESEVEN,
   WEAPON_GLOCK,
   WEAPON_AK47 = 7,
   WEAPON_AUG,
   WEAPON_AWP,
   WEAPON_FAMAS,
   WEAPON_G3SG1,
   WEAPON_GALIL = 13,
   WEAPON_M249,
   WEAPON_M4A4 = 16,
   WEAPON_MAC10,
   WEAPON_P90 = 19,
   WEAPON_ZONE_REPULSOR,
   WEAPON_MP5SD = 23,
   WEAPON_UMP45,
   WEAPON_XM1014,
   WEAPON_BIZON,
   WEAPON_MAG7,
   WEAPON_NEGEV,
   WEAPON_SAWEDOFF,
   WEAPON_TEC9,
   WEAPON_ZEUS,
   WEAPON_P2000,
   WEAPON_MP7,
   WEAPON_MP9,
   WEAPON_NOVA,
   WEAPON_P250,
   WEAPON_SHIELD,
   WEAPON_SCAR20,
   WEAPON_SG553,
   WEAPON_SSG08,
   WEAPON_KNIFEGG,
   WEAPON_KNIFE,
   WEAPON_FLASHBANG,
   WEAPON_HEGRENADE,
   WEAPON_SMOKEGRENADE,
   WEAPON_MOLOTOV,
   WEAPON_DECOY,
   WEAPON_INC,
   WEAPON_C4,
   WEAPON_HEALTHSHOT = 57,
   WEAPON_KNIFE_T = 59,
   WEAPON_M4A1S,
   WEAPON_USPS,
   WEAPON_CZ75 = 63,
   WEAPON_REVOLVER,
   WEAPON_TAGRENADE = 68,
   WEAPON_FISTS,
   WEAPON_BREACHCHARGE,
   WEAPON_TABLET = 72,
   WEAPON_MELEE = 74,
   WEAPON_AXE,
   WEAPON_HAMMER,
   WEAPON_SPANNER = 78,
   WEAPON_KNIFE_GHOST = 80,
   WEAPON_FIREBOMB,
   WEAPON_DIVERSION,
   WEAPON_FRAG_GRENADE,
   WEAPON_SNOWBALL,
   WEAPON_BUMPMINE,
   WEAPON_KNIFE_BAYONET = 500,
   WEAPON_KNIFE_FLIP = 505,
   WEAPON_KNIFE_GUT,
   WEAPON_KNIFE_KARAMBIT,
   WEAPON_KNIFE_M9_BAYONET,
   WEAPON_KNIFE_TACTICAL,
   WEAPON_KNIFE_FALCHION = 512,
   WEAPON_KNIFE_SURVIVAL_BOWIE = 514,
   WEAPON_KNIFE_BUTTERFLY,
   WEAPON_KNIFE_PUSH = 516,
   WEAPON_KNIFE_CORD = 517,
   WEAPON_KNIFE_CANIS = 518,
   WEAPON_KNIFE_URSUS = 519,
   WEAPON_KNIFE_GYPSY_JACKKNIFE = 520,
   WEAPON_KNIFE_OUTDOOR = 521,
   WEAPON_KNIFE_STILETTO = 522,
   WEAPON_KNIFE_WIDOWMAKER = 523,
   WEAPON_KNIFE_SKELETON = 525,
   GLOVE_STUDDED_BLOODHOUND = 5027,
   GLOVE_T_SIDE = 5028,
   GLOVE_CT_SIDE = 5029,
   GLOVE_SPORTY = 5030,
   GLOVE_SLICK = 5031,
   GLOVE_LEATHER_WRAP = 5032,
   GLOVE_MOTORCYCLE = 5033,
   GLOVE_SPECIALIST = 5034,
   GLOVE_HYDRA = 5035,
   MAX_ITEM_DEFINITION_INDEX,
};

enum FontFeature {
   FONT_FEATURE_ANTIALIASED_FONTS = 1,
   FONT_FEATURE_DROPSHADOW_FONTS = 2,
   FONT_FEATURE_OUTLINE_FONTS = 6,
};

enum FontDrawType {
   FONT_DRAW_DEFAULT = 0,
   FONT_DRAW_NONADDITIVE,
   FONT_DRAW_ADDITIVE,
   FONT_DRAW_TYPE_COUNT = 2,
};

enum MoveType {
   MOVETYPE_NONE = 0,
   MOVETYPE_ISOMETRIC,
   MOVETYPE_WALK,
   MOVETYPE_STEP,
   MOVETYPE_FLY,
   MOVETYPE_FLYGRAVITY,
   MOVETYPE_VPHYSICS,
   MOVETYPE_PUSH,
   MOVETYPE_NOCLIP,
   MOVETYPE_LADDER,
   MOVETYPE_OBSERVER,
   MOVETYPE_CUSTOM,
};

enum CompiledVtfFlags {
   TEXTUREFLAGS_POINTSAMPLE = 0x00000001,
   TEXTUREFLAGS_TRILINEAR = 0x00000002,
   TEXTUREFLAGS_CLAMPS = 0x00000004,
   TEXTUREFLAGS_CLAMPT = 0x00000008,
   TEXTUREFLAGS_ANISOTROPIC = 0x00000010,
   TEXTUREFLAGS_HINT_DXT5 = 0x00000020,
   TEXTUREFLAGS_PWL_CORRECTED = 0x00000040,
   TEXTUREFLAGS_NORMAL = 0x00000080,
   TEXTUREFLAGS_NOMIP = 0x00000100,
   TEXTUREFLAGS_NOLOD = 0x00000200,
   TEXTUREFLAGS_ALL_MIPS = 0x00000400,
   TEXTUREFLAGS_PROCEDURAL = 0x00000800,
   TEXTUREFLAGS_ONEBITALPHA = 0x00001000,
   TEXTUREFLAGS_EIGHTBITALPHA = 0x00002000,
   TEXTUREFLAGS_ENVMAP = 0x00004000,
   TEXTUREFLAGS_RENDERTARGET = 0x00008000,
   TEXTUREFLAGS_DEPTHRENDERTARGET = 0x00010000,
   TEXTUREFLAGS_NODEBUGOVERRIDE = 0x00020000,
   TEXTUREFLAGS_SINGLECOPY = 0x00040000,
   TEXTUREFLAGS_PRE_SRGB = 0x00080000,
   TEXTUREFLAGS_UNUSED_00100000 = 0x00100000,
   TEXTUREFLAGS_UNUSED_00200000 = 0x00200000,
   TEXTUREFLAGS_UNUSED_00400000 = 0x00400000,
   TEXTUREFLAGS_NODEPTHBUFFER = 0x00800000,
   TEXTUREFLAGS_UNUSED_01000000 = 0x01000000,
   TEXTUREFLAGS_CLAMPU = 0x02000000,
   TEXTUREFLAGS_VERTEXTEXTURE = 0x04000000,
   TEXTUREFLAGS_SSBUMP = 0x08000000,
   TEXTUREFLAGS_UNUSED_10000000 = 0x10000000,
   TEXTUREFLAGS_BORDER = 0x20000000,
   TEXTUREFLAGS_UNUSED_40000000 = 0x40000000,
   TEXTUREFLAGS_UNUSED_80000000 = 0x80000000
};

enum StandardLightmap_t {
   MATERIAL_SYSTEM_LIGHTMAP_PAGE_WHITE = -1,
   MATERIAL_SYSTEM_LIGHTMAP_PAGE_WHITE_BUMP = -2,
   MATERIAL_SYSTEM_LIGHTMAP_PAGE_USER_DEFINED = -3
};

enum MaterialThreadMode_t {
   MATERIAL_SINGLE_THREADED,
   MATERIAL_QUEUED_SINGLE_THREADED,
   MATERIAL_QUEUED_THREADED
};

enum MaterialContextType_t {
   MATERIAL_HARDWARE_CONTEXT,
   MATERIAL_QUEUED_CONTEXT,
   MATERIAL_NULL_CONTEXT
};

enum {
   MATERIAL_ADAPTER_NAME_LENGTH = 512
};

enum HDRType_t {
   HDR_TYPE_NONE,
   HDR_TYPE_INTEGER,
   HDR_TYPE_FLOAT,
};

enum RestoreChangeFlags_t {
   MATERIAL_RESTORE_VERTEX_FORMAT_CHANGED = 0x1,
   MATERIAL_RESTORE_RELEASE_MANAGED_RESOURCES = 0x2,
};

enum RenderTargetSizeMode_t {
   RT_SIZE_NO_CHANGE = 0,
   RT_SIZE_DEFAULT = 1,
   RT_SIZE_PICMIP = 2,
   RT_SIZE_HDR = 3,
   RT_SIZE_FULL_FRAME_BUFFER = 4,
   RT_SIZE_OFFSCREEN = 5,
   RT_SIZE_FULL_FRAME_BUFFER_ROUNDED_UP = 6
};

enum MaterialRenderTargetDepth_t {
   MATERIAL_RT_DEPTH_SHARED = 0x0,
   MATERIAL_RT_DEPTH_SEPARATE = 0x1,
   MATERIAL_RT_DEPTH_NONE = 0x2,
   MATERIAL_RT_DEPTH_ONLY = 0x3,
};

enum CSWeaponType {
   WEAPONTYPE_KNIFE = 0,
   WEAPONTYPE_PISTOL,
   WEAPONTYPE_SUBMACHINEGUN,
   WEAPONTYPE_RIFLE,
   WEAPONTYPE_SHOTGUN,
   WEAPONTYPE_SNIPER_RIFLE,
   WEAPONTYPE_MACHINEGUN,
   WEAPONTYPE_C4,
   WEAPONTYPE_PLACEHOLDER,
   WEAPONTYPE_GRENADE,
   WEAPONTYPE_UNKNOWN
};

enum PLAYER_TEAM {
   TEAM_NONE = 0,
   TEAM_SPEC = 1,
   TEAM_TT,
   TEAM_CT
};

enum SolidType_t {
   SOLID_NONE = 0,	// no solid model
   SOLID_BSP = 1,	// a BSP tree
   SOLID_BBOX = 2,	// an AABB
   SOLID_OBB = 3,	// an OBB (not implemented yet)
   SOLID_OBB_YAW = 4,	// an OBB, constrained so that it can only yaw
   SOLID_CUSTOM = 5,	// Always call into the entity for tests
   SOLID_VPHYSICS = 6,	// solid vphysics object, get vcollide from the model and collide with that
   SOLID_LAST,
};


enum Collision_Groups {
   COLLISION_GROUP_NONE = 0,
   COLLISION_GROUP_DEBRIS,			// Collides with nothing but world and static stuff
   COLLISION_GROUP_DEBRIS_TRIGGER,         // Same as debris, but hits triggers
   COLLISION_GROUP_INTERACTIVE_DEBRIS,	// Collides with everything except other interactive debris or debris
   COLLISION_GROUP_INTERACTIVE,		// Collides with everything except interactive debris or debris
   COLLISION_GROUP_PLAYER,
   COLLISION_GROUP_BREAKABLE_GLASS,
   COLLISION_GROUP_VEHICLE,
   COLLISION_GROUP_PLAYER_MOVEMENT,  	// For HL2, same as Collision_Group_Player, for
						   // TF2, this filters out other players and CBaseObjects
   COLLISION_GROUP_NPC,			// Generic NPC group
   COLLISION_GROUP_IN_VEHICLE,		// for any entity inside a vehicle
   COLLISION_GROUP_WEAPON,			// for any weapons that need collision detection
   COLLISION_GROUP_VEHICLE_CLIP,		// vehicle clip brush to restrict vehicle movement
   COLLISION_GROUP_PROJECTILE,		// Projectiles!	  
   COLLISION_GROUP_DOOR_BLOCKER,		// Blocks entities not permitted to get near moving doors
   COLLISION_GROUP_PASSABLE_DOOR,		// Doors that the player shouldn't collide with
   COLLISION_GROUP_DISSOLVING,		// Things that are dissolving are in this group
   COLLISION_GROUP_PUSHAWAY,		// Nonsolid on client and server, pushaway in player code

   COLLISION_GROUP_NPC_ACTOR,		// Used so NPCs in scripts ignore the player.
   COLLISION_GROUP_NPC_SCRIPTED,		// Used for NPCs in scripts that should not collide with each other
};
enum ClassId_t {
	CTestTraceline = 196,
	CTEWorldDecal = 197,
	CTESpriteSpray = 194,
	CTESprite = 193,
	CTESparks = 192,
	CTESmoke = 191,
	CTEShowLine = 189,
	CTEProjectedDecal = 186,
	CFEPlayerDecal = 61,
	CTEPlayerDecal = 185,
	CTEPhysicsProp = 182,
	CTEParticleSystem = 181,
	CTEMuzzleFlash = 180,
	CTELargeFunnel = 178,
	CTEKillPlayerAttachments = 177,
	CTEImpact = 176,
	CTEGlowSprite = 175,
	CTEShatterSurface = 188,
	CTEFootprintDecal = 172,
	CTEFizz = 171,
	CTEExplosion = 169,
	CTEEnergySplash = 168,
	CTEEffectDispatch = 167,
	CTEDynamicLight = 166,
	CTEDecal = 164,
	CTEClientProjectile = 163,
	CTEBubbleTrail = 162,
	CTEBubbles = 161,
	CTEBSPDecal = 160,
	CTEBreakModel = 159,
	CTEBloodStream = 158,
	CTEBloodSprite = 157,
	CTEBeamSpline = 156,
	CTEBeamRingPoint = 155,
	CTEBeamRing = 154,
	CTEBeamPoints = 153,
	CTEBeamLaser = 152,
	CTEBeamFollow = 151,
	CTEBeamEnts = 150,
	CTEBeamEntPoint = 149,
	CTEBaseBeam = 148,
	CTEArmorRicochet = 147,
	CTEMetalSparks = 179,
	CSteamJet = 142,
	CSmokeStack = 135,
	DustTrail = 247,
	CFireTrail = 64,
	SporeTrail = 253,
	SporeExplosion = 252,
	RocketTrail = 250,
	SmokeTrail = 251,
	CPropVehicleDriveable = 122,
	ParticleSmokeGrenade = 249,
	CParticleFire = 100,
	MovieExplosion = 248,
	CTEGaussExplosion = 174,
	CEnvQuadraticBeam = 56,
	CEmbers = 46,
	CEnvWind = 60,
	CPrecipitation = 116,
	CPrecipitationBlocker = 117,
	CBaseTempEntity = 18,
	NextBotCombatCharacter = 0,
	CEconWearable = 45,
	CBaseAttributableItem = 4,
	CEconEntity = 44,
	CWeaponXM1014 = 244,
	CWeaponTaser = 239,
	CSmokeGrenade = 133,
	CWeaponSG552 = 236,
	CSensorGrenade = 129,
	CWeaponSawedoff = 232,
	CWeaponNOVA = 228,
	CIncendiaryGrenade = 87,
	CMolotovGrenade = 97,
	CWeaponM3 = 220,
	CKnifeGG = 94,
	CKnife = 93,
	CHEGrenade = 84,
	CFlashbang = 66,
	CWeaponElite = 211,
	CDecoyGrenade = 40,
	CDEagle = 39,
	CWeaponUSP = 243,
	CWeaponM249 = 219,
	CWeaponUMP45 = 242,
	CWeaponTMP = 241,
	CWeaponTec9 = 240,
	CWeaponSSG08 = 238,
	CWeaponSG556 = 237,
	CWeaponSG550 = 235,
	CWeaponScout = 234,
	CWeaponSCAR20 = 233,
	CSCAR17 = 127,
	CWeaponP90 = 231,
	CWeaponP250 = 230,
	CWeaponP228 = 229,
	CWeaponNegev = 227,
	CWeaponMP9 = 226,
	CWeaponMP7 = 225,
	CWeaponMP5Navy = 224,
	CWeaponMag7 = 223,
	CWeaponMAC10 = 222,
	CWeaponM4A1 = 221,
	CWeaponHKP2000 = 218,
	CWeaponGlock = 217,
	CWeaponGalilAR = 216,
	CWeaponGalil = 215,
	CWeaponG3SG1 = 214,
	CWeaponFiveSeven = 213,
	CWeaponFamas = 212,
	CWeaponBizon = 207,
	CWeaponAWP = 205,
	CWeaponAug = 204,
	CAK47 = 1,
	CWeaponCSBaseGun = 209,
	CWeaponCSBase = 208,
	CC4 = 29,
	CWeaponBaseItem = 206,
	CBaseCSGrenade = 8,
	CSmokeGrenadeProjectile = 134,
	CSensorGrenadeProjectile = 130,
	CMolotovProjectile = 98,
	CItem_Healthshot = 91,
	CItemDogtags = 92,
	CDecoyProjectile = 41,
	CFireCrackerBlast = 62,
	CInferno = 88,
	CChicken = 31,
	CFootstepControl = 68,
	CCSGameRulesProxy = 34,
	CWeaponCubemap = 0,
	CWeaponCycler = 210,
	CTEPlantBomb = 183,
	CTEFireBullets = 170,
	CTERadioIcon = 187,
	CPlantedC4 = 108,
	CCSTeam = 38,
	CCSPlayerResource = 36,
	CCSPlayer = 35,
	CCSRagdoll = 37,
	CTEPlayerAnimEvent = 184,
	CHostage = 85,
	CHostageCarriableProp = 86,
	CBaseCSGrenadeProjectile = 9,
	CHandleTest = 83,
	CTeamplayRoundBasedRulesProxy = 146,
	CSpriteTrail = 140,
	CSpriteOriented = 139,
	CSprite = 138,
	CRagdollPropAttached = 125,
	CRagdollProp = 124,
	CPredictedViewModel = 118,
	CPoseController = 114,
	CGameRulesProxy = 82,
	CInfoLadderDismount = 89,
	CFuncLadder = 74,
	CTEFoundryHelpers = 173,
	CEnvDetailController = 52,
	CWorldVguiText = 246,
	CWorld = 245,
	CWaterLODControl = 203,
	CWaterBullet = 202,
	CVoteController = 201,
	CVGuiScreen = 200,
	CPropJeep = 121,
	CPropVehicleChoreoGeneric = 0,
	CTriggerSoundOperator = 199,
	CBaseVPhysicsTrigger = 22,
	CTriggerPlayerMovement = 198,
	CBaseTrigger = 20,
	CTest_ProxyToggle_Networkable = 195,
	CTesla = 190,
	CBaseTeamObjectiveResource = 17,
	CTeam = 145,
	CSunlightShadowControl = 144,
	CSun = 143,
	CParticlePerformanceMonitor = 101,
	CSpotlightEnd = 137,
	CSpatialEntity = 136,
	CSlideshowDisplay = 132,
	CShadowControl = 131,
	CSceneEntity = 128,
	CRopeKeyframe = 126,
	CRagdollManager = 123,
	CPhysicsPropMultiplayer = 106,
	CPhysBoxMultiplayer = 104,
	CPropDoorRotating = 120,
	CBasePropDoor = 16,
	CDynamicProp = 43,
	CProp_Hallucination = 119,
	CPostProcessController = 115,
	CPointWorldText = 113,
	CPointCommentaryNode = 112,
	CPointCamera = 111,
	CPlayerResource = 110,
	CPlasma = 109,
	CPhysMagnet = 107,
	CPhysicsProp = 105,
	CStatueProp = 141,
	CPhysBox = 103,
	CParticleSystem = 102,
	CMovieDisplay = 99,
	CMaterialModifyControl = 96,
	CLightGlow = 95,
	CItemAssaultSuitUseable = 0,
	CItem = 0,
	CInfoOverlayAccessor = 90,
	CFuncTrackTrain = 81,
	CFuncSmokeVolume = 80,
	CFuncRotating = 79,
	CFuncReflectiveGlass = 78,
	CFuncOccluder = 77,
	CFuncMoveLinear = 76,
	CFuncMonitor = 75,
	CFunc_LOD = 70,
	CTEDust = 165,
	CFunc_Dust = 69,
	CFuncConveyor = 73,
	CFuncBrush = 72,
	CBreakableSurface = 28,
	CFuncAreaPortalWindow = 71,
	CFish = 65,
	CFireSmoke = 63,
	CEnvTonemapController = 59,
	CEnvScreenEffect = 57,
	CEnvScreenOverlay = 58,
	CEnvProjectedTexture = 55,
	CEnvParticleScript = 54,
	CFogController = 67,
	CEnvDOFController = 53,
	CCascadeLight = 30,
	CEnvAmbientLight = 51,
	CEntityParticleTrail = 50,
	CEntityFreezing = 49,
	CEntityFlame = 48,
	CEntityDissolve = 47,
	CDynamicLight = 42,
	CColorCorrectionVolume = 33,
	CColorCorrection = 32,
	CBreakableProp = 27,
	CBeamSpotlight = 25,
	CBaseButton = 5,
	CBaseToggle = 19,
	CBasePlayer = 15,
	CBaseFlex = 12,
	CBaseEntity = 11,
	CBaseDoor = 10,
	CBaseCombatCharacter = 6,
	CBaseAnimatingOverlay = 3,
	CBoneFollower = 26,
	CBaseAnimating = 2,
	CAI_BaseNPC = 0,
	CBeam = 24,
	CBaseViewModel = 21,
	CBaseParticleEntity = 14,
	CBaseGrenade = 13,
	CBaseCombatWeapon = 7,
	CBaseWeaponWorldModel = 23
};

enum PreviewImageRetVal_t {
   MATERIAL_PREVIEW_IMAGE_BAD = 0,
   MATERIAL_PREVIEW_IMAGE_OK,
   MATERIAL_NO_PREVIEW_IMAGE,
};

enum class TraceType {
   TRACE_EVERYTHING = 0,
   TRACE_WORLD_ONLY,
   TRACE_ENTITIES_ONLY,
   TRACE_EVERYTHING_FILTER_PROPS,
};

enum class DebugTraceCounterBehavior_t {
   kTRACE_COUNTER_SET = 0,
   kTRACE_COUNTER_INC,
};

enum MaterialVarFlags_t {
   MATERIAL_VAR_DEBUG = ( 1 << 0 ),
   MATERIAL_VAR_NO_DEBUG_OVERRIDE = ( 1 << 1 ),
   MATERIAL_VAR_NO_DRAW = ( 1 << 2 ),
   MATERIAL_VAR_USE_IN_FILLRATE_MODE = ( 1 << 3 ),
   MATERIAL_VAR_VERTEXCOLOR = ( 1 << 4 ),
   MATERIAL_VAR_VERTEXALPHA = ( 1 << 5 ),
   MATERIAL_VAR_SELFILLUM = ( 1 << 6 ),
   MATERIAL_VAR_ADDITIVE = ( 1 << 7 ),
   MATERIAL_VAR_ALPHATEST = ( 1 << 8 ),
   //MATERIAL_VAR_UNUSED = (1 << 9),
   MATERIAL_VAR_ZNEARER = ( 1 << 10 ),
   MATERIAL_VAR_MODEL = ( 1 << 11 ),
   MATERIAL_VAR_FLAT = ( 1 << 12 ),
   MATERIAL_VAR_NOCULL = ( 1 << 13 ),
   MATERIAL_VAR_NOFOG = ( 1 << 14 ),
   MATERIAL_VAR_IGNOREZ = ( 1 << 15 ),
   MATERIAL_VAR_DECAL = ( 1 << 16 ),
   MATERIAL_VAR_ENVMAPSPHERE = ( 1 << 17 ), // OBSOLETE
   MATERIAL_VAR_UNUSED = ( 1 << 18 ), // UNUSED
   MATERIAL_VAR_ENVMAPCAMERASPACE = ( 1 << 19 ), // OBSOLETE
   MATERIAL_VAR_BASEALPHAENVMAPMASK = ( 1 << 20 ),
   MATERIAL_VAR_TRANSLUCENT = ( 1 << 21 ),
   MATERIAL_VAR_NORMALMAPALPHAENVMAPMASK = ( 1 << 22 ),
   MATERIAL_VAR_NEEDS_SOFTWARE_SKINNING = ( 1 << 23 ), // OBSOLETE
   MATERIAL_VAR_OPAQUETEXTURE = ( 1 << 24 ),
   MATERIAL_VAR_ENVMAPMODE = ( 1 << 25 ), // OBSOLETE
   MATERIAL_VAR_SUPPRESS_DECALS = ( 1 << 26 ),
   MATERIAL_VAR_HALFLAMBERT = ( 1 << 27 ),
   MATERIAL_VAR_WIREFRAME = ( 1 << 28 ),
   MATERIAL_VAR_ALLOWALPHATOCOVERAGE = ( 1 << 29 ),
   MATERIAL_VAR_ALPHA_MODIFIED_BY_PROXY = ( 1 << 30 ),
   MATERIAL_VAR_VERTEXFOG = ( 1 << 31 ),
};

enum MaterialType_t {
   CHAR_TEX_ANTLION = 'A',
   CHAR_TEX_BLOODYFLESH = 'B',
   CHAR_TEX_CONCRETE = 'C',
   CHAR_TEX_DIRT = 'D',
   CHAR_TEX_EGGSHELL = 'E',
   CHAR_TEX_FLESH = 'F',
   CHAR_TEX_GRATE = 'G',
   CHAR_TEX_ALIENFLESH = 'H',
   CHAR_TEX_CLIP = 'I',
   CHAR_TEX_PLASTIC = 'L',
   CHAR_TEX_METAL = 'M',
   CHAR_TEX_SAND = 'N',
   CHAR_TEX_FOLIAGE = 'O',
   CHAR_TEX_COMPUTER = 'P',
   CHAR_TEX_SLOSH = 'S',
   CHAR_TEX_TILE = 'T',
   CHAR_TEX_CARDBOARD = 'U',
   CHAR_TEX_VENT = 'V',
   CHAR_TEX_WOOD = 'W',
   CHAR_TEX_GLASS = 'Y',
   CHAR_TEX_WARPSHIELD = 'Z',
};

// entity effects C_BaseEntity::m_fEffects
enum EntityEffects_t : int {
   EF_BONEMERGE = 0x001,          // Performs bone merge on client side
   EF_BRIGHTLIGHT = 0x002,        // DLIGHT centered at entity origin
   EF_DIMLIGHT = 0x004,           // player flashlight
   EF_NOINTERP = 0x008,           // don't interpolate the next frame
   EF_NOSHADOW = 0x010,           // Don't cast no shadow
   EF_NODRAW = 0x020,             // don't draw entity
   EF_NORECEIVESHADOW = 0x040,    // Don't receive no shadow
   EF_BONEMERGE_FASTCULL = 0x080, // For use with EF_BONEMERGE. If this is set, then it places this ent's origin at its
											 // parent and uses the parent's bbox + the max extents of the aiment.
											 // Otherwise, it sets up the parent's bones every frame to figure out where to place
											 // the aiment, which is inefficient because it'll setup the parent's bones even if
											 // the parent is not in the PVS.

											 EF_ITEM_BLINK = 0x100,         // blink an item so that the user notices it.
											 EF_PARENT_ANIMATES = 0x200,    // always assume that the parent entity is animating
											 EF_MAX_BITS = 10
};

// entity flags, CBaseEntity::m_iEFlags
enum EFlags_t : int {
   EFL_KILLME = ( 1 << 0 ),                    // This entity is marked for death -- This allows the game to actually delete ents at a safe time
   EFL_DORMANT = ( 1 << 1 ),                   // Entity is dormant, no updates to client
   EFL_NOCLIP_ACTIVE = ( 1 << 2 ),             // Lets us know when the noclip command is active.
   EFL_SETTING_UP_BONES = ( 1 << 3 ),          // Set while a model is setting up its bones.
   EFL_KEEP_ON_RECREATE_ENTITIES = ( 1 << 4 ), // This is a special entity that should not be deleted when we restart entities only

   EFL_HAS_PLAYER_CHILD = ( 1 << 4 ), // One of the child entities is a player.

   EFL_DIRTY_SHADOWUPDATE = ( 1 << 5 ), // Client only- need shadow manager to update the shadow...
   EFL_NOTIFY = ( 1 << 6 ),             // Another entity is watching events on this entity (used by teleport)

	 // The default behavior in ShouldTransmit is to not send an entity if it doesn't
	 // have a model. Certain entities want to be sent anyway because all the drawing logic
	 // is in the client DLL. They can set this flag and the engine will transmit them even
	 // if they don't have a model.
	 EFL_FORCE_CHECK_TRANSMIT = ( 1 << 7 ),

	 EFL_BOT_FROZEN = ( 1 << 8 ),            // This is set on bots that are frozen.
	 EFL_SERVER_ONLY = ( 1 << 9 ),           // Non-networked entity.
	 EFL_NO_AUTO_EDICT_ATTACH = ( 1 << 10 ), // Don't attach the edict; we're doing it explicitly

	  // Some dirty bits with respect to abs computations
	  EFL_DIRTY_ABSTRANSFORM = ( 1 << 11 ),
	  EFL_DIRTY_ABSVELOCITY = ( 1 << 12 ),
	  EFL_DIRTY_ABSANGVELOCITY = ( 1 << 13 ),
	  EFL_DIRTY_SURROUNDING_COLLISION_BOUNDS = ( 1 << 14 ),
	  EFL_DIRTY_SPATIAL_PARTITION = ( 1 << 15 ),
	  //	UNUSED						= (1<<16),

	  // This is set if the entity detects that it's in the skybox.
	  // This forces it to pass the "in PVS" for transmission.
	  EFL_IN_SKYBOX = ( 1 << 17 ),

	  EFL_USE_PARTITION_WHEN_NOT_SOLID = ( 1 << 18 ), // Entities with this flag set show up in the partition even when not solid
	  EFL_TOUCHING_FLUID = ( 1 << 19 ),               // Used to determine if an entity is floating

	  // FIXME: Not really sure where I should add this...
	  EFL_IS_BEING_LIFTED_BY_BARNACLE = ( 1 << 20 ),
	  EFL_NO_ROTORWASH_PUSH = ( 1 << 21 ), // I shouldn't be pushed by the rotorwash
	  EFL_NO_THINK_FUNCTION = ( 1 << 22 ),
	  EFL_NO_GAME_PHYSICS_SIMULATION = ( 1 << 23 ),

	  EFL_CHECK_UNTOUCH = ( 1 << 24 ),
	  EFL_DONTBLOCKLOS = ( 1 << 25 ),              // I shouldn't block NPC line-of-sight
	  EFL_DONTWALKON = ( 1 << 26 ),                // NPC;s should not walk on this entity
	  EFL_NO_DISSOLVE = ( 1 << 27 ),               // These guys shouldn't dissolve
	  EFL_NO_MEGAPHYSCANNON_RAGDOLL = ( 1 << 28 ), // Mega physcannon can't ragdoll these guys.
	  EFL_NO_WATER_VELOCITY_CHANGE = ( 1 << 29 ),  // Don't adjust this entity's velocity when transitioning into water
	  EFL_NO_PHYSCANNON_INTERACTION = ( 1 << 30 ), // Physcannon can't pick these up or punt them
	  EFL_NO_DAMAGE_FORCES = ( 1 << 31 ),          // Doesn't accept forces from physics damage
};

enum LightType_t {
   MATERIAL_LIGHT_DISABLE = 0,
   MATERIAL_LIGHT_POINT,
   MATERIAL_LIGHT_DIRECTIONAL,
   MATERIAL_LIGHT_SPOT,
};

enum {
   DRAWWORLDLISTS_DRAW_STRICTLYABOVEWATER = 0x001,
   DRAWWORLDLISTS_DRAW_STRICTLYUNDERWATER = 0x002,
   DRAWWORLDLISTS_DRAW_INTERSECTSWATER = 0x004,
   DRAWWORLDLISTS_DRAW_WATERSURFACE = 0x008,
   DRAWWORLDLISTS_DRAW_SKYBOX = 0x010,
   DRAWWORLDLISTS_DRAW_CLIPSKYBOX = 0x020,
   DRAWWORLDLISTS_DRAW_SHADOWDEPTH = 0x040,
   DRAWWORLDLISTS_DRAW_REFRACTION = 0x080,
   DRAWWORLDLISTS_DRAW_REFLECTION = 0x100,
   DRAWWORLDLISTS_DRAW_WORLD_GEOMETRY = 0x200,
   DRAWWORLDLISTS_DRAW_DECALS_AND_OVERLAYS = 0x400,
};

enum {
   MAT_SORT_GROUP_STRICTLY_ABOVEWATER = 0,
   MAT_SORT_GROUP_STRICTLY_UNDERWATER,
   MAT_SORT_GROUP_INTERSECTS_WATER_SURFACE,
   MAT_SORT_GROUP_WATERSURFACE,

   MAX_MAT_SORT_GROUPS
};

enum class usermsg_type {
   CS_UM_VGUIMenu = 1,
   CS_UM_Geiger,
   CS_UM_Train,
   CS_UM_HudText,
   CS_UM_SayText,							// team chat?
   CS_UM_SayText2,							// all chat
   CS_UM_TextMsg,
   CS_UM_HudMsg,
   CS_UM_ResetHud,
   CS_UM_GameTitle,
   CS_UM_Shake = 12,						// map/server plugin shake effect?
   CS_UM_Fade,								// fade HUD in/out
   CS_UM_Rumble,							// controller rumble?
   CS_UM_CloseCaption,
   CS_UM_CloseCaptionDirect,
   CS_UM_SendAudio,
   CS_UM_RawAudio,
   CS_UM_VoiceMask,
   CS_UM_RequestState,
   CS_UM_Damage,
   CS_UM_RadioText,
   CS_UM_HintText,
   CS_UM_KeyHintText,
   CS_UM_ProcessSpottedEntityUpdate,
   CS_UM_ReloadEffect,
   CS_UM_AdjustMoney,
   CS_UM_UpdateTeamMoney,
   CS_UM_StopSpectatorMode,
   CS_UM_KillCam,
   CS_UM_DesiredTimescale,
   CS_UM_CurrentTimescale,
   CS_UM_AchievementEvent,
   CS_UM_MatchEndConditions,
   CS_UM_DisconnectToLobby,
   CS_UM_DisplayInventory = 37,
   CS_UM_WarmupHasEnded,
   CS_UM_ClientInfo,
   CS_UM_CallVoteFailed = 45,
   CS_UM_VoteStart,
   CS_UM_VotePass,
   CS_UM_VoteFailed,
   CS_UM_VoteSetup,
   CS_UM_SendLastKillerDamageToClient = 51,
   CS_UM_ItemPickup = 53,
   CS_UM_ShowMenu,							// show hud menu
   CS_UM_BarTime,							// For the C4 progress bar.
   CS_UM_AmmoDenied,
   CS_UM_MarkAchievement,
   CS_UM_ItemDrop = 59,
   CS_UM_GlowPropTurnOff
};