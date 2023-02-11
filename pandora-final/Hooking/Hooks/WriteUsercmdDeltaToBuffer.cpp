#include "../Hooked.hpp"
#include "../../SDK/sdk.hpp"
#include "../../SDK/Classes/Exploits.hpp"
#include "../../SDK/Displacement.hpp"

#include "../../Features/Rage/TickbaseShift.hpp"

void WriteUsercmd( bf_write* buf, CUserCmd* incmd, CUserCmd* outcmd ) {
    __asm
    {
        mov     ecx, buf
        mov     edx, incmd
        push    outcmd
        call    Engine::Displacement.Function.m_WriteUsercmd
        add     esp, 4
    }
}

bool __fastcall Hooked::WriteUsercmdDeltaToBuffer( void* ecx, void* edx, int nSlot, void* pBuf, int nFrom, int nTo, bool newCmd ) {
	static auto sendmovecall = (void*)Memory::Scan( XorStr( "engine.dll" ), XorStr( "84 C0 74 04 B0 01 EB 02 32 C0 8B FE 46 3B F3 7E C9 84 C0 0F 84" ) );
	if( _ReturnAddress( ) != sendmovecall || g_TickbaseController.m_iShiftAmount <= 0 ) {
		return oWriteUsercmdDeltaToBuffer( ecx, nSlot, pBuf, nFrom, nTo, newCmd );
	}

	if( nFrom != -1 )
		return true;

    int* pNumBackupCommands = ( int* )( ( std::uintptr_t )pBuf - 0x30 );
    int* m_nNewCmdsPtr = ( int* )( ( std::uintptr_t )pBuf - 0x2C );
    int m_nNewCmds = *m_nNewCmdsPtr;

	int m_nTotalNewCmds = abs( g_TickbaseController.m_iShiftAmount ) + m_nNewCmds;

    g_TickbaseController.m_iShiftAmount = 0;

	if( m_nTotalNewCmds > 62 )
		m_nTotalNewCmds = 62;

	nFrom = -1;

	*m_nNewCmdsPtr = m_nTotalNewCmds;
    *pNumBackupCommands = 0;

	int m_nNextCmd = m_pClientState->m_nLastOutgoingCommand( ) + m_pClientState->m_nChokedCommands( ) + 1;

	nTo = m_nNextCmd - m_nNewCmds + 1;
    if( nTo > m_nNextCmd ) {
    Run:
        CUserCmd* m_pCmd = m_pInput->GetUserCmd( nSlot, nFrom );
        if( m_pCmd ) {
            CUserCmd m_nToCmd = *m_pCmd, m_nFromCmd = *m_pCmd;
            m_nToCmd.command_number++;
            m_nToCmd.tick_count += 64 + 2 * 64;

            if( m_nNewCmds <= m_nTotalNewCmds ) {
                int m_shift = m_nTotalNewCmds - m_nNewCmds + 1;

                do {
                    WriteUsercmd( (bf_write*)pBuf, &m_nToCmd, &m_nFromCmd );
                    m_nFromCmd = m_nToCmd;
                    m_nFromCmd.buttons = m_nToCmd.buttons;
                    m_nFromCmd.viewangles.x = m_nToCmd.viewangles.x;
                    m_nFromCmd.impulse = m_nToCmd.impulse;
                    m_nFromCmd.weaponselect = m_nToCmd.weaponselect;
                    m_nFromCmd.aimdirection.y = m_nToCmd.aimdirection.y;
                    m_nFromCmd.weaponsubtype = m_nToCmd.weaponsubtype;
                    m_nFromCmd.upmove = m_nToCmd.upmove;
                    m_nFromCmd.random_seed = m_nToCmd.random_seed;
                    m_nFromCmd.mousedx = m_nToCmd.mousedx;
                    m_nFromCmd.pad_0x4C[ 3 ] = m_nToCmd.pad_0x4C[ 3 ];
                    m_nFromCmd.command_number = m_nToCmd.command_number;
                    m_nFromCmd.tick_count = m_nToCmd.tick_count;
                    m_nFromCmd.mousedy = m_nToCmd.mousedy;
                    m_nFromCmd.pad_0x4C[ 19 ] = m_nToCmd.pad_0x4C[ 19 ];
                    m_nFromCmd.hasbeenpredicted = m_nToCmd.hasbeenpredicted;
                    m_nFromCmd.pad_0x4C[ 23 ] = m_nToCmd.pad_0x4C[ 23 ];
                    m_nToCmd.command_number++;
                    m_nToCmd.tick_count++;
                    --m_shift;
                } while( m_shift );
            }
        }
        return true;
    }
    else {
        while( oWriteUsercmdDeltaToBuffer( ecx, nSlot, pBuf, nFrom, nTo, true ) ) {

            nFrom = nTo++;

            if( nTo > m_nNextCmd )
                goto Run;

        }
        return false;
    }

    return true;
}