#include "../Hooked.hpp"
#include "../../SDK/Classes/Player.hpp"

#define NET_FRAMES_BACKUP 64 // must be power of 2. 
#define NET_FRAMES_MASK ( NET_FRAMES_BACKUP - 1 )

int __fastcall Hooked::SendDatagram( INetChannel* ecx, void* edx, void* a3 ) {
	const auto nBackupInSequenceNr = ecx->m_nInSequenceNr;
	
	static int nInSequenceNr = 0;

	if( nInSequenceNr != ecx->m_nInSequenceNr ) {
		nInSequenceNr = ecx->m_nInSequenceNr;

		ecx->m_nInSequenceNr += NET_FRAMES_MASK * 2 - static_cast< uint32_t >( NET_FRAMES_MASK * ( 200.f /*change to slider later*/ / 1000.f ) );
	}

	auto oReturn = oSendDatagram( ecx, a3 );
	ecx->m_nInSequenceNr = nBackupInSequenceNr;

	return oReturn;
}

void __fastcall Hooked::ProcessPacket( INetChannel* ecx, void* edx, void* a3, bool a4 ) {
	oProcessPacket( ecx, a3, a4 );
	///nvm
}