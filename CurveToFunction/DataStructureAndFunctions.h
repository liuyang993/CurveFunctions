/*-------------------------------------------------------------------------------------------------------------------------------
 |
 |Purpose:
 |    save today high , low point etc
 |
 |
 |
 |
 *------------------------------------------------------------------------------------------------------------------------------*/
typedef struct _CTTodayInfo
{
	double					WholeDayHighest;
	double					WholeDayLowest;

	double					NearestHigh;
	double					NearestLow;

	bool					IfAlreadyBreakImportPoint;
	double					NextImportPoint;


	TBX_UINT32						un32MsgVersion;
	TBX_BOOLEAN						fCls;
	TBX_UINT8						un8Padding0 [3];		/* Align struct on 64 bits */

	TBX_UINT8						un8Padding1 [4];		/* Align struct on 64 bits */
	TBX_UINT32						un32EvtRemoteScreenStrlen;
	TBX_CHAR						szScreen[ 1 ];

} TBX_CLI_TOOLS_EVT_NOTIF_CLI_TOOLS_SCREEN_UPDATE, *PTBX_CLI_TOOLS_EVT_NOTIF_CLI_TOOLS_SCREEN_UPDATE;