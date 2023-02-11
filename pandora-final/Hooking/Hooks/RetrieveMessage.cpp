#include "../Hooked.hpp"

enum ECsgoGCMsg {
	k_EMsgGCCStrike15_v2_Base = 9100,
	k_EMsgGCCStrike15_v2_MatchmakingStart = 9101,
	k_EMsgGCCStrike15_v2_MatchmakingStop = 9102,
	k_EMsgGCCStrike15_v2_MatchmakingClient2ServerPing = 9103,
	k_EMsgGCCStrike15_v2_MatchmakingGC2ClientUpdate = 9104,
	k_EMsgGCCStrike15_v2_MatchmakingGC2ServerReserve = 9105,
	k_EMsgGCCStrike15_v2_MatchmakingServerReservationResponse = 9106,
	k_EMsgGCCStrike15_v2_MatchmakingGC2ClientReserve = 9107,
	k_EMsgGCCStrike15_v2_MatchmakingServerRoundStats = 9108,
	k_EMsgGCCStrike15_v2_MatchmakingClient2GCHello = 9109,
	k_EMsgGCCStrike15_v2_MatchmakingGC2ClientHello = 9110,
	k_EMsgGCCStrike15_v2_MatchmakingServerMatchEnd = 9111,
	k_EMsgGCCStrike15_v2_MatchmakingGC2ClientAbandon = 9112,
	k_EMsgGCCStrike15_v2_MatchmakingServer2GCKick = 9113,
	k_EMsgGCCStrike15_v2_MatchmakingGC2ServerConfirm = 9114,
	k_EMsgGCCStrike15_v2_MatchmakingGCOperationalStats = 9115,
	k_EMsgGCCStrike15_v2_MatchmakingGC2ServerRankUpdate = 9116,
	k_EMsgGCCStrike15_v2_MatchmakingOperator2GCBlogUpdate = 9117,
	k_EMsgGCCStrike15_v2_ServerNotificationForUserPenalty = 9118,
	k_EMsgGCCStrike15_v2_ClientReportPlayer = 9119,
	k_EMsgGCCStrike15_v2_ClientReportServer = 9120,
	k_EMsgGCCStrike15_v2_ClientCommendPlayer = 9121,
	k_EMsgGCCStrike15_v2_ClientReportResponse = 9122,
	k_EMsgGCCStrike15_v2_ClientCommendPlayerQuery = 9123,
	k_EMsgGCCStrike15_v2_ClientCommendPlayerQueryResponse = 9124,
	k_EMsgGCCStrike15_v2_WatchInfoUsers = 9126,
	k_EMsgGCCStrike15_v2_ClientRequestPlayersProfile = 9127,
	k_EMsgGCCStrike15_v2_PlayersProfile = 9128,
	k_EMsgGCCStrike15_v2_PlayerOverwatchCaseUpdate = 9131,
	k_EMsgGCCStrike15_v2_PlayerOverwatchCaseAssignment = 9132,
	k_EMsgGCCStrike15_v2_PlayerOverwatchCaseStatus = 9133,
	k_EMsgGCCStrike15_v2_GC2ClientTextMsg = 9134,
	k_EMsgGCCStrike15_v2_Client2GCTextMsg = 9135,
	k_EMsgGCCStrike15_v2_MatchEndRunRewardDrops = 9136,
	k_EMsgGCCStrike15_v2_MatchEndRewardDropsNotification = 9137,
	k_EMsgGCCStrike15_v2_ClientRequestWatchInfoFriends2 = 9138,
	k_EMsgGCCStrike15_v2_MatchList = 9139,
	k_EMsgGCCStrike15_v2_MatchListRequestCurrentLiveGames = 9140,
	k_EMsgGCCStrike15_v2_MatchListRequestRecentUserGames = 9141,
	k_EMsgGCCStrike15_v2_GC2ServerReservationUpdate = 9142,
	k_EMsgGCCStrike15_v2_ClientVarValueNotificationInfo = 9144,
	k_EMsgGCCStrike15_v2_TournamentMatchRewardDropsNotification = 9145,
	k_EMsgGCCStrike15_v2_MatchListRequestTournamentGames = 9146,
	k_EMsgGCCStrike15_v2_MatchListRequestFullGameInfo = 9147,
	k_EMsgGCCStrike15_v2_GiftsLeaderboardRequest = 9148,
	k_EMsgGCCStrike15_v2_GiftsLeaderboardResponse = 9149,
	k_EMsgGCCStrike15_v2_ServerVarValueNotificationInfo = 9150,
	k_EMsgGCToGCReloadVersions = 9151,
	k_EMsgGCCStrike15_v2_ClientSubmitSurveyVote = 9152,
	k_EMsgGCCStrike15_v2_Server2GCClientValidate = 9153,
	k_EMsgGCCStrike15_v2_MatchListRequestLiveGameForUser = 9154,
	k_EMsgGCCStrike15_v2_Server2GCPureServerValidationFailure = 9155,
	k_EMsgGCCStrike15_v2_Client2GCEconPreviewDataBlockRequest = 9156,
	k_EMsgGCCStrike15_v2_Client2GCEconPreviewDataBlockResponse = 9157,
	k_EMsgGCCStrike15_v2_AccountPrivacySettings = 9158,
	k_EMsgGCCStrike15_v2_SetMyActivityInfo = 9159,
	k_EMsgGCCStrike15_v2_MatchListRequestTournamentPredictions = 9160,
	k_EMsgGCCStrike15_v2_MatchListUploadTournamentPredictions = 9161,
	k_EMsgGCCStrike15_v2_DraftSummary = 9162,
	k_EMsgGCCStrike15_v2_ClientRequestJoinFriendData = 9163,
	k_EMsgGCCStrike15_v2_ClientRequestJoinServerData = 9164,
	k_EMsgGCCStrike15_v2_ClientRequestNewMission = 9165,
	k_EMsgGCCStrike15_v2_GC2ServerNotifyXPRewarded = 9166,
	k_EMsgGCCStrike15_v2_GC2ClientTournamentInfo = 9167,
	k_EMsgGC_GlobalGame_Subscribe = 9168,
	k_EMsgGC_GlobalGame_Unsubscribe = 9169,
	k_EMsgGC_GlobalGame_Play = 9170,
	k_EMsgGCCStrike15_v2_AcknowledgePenalty = 9171,
	k_EMsgGCCStrike15_v2_Client2GCRequestPrestigeCoin = 9172,
	k_EMsgGCCStrike15_v2_GC2ClientGlobalStats = 9173,
	k_EMsgGCCStrike15_v2_Client2GCStreamUnlock = 9174,
	k_EMsgGCCStrike15_v2_FantasyRequestClientData = 9175,
	k_EMsgGCCStrike15_v2_FantasyUpdateClientData = 9176,
	k_EMsgGCCStrike15_v2_GCToClientSteamdatagramTicket = 9177,
	k_EMsgGCCStrike15_v2_ClientToGCRequestTicket = 9178,
	k_EMsgGCCStrike15_v2_ClientToGCRequestElevate = 9179,
	k_EMsgGCCStrike15_v2_GlobalChat = 9180,
	k_EMsgGCCStrike15_v2_GlobalChat_Subscribe = 9181,
	k_EMsgGCCStrike15_v2_GlobalChat_Unsubscribe = 9182,
	k_EMsgGCCStrike15_v2_ClientAuthKeyCode = 9183,
	k_EMsgGCCStrike15_v2_GotvSyncPacket = 9184,
	k_EMsgGCCStrike15_v2_ClientPlayerDecalSign = 9185,
	k_EMsgGCCStrike15_v2_ClientLogonFatalError = 9187,
	k_EMsgGCCStrike15_v2_ClientPollState = 9188,
	k_EMsgGCCStrike15_v2_Party_Register = 9189,
	k_EMsgGCCStrike15_v2_Party_Unregister = 9190,
	k_EMsgGCCStrike15_v2_Party_Search = 9191,
	k_EMsgGCCStrike15_v2_Party_Invite = 9192,
	k_EMsgGCCStrike15_v2_Account_RequestCoPlays = 9193,
	k_EMsgGCCStrike15_v2_ClientGCRankUpdate = 9194,
	k_EMsgGCCStrike15_v2_ClientRequestOffers = 9195,
	k_EMsgGCCStrike15_v2_ClientAccountBalance = 9196,
	k_EMsgGCCStrike15_v2_ClientPartyJoinRelay = 9197,
	k_EMsgGCCStrike15_v2_ClientPartyWarning = 9198,
	k_EMsgGCCStrike15_v2_MatchmakingServerMatchEndPartial = 9199,
	k_EMsgGCCStrike15_v2_ClientRequestSouvenir = 9204
};

EGCResults __fastcall Hooked::RetrieveMessage( void* ecx, void* edx, uint32_t* punMsgType, void* pubDest, uint32_t cubDest, uint32_t* pcubMsgSize ) {

	EGCResults status = oRetrieveMessage( ecx, punMsgType, pubDest, cubDest, pcubMsgSize );
	if( status != k_EGCResultOK )
		return status;

	auto msg = *punMsgType & 0x7FFFFFFF;

	//printf( "[->] Message got from GC [%d]!\n", msg );

	if( g_Vars.misc.auto_accept && msg == ECsgoGCMsg::k_EMsgGCCStrike15_v2_GCToClientSteamdatagramTicket )
	{
		auto SetLocalPlayerReady = [ & ] ( ) -> void {
			// xref deferred ( if ( sub_108E78E0(str, "deferred") ) )

			static auto SetLocalPlayerReadyFn = reinterpret_cast< bool( __stdcall* )( const char* ) >( Memory::Scan( XorStr( "client.dll" ), XorStr( "55 8B EC 83 E4 F8 8B 4D 08 BA ? ? ? ? E8 ? ? ? ? 85 C0 75 12" ) ) );
			if( SetLocalPlayerReadyFn )
				SetLocalPlayerReadyFn( XorStr( "" ) );
		};

		SetLocalPlayerReady( );
	}

	return status;
}