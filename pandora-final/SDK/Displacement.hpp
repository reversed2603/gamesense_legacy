#pragma once

#include "sdk.hpp"

struct DllInitializeData {
   DWORD32 dwDataN;

   // C_BaseEntity
   struct {
	  DWORD32 m_MoveType;
	  DWORD32 m_rgflCoordinateFrame;
   } C_BaseEntity;

   // DT_BaseEntity
   struct {
	  DWORD32 m_bSpotted;
	  DWORD32 m_iTeamNum;
	  DWORD32 m_vecOrigin;
	  DWORD32 m_flSimulationTime;
	  DWORD32 m_fEffects;
	  DWORD32 m_iEFlags;
	  DWORD32 m_hOwnerEntity;
	  DWORD32 moveparent;
	  DWORD32 m_nModelIndex;
	  DWORD32 m_Collision;
	  DWORD32 m_CollisionGroup;

	  DWORD32 m_nForceBone;
	  DWORD32 InvalidateBoneCache;
	  DWORD32 m_flAnimTime;
   } DT_BaseEntity;  
   
   // DT_BaseWeaponWorldModel
   struct {
	  DWORD32 m_hCombatWeaponParent;
   } DT_BaseWeaponWorldModel;

   // C_BaseAnimating
   struct {
	  DWORD32 InvalidateBoneCache;
	  DWORD32 m_BoneAccessor;
	  DWORD32 m_iPrevBoneMask;
	  DWORD32 m_iAccumulatedBoneMask;
	  DWORD32 m_iOcclusionFlags;
	  DWORD32 m_iOcclusionFramecount;
	  DWORD32 m_bIsJiggleBonesEnabled;
	  DWORD32 m_iMostRecentModelBoneCounter;
	  DWORD32 m_flLastBoneSetupTime;
	  DWORD32 m_CachedBoneData;
	  DWORD32 m_AnimOverlay;

	  DWORD32 BoneSnapshotsCall;
	  DWORD32 m_pFirstBoneSnapshot;
	  DWORD32 m_pSecondBoneSnapshot;

	  DWORD32 m_pIk;
	  DWORD32 m_pBoneMerge;

	  DWORD32 CacheBoneData;
	  DWORD32 m_nCachedBonesPosition;
	  DWORD32 m_nCachedBonesRotation;
	  DWORD32 m_pStudioHdr;
	  DWORD32 m_bShouldDraw;
   } C_BaseAnimating;

   // DT_BaseAnimating
   struct {
	  DWORD32 m_bClientSideAnimation;
	  DWORD32 m_bClientSideRagdoll;
	  DWORD32 m_flPoseParameter;
	  DWORD32 m_nHitboxSet;
	  DWORD32 m_flCycle;
	  DWORD32 m_nSequence;
	  DWORD32 m_flEncodedController;

   } DT_BaseAnimating;

   // DT_BaseCombatCharacter
   struct {
	  DWORD32 m_hActiveWeapon;
	  DWORD32 m_flNextAttack;
	  DWORD32 m_hMyWeapons;
	  DWORD32 m_hMyWearables;
   } DT_BaseCombatCharacter;

   // C_BasePlayer
   struct {
	  DWORD32 m_pCurrentCommand;
	  DWORD32 relative_call;
	  DWORD32 offset;
	  DWORD32 UpdateVisibilityAllEntities;
   } C_BasePlayer;

   // DT_BasePlayer
   struct {
	  DWORD32 m_aimPunchAngle;
	  DWORD32 m_aimPunchAngleVel;
	  DWORD32 m_viewPunchAngle;
	  DWORD32 m_vecViewOffset;
	  DWORD32 m_vecVelocity;
	  DWORD32 m_vecBaseVelocity;
	  DWORD32 m_vecLadderNormal;
	  DWORD32 m_flFallVelocity;
	  DWORD32 m_flDuckAmount;
	  DWORD32 m_flDuckSpeed;
	  DWORD32 m_lifeState;
	  DWORD32 m_nTickBase;
	  DWORD32 m_iHealth;
	  DWORD32 m_iDefaultFOV;
	  DWORD32 m_fFlags;
	  DWORD32 pl;
	  DWORD32 m_hObserverTarget;
	  DWORD32 m_hViewModel;
	  DWORD32 m_ubEFNoInterpParity;
	  DWORD32 m_ubOldEFNoInterpParity;
	  DWORD32 m_iObserverMode;
   } DT_BasePlayer;

   struct {
	   DWORD32 m_bombsiteCenterA;
	   DWORD32 m_bombsiteCenterB;
   } DT_CSPlayerResource;

   // C_CSPlayer
   struct {
	  DWORD32 m_PlayerAnimState;
	  DWORD32 m_flSpawnTime;
   } C_CSPlayer;

   // DT_CSPlayer 
   struct {
	  DWORD32 m_angEyeAngles;
	  DWORD32 m_nSurvivalTeam;
	  DWORD32 m_bHasHelmet;
	  DWORD32 m_bHasHeavyArmor;
	  DWORD32 m_ArmorValue;
	  DWORD32 m_bScoped;
	  DWORD32 m_bIsWalking;
	  DWORD32 m_iAccount;
	  DWORD32 m_iShotsFired;
	  DWORD32 m_flFlashDuration;
	  DWORD32 m_flLowerBodyYawTarget;
	  DWORD32 m_flVelocityModifier;
	  DWORD32 m_bGunGameImmunity;
	  DWORD32 m_flHealthShotBoostExpirationTime;
	  DWORD32 m_iMatchStats_Kills;
	  DWORD32 m_iMatchStats_Deaths;
	  DWORD32 m_iMatchStats_HeadShotKills;
	  DWORD32 m_iMoveState;
	  DWORD32 m_bWaitForNoAttack;
	  DWORD32 m_bCustomPlayer;
	  DWORD32 m_iPlayerState;
	  DWORD32 m_bIsDefusing;
	  DWORD32 m_bHasDefuser;
	  DWORD32 m_bIsPlayerGhost;
	  DWORD32 m_iFOV;
	  DWORD32 m_vecPlayerPatchEconIndices;
	  DWORD32 m_hRagdoll;
   } DT_CSPlayer;

   // DT_CSRagdoll
   struct {
	  DWORD32 m_hPlayer;
   } DT_CSRagdoll;   
   
   // DT_FogController
   struct {
	  DWORD32 m_fog_enable;
   } DT_FogController;

   // DT_Precipitation
   struct {
	   DWORD32 m_nPrecipType;
   } DT_Precipitation;

   // DT_BaseCombatWeapon
   struct {
	  DWORD32 m_flNextPrimaryAttack;
	  DWORD32 m_flNextSecondaryAttack;
	  DWORD32 m_hOwner;
	  DWORD32 m_iClip1;
	  DWORD32 m_iPrimaryReserveAmmoCount;
	  DWORD32 m_iItemDefinitionIndex;
	  DWORD32 m_hWeaponWorldModel;
	  DWORD32 m_iWorldModelIndex;
	  DWORD32 m_iWorldDroppedModelIndex;
	  DWORD32 m_iViewModelIndex;

	  DWORD32 m_CustomMaterials;
	  DWORD32 m_bCustomMaterialInitialized;
   } DT_BaseCombatWeapon;

   // DT_WeaponCSBase
   struct {
	  DWORD32 m_flRecoilIndex;
	  DWORD32 m_weaponMode;
	  DWORD32 m_flPostponeFireReadyTime;
	  DWORD32 m_fLastShotTime;
   } DT_WeaponCSBase;

   // DT_WeaponCSBaseGun
   struct {
	  DWORD32 m_zoomLevel;
	  DWORD32 m_iBurstShotsRemaining;
	  DWORD32 m_fNextBurstShot;
   } DT_WeaponCSBaseGun;

   // DT_BaseCSGrenade
   struct {
	  DWORD32 m_bPinPulled;
	  DWORD32 m_fThrowTime;
	  DWORD32 m_flThrowStrength;
   } DT_BaseCSGrenade;

   // DT_BaseAttributableItem
   struct {
	  DWORD32 m_flFallbackWear;
	  DWORD32 m_nFallbackPaintKit;
	  DWORD32 m_nFallbackSeed;
	  DWORD32 m_nFallbackStatTrak;
	  DWORD32 m_OriginalOwnerXuidHigh;
	  DWORD32 m_OriginalOwnerXuidLow;
	  DWORD32 m_szCustomName;
	  DWORD32 m_bInitialized;
	  DWORD32 m_iAccountID;
	  DWORD32 m_iEntityLevel;
	  DWORD32 m_iEntityQuality;
	  DWORD32 m_iItemDefinitionIndex;
	  DWORD32 m_Item;
	  DWORD32 m_iItemIDLow;
	  DWORD32 m_iItemIDHigh;
   } DT_BaseAttributableItem;

   // DT_BaseViewModel
   struct {
	  DWORD32 m_hOwner;
	  DWORD32 m_hWeapon;
	  DWORD32 m_nSequence;
   } DT_BaseViewModel;

   // DT_SmokeGrenadeProjectile
   struct {
	  DWORD32 m_nSmokeEffectTickBegin;
	  DWORD32 m_bDidSmokeEffect;
	  DWORD32 m_SmokeParticlesSpawned;
   } DT_SmokeGrenadeProjectile;

   // DT_PlantedC4
   struct {
	  DWORD32 m_flC4Blow;
	  DWORD32 m_flDefuseCountDown;
	  DWORD32 m_bBombDefused;
   } DT_PlantedC4;

   // CClientState
   struct {
	  DWORD32 m_nDeltaTick;
	  DWORD32 m_nLastOutgoingCommand;
	  DWORD32 m_nChokedCommands;
	  DWORD32 m_nLastCommandAck;
	  DWORD32 m_bIsHLTV;
	  DWORD32 m_nMaxClients;
   } CClientState;

   // CBoneMergeCache
   struct {
	  DWORD32 m_nConstructor;
	  DWORD32 m_nInit;
	  DWORD32 m_nUpdateCache;
	  DWORD32 m_CopyToFollow;
	  DWORD32 m_CopyFromFollow;
   } CBoneMergeCache;

   // CIKContext
   struct {
	  DWORD32 m_nConstructor;
	  DWORD32 m_nDestructor;
	  DWORD32 m_nInit;
	  DWORD32 m_nUpdateTargets;
	  DWORD32 m_nSolveDependencies;
   } CIKContext;

   // CBoneSetup
   struct {
	  DWORD32 InitPose;
	  DWORD32 AccumulatePose;
	  DWORD32 CalcAutoplaySequences;
	  DWORD32 CalcBoneAdj;
   } CBoneSetup; 
   
   // IPrediction
   struct {
	  DWORD32 m_nCommandsPredicted;
   } IPrediction;

   struct {
	  DWORD32 m_uMoveHelper;
	  DWORD32 m_uInput;
	  DWORD32 m_uGlobalVars;
	  DWORD32 m_uPredictionRandomSeed;
	  DWORD32 m_uPredictionPlayer;
	  DWORD32 m_uModelBoneCounter;
	  DWORD32 m_uClientSideAnimationList;
	  DWORD32 m_uGlowObjectManager;
	  DWORD32 m_uCamThink;
	  DWORD32 m_uRenderBeams;
	  DWORD32 m_uSmokeCount;
	  DWORD32 m_uCenterPrint;
	  DWORD32 m_uHostFrameTicks;
	  DWORD32 m_uServerGlobals;
	  DWORD32 m_uServerPoseParameters;
	  DWORD32 m_uServerAnimState;
	  DWORD32 m_uTicksAllowed;
	  DWORD32 m_uHudElement;
	  DWORD32 m_uListLeavesInBoxReturn;
	  DWORD32 s_bAllowExtrapolation;
	  DWORD32 m_FireBulletsReturn;
	  DWORD32 m_D3DDevice;
	  DWORD32 m_SoundService;
	  DWORD32 m_InterpolateServerEntities;
	  DWORD32 m_SendNetMsg;
	  DWORD32 m_ModifyEyePos;
	  DWORD32 m_ResetContentsCache;
	  DWORD32 m_ProcessInterpolatedList;
	  DWORD32 CheckReceivingListReturn;
	  DWORD32 ReadSubChannelDataReturn;
	  DWORD32 SendDatagram;
	  DWORD32 ProcessPacket;
   } Data;

   struct {
	  DWORD32 m_uRandomSeed;
	  DWORD32 m_uRandomFloat;
	  DWORD32 m_uRandomInt;
	  DWORD32 m_uSetAbsOrigin;
	  DWORD32 m_uSetAbsAngles;
	  DWORD32 m_uIsBreakable;
	  DWORD32 m_uClearHudWeaponIcon;
	  DWORD32 m_uLoadNamedSkys;
	  DWORD32 m_uCreateAnimState;
	  DWORD32 m_uResetAnimState;
	  DWORD32 m_uUpdateAnimState;
	  DWORD32 m_uClanTagChange;
	  DWORD32 m_uGetSequenceActivity;
	  DWORD32 m_uInvalidatePhysics;
	  DWORD32 m_uPostThinkVPhysics;
	  DWORD32 m_SimulatePlayerSimulatedEntities;
	  DWORD32 m_uImplPhysicsRunThink;
	  DWORD32 m_uClearDeathNotices;
	  DWORD32 m_uSetTimeout;
	  DWORD32 m_uFindHudElement;
	  DWORD32 m_SetCollisionBounds;
	  DWORD32 m_MD5PseudoRandom;
	  DWORD32 m_WriteUsercmd;
	  DWORD32 m_StdStringAssign;
	  DWORD32 m_pPoseParameter;
	  DWORD32 m_AttachmentHelper;
	  DWORD32 m_LockStudioHdr;
	  DWORD32 m_LineGoesThroughSmoke;
	  DWORD32 m_RunSimulation;
	  DWORD32 m_TraceFilterSimple;
   } Function;

   struct {
	   DWORD32 m_bIsValveDS;
	   DWORD32 m_bFreezePeriod;
   } DT_CSGameRulesProxy;
};

struct DllArgsBase {
   long long size;
};

struct DllArgsToRecieve : DllArgsBase {
   DWORD32 dwMemorySize;
   DWORD32 lpInitStruct; //Allocated memory (R. Cast lpInitStruct to DllInitializeData*

   char login[ 256 ] = "\0";
   char password[ 256 ] = "\0";
};

namespace Engine
{
   extern DllInitializeData Displacement;

   bool CreateDisplacement( void* reserved );
}

