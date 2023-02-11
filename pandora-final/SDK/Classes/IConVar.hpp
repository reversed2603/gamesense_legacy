#pragma once
// The default, no flags at all
#define FCVAR_NONE				0 

	// Command to ConVars and ConCommands
	// ConVar Systems
#define FCVAR_UNREGISTERED		(1<<0)	// If this is set, don't add to linked list, etc.
#define FCVAR_DEVELOPMENTONLY	(1<<1)	// Hidden in released products. Flag is removed automatically if ALLOW_DEVELOPMENT_CVARS is defined.
#define FCVAR_GAMEDLL			(1<<2)	// defined by the game DLL
#define FCVAR_CLIENTDLL			(1<<3)  // defined by the client DLL
#define FCVAR_HIDDEN			(1<<4)	// Hidden. Doesn't appear in find or autocomplete. Like DEVELOPMENTONLY, but can't be compiled out.

	// ConVar only
#define FCVAR_PROTECTED			(1<<5)  // It's a server cvar, but we don't send the data since it's a password, etc.  Sends 1 if it's not bland/zero, 0 otherwise as value
#define FCVAR_SPONLY			(1<<6)  // This cvar cannot be changed by clients connected to a multiplayer server.
#define	FCVAR_ARCHIVE			(1<<7)	// set to cause it to be saved to vars.rc
#define	FCVAR_NOTIFY			(1<<8)	// notifies players when changed
#define	FCVAR_USERINFO			(1<<9)	// changes the client's info string
#define FCVAR_CHEAT				(1<<14) // Only useable in singleplayer / debug / multiplayer & sv_cheats

#define FCVAR_PRINTABLEONLY		(1<<10)  // This cvar's string cannot contain unprintable characters ( e.g., used for player name etc ).
#define FCVAR_UNLOGGED			(1<<11)  // If this is a FCVAR_SERVER, don't log changes to the log file / console if we are creating a log
#define FCVAR_NEVER_AS_STRING	(1<<12)  // never try to print that cvar

	// It's a ConVar that's shared between the client and the server.
	// At signon, the values of all such ConVars are sent from the server to the client (skipped for local
	//  client, of course )
	// If a change is requested it must come from the console (i.e., no remote client changes)
	// If a value is changed while a server is active, it's replicated to all connected clients
#define FCVAR_REPLICATED		(1<<13)	// server setting enforced on clients, TODO rename to FCAR_SERVER at some time
#define FCVAR_DEMO				(1<<16)  // record this cvar when starting a demo file
#define FCVAR_DONTRECORD		(1<<17)  // don't record these command in demofiles
#define FCVAR_RELOAD_MATERIALS	(1<<20)	// If this cvar changes, it forces a material reload
#define FCVAR_RELOAD_TEXTURES	(1<<21)	// If this cvar changes, if forces a texture reload

#define FCVAR_NOT_CONNECTED		(1<<22)	// cvar cannot be changed by a client that is connected to a server
#define FCVAR_MATERIAL_SYSTEM_THREAD (1<<23)	// Indicates this cvar is read from the material system thread
#define FCVAR_ARCHIVE_XBOX		(1<<24) // cvar written to config.cfg on the Xbox

#define FCVAR_ACCESSIBLE_FROM_THREADS	(1<<25)	// used as a debugging tool necessary to check material system thread convars

#define FCVAR_SERVER_CAN_EXECUTE	(1<<28)// the server is allowed to execute this command on clients via ClientCommand/NET_StringCmd/CBaseClientState::ProcessStringCmd.
#define FCVAR_SERVER_CANNOT_QUERY	(1<<29)// If this is set, then the server is not allowed to query this cvar's value (via IServerPluginHelpers::StartQueryCvarValue).
#define FCVAR_CLIENTCMD_CAN_EXECUTE	(1<<30)	// IVEngineClient::ClientCmd is allowed to execute this command. 

#include "../Valve/characterset.hpp"
#include "../Valve/UtlVector.hpp"

class CCommand
{
public:
	CCommand();
	CCommand(int nArgC, const char **ppArgV);
	bool Tokenize(const char *pCommand, characterset_t *pBreakSet = NULL);
	void Reset();

	int ArgC() const;
	const char **ArgV() const;
	const char *ArgS() const;					// All args that occur after the 0th arg, in string form
	const char *GetCommandString() const;		// The entire command in string form, including the 0th arg
	const char *operator[](int nIndex) const;	// Gets at arguments
	const char *Arg(int nIndex) const;		// Gets at arguments

	// Helper functions to parse arguments to commands.
	const char* FindArg(const char *pName) const;
	int FindArgInt(const char *pName, int nDefaultVal) const;

	static int MaxCommandLength();
	static characterset_t* DefaultBreakSet();

private:
	enum
	{
		COMMAND_MAX_ARGC = 64,
		COMMAND_MAX_LENGTH = 512,
	};

	int		m_nArgc;
	int		m_nArgv0Size;
	char	m_pArgSBuffer[COMMAND_MAX_LENGTH];
	char	m_pArgvBuffer[COMMAND_MAX_LENGTH];
	const char*	m_ppArgv[COMMAND_MAX_ARGC];
};

class ConVar
{
public:
	char pad_0x0000[0x4];
	ConVar * pNext;
	int32_t bRegistered;
	char* pszName;
	char* pszHelpString;
	int32_t nFlags;
	char pad_0x0018[0x4];
	ConVar* pParent;
	char* pszDefaultValue;
	char* strString;
	int32_t StringLength;
	float fValue;
	int32_t nValue;
	int32_t bHasMin;
	float fMinVal;
	int32_t bHasMax;
	float fMaxVal;
	CUtlVector<ChangeCallback_t> fnChangeCallback;
	float GetFloat()
	{
		auto temp = *(int*)(&fValue);
		auto temp_result = (int)(temp ^ (DWORD)this);

		return *(float*)(&temp_result);
	}
	int GetInt()
	{
		auto temp = *(int*)(&nValue);
		auto temp_result = (int)(temp ^ (uintptr_t)this);

		return *(int*)(&temp_result);
	}
	bool GetBool() { return !!GetInt(); }

	void SetValue(const char* value)
	{
		typedef void(__thiscall* OriginalFn)(void*, const char *);
		return Memory::VCall<OriginalFn>(this, 14)(this, value);
	}
	void SetValueStr(const char* str)
	{
		typedef void(__thiscall* SetStringFn)(void*, const char*);
		return Memory::VCall<SetStringFn>(this, 13)(this, str);
	}
	void SetValue(int value)
	{
		typedef void(__thiscall* OriginalFn)(void*, int);
		return Memory::VCall<OriginalFn>(this, 16)(this, value);
	}

	/* bruh these are for lua*/
	void SetValueString( const char* value ) {
		typedef void( __thiscall* OriginalFn )( void*, const char* );
		return Memory::VCall<OriginalFn>( this, 14 )( this, value );
	}

	void SetValueInt( int value ) {
		typedef void( __thiscall* OriginalFn )( void*, int );
		return Memory::VCall<OriginalFn>( this, 16 )( this, value );
	}

	void SetValueFloat( float value ) {
		typedef void( __thiscall* OriginalFn )( void*, float );
		return Memory::VCall<OriginalFn>( this, 15 )( this, value );
	}

	void SetValueChar(const char* name)
	{
		typedef void(__thiscall* SetNameFN)(void*, const char*);
		return Memory::VCall<SetNameFN>(this, 14)(this, name);
	}
	void InternalSetString(const char* str)
	{
		typedef void(__thiscall* SetStringFn)(void*, const char*);
		return Memory::VCall<SetStringFn>(this, 17)(this, str);
	}
	char* GetBaseName()
	{
		typedef char*(__thiscall* SetStringFn)(void*);
		return Memory::VCall<SetStringFn>(this, 6)(this);
	}

	const char* GetString() {
		return pParent->pszDefaultValue;
	}

	bool Valid(  ) {
		return this != nullptr;
	}

};//Size=0x0048

class ICVar
{
public:
	virtual void			RegisterConCommand(ConVar *pCommandBase) = 0;
	virtual void			UnregisterConCommand(ConVar *pCommandBase) = 0;

	ConVar *FindVar(const char *var_name)
	{
		typedef ConVar*(__thiscall* FindVarFn)(void*, const char *);
		return Memory::VCall<FindVarFn>(this, 14)(this, var_name);
	}
	void const ConsoleColorPrintf(const Color& clr, const char *pFormat, ...) {
		typedef void(__cdecl *OriginalFn)(void*, const Color&, const char *, ...);

		if (pFormat == nullptr)
			return;

		char buf[8192];

		va_list list;
		va_start(list, pFormat);
		_vsnprintf(buf, sizeof(buf) - 1, pFormat, list);
		va_end(list);
		buf[sizeof(buf) - 1] = 0;

		Memory::VCall<OriginalFn>(this, 25)(this, clr, buf, list);
	}
};

