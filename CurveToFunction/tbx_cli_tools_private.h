/*--------------------------------------------------------------------------------------------------------------------------------
 |
 |	Project:    	TBX sample common sour code
 |
 |	Filename:   	tbx_cli_tools_private.h
 |
 |	Copyright:  	TelcoBridges 2002-2006, All Rights Reserved
 |
 |	Description:	This file contains private definition for tbx_cli_tools.
 |
 |	Notes:      	Tabs = 4
 |
 *-------------------------------------------------------------------------------------------------------------------------------
 |
 |	Revision:   	$Revision: 136500 $
 |
 *------------------------------------------------------------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------------------------------------------------------------
 |  Define header file
 *------------------------------------------------------------------------------------------------------------------------------*/
#ifndef __TBX_CLI_TOOLS_PRIVATE_H__
#define __TBX_CLI_TOOLS_PRIVATE_H__


/*--------------------------------------------------------------------------------------------------------------------------------
 |  C++ support
 *------------------------------------------------------------------------------------------------------------------------------*/
#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

/*--------------------------------------------------------------------------------------------------------------------------------
 |  Includes
 *------------------------------------------------------------------------------------------------------------------------------*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <ctype.h>

/* Local includes */
#include "tbx_ostypes.h"
//#include "tbx_buildtag.h"

#include "tbx_id.h"
#include "tbx_result.h"
#include "tbx_std.h"
#include "tbx_os_wrapper.h"
#include "tbx_cli.h"
#include "tbx_msg.h"




//#include "tbx_api.h"
//#include "tbx_env.h"
//#include "tbx_msgset.h"
//#include "tbx_launchlib.h"

//#include "tbx_apps_id.h"

#include "tbx_cli_tools.h"


/*--------------------------------------------------------------------------------------------------------------------------------
 |  OS-specific defines
 *------------------------------------------------------------------------------------------------------------------------------*/

#define TBX_CLI_TOOLS_USE_LOG_FILE
#define TBX_CLI_TOOLS_USE_REMOTE_CONTROL

/*--------------------------------------------------------------------------------------------------------------------------------
 |  Defines
 *------------------------------------------------------------------------------------------------------------------------------*/

#define TBX_CLI_TOOLS_CLI_INVALID_VALUE_SHAKE_DELAY_MS					2000
																		
#define TBX_CLI_TOOLS_SCROLL_ACCELERATE_DELAY_MS						400
#define TBX_CLI_TOOLS_SCROLL_ACCELERATE_PERIOD_MS						200
#define TBX_CLI_TOOLS_SCROLL_ACCELERATE_MAX_FACTOR						2
#define TBX_CLI_TOOLS_SCROLL_ACCELERATE_MAX_FACTOR_PAGE_UP				2
#define TBX_CLI_TOOLS_CLI_SCREEN_WIDTH_MAX_PRIVATE						2048
#define TBX_CLI_TOOLS_MAX_REMOTE_SCREEN_BUF_SIZE						(256*1024)
#define TBX_CLI_TOOLS_MAX_REMOTE_SCREEN_BUF_SIZE_NOT_REMOTE_CONTROLLED	(1024)

#define TBX_CLI_TOOLS_LINE_MIN_WRAP_LENGTH					80	/* Minimum number of characters on one line before wrap */

#define TBX_CLI_TOOLS_TMP_LINE_MAX_SIZE						(16*1024)
#define TBX_CLI_TOOLS_DISK_BUFFER_SIZE						(128*1024)
#define TBX_CLI_TOOLS_DISK_BUFFER_FLUSH_DELAY_MS			1000

#define TBX_CLI_TOOLS_MAX_REMOTE_SCREEN_UPDATE_DELAY_MS		10000
#define TBX_CLI_TOOLS_MAX_REMOTE_CLS_DELAY_MS				30000
#define TBX_CLI_TOOLS_MAX_REMOTE_REFRESH_DELAY_MS			15000

#define TBX_CLI_TOOLS_MAX_FORCED_KEYS						256

#define TBX_CLI_TOOLS_LINE_OVERFLOW_SYMBOL					"..."


#define TBX_CLI_TOOLS_TELCOBRIDGES_LOGO																																	\
{																																										\
	" " ESC F_RED AND_COLOR B_BLUE END_COLOR "   "                                        "                                                                          " DEFAULT_COLOR,	\
	" " ESC F_RED AND_COLOR B_BLUE END_COLOR "   "                                        "                                        *******************               " DEFAULT_COLOR,	\
	" " ESC F_RED AND_COLOR B_BLUE END_COLOR "   "                                        "                                *********************************         " DEFAULT_COLOR,	\
	" " ESC F_RED AND_COLOR B_BLUE END_COLOR "   "                                        "                        *******************************************       " DEFAULT_COLOR,	\
	" " ESC F_RED AND_COLOR B_BLUE END_COLOR "   "                                        "                  ****************************************************    " DEFAULT_COLOR,	\
	" " ESC F_RED AND_COLOR B_BLUE END_COLOR "   "                                        "            **************                                   **********   " DEFAULT_COLOR,	\
	" " ESC F_RED AND_COLOR B_BLUE END_COLOR "   "                                        "         **********                                             ********  " DEFAULT_COLOR,	\
	" " ESC F_RED AND_COLOR B_BLUE END_COLOR "   "                                        "      ********                                                    *****   " DEFAULT_COLOR,	\
	" " ESC F_RED AND_COLOR B_BLUE END_COLOR "   "                                        "  ******                                                           ***    " DEFAULT_COLOR,	\
	" " ESC F_RED AND_COLOR B_BLUE END_COLOR "  *"                                        "****                                                                      " DEFAULT_COLOR,	\
	" " ESC F_RED AND_COLOR B_BLUE END_COLOR "***" ESC F_GREEN AND_COLOR B_BLUE END_COLOR "TTTTTTTTTTTTTTT                                                           " DEFAULT_COLOR,	\
	" " ESC F_RED AND_COLOR B_BLUE END_COLOR "   " ESC F_GREEN AND_COLOR B_BLUE END_COLOR "TTTTTTTTTTTTTTT                                                           " DEFAULT_COLOR,	\
	" " ESC F_RED AND_COLOR B_BLUE END_COLOR "   " ESC F_GREEN AND_COLOR B_BLUE END_COLOR "TTTTTTTTTTTTTTT                                                           " DEFAULT_COLOR,	\
	" " ESC F_RED AND_COLOR B_BLUE END_COLOR "   " ESC F_GREEN AND_COLOR B_BLUE END_COLOR "    TTTTT    EEEE L    CC  OO  BBB  RRR  III DDD   GGG EEEE  SSS          " DEFAULT_COLOR,	\
	" " ESC F_RED AND_COLOR B_BLUE END_COLOR "   " ESC F_GREEN AND_COLOR B_BLUE END_COLOR "   TTTTTT    E    L   C   O  O B  B R  R  I  D  D G    E    S             " DEFAULT_COLOR,	\
	" " ESC F_RED AND_COLOR B_BLUE END_COLOR "   " ESC F_GREEN AND_COLOR B_BLUE END_COLOR "   TTTTT     EE   L   C   O  O B B  RR    I  D  D G G  EEE   SS           " DEFAULT_COLOR,	\
	" " ESC F_RED AND_COLOR B_BLUE END_COLOR "   " ESC F_GREEN AND_COLOR B_BLUE END_COLOR "  TTTTTT     E    L   C   O  O B  B R R   I  D  D G  G E       S          " DEFAULT_COLOR,	\
	" " ESC F_RED AND_COLOR B_BLUE END_COLOR "   " ESC F_GREEN AND_COLOR B_BLUE END_COLOR "  TTTTT      EEEE LLL  CC  OO  BBB  R  R III DDD   GG  EEEE SSS           " DEFAULT_COLOR,	\
	" " ESC F_RED AND_COLOR B_BLUE END_COLOR "   " ESC F_GREEN AND_COLOR B_BLUE END_COLOR "                                                                          " DEFAULT_COLOR	\
}

#define TBX_CLI_TOOLS_VXWORKS_SIM_OUTPUT_FILENAME			"vxwim_tty_ouput.txt"

/*--------------------------------------------------------------------------------------------------------------------------------
 |  Messages
 *------------------------------------------------------------------------------------------------------------------------------*/
#define	TBX_CLI_TOOLS_MSG_ID_NOTIF_CLI_TOOLS_SCREEN_UPDATE		TBX_MSG_ID_GEN (TBX_ID_CLASS_TBX_APPS_CLI_TOOLS_PRIVATE, 0x001)
#define	TBX_CLI_TOOLS_MSG_ID_NOTIF_CLI_TOOLS_CLS_REQUEST		TBX_MSG_ID_GEN (TBX_ID_CLASS_TBX_APPS_CLI_TOOLS_PRIVATE, 0x002)
#define	TBX_CLI_TOOLS_MSG_ID_NOTIF_CLI_TOOLS_REMOTE_KEY			TBX_MSG_ID_GEN (TBX_ID_CLASS_TBX_APPS_CLI_TOOLS_PRIVATE, 0x003)

	

/*-------------------------------------------------------------------------------------------------------------------------------
 |
 |	Message:
 |		TBX_CLI_TOOLS_MSG_ID_NOTIF_CLI_TOOLS_SCREEN_UPDATE
 |
 |	Description:
 |		This event message is broadcasted by tbx_cli_tools to allow remote monitoring of this instance
 |
 |	Event parameters:
 |		Header							Generic message header. Should not be modified directly by the application.
 |		un32MsgVersion					Version of the message. Should be set to 1.
 |		fCls							If TBX_TRUE, remote screen must be cleared before printing this buffer.
 |										Otherwise, remote screen must be returned 'home' without clearing.
 |		un32EvtRemoteScreenStrlen		String length for szScreen
 |		szScreen						Content to print on the remote screen.
 |										This refreshes entire screen. Remote application must clear screen,
 |										or return home (according to fCls) before printing this buffer.
 |
 |	Notices:
 |		~
 |
 |	Compatibility:
 |		~
 |
 |	Side effects:
 |		~
 |
 *------------------------------------------------------------------------------------------------------------------------------*/
typedef struct _TBX_CLI_TOOLS_EVT_NOTIF_CLI_TOOLS_SCREEN_UPDATE
{
	TBX_MSG_HEADER					Header;

	TBX_UINT32						un32MsgVersion;
	TBX_BOOLEAN						fCls;
	TBX_UINT8						un8Padding0 [3];		/* Align struct on 64 bits */

	TBX_UINT8						un8Padding1 [4];		/* Align struct on 64 bits */
	TBX_UINT32						un32EvtRemoteScreenStrlen;
	TBX_CHAR						szScreen[ 1 ];

} TBX_CLI_TOOLS_EVT_NOTIF_CLI_TOOLS_SCREEN_UPDATE, *PTBX_CLI_TOOLS_EVT_NOTIF_CLI_TOOLS_SCREEN_UPDATE;

typedef union _TBX_CLI_TOOLS_MSG_NOTIF_CLI_TOOLS_SCREEN_UPDATE
{
	TBX_MSG_HEADER									Header;
	TBX_CLI_TOOLS_EVT_NOTIF_CLI_TOOLS_SCREEN_UPDATE	Event;

} TBX_CLI_TOOLS_MSG_NOTIF_CLI_TOOLS_SCREEN_UPDATE, *PTBX_CLI_TOOLS_MSG_NOTIF_CLI_TOOLS_SCREEN_UPDATE;


/*-------------------------------------------------------------------------------------------------------------------------------
 |
 |	Message:
 |		TBX_CLI_TOOLS_MSG_ID_NOTIF_CLI_TOOLS_CLS_REQUEST
 |
 |	Description:
 |		This event message is sent to the actually remotely controlled host to request a full screen update (cls)
 |
 |	Event parameters:
 |		Header							Generic message header. Should not be modified directly by the application.
 |		un32MsgVersion					Version of the message. Should be set to 1.
 |		fLast							Tells if this is the last CLS request, due to the fact
 |										remote control is stopping now.
 |		fPollOnly						If TBX_TRUE, polling only (to tell remotely controlled application we're
 |										still alive). TBX_FALSE to actually clear screen
 |		un32CurrentScreenWidth			Current screen width of the remote controlling tool
 |		un32CurrentScreenHeight			Current screen height of the remote controlling tool
 |
 |	Notices:
 |		~
 |
 |	Compatibility:
 |		~
 |
 |	Side effects:
 |		~
 |
 *------------------------------------------------------------------------------------------------------------------------------*/
typedef struct _TBX_CLI_TOOLS_EVT_NOTIF_CLI_TOOLS_CLS_REQUEST
{
	TBX_MSG_HEADER					Header;

	TBX_UINT32						un32MsgVersion;
	TBX_BOOLEAN						fLast;
	TBX_BOOLEAN						fPollOnly;
	TBX_UINT8						un8Padding0 [2];		/* Align struct on 64 bits */

	TBX_UINT32						un32CurrentScreenWidth;
	TBX_UINT32						un32CurrentScreenHeight;

} TBX_CLI_TOOLS_EVT_NOTIF_CLI_TOOLS_CLS_REQUEST, *PTBX_CLI_TOOLS_EVT_NOTIF_CLI_TOOLS_CLS_REQUEST;

typedef union _TBX_CLI_TOOLS_MSG_NOTIF_CLI_TOOLS_CLS_REQUEST
{
	TBX_MSG_HEADER									Header;
	TBX_CLI_TOOLS_EVT_NOTIF_CLI_TOOLS_CLS_REQUEST	Event;

} TBX_CLI_TOOLS_MSG_NOTIF_CLI_TOOLS_CLS_REQUEST, *PTBX_CLI_TOOLS_MSG_NOTIF_CLI_TOOLS_CLS_REQUEST;


/*-------------------------------------------------------------------------------------------------------------------------------
 |
 |	Message:
 |		TBX_CLI_TOOLS_MSG_ID_NOTIF_CLI_TOOLS_REMOTE_KEY
 |
 |	Description:
 |		This event message is sent to the actually remotely controlled host to push a pressed key
 |
 |	Event parameters:
 |		Header							Generic message header. Should not be modified directly by the application.
 |		un32MsgVersion					Version of the message. Should be set to 1.
 |		Key								Key that was pressed
 |
 |	Notices:
 |		~
 |
 |	Compatibility:
 |		~
 |
 |	Side effects:
 |		~
 |
 *------------------------------------------------------------------------------------------------------------------------------*/
typedef struct _TBX_CLI_TOOLS_EVT_NOTIF_CLI_TOOLS_REMOTE_KEY
{
	TBX_MSG_HEADER					Header;

	TBX_UINT32						un32MsgVersion;
	TBX_UINT32						Key;

} TBX_CLI_TOOLS_EVT_NOTIF_CLI_TOOLS_REMOTE_KEY, *PTBX_CLI_TOOLS_EVT_NOTIF_CLI_TOOLS_REMOTE_KEY;

typedef union _TBX_CLI_TOOLS_MSG_NOTIF_CLI_TOOLS_REMOTE_KEY
{
	TBX_MSG_HEADER									Header;
	TBX_CLI_TOOLS_EVT_NOTIF_CLI_TOOLS_REMOTE_KEY	Event;

} TBX_CLI_TOOLS_MSG_NOTIF_CLI_TOOLS_REMOTE_KEY, *PTBX_CLI_TOOLS_MSG_NOTIF_CLI_TOOLS_REMOTE_KEY;


/*--------------------------------------------------------------------------------------------------------------------------------
 |  Types
 *------------------------------------------------------------------------------------------------------------------------------*/

/* One display buffer line */
typedef struct _TBX_CLI_TOOLS_DISPLAY_LINE_PRIVATE
{
	PTBX_CHAR	pszLine;
} TBX_CLI_TOOLS_DISPLAY_LINE_PRIVATE, *PTBX_CLI_TOOLS_DISPLAY_LINE_PRIVATE;

typedef struct _TBX_CLI_TOOLS_DISPLAY_BUFFER
{
	TBX_UINT32								un32CurrentWidth;
	TBX_UINT32								un32CurrentHeight;

	TBX_UINT32								un32NbLines;	/* Number of display lines in aLines */
	TBX_CLI_TOOLS_DISPLAY_LINE_PRIVATE		aLines[ TBX_CLI_TOOLS_CLI_SCREEN_HEIGHT_MAX ];
	TBX_BOOLEAN								afLinesBlink[ TBX_CLI_TOOLS_CLI_SCREEN_HEIGHT_MAX ];
} TBX_CLI_TOOLS_DISPLAY_BUFFER, *PTBX_CLI_TOOLS_DISPLAY_BUFFER;

typedef struct _TBX_CLI_TOOLS_THREAD_CONTEXT
{
	//TBX_THREAD									hThread;
	HANDLE                                      hThread;
	TBX_BOOL									fRunning;
	TBX_BOOL									fDone;
	TBX_BOOL									fContinue;

} TBX_CLI_TOOLS_THREAD_CONTEXT, *PTBX_CLI_TOOLS_THREAD_CONTEXT;

typedef struct _TBX_CLI_TOOLS_CMD_PRIVATE
{
	/* Public part of the structure */
	TBX_CLI_TOOLS_CMD						Public;

	TBX_BOOL								fIsPopupList;
	TBX_BOOL								fUserEnteredValue;
	TBX_BOOL								fEnteredValueValid;
	TBX_UINT32								un32EnteredValueTimestamp;

} TBX_CLI_TOOLS_CMD_PRIVATE, *PTBX_CLI_TOOLS_CMD_PRIVATE;


/* Context of a remotely controlled host */
//typedef struct _TBX_CLI_TOOLS_REMOTE_HOST_CTX
//{
//	/* Handle of this host */
//	TBX_ADAPTER_HANDLE			hRemoteHost;
//	TBX_HOST_INFO				HostInfo;
//	TBX_BOOL					fClsReceived;
//	TBX_UINT32					un32LastScreenUpdateTimestamp;
//
//} TBX_CLI_TOOLS_REMOTE_HOST_CTX, *PTBX_CLI_TOOLS_REMOTE_HOST_CTX;


/* Context of the CLI interface */
typedef struct _TBX_CLI_TOOLS_CLI_CONTEXT
{
	TBX_CLI_TOOLS_INIT_PARAMS					Params;

	/* Display buffer and log file */
	TBX_BOOL									fPrintAllowed;
	TBX_BOOL									fFirstRunDone;
	PTBX_CLI_TOOLS_DISPLAY_BUFFER				pDisplayBuf;
	PTBX_CLI_TOOLS_DISPLAY_BUFFER				pPrevDisplayBuf;
	PTBX_UINT8									pafEscapeChars;
	#ifdef TBX_CLI_TOOLS_USE_LOG_FILE
	FILE*										pLogFile;
	TBX_CHAR									szCurrentLogFileTime[ 64 ];
	TBX_CHAR									szCurrentLogFilePath[ 512 ];
	TBX_UINT32									un32CurrentLogFileSize;
	TBX_UINT32									un32CurrentLogFileSegment;
	#endif

	/* Buffer to gather user input */
	TBX_CLI_TOOLS_CMD_PRIVATE					CmdInput;

	/* Timestamps and other flags used for display */
	TBX_UINT32									un32LastRefreshTimestamp;
	TBX_BOOL									fRefreshRequired;
	TBX_BOOL									fKeyPressedRefreshRequired;
	TBX_BOOL									fClsRequired;
	TBX_BOOL									fImmediateValidation;
	TBX_BOOL									fNoWrap;
	TBX_UINT32									un32ScrollPositionMax;	/* Current maximum scroll position */
	TBX_UINT32									un32ScrollPosition;
	TBX_UINT32									un32ScrollPositionHor;	/* Horizontal scroll position */
	TBX_UINT32									un32ScrollSpeed;
	TBX_UINT32									un32ScrollSpeedTimestamp;
	TBX_UINT32									un32ScrollSpeedAccelerateTimestamp;
	TBX_UINT32									un32NbLogLinesDisplayed;
	TBX_UINT32									un32SelectedLineToBlink;
	TBX_UINT32									un32CheckpointCounter;
	TBX_UINT32									un32RefreshCounter;
	TBX_UINT32									un32BlinkCounter;
	TBX_UINT32									un32BlinkTimestamp;

	/* Circular buffer log */
	TBX_BOOL									fCircularLogFull;
	TBX_UINT32									un32CircularLogNextLineIndex;
	TBX_UINT32									un32CircularLogCurrentNbLines;
	TBX_UINT32									un32LastLogDisplayNbLines;
	PTBX_CLI_TOOLS_DISPLAY_LINE_PRIVATE			paCircularLog;
	PTBX_UINT8									pafCircularLogCheckpoints;
	PTBX_CHAR									pszTmpLine;

	#ifdef TBX_CLI_TOOLS_USE_LOG_FILE
	/* Disk write buffer */
	PTBX_CHAR									pszDiskWriteBuffer;
	TBX_UINT32									un32DiskWriteBufferCurLen;
	TBX_UINT32									un32DiskWriteBufferTimestamp;
	#endif

	/* Fifo of 'forced' keys */
	TBX_INT										aForcedKeysFifo[ TBX_CLI_TOOLS_MAX_FORCED_KEYS ];
	TBX_UINT32									un32ForcedKeysFifoPopIndex;
	TBX_UINT32									un32ForcedKeysFifoCount;

	/* Status of the usage of hostlib */
	TBX_BOOL									fUseHostlib;
	TBX_BOOL									fHostlibInUse;

	/* Hostlib message to send remote screen updates */
	TBX_BOOL											fGenerateRemoteScreen;
	//TBX_MSG_HANDLE										hEvtRemoteScreen;
	PTBX_CLI_TOOLS_EVT_NOTIF_CLI_TOOLS_SCREEN_UPDATE	pEvtRemoteScreen;
	TBX_UINT32											un32EvtRemoteScreenMaxStrlen;
	TBX_UINT32											un32RemoteCurrentWidth;
	TBX_UINT32											un32RemoteCurrentHeight;
	TBX_UINT32											un32RemoteClsTimestamp;
	TBX_UINT32											un32RemotePollTimestamp;
	TBX_UINT32											un32LastRemoteRefreshSendTimestamp;
	//TBX_HOST_INFO										RemoteControllingHostInfo;
	TBX_BOOL											fActuallyRemoteControlled;
	TBX_BOOL											fRemoteRefreshRequested;

	/* Used when remote controlling other host */
	//TBX_FILTER_HANDLE									hRemoteScreenFilter;
	/* Array of detected remote hosts */
	//TBX_CLI_TOOLS_REMOTE_HOST_CTX				aRemoteScreenHosts[ TBX_CLI_TOOLS_MAX_REMOTE_SCREEN_HOSTS ];
	TBX_UINT32									un32RemoteScreenCurrentIdx;
	TBX_UINT32									un32LastEscapePressedTimestamp;
	TBX_CHAR									szRemoteScreenHostName [TBX_API_NAME_MAX_LEN];
	TBX_CHAR									szRemoteScreenApplicationName [TBX_API_NAME_MAX_LEN];

	/* Threads and semaphores */
	TBX_CLI_TOOLS_THREAD_CONTEXT				CliThreadContext;
	TBX_MUTEX									Mutex;

	TBX_UINT32									un32StartTimestamp;
	TBX_UINT32									un32CurrentTimestamp;

} TBX_CLI_TOOLS_CLI_CONTEXT, *PTBX_CLI_TOOLS_CLI_CONTEXT;






/*--------------------------------------------------------------------------------------------------------------------------------
 |  Global variables
 *------------------------------------------------------------------------------------------------------------------------------*/
extern PTBX_CHAR	g_pszEndline;
extern PTBX_CHAR	g_pszTrunkatedEndline;
extern PTBX_CHAR	g_pszTrunkatedEndline2;
extern TBX_UINT32	g_un32EndlineStrlen;
extern TBX_UINT32	g_un32TrunkatedEndlineLen;
extern TBX_UINT32	g_un32TrunkatedEndlineStrlen;


/*--------------------------------------------------------------------------------------------------------------------------------
 |  Macros
 *------------------------------------------------------------------------------------------------------------------------------*/
int
TbxCliTools_strnicmp(
  IN 	const char *psz1,
  IN 	const char *psz2,
  IN 	int n );

char *
TbxCliTools_stristr(
  IN	char *psz1,
  IN	const char *psz2 );

/*--------------------------------------------------------------------------------------------------------------------------------
 |  Function Prototypes
 *------------------------------------------------------------------------------------------------------------------------------*/
TBX_INT  TbxCliToolsGetch( PTBX_CLI_TOOLS_CLI_CONTEXT );

TBX_VOID TbxCliToolsGatherUserInput
(
  IN		PTBX_CLI_TOOLS_CLI_CONTEXT					in_pCliToolsCtx,
  IN		TBX_INT										in_nOption
);

TBX_VOID TbxCliToolsUserInputValidate
(
  IN		PTBX_CLI_TOOLS_CLI_CONTEXT					in_pCliToolsCtx,
  IN		TBX_INT										in_nOption,
  IN		TBX_INT32									in_n32Increment,
  IN		TBX_BOOL									in_fTestAll
);

TBX_VOID TbxCliToolsCliUserPromptDisplay
(
  IN		PTBX_CLI_TOOLS_CLI_CONTEXT		in_pCliToolsCtx
);

TBX_RESULT TbxCliToolsGetLogDisplay
(
	IN		PTBX_CLI_TOOLS_CLI_CONTEXT			in_pCliToolsCtx,
	OUT		PTBX_UINT32							out_pun32DisplayBufLineToBlink
);

TBX_VOID TbxCliToolsDisplayBufferCleanup
(
	IN		PTBX_CLI_TOOLS_CLI_CONTEXT			in_pCliToolsCtx
);

TBX_RESULT TbxCliToolsLogFilesRotation
(
	IN		PTBX_CLI_TOOLS_CLI_CONTEXT				pCliToolsCtx
);

TBX_VOID TbxCliToolsFlushDiskWriteBuffer
(
	IN		PTBX_CLI_TOOLS_CLI_CONTEXT				pCliToolsCtx,
	IN		TBX_BOOL								in_fForced
);

void TbxCliToolsPrintFillLinePrivate
(
	IN		PTBX_CLI_TOOLS_CLI_CONTEXT				pCliToolsCtx,
	IN		TBX_CHAR								in_cFillChar,
	IN		TBX_UINT32								in_un32HighlightPos
);

void TbxCliToolsvPrintPrivate
(
	IN		PTBX_CLI_TOOLS_CLI_CONTEXT				pCliToolsCtx,
	IN		TBX_BOOL								in_fNewLineDefaultColor,
	IN		PTBX_CHAR								in_pszString,
	IN		va_list									in_Marker
);

TBX_INT TBXCliToolsCheckConsoleInput (void);

TBX_UINT32 TbxCliToolsFormatTimePrivate
(
	IN		PTBX_CLI_TOOLS_TIMESTAMP				in_pTimestamp,
	IN		PTBX_CHAR								out_pszBuffer
);

/*
 * Function prototypes for remote control (tbx_cli_tools_remote.c)
*/
TBX_RESULT TBXCliToolsRemoteStartUseHostlib
(
  IN		PTBX_CLI_TOOLS_CLI_CONTEXT			in_pCliToolsCtx
);
TBX_VOID TBXCliToolsRemoteStopUseHostlib
(
  IN		PTBX_CLI_TOOLS_CLI_CONTEXT			in_pCliToolsCtx
);
//TBX_RESULT TBXCliToolsRemoteHandleMsg
//(
//  IN		PTBX_CLI_TOOLS_CLI_CONTEXT			in_pCliToolsCtx,
//  IN		TBX_MSG_HANDLE						in_hMsg
//);

TBX_VOID TBXCliToolsCls
(
  IN		PTBX_CLI_TOOLS_CLI_CONTEXT			in_pCliToolsCtx
);
TBX_VOID TBXCliToolsLine_fprintf
(
  IN		PTBX_CLI_TOOLS_CLI_CONTEXT			in_pCliToolsCtx,
  IN		PTBX_CHAR							in_pszLine
);
TBX_VOID TBXCliToolsRemoteRefreshSend
(
  IN		PTBX_CLI_TOOLS_CLI_CONTEXT			in_pCliToolsCtx,
  IN		TBX_BOOL							in_fScreenCleared
);

TBX_VOID TBXCliToolsRemoteControlStop
(
  IN		PTBX_CLI_TOOLS_CLI_CONTEXT			in_pCliToolsCtx,
  IN		TBX_BOOL							in_fExplicit
);
TBX_RESULT TBXCliToolsRemoteSendClsRequest
(
  IN		PTBX_CLI_TOOLS_CLI_CONTEXT			in_pCliToolsCtx,
  IN		TBX_BOOL							in_fLast,
  IN		TBX_BOOL							in_fPollOnly
);
TBX_RESULT TBXCliToolsRemoteSendRemoteKey
(
  IN		PTBX_CLI_TOOLS_CLI_CONTEXT			in_pCliToolsCtx,
  IN		TBX_INT								in_Key,
  IN		TBX_ADAPTER_HANDLE					in_hAlternateHost
);
TBX_RESULT TBXCliToolsRemoteTestRemoteHostTimeout
(
  IN		PTBX_CLI_TOOLS_CLI_CONTEXT			in_pCliToolsCtx
);

TBX_BOOL TbxCliToolsIsSomeoneWatchingInternal
(
  IN		PTBX_CLI_TOOLS_CLI_CONTEXT			in_pCliToolsCtx
);


/*--------------------------------------------------------------------------------------------------------------------------------
 |  C++ support
 *------------------------------------------------------------------------------------------------------------------------------*/
#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __TBX_CLI_TOOLS_PRIVATE_H__ */

