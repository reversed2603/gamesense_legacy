#pragma once
class IInput
{
public:
	virtual void Init_All( void ) = 0;
	virtual void Shutdown_All( void ) = 0;
	virtual int GetButtonBits( int ) = 0;
	virtual void CreateMove( int sequence_number, float input_sample_frametime, bool active ) = 0;
	virtual void ExtraMouseSample( float frametime, bool active ) = 0;
	virtual bool WriteUsercmdDeltaToBuffer( int nSlot, void* buf, int from, int to, bool isnewcommand ) = 0;
	virtual void EncodeUserCmdToBuffer( void* buf, int slot ) = 0;
	virtual void DecodeUserCmdFromBuffer( void* buf, int slot ) = 0;
	virtual CUserCmd* GetUserCmd( int slot, int sequence_number ) = 0;

	//void* pvftable;
	bool				m_pad_something;
	bool				m_mouse_initialized;
	bool				m_mouse_active;
	bool				pad_something01;
	char				pad_0x08[ 0x2C ];
	void* m_keys;
	char				pad_0x38[ 0x64 ];
	__int32				pad_0x41;
	__int32				pad_0x42;
	bool				m_camera_intercepting_mouse;
	bool m_fCameraInThirdPerson; //0x00A5 
	bool m_fCameraMovingWithMouse; //0x00A6 
	char pad_0x00A7[0x1]; //0x00A7
	Vector m_vecCameraOffset; //0x00A8 
	float m_fCameraDistanceMove; //0x00B4 
	__int32 m_nCameraOldX; //0x00B8 
	__int32 m_nCameraOldY; //0x00BC 
	__int32 m_nCameraX; //0x00C0 
	__int32 m_nCameraY; //0x00C4 
	unsigned char m_CameraIsOrthographic; //0x00C8 
	char pad_0x00C9[0x3]; //0x00C9
	Vector m_angPreviousViewAngles; //0x00CC 
	Vector m_angPreviousViewAnglesTilt; //0x00D8 
	float m_flLastForwardMove; //0x00E4 
	__int32 m_nClearInputState; //0x00E8 
	CUserCmd* m_pCommands; //0x00EC 
	CVerifiedUserCmd* m_pVerifiedCommands; //0x00F0 

	//struct ez fixed&recreated in reclass by led 

	__forceinline int CAM_IsThirdPerson( int slot = -1 ) {
		return Memory::VCall< int( __thiscall* )( decltype( this ), int ) >( this, 32 )( this, slot );
	}

	__forceinline void CAM_ToThirdPerson( ) {
		return Memory::VCall< void( __thiscall* )( decltype( this ) ) >( this, 35 )( this );
	}

	__forceinline void CAM_ToFirstPerson( ) {
		return Memory::VCall< void( __thiscall* )( decltype( this ) ) >( this, 36 )( this );
	}
};
