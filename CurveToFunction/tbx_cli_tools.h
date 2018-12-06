/*--------------------------------------------------------------------------------------------------------------------------------
 |
 |	Project:    	TBX sample common sour code
 |
 |	Filename:   	tbx_cli_tools.h
 |
 |	Copyright:  	TelcoBridges 2002-2005, All Rights Reserved
 |
 |	Description:	This file contains the prototypes of a set of functions
 |					used to build command-line tools with menus, command prompt
 |					and scrollable log file.
 |
 |	Notes:      	Tabs = 4
 |
 *-------------------------------------------------------------------------------------------------------------------------------
 |
 |	Revision:   	$Revision: 137409 $
 |
 *------------------------------------------------------------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------------------------------------------------------------
 |  Define header file
 *------------------------------------------------------------------------------------------------------------------------------*/
#ifndef __TBX_CLI_TOOLS_H__
#define __TBX_CLI_TOOLS_H__


#include "tbx_ostypes.h"
#include "tbx_std.h"
#include "tbx_result.h"
//#include "minwindef.h"

#include <stdio.h>      /* printf */
#include <stdarg.h> 

#include <windows.h>



/*--------------------------------------------------------------------------------------------------------------------------------
 |  C++ support
 *------------------------------------------------------------------------------------------------------------------------------*/
#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */


/*--------------------------------------------------------------------------------------------------------------------------------
 |  Defines
 *------------------------------------------------------------------------------------------------------------------------------*/
#define TBX_API_NAME_MAX_LEN				128




#define TBX_CLI_TOOLS_CLI_SCREEN_WIDTH_MAX					1024
#define TBX_CLI_TOOLS_CLI_SCREEN_HEIGHT_MAX					200
#define TBX_CLI_TOOLS_CLI_SCREEN_WIDTH_MIN					80
#define TBX_CLI_TOOLS_CLI_SCREEN_HEIGHT_MIN					23

#define TBX_CLI_TOOLS_CLI_LINE_WIDTH_MAX					256

#define TBX_CLI_TOOLS_CLI_MAX_PROMPT_LINES					32
#define TBX_CLI_TOOLS_CLI_MAX_LOG_BUFFER_LINES				1000000
#define TBX_CLI_TOOLS_CLI_DEFAULT_MAX_LOG_FILE_SIZE			(100*1024*1024)	/* 100 Megabytes */

#define TBX_CLI_TOOLS_CLI_MIN_DELAY_BETWEEN_REFRESH_MS		20
#define TBX_CLI_TOOLS_CLI_MAX_DELAY_BETWEEN_REFRESH_MS		1000

#define TBX_CLI_TOOLS_CLI_MIN_DELAY_BETWEEN_CLS_SEC			10

#define TBX_CLI_TOOLS_CLI_MAX_LOG_FILE_SIZE_MINVALUE		10000	/* 10k is smaller accepted log file size */

#define TBX_CLI_TOOLS_MAX_TBX_GW_REMOTE_ADDR				16

#define TBX_CLI_TOOLS_MAX_REMOTE_SCREEN_HOSTS				32

#define TBX_CLI_TOOLS_SCROLL_END							((TBX_UINT32)-1)
#define TBX_CLI_TOOLS_SCROLL_PAGE							((TBX_UINT32)-2)
#define TBX_CLI_TOOLS_SCROLL_SPEED_DEFAULT					1
#define TBX_CLI_TOOLS_SCROLL_WHEEL_SPEED_DEFAULT			3

/* Masks for a key description */
#define TBX_CLI_TOOLS_KEY_CODE_MASK							0x1fff
#define TBX_CLI_TOOLS_KEY_MODIFIERs_MASK					0xE000
#define TBX_CLI_TOOLS_KEY_OTHER_INFO_MASK					0xFF000000 /* Mask to separate 'other info' from pressed key */

/* Simulated extended ASCII for comparability between Windows/Linux/Solaris/... */
#define TBX_CLI_TOOLS_KEY_FIRST								0x1000
#define TBX_CLI_TOOLS_KEY_UP_ARROW_KEY						0x1001
#define TBX_CLI_TOOLS_KEY_DOWN_ARROW_KEY					0x1002
#define TBX_CLI_TOOLS_KEY_LEFT_ARROW_KEY					0x1003
#define TBX_CLI_TOOLS_KEY_RIGHT_ARROW_KEY					0x1004
#define TBX_CLI_TOOLS_KEY_PAGE_UP_KEY						0x1005
#define TBX_CLI_TOOLS_KEY_PAGE_DOWN_KEY						0x1006
#define TBX_CLI_TOOLS_KEY_HOME_KEY							0x1007
#define TBX_CLI_TOOLS_KEY_END_KEY							0x1008
#define TBX_CLI_TOOLS_KEY_ESCAPE							0x1009
#define TBX_CLI_TOOLS_KEY_CONTROL_C							0x100A
#define TBX_CLI_TOOLS_KEY_BACKSPACE							0x100B
#define TBX_CLI_TOOLS_KEY_INSERT							0x100C
#define TBX_CLI_TOOLS_KEY_DELETE							0x100D
#define TBX_CLI_TOOLS_KEY_PRINTSCREEN						0x100E
#define TBX_CLI_TOOLS_KEY_WINDOWS_KEY						0x100F
#define TBX_CLI_TOOLS_KEY_MOUSE_WHEEL_UP					0x1100
#define TBX_CLI_TOOLS_KEY_MOUSE_WHEEL_DOWN					0x1101

#define TBX_CLI_TOOLS_KEY_F1								0x1011
#define TBX_CLI_TOOLS_KEY_F2								0x1012
#define TBX_CLI_TOOLS_KEY_F3								0x1013
#define TBX_CLI_TOOLS_KEY_F4								0x1014
#define TBX_CLI_TOOLS_KEY_F5								0x1015
#define TBX_CLI_TOOLS_KEY_F6								0x1016
#define TBX_CLI_TOOLS_KEY_F7								0x1017
#define TBX_CLI_TOOLS_KEY_F8								0x1018
#define TBX_CLI_TOOLS_KEY_F9								0x1019
#define TBX_CLI_TOOLS_KEY_F10								0x101A
#define TBX_CLI_TOOLS_KEY_F11								0x101B
#define TBX_CLI_TOOLS_KEY_F12								0x101C

#define TBX_CLI_TOOLS_KEY_TAB								0x9

/* Key modifiers */
#define TBX_CLI_TOOLS_KEY_ALT_MODIFIER_MASK					0x2000
#define TBX_CLI_TOOLS_KEY_CTRL_MODIFIER_MASK				0x4000
#define TBX_CLI_TOOLS_KEY_SHIFT_MODIFIER_MASK				0x8000

/* Other information */
#define TBX_CLI_TOOLS_KEY_REMOTE_CONTROL_MODIFIER_MASK		0x01000000

/*
 * Default keys that all applications that use tbx_cli_tools
*/
#define TBX_CLI_TOOLS_DEFAULT_KEY_LOG_SCROLL_UP				TBX_CLI_TOOLS_KEY_UP_ARROW_KEY
#define TBX_CLI_TOOLS_DEFAULT_KEY_LOG_SCROLL_DOWN			TBX_CLI_TOOLS_KEY_DOWN_ARROW_KEY
#define TBX_CLI_TOOLS_DEFAULT_KEY_LOG_SCROLL_LEFT			TBX_CLI_TOOLS_KEY_LEFT_ARROW_KEY
#define TBX_CLI_TOOLS_DEFAULT_KEY_LOG_SCROLL_LEFTMOST		(TBX_CLI_TOOLS_KEY_LEFT_ARROW_KEY | TBX_CLI_TOOLS_KEY_ALT_MODIFIER_MASK)
#define TBX_CLI_TOOLS_DEFAULT_KEY_LOG_SCROLL_LEFTMOST_2		(TBX_CLI_TOOLS_KEY_LEFT_ARROW_KEY | TBX_CLI_TOOLS_KEY_CTRL_MODIFIER_MASK)
#define TBX_CLI_TOOLS_DEFAULT_KEY_LOG_SCROLL_RIGHT			TBX_CLI_TOOLS_KEY_RIGHT_ARROW_KEY
#define TBX_CLI_TOOLS_DEFAULT_KEY_LOG_SCROLL_PAGE_UP		TBX_CLI_TOOLS_KEY_PAGE_UP_KEY
#define TBX_CLI_TOOLS_DEFAULT_KEY_LOG_SCROLL_PAGE_DOWN		TBX_CLI_TOOLS_KEY_PAGE_DOWN_KEY
#define TBX_CLI_TOOLS_DEFAULT_KEY_LOG_SCROLL_BEGIN			TBX_CLI_TOOLS_KEY_HOME_KEY
#define TBX_CLI_TOOLS_DEFAULT_KEY_LOG_SCROLL_END			TBX_CLI_TOOLS_KEY_END_KEY
#define TBX_CLI_TOOLS_DEFAULT_KEY_LOG_SCROLL_DOWN_MOUSE		TBX_CLI_TOOLS_KEY_MOUSE_WHEEL_DOWN
#define TBX_CLI_TOOLS_DEFAULT_KEY_LOG_SCROLL_UP_MOUSE		TBX_CLI_TOOLS_KEY_MOUSE_WHEEL_UP
#define TBX_CLI_TOOLS_DEFAULT_KEY_LOG_SCROLL_RIGHT_MOUSE	TBX_CLI_TOOLS_KEY_MOUSE_WHEEL_DOWN | TBX_CLI_TOOLS_KEY_SHIFT_MODIFIER_MASK
#define TBX_CLI_TOOLS_DEFAULT_KEY_LOG_SCROLL_LEFT_MOUSE		TBX_CLI_TOOLS_KEY_MOUSE_WHEEL_UP | TBX_CLI_TOOLS_KEY_SHIFT_MODIFIER_MASK
#define TBX_CLI_TOOLS_DEFAULT_CHECKPOINT_SET				'*'
#define TBX_CLI_TOOLS_DEFAULT_CHECKPOINT_NEXT				'+'
#define TBX_CLI_TOOLS_DEFAULT_CHECKPOINT_PREV				'-'
#define TBX_CLI_TOOLS_DEFAULT_LOG_FIND						'/'
#define TBX_CLI_TOOLS_DEFAULT_LOG_FILE_SPLIT				'\\'
#define TBX_CLI_TOOLS_DEFAULT_REFRESH_SCREEN				' '


/* Characters used to build a "user input" value that this tbx_cli_tools library handles
   as a list of options that the user can select using keyboard arrows.
   The caller must build a list of the following form:
   { item_1, item_2, @item_3, item_4 }
   Where @ marks the current selected element that will be changed by tbx_cli_tools
   when the user uses the keyboard arrows.
*/
#define TBX_CLI_TOOLS_POPUP_LIST_START_CHAR						'{'	/* Character to use to make the user input value behave as a popup list */
#define TBX_CLI_TOOLS_POPUP_LIST_END_CHAR						'}'	/* Character to use to make the user input value behave as a popup list */
#define TBX_CLI_TOOLS_POPUP_LIST_CURR_ELEM_START_CHAR			'@'	/* Character to mark the 'current' element value */
#define TBX_CLI_TOOLS_POPUP_LIST_SEPARATOR						','	/* Character to separate the elements */
#define TBX_CLI_TOOLS_POPUP_LIST_SHORT_OPTION_CHAR				'$'	/* Character to use to make the user input value behave as a popup list showing one item at a time */
#define TBX_CLI_TOOLS_POPUP_LIST_OPTION_NO_IMMEDIATE_VALIDATION	'!'	/* Prevents immediate validation when user changes from one selection to the other among the choices (by default validation function is called between each) */

/*
 * Messages and error tracing level for log buffer / log file
*/
typedef enum _TRACE_LEVEL
{
	TRACE_LEVEL_0		= TBX_TRACE_LEVEL_0,
	TRACE_LEVEL_1		= TBX_TRACE_LEVEL_1,
	TRACE_LEVEL_2		= TBX_TRACE_LEVEL_2,
	TRACE_LEVEL_3		= TBX_TRACE_LEVEL_3,
	TRACE_LEVEL_4		= TBX_TRACE_LEVEL_4,
	TRACE_LEVEL_MAX		= TBX_TRACE_LEVEL_MAX,
	TRACE_LEVEL_ALWAYS	= TBX_TRACE_LEVEL_ALWAYS,
	TRACE_LEVEL_ERROR	= TBX_TRACE_LEVEL_ERROR
} TRACE_LEVEL, *PTRACE_LEVEL;


/*--------------------------------------------------------------------------------------------------------------------------------
 |  Types
 *------------------------------------------------------------------------------------------------------------------------------*/

/* Handle of this library */
typedef TBX_HANDLE	TBX_CLI_TOOLS_HANDLE, *PTBX_CLI_TOOLS_HANDLE;


/* One display buffer line */
typedef struct _TBX_CLI_TOOLS_DISPLAY_LINE
{
	TBX_CHAR	szLine[ TBX_CLI_TOOLS_CLI_LINE_WIDTH_MAX ];
	TBX_BOOL	fBlink;
} TBX_CLI_TOOLS_DISPLAY_LINE, *PTBX_CLI_TOOLS_DISPLAY_LINE;

/*
 * Structure that contains information used when user enters commands
*/
typedef struct _TBX_CLI_TOOLS_CMD
{
	/* Type of command actually being entered */
	TBX_UINT32								CmdType;		/* WARNING: 0 is reserved for "NO COMMAND" */

	/* Current index of the input being gathered from user. */
	TBX_UINT32								un32CurrentPromptIdx;

	/* Array of un32MaxPromptLines prompts to display to user */
	PTBX_CLI_TOOLS_DISPLAY_LINE				paszPrompt;
	/* Array of un32MaxPromptLines buffer used to gather user input. Please fill with default values. */
	PTBX_CLI_TOOLS_DISPLAY_LINE				paszUserInput;

} TBX_CLI_TOOLS_CMD, *PTBX_CLI_TOOLS_CMD;

/*
 * Structure that holds a timestamp
*/
typedef struct _TBX_CLI_TOOLS_TIMESTAMP
{
	#ifdef WIN32
		FILETIME		FileTime;
	#elif defined MQX
		TIME_STRUCT		Timestruct;
	#else
		struct timeval	Timeval;
	#endif
} TBX_CLI_TOOLS_TIMESTAMP, *PTBX_CLI_TOOLS_TIMESTAMP;

/*-------------------------------------------------------------------------------------------------------------------------------
 |
 |  Callback function used to display application menu.
 |	in_hCliTools			:	Handle of the TBX CLI library
 |  in_pContext				:	User defined context
 |
 *------------------------------------------------------------------------------------------------------------------------------*/
typedef TBX_VOID (*PFCT_TBX_CLI_TOOLS_DISPLAY_MENU_CALLBACK)
(
	IN		TBX_CLI_TOOLS_HANDLE					in_hCliTools,
	IN		PTBX_VOID								in_pContext
);

/*-------------------------------------------------------------------------------------------------------------------------------
 |
 |  Callback function used to choose the current input function according to key pressed.
 |	in_hCliTools			:	Handle of the TBX CLI library
 |  in_pContext				:	User defined context
 |  in_KeyPressed			:	Key pressed
 |  io_pCmdInput			:	Command input structure.
 |								Application must fill it with prompts to display to user
 |								and default input values.
 |
 *------------------------------------------------------------------------------------------------------------------------------*/
typedef TBX_VOID (*PFCT_TBX_CLI_TOOLS_MENU_CHOICE_CALLBACK)
(
	IN		TBX_CLI_TOOLS_HANDLE					in_hCliTools,
	IN		PTBX_VOID								in_pContext,
	IN		TBX_INT									in_KeyPressed,
	IN_OUT	PTBX_CLI_TOOLS_CMD						io_pCmdInput
);

/*-------------------------------------------------------------------------------------------------------------------------------
 |
 |  Callback function used to validate a user input.
 |	in_hCliTools			:	Handle of the TBX CLI library
 |  in_pContext				:	User defined context
 |  io_pCmdInput			:	Command input structure.
 |								The application must validate that the prompt index un32CurrentPromptIdx
 |								is valid (and maybe also all previous prompts).
 |								If user input is invalid, please replace it with something valid.
 |  Return					:	TBX_TRUE if user input is valid, TBX_FALSE otherwise.
 |
 *------------------------------------------------------------------------------------------------------------------------------*/
typedef TBX_BOOL (*PFCT_TBX_CLI_TOOLS_VALIDATE_INPUT_CALLBACK)
(
	IN		TBX_CLI_TOOLS_HANDLE					in_hCliTools,
	IN		PTBX_VOID								in_pContext,
	IN_OUT	PTBX_CLI_TOOLS_CMD						io_pCmdInput
);

/*-------------------------------------------------------------------------------------------------------------------------------
 |
 |  Callback function used to apply a user input.
 |	in_hCliTools			:	Handle of the TBX CLI library
 |  in_pContext				:	User defined context
 |  io_pCmdInput			:	Command input structure.
 |								All user inputs are now filled and validated.
 |								Command should be ready to be applied.
 |
 *------------------------------------------------------------------------------------------------------------------------------*/
typedef TBX_VOID (*PFCT_TBX_CLI_TOOLS_APPLY_INPUT_CALLBACK)
(
	IN		TBX_CLI_TOOLS_HANDLE					in_hCliTools,
	IN		PTBX_VOID								in_pContext,
	IN_OUT	PTBX_CLI_TOOLS_CMD						io_pCmdInput
);


/*
 * Parameters of this library
*/
typedef struct _TBX_CLI_TOOLS_INIT_PARAMS
{
	/* Min/max command-line interface screen width/height in number of characters/lines */
	TBX_UINT32									un32MinScreenWidth;
	TBX_UINT32									un32MinScreenHeight;
	TBX_UINT32									un32MaxScreenWidth;
	TBX_UINT32									un32MaxScreenHeight;

	/* Maximum command-line interface screen width/height. Used for platforms where dynamic resizing is not implemented */
	TBX_UINT32									un32DefaultScreenWidth;
	TBX_UINT32									un32DefaultScreenHeight;

	/* Maximum number of lines to prompt for commands */
	TBX_UINT32									un32MaxPromptLines;

	/* Display the log window or not */
	TBX_BOOL									fDisplayLog;
	/* Hide the command prompt or not (commands can be entered even if not shown) */
	TBX_BOOL									fHidePrompt;

	/* Refresh delay */
	TBX_UINT32									un32MinRefreshDelay;	/* Minimum time between screen redraw */
	TBX_UINT32									un32MaxRefreshDelay;	/* Maximum time between screen redraw */

	/* Clear screen delay */
	TBX_UINT32									un32MinClsDelay;		/* Minimum time between clear screen */

	/* Tells if the CLI thread must be launched low priority */
	TBX_BOOL									fLowPrioryThread;

	/* Tells if must read from terminal input or not.
	   Not reading from input allows running as background process */
	TBX_BOOL									fDisableTerminalInput;
	/* Tells if must print to terminal output or not */
	TBX_BOOL									fDisableTerminalOutput;

	/* Trace level and log files parameters */
	TRACE_LEVEL									TraceLevelStdout;
	TRACE_LEVEL									TraceLevelLogFile;
	TBX_CHAR									szLogFileName[ 256 ];
	TBX_BOOL									fDontRotateLogFile;		/* Disable log files rotation */
	TBX_BOOL									fFlushLogOnlyOnError;	/* Increase logs performance by avoiding fflush on each log trace, doing it only on errors */
	TBX_BOOL									fLogKeepColorCodes;		/* Keep the color codes in the log file or not */
	TBX_UINT32									un32LogBufferMaxLines;	/* Length of the log buffer (number of lines) */
	TBX_UINT32									un32MaxLogFileSize;		/* Maximum log file size.
																		   After reaching this size, log is split.
																		   Value of 0 uses default TBX_CLI_TOOLS_CLI_DEFAULT_MAX_LOG_FILE_SIZE */
	TBX_UINT32									un32MaxLogFilesPerDay;	/* Determines the maximum number of log files per day.
																		   Older segments will be deleted, if appropriate.
																		   Value of 0 will put no limit on number of files */

	/* Callback function to display the menu */
	PFCT_TBX_CLI_TOOLS_DISPLAY_MENU_CALLBACK	pFctDisplayMenu;
	PTBX_VOID									pCtxDisplayMenu;	/* User context for pFctDisplayMenu */

	/* Callback function to choose the current input function according to key pressed */
	PFCT_TBX_CLI_TOOLS_MENU_CHOICE_CALLBACK		pFctHandleMenuChoice;
	PTBX_VOID									pCtxHandleMenuChoice;	/* User context for pFctHandleMenuChoice */

	/* Callback function to validate a user input */
	PFCT_TBX_CLI_TOOLS_VALIDATE_INPUT_CALLBACK	pFctValidateUserInput;
	PTBX_VOID									pCtxValidateUserInput;	/* User context for pFctValidateUserInput */

	/* Callback function to apply a user input */
	PFCT_TBX_CLI_TOOLS_APPLY_INPUT_CALLBACK		pFctApplyUserInput;
	PTBX_VOID									pCtxApplyUserInput;	/* User context for pFctApplyUserInput */


	/* Optional handle to TBX hostlib. If provided, tbx_cli_tools will use hostlib
	   to allow being remotely controlled by another tbx_cli_tools application */
	TBX_LIB_HANDLE								hTbxLib;
	TBX_BOOL									fReceiveRemoteScreen;	/* If set, will not send screen to remote, but instead receive remote screens */

} TBX_CLI_TOOLS_INIT_PARAMS, *PTBX_CLI_TOOLS_INIT_PARAMS;




/* Network configuration structure read from config file through TbxCliToolsNetworkConfigParseLine */
//typedef struct _TBX_CLI_TOOLS_CONFIG_NETWORK_PARAMS
//{
//	TBX_UINT32							un32Reserved;
//
//	TBX_CHAR							szApplicationName[ TBX_API_NAME_MAX_LEN ];
//
//	/* Local IP/Port */
//	TBX_UINT16							aun16TbxGwPort[TBX_API_MAX_EXTRA_GW_PORTS + 1];
//	TBX_CHAR							szInterfaceIp0[ 64 ];
//	TBX_CHAR							szInterfaceIp1[ 64 ];
//
//	/* Specific remote IP/Port to reach */
//	TBX_UINT32							un32NbExtraGwAddr;
//	TBX_LIB_IP_PORT_PAIR				aExtraGwAddr[ TBX_CLI_TOOLS_MAX_TBX_GW_REMOTE_ADDR ];
//
//	/* Network timeouts and configuration */
//	TBX_UINT32							un32PollDelayMs;
//	TBX_UINT32							un32NetworkDownDelayMs;
//	TBX_UINT32							un32AdapterDownDelayMs;
//	TBX_LIB_FLOW_CONTROL_MODE			FlowControlMode;
//
//} TBX_CLI_TOOLS_CONFIG_NETWORK_PARAMS, *PTBX_CLI_TOOLS_CONFIG_NETWORK_PARAMS;


/* CLI configuration structure read from config file through TbxCliToolsCliConfigParseLine */
typedef struct _TBX_CLI_TOOLS_CONFIG_PARAMS
{
	/* Parameters related to log file */
	struct
	{
		TBX_UINT32							un32Reserved;
		TRACE_LEVEL							TraceLevelStdout;
		TRACE_LEVEL							TraceLevelLogFile;
		TBX_CHAR							szLogFileName[ 256 ];
		TBX_UINT32							un32MaxLogFileSize;
		TBX_UINT32							un32MaxLogFilesPerDay;
		TBX_BOOL							fLogKeepColorCodes;
	} LogFileConfig;

	/* Parameters related to on-screen display */
	struct
	{
		TBX_UINT32							un32Reserved;
		TBX_BOOL							fDisableTerminalInput;
		TBX_BOOL							fDisableTerminalOutput;
		TBX_UINT32							un32LogBufferMaxLines;
		TBX_UINT32							un32MinRefreshDelay;
		TBX_BOOL							fLowPrioryThread;
	} CliConfig;

} TBX_CLI_TOOLS_CONFIG_PARAMS, *PTBX_CLI_TOOLS_CONFIG_PARAMS;

typedef struct _TBX_CLI_TOOLS_CUSTOM_CHECKPOINT_PARAMS
{
	/* String to print with the checkpoint */
	PTBX_CHAR								pszCheckpointString;

	/* Number of lines, up from current line after printing pszCheckpointString,
	   where the checkpoint should apply */
	TBX_UINT32								un32LinesOffset;
} TBX_CLI_TOOLS_CUSTOM_CHECKPOINT_PARAMS, *PTBX_CLI_TOOLS_CUSTOM_CHECKPOINT_PARAMS;

/*--------------------------------------------------------------------------------------------------------------------------------
 |  Function Prototypes
 *------------------------------------------------------------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------------------------------------------------------------
 |
 |  TbxCliToolsInit					:	Initialize the TBX CLI Tool library.
 |
 |	in_pParams						:	Parameters for this library
 |	out_phCliToolsHandle			:	Returns the handle of the allocated TBX CLI library
 |
 |  Note							:	It is mandatory to call TbxCliToolsStart() after
 |										TbxCliToolsInit to make the library in function.
 |
 |  Return							:	TBX_RESULT_OK if succeeded. Other error code otherwise.
 |
 *------------------------------------------------------------------------------------------------------------------------------*/
TBX_RESULT TbxCliToolsInit
(
	IN		PTBX_CLI_TOOLS_INIT_PARAMS				in_pParams,
	IN		PTBX_CLI_TOOLS_HANDLE					out_phCliToolsHandle
);

/*-------------------------------------------------------------------------------------------------------------------------------
 |
 |  TbxCliToolsStart				:	Start the command-line thread.
 |
 |	in_hCliToolsHandle				:	Handle of the allocated TBX CLI library
 |
 |  Note							:	~
 |
 |  Return							:	TBX_RESULT_OK if succeeded. Other error code otherwise.
 |
 *------------------------------------------------------------------------------------------------------------------------------*/
TBX_RESULT TbxCliToolsStart
(
	IN		TBX_CLI_TOOLS_HANDLE					in_hCliToolsHandle
);

/*-------------------------------------------------------------------------------------------------------------------------------
 |
 |  TbxCliToolsStop					:	Stop the command-line thread.
 |
 |	in_hCliToolsHandle				:	Handle of the allocated TBX CLI library
 |
 |  Note							:	~
 |
 |  Return							:	TBX_RESULT_OK if succeeded. Other error code otherwise.
 |
 *------------------------------------------------------------------------------------------------------------------------------*/
TBX_RESULT TbxCliToolsStop
(
	IN		TBX_CLI_TOOLS_HANDLE					in_hCliToolsHandle
);

/*-------------------------------------------------------------------------------------------------------------------------------
 |
 |  TbxCliToolsTerm					:	Un-initializes the TBX CLI Tool library.
 |
 |	in_hCliToolsHandle				:	Handle of the allocated TBX CLI library
 |
 |  Note							:	~
 |
 |  Return							:	TBX_RESULT_OK if succeeded. Other error code otherwise.
 |
 *------------------------------------------------------------------------------------------------------------------------------*/
TBX_RESULT TbxCliToolsTerm
(
	IN		TBX_CLI_TOOLS_HANDLE					in_hCliToolsHandle
);

/*-------------------------------------------------------------------------------------------------------------------------------
 |
 |  TbxCliToolsGetParams			:	Get the current TBX CLI Library parameters.
 |
 |	in_hCliToolsHandle				:	Handle of the allocated TBX CLI library
 |	out_pParams						:	Returns current parameters
 |	out_pun32CurrentWidth			:	Current CLI width (in number of characters)
 |	out_pun32CurrentHeight			:	Current CLI height (in number of lines)
 |
 |  Note							:	~
 |
 |  Return							:	TBX_RESULT_OK if succeeded. Other error code otherwise.
 |
 *------------------------------------------------------------------------------------------------------------------------------*/
TBX_RESULT TbxCliToolsGetParams
(
	IN		TBX_CLI_TOOLS_HANDLE					in_hCliToolsHandle,
	OUT		PTBX_CLI_TOOLS_INIT_PARAMS				out_pParams,
	OUT		PTBX_UINT32								out_pun32CurrentWidth,
	OUT		PTBX_UINT32								out_pun32CurrentHeight
);


/*-------------------------------------------------------------------------------------------------------------------------------
 |
 |  TbxCliToolsSetParams			:	Changes the TBX CLI Library parameters.
 |
 |	in_hCliToolsHandle				:	Handle of the allocated TBX CLI library
 |	in_pParams						:	New parameters to set
 |
 |  Note							:	Some parameters can't be changed live
 |
 |  Return							:	TBX_RESULT_OK if succeeded. Other error code otherwise.
 |
 *------------------------------------------------------------------------------------------------------------------------------*/
TBX_RESULT TbxCliToolsSetParams
(
	IN		TBX_CLI_TOOLS_HANDLE					in_hCliToolsHandle,
	IN		PTBX_CLI_TOOLS_INIT_PARAMS				in_pParams
);

/*-------------------------------------------------------------------------------------------------------------------------------
 |
 |  TbxCliToolsValidateParams		:	Validates the TBX CLI Library parameters.
 |
 |	in_hCliToolsHandle				:	Handle of the allocated TBX CLI library (used only to print errors)
 |	in_pParams						:	New parameters to validate
 |
 |  Note							:
 |
 |  Return							:	TBX_RESULT_OK if succeeded. Other error code otherwise.
 |
 *------------------------------------------------------------------------------------------------------------------------------*/
TBX_RESULT TbxCliToolsValidateParams
(
	IN		TBX_CLI_TOOLS_HANDLE					in_hCliToolsHandle,
	IN		PTBX_CLI_TOOLS_INIT_PARAMS				in_pParams
);

/*-------------------------------------------------------------------------------------------------------------------------------
 |
 |  TbxCliToolsDefaultKeysHandler	:	Function that handles default keys common to all applications
 |										that use tbx_cli_tools library.
 |										The application should call this function in it's
 |										pFctHandleMenuChoice callback, unless it wants to override the
 |										default behavior of default keys.
 |
 |
 |	in_hCliToolsHandle				:	Handle of the allocated TBX CLI library
 |  in_KeyPressed					:	Key that was pressed
 |
 |  Note							:	~
 |
 |  Return							:	TBX_TRUE if the key was handled, TBX_FALSE otherwise
 |
 *------------------------------------------------------------------------------------------------------------------------------*/
TBX_BOOL TbxCliToolsDefaultKeysHandler
(
	IN		TBX_CLI_TOOLS_HANDLE					in_hCliToolsHandle,
	IN		TBX_INT									in_KeyPressed
);

/*-------------------------------------------------------------------------------------------------------------------------------
 |
 |  TbxCliToolsPutKey		:	Function that 'passes' a key to the library that will simulate
 |								that this key was read from the standard input.
 |
 |
 |	in_hCliToolsHandle		:	Handle of the allocated TBX CLI library
 |  in_KeyPressed			:	Key to force TBX CLI library to process
 |
 |  Note					:	~
 |
 |  Return					:	~
 |
 *------------------------------------------------------------------------------------------------------------------------------*/
TBX_RESULT TbxCliToolsPutKey
(
	IN		TBX_CLI_TOOLS_HANDLE					in_hCliToolsHandle,
	IN		TBX_INT									in_KeyPressed
);

/*-------------------------------------------------------------------------------------------------------------------------------
 |
 |  TbxCliToolsNeedRedraw			:	Tells the TBX CLI Tools library that a screen redraw is required.
 |
 |	in_hCliToolsHandle				:	Handle of the allocated TBX CLI library
 |
 |  Note							:	Some parameters can't be changed live
 |
 |  Return							:	~
 |
 *------------------------------------------------------------------------------------------------------------------------------*/
TBX_VOID TbxCliToolsNeedRedraw
(
	IN		TBX_CLI_TOOLS_HANDLE					in_hCliToolsHandle
);


/*-------------------------------------------------------------------------------------------------------------------------------
 |
 |  TbxCliToolsNeedCls				:	Tells the TBX CLI Tools library that a clear screen is required.
 |
 |	in_hCliToolsHandle				:	Handle of the allocated TBX CLI library
 |
 |  Note							:	Some parameters can't be changed live
 |
 |  Return							:	~
 |
 *------------------------------------------------------------------------------------------------------------------------------*/
TBX_VOID TbxCliToolsNeedCls
(
	IN		TBX_CLI_TOOLS_HANDLE					in_hCliToolsHandle
);

/*-------------------------------------------------------------------------------------------------------------------------------
 |
 |  TbxCliToolsLogClearBuffer	:	Tells the TBX CLI Tools library to clear buffer log
 |
 |	in_hCliToolsHandle			:	Handle of the allocated TBX CLI library
 |
 |  Note							:	~
 |
 |  Return							:	TBX_RESULT_OK
 |										TBX_RESULT_FAIL
 |
 *------------------------------------------------------------------------------------------------------------------------------*/
TBX_VOID TbxCliToolsLogClearBuffer
(
	IN		TBX_CLI_TOOLS_HANDLE					in_hCliToolsHandle
);


/*-------------------------------------------------------------------------------------------------------------------------------
 |
 |  TbxCliToolsLogScroll			:	This function scrolls up/down the log in the log window
 |
 |	in_hCliToolsHandle				:	Handle of the allocated TBX CLI library
 |	in_fUp							:	Scroll UP or Down
 |	in_un32Nb						:	Number of lines to scroll.
 |											TBX_CLI_TOOLS_SCROLL_END	=> Scroll to end
 |											TBX_CLI_TOOLS_SCROLL_PAGE	=> Scroll one "page"
 |
 |  Note							:	~
 |
 |  Return							:	TBX_RESULT_OK
 |										TBX_RESULT_FAIL
 |
 *------------------------------------------------------------------------------------------------------------------------------*/
TBX_VOID TbxCliToolsLogScroll
(
	IN		TBX_CLI_TOOLS_HANDLE					in_hCliToolsHandle,
	IN		TBX_BOOL								in_fUp,
	IN		TBX_UINT32								in_un32Nb
);

/*-------------------------------------------------------------------------------------------------------------------------------
 |
 |  TbxCliToolsLogScrollHorizontal	:	This function scrolls left/right the log in the log window
 |
 |	in_hCliToolsHandle				:	Handle of the allocated TBX CLI library
 |	in_fLeft						:	Scroll Left or right
 |	in_un32Nb						:	Number of characters to scroll.
 |
 |  Note							:	~
 |
 |  Return							:	TBX_RESULT_OK
 |										TBX_RESULT_FAIL
 |
 *------------------------------------------------------------------------------------------------------------------------------*/
TBX_VOID TbxCliToolsLogScrollHorizontal
(
	IN		TBX_CLI_TOOLS_HANDLE					in_hCliToolsHandle,
	IN		TBX_BOOL								in_fLeft,
	IN		TBX_UINT32								in_un32Nb
);

/*-------------------------------------------------------------------------------------------------------------------------------
 |
 |  TbxCliToolsLogSetCheckpoint		:	This function set a log checkpoint to the current
 |										log position.
 |
 |	in_hCliToolsHandle				:	Handle of the allocated TBX CLI library
 |
 |  Note							:	~
 |
 |  Return							:	TBX_RESULT_OK
 |										TBX_RESULT_FAIL
 |
 *------------------------------------------------------------------------------------------------------------------------------*/
TBX_VOID TbxCliToolsLogSetCheckpoint
(
	IN		TBX_CLI_TOOLS_HANDLE					in_hCliToolsHandle
);


/*-------------------------------------------------------------------------------------------------------------------------------
 |
 |  TbxCliToolsLogSetCustomCheckpoint	:	This function set a log checkpoint to the current
 |											log position.
 |
 |	in_hCliToolsHandle					:	Handle of the allocated TBX CLI library
 |	in_pCustomCheckpointParams			:	Custom parameters of this checkpoint
 |
 |  Note								:	~
 |
 |  Return								:	TBX_RESULT_OK
 |											TBX_RESULT_FAIL
 |
 *------------------------------------------------------------------------------------------------------------------------------*/
TBX_VOID TbxCliToolsLogSetCustomCheckpoint
(
	IN		TBX_CLI_TOOLS_HANDLE					in_hCliToolsHandle,
	IN		PTBX_CLI_TOOLS_CUSTOM_CHECKPOINT_PARAMS	in_pCustomCheckpointParams
);


/*-------------------------------------------------------------------------------------------------------------------------------
 |
 |  TbxCliToolsLogGotoCheckpoint	:	This function go to the next/previous checkpoint from current position.
 |
 |	in_hCliToolsHandle		:	Handle of the allocated TBX CLI library
 |  in_fNext						:	Get next checkpoint (else previous)
 |
 |  Note							:	~
 |
 |  Return							:	TBX_RESULT_OK
 |										TBX_RESULT_FAIL
 |
 *------------------------------------------------------------------------------------------------------------------------------*/
TBX_VOID TbxCliToolsLogGotoCheckpoint
(
	IN		TBX_CLI_TOOLS_HANDLE					in_hCliToolsHandle,
	IN		TBX_BOOL								in_fNext
);

/*-------------------------------------------------------------------------------------------------------------------------------
 |
 |  TbxCliToolsLogGotoString		:	This function moves to log to the next/prev occurence
 |										of the specified string.
 |
 |	in_pszToFind					:	String to find
 |  in_fNext						:	Get next checkpoint (else previous)
 |										to select line to blink.
 |  fFindFromFirstCharOnly			:	Match only the lines that start with the specified string.
 |
 |  Note							:	~
 |
 |  Return							:	TBX_RESULT_OK
 |										TBX_RESULT_FAIL
 |
 *------------------------------------------------------------------------------------------------------------------------------*/
TBX_VOID TbxCliToolsLogGotoString
(
	IN		TBX_CLI_TOOLS_HANDLE					in_hCliToolsHandle,
	IN		const TBX_CHAR*							in_pszToFind,
	IN		TBX_BOOL								in_fNext,
	IN		TBX_BOOL								in_fFindFromFirstCharOnly
);


/*-------------------------------------------------------------------------------------------------------------------------------
 |
 |  TbxCliToolsPrint		:	Print to screen buffer.
 |
 |
 |	in_hCliToolsHandle		:	Handle of the allocated TBX CLI library
 |  in_pszString			:	Format for print (same as fprintf)
 |	...						:	Variable number of arguments for specified format (same as fprintf)
 |
 |  Note					:	WARNING: This function is allowed only within the callback pFctDisplayMenu.
 |										 If called any other time, an error will be displayed to the log.
 |
 |  Return					:	~
 |
 *------------------------------------------------------------------------------------------------------------------------------*/
void TbxCliToolsPrint
(
	IN		TBX_CLI_TOOLS_HANDLE					in_hCliToolsHandle,
	IN		PTBX_CHAR								in_pszString,
	...
);
/* Other variant of TbxCliToolsPrint */
void TbxCliToolsvPrint
(
	IN		TBX_CLI_TOOLS_HANDLE					in_hCliToolsHandle,
	IN		PTBX_CHAR								in_pszString,
	IN		va_list									in_Marker
);

/*-------------------------------------------------------------------------------------------------------------------------------
 |
 |  TbxCliToolsPrintFillLine:	Fill a line with the specified character.
 |
 |
 |	in_hCliToolsHandle		:	Handle of the allocated TBX CLI library
 |  in_cFillChar			:	Character to fill with
 |
 |  Note					:	WARNING: This function is allowed only within the callback pFctDisplayMenu.
 |										 If called any other time, an error will be displayed to the log.
 |
 |  Return					:	~
 |
 *------------------------------------------------------------------------------------------------------------------------------*/
void TbxCliToolsPrintFillLine
(
	IN		TBX_CLI_TOOLS_HANDLE					in_hCliToolsHandle,
	IN		TBX_CHAR								in_cFillChar
);

/*-------------------------------------------------------------------------------------------------------------------------------
 |
 |  TbxCliToolsBlinkLine	:	Make current line blink.
 |
 |
 |	in_hCliToolsHandle		:	Handle of the allocated TBX CLI library
 |
 |  Note					:	WARNING: This function is allowed only within the callback pFctDisplayMenu.
 |										 If called any other time, an error will be displayed to the log.
 |
 |  Return					:	~
 |
 *------------------------------------------------------------------------------------------------------------------------------*/
void TbxCliToolsBlinkLine
(
	IN		TBX_CLI_TOOLS_HANDLE					in_hCliToolsHandle
);

/*-------------------------------------------------------------------------------------------------------------------------------
 |
 |  TbxCliToolsLineWrap		:	Set the line wrap option (on by default, this function is used to change it).
 |
 |
 |	in_hCliToolsHandle		:	Handle of the allocated TBX CLI library
 |	in_fLineWrap			:	TBX_FALSE -> No line wrap. Otherwise, line wrap is used based on screen width.
 |
 |  Note					:	~
 |
 |  Return					:	Previous "line wrap" value
 |
 *------------------------------------------------------------------------------------------------------------------------------*/
TBX_BOOL TbxCliToolsLineWrap
(
	IN		TBX_CLI_TOOLS_HANDLE					in_hCliToolsHandle,
	IN		TBX_BOOL								in_fLineWrap
);

/*-------------------------------------------------------------------------------------------------------------------------------
 |
 |  TbxCliToolsGetRemainingLines	:	Get the current number of lines remaining in current screen.
 |
 |	in_hCliToolsHandle				:	Handle of the allocated TBX CLI library
 |
 |  Note							:	WARNING: This function is allowed only within the callback pFctDisplayMenu.
 |												 If called any other time, an error will be displayed to the log.
 |
 |  Return							:	Number of lines remaining in current screen.
 |
 *------------------------------------------------------------------------------------------------------------------------------*/
TBX_UINT32 TbxCliToolsGetRemainingLines
(
	IN		TBX_CLI_TOOLS_HANDLE					in_hCliToolsHandle
);

/*-------------------------------------------------------------------------------------------------------------------------------
 |
 |  TbxCliToolsGetRemainingChars	:	Get the current number of characters that can be printed on current line
 |
 |	in_hCliToolsHandle				:	Handle of the allocated TBX CLI library
 |
 |  Note							:	WARNING: This function is allowed only within the callback pFctDisplayMenu.
 |												 If called any other time, an error will be displayed to the log.
 |
 |  Return							:	Number of characters remaining in current line before reaching screen width
 |
 *------------------------------------------------------------------------------------------------------------------------------*/
TBX_UINT32 TbxCliToolsGetRemainingChars
(
	IN		TBX_CLI_TOOLS_HANDLE					in_hCliToolsHandle
);


/*-------------------------------------------------------------------------------------------------------------------------------
 |
 |  TbxCliToolsIsSomeoneWatching	:	Indicates if someone is actually "watching" the application's screen
 |										(application is in foreground, or background but remotely controlled).
 |
 |	in_hCliToolsHandle				:	Handle of the allocated TBX CLI library
 |
 |  Note							:	~
 |
 |  Return							:	TBX_TRUE if application is being watched, TBX_FALSE otherwise.
 |
 *------------------------------------------------------------------------------------------------------------------------------*/
TBX_BOOL TbxCliToolsIsSomeoneWatching
(
	IN		TBX_CLI_TOOLS_HANDLE					in_hCliToolsHandle
);

/*-------------------------------------------------------------------------------------------------------------------------------
 |
 |  TbxCliToolsLogPrint		:	Format the specified string to log file and log buffer.
 |
 |
 |	in_hCliToolsHandle		:	Handle of the allocated TBX CLI library
 |  in_TraceLevel			:	Trace level for this print
 |	in_pszColor				:	Color to display (see FBLUE and others in tbx_cli.h)
 |  in_pszString			:	Format for print (same as fprintf)
 |	...						:	Variable number of arguments for specified format (same as fprintf)
 |
 |  Note					:	~
 |
 |  Return					:	~
 |
 *------------------------------------------------------------------------------------------------------------------------------*/
void TbxCliToolsLogPrint
(
	IN		TBX_CLI_TOOLS_HANDLE					in_hCliToolsHandle,
	IN		TRACE_LEVEL								in_TraceLevel,
	IN		const TBX_CHAR*							in_pszColor,
	IN		const TBX_CHAR*							in_pszString,
	...
);
/* Other variant of TbxCliToolsLogPrint */
void TbxCliToolsLogvPrint
(
	IN		TBX_CLI_TOOLS_HANDLE					in_hCliToolsHandle,
	IN		TRACE_LEVEL								in_TraceLevel,
	IN		const TBX_CHAR*							in_pszColor,
	IN		const TBX_CHAR*							in_pszString,
	IN		va_list									in_Marker
);

/*-------------------------------------------------------------------------------------------------------------------------------
 |
 |  TbxCliToolsLogPrintLock		:	Lock the log print for atomic prints.
 |
 |	in_hCliToolsHandle			:	Handle of the allocated TBX CLI library
 |
 |  Note						:	~
 |
 |  Return						:	~
 |
 *------------------------------------------------------------------------------------------------------------------------------*/
void TbxCliToolsLogPrintLock
(
	IN		TBX_CLI_TOOLS_HANDLE					in_hCliToolsHandle
);

/*-------------------------------------------------------------------------------------------------------------------------------
 |
 |  TbxCliToolsLogPrintUnlock	:	Unlock the log print for atomic prints.
 |
 |	in_hCliToolsHandle			:	Handle of the allocated TBX CLI library
 |
 |  Note						:	~
 |
 |  Return						:	~
 |
 *------------------------------------------------------------------------------------------------------------------------------*/
void TbxCliToolsLogPrintUnlock
(
	IN		TBX_CLI_TOOLS_HANDLE					in_hCliToolsHandle
);


/*-------------------------------------------------------------------------------------------------------------------------------
 |
 |  TbxCliToolsLogFileSplit		:	Splits the log file now so it can be viewed
 |									(current process closes the file so it can be opened by other process).
 |
 |	in_hCliToolsHandle			:	Handle of the allocated TBX CLI library
 |
 |  Note						:	~
 |
 |  Return						:	~
 |
 *------------------------------------------------------------------------------------------------------------------------------*/
void TbxCliToolsLogFileSplit
(
	IN		TBX_CLI_TOOLS_HANDLE					in_hCliToolsHandle
);

/*-------------------------------------------------------------------------------------------------------------------------------
 |
 |  TbxCliToolsGetLogFilePath	:	Returns the actual log file path used.
 |
 |	in_hCliToolsHandle			:	Handle of the allocated TBX CLI library
 |	out_szLogFilePath			:	Returns current log file path
 |
 |  Note						:	~
 |
 |  Return						:	~
 |
 *------------------------------------------------------------------------------------------------------------------------------*/
TBX_RESULT TbxCliToolsGetLogFilePath
(
	IN		TBX_CLI_TOOLS_HANDLE					in_hCliToolsHandle,
	OUT		TBX_CHAR								out_szLogFilePath[ 256 ]
);

/*-------------------------------------------------------------------------------------------------------------------------------
 |
 |  TbxCliToolsGetTimestamp		:	Returns current timestamp.
 |
 |	out_pTimestamp				:	Timestamp returned
 |
 |  Note						:	~
 |
 |  Return						:	TBX_RESULT_OK
 |									TBX_RESULT_FAIL
 |
 *------------------------------------------------------------------------------------------------------------------------------*/
TBX_RESULT TbxCliToolsGetTimestamp
(
	IN		PTBX_CLI_TOOLS_TIMESTAMP				out_pTimestamp
);

/*-------------------------------------------------------------------------------------------------------------------------------
 |
 |  TbxCliToolsGetTimestampMs	:	Returns current timestamp in milliseconds since epoch.
 |
 |  Note						:	~
 |
 |  Return						:	Timestamp in milliseconds since epoch
 |
 *------------------------------------------------------------------------------------------------------------------------------*/
TBX_UINT64 TbxCliToolsGetTimestampMs(void);

/*-------------------------------------------------------------------------------------------------------------------------------
 |
 |  TbxCliToolsGetElapsedTime	:	Returns the time difference in ms between two timestamps.
 |
 |	in_pTimestamp1				:	First timestamp
 |	in_pTimestamp2				:	Second timestamp
 |	out_pun32ElapsedTimeInMs	:	The difference in ms between the two timestamps
 |
 |  Note						:	~
 |
 |  Return						:	TBX_RESULT_OK
 |									TBX_RESULT_FAIL
 |
 *------------------------------------------------------------------------------------------------------------------------------*/
TBX_RESULT TbxCliToolsGetElapsedTime
(
	IN		PTBX_CLI_TOOLS_TIMESTAMP				in_pTimestamp1,
	IN		PTBX_CLI_TOOLS_TIMESTAMP				in_pTimestamp2,
	IN		PTBX_UINT32								out_pun32ElapsedTimeInMs
);

/*-------------------------------------------------------------------------------------------------------------------------------
 |
 |  TbxCliToolsFormatTime		:	Format a timestamp as a string.
 |
 |	in_pTimestamp				:	Timestamp to format
 |	out_pszBuffer				:	Buffer to return formatted timestamp
 |	in_un32BufferSize			:	Size of the buffer out_pszBuffer
 |
 |  Note						:	~
 |
 |  Return						:	~
 |
 *------------------------------------------------------------------------------------------------------------------------------*/
TBX_VOID TbxCliToolsFormatTime
(
	IN		PTBX_CLI_TOOLS_TIMESTAMP				in_pTimestamp,
	IN		PTBX_CHAR								out_pszBuffer,
	IN		TBX_UINT32								in_un32BufferSize
);


/*-------------------------------------------------------------------------------------------------------------------------------
 |
 |  TbxCliToolsGetOptionListSelected	:	Get the element selected from an option list.
 |
 |	in_pszLine							:	String entered by user, that may contain option list
 |	out_pszValue						:	Extracted value
 |
 |  Note								:	~
 |									
 |  Return								:	~
 |
 *------------------------------------------------------------------------------------------------------------------------------*/
TBX_VOID TbxCliToolsGetOptionListSelected
( 
	IN		TBX_CHAR					in_pszLine[ TBX_CLI_TOOLS_CLI_LINE_WIDTH_MAX ],
	OUT		TBX_CHAR					out_pszValue[ TBX_CLI_TOOLS_CLI_LINE_WIDTH_MAX ]
);

/*--------------------------------------------------------------------------------------------------------------------------------
 |  Function prototypes for remote controlling tbx_cli_tools applications
 *------------------------------------------------------------------------------------------------------------------------------*/

 /*-------------------------------------------------------------------------------------------------------------------------------
 |
 |  TbxCliToolsGetRemoteScreensList	:	Returns the list of remote hosts which we are
 |										actually receiving remote screen updates.
 |
 |	in_hCliToolsHandle				:	Handle of the allocated TBX CLI library
 |	out_pun32NbRemoteHostScrens		:	Returns number of host handles provided in out_ahRemoteHostScreens
 |	out_ahRemoteHostScreens			:	Array to return handle of all remote hosts which we are
 |										actually receiving remote screen updates.
 |
 |  Note							:	~
 |
 |  Return							:	TBX_RESULT_OK if returned valid list, other error code otherwise
 |
 *------------------------------------------------------------------------------------------------------------------------------*/
//TBX_RESULT TbxCliToolsGetRemoteScreensList
//(
//	IN		TBX_CLI_TOOLS_HANDLE					in_hCliToolsHandle,
//	OUT		PTBX_UINT32								out_pun32NbRemoteHostScrens,
//	OUT		TBX_ADAPTER_HANDLE						out_ahRemoteHostScreens[ TBX_CLI_TOOLS_MAX_REMOTE_SCREEN_HOSTS ]
//);

 /*-------------------------------------------------------------------------------------------------------------------------------
 |
 |  TbxCliToolsGetSelectedRemoteScreens	:	Returns the actually selected remote screen.
 |
 |	in_hCliToolsHandle					:	Handle of the allocated TBX CLI library
 |										
 |	out_phRemoteHostScreen				:	Returns the handle of actually remotely controlled host.
 |											Will return NULL if host is disconnected, or if not controlling any remote host
 |	out_szRemoteScreenHostName			:	Returns the name of the remotely controlled application's host, or empty string if none.
 |	out_szRemoteScreenApplicationName	:	Returns the name of the remotely controlled application, or empty string if none.
 |
 |  Note								:	~
 |										
 |  Return								:	TBX_RESULT_OK if returned valid list, other error code otherwise
 |										
 *------------------------------------------------------------------------------------------------------------------------------*/
//TBX_RESULT TbxCliToolsGetSelectedRemoteScreens
//(
//	IN		TBX_CLI_TOOLS_HANDLE					in_hCliToolsHandle,
//	OUT		PTBX_ADAPTER_HANDLE						out_phRemoteHostScreen,
//	OUT		TBX_CHAR								out_szRemoteScreenHostName [TBX_API_NAME_MAX_LEN],
//	OUT		TBX_CHAR								out_szRemoteScreenApplicationName [TBX_API_NAME_MAX_LEN]
//);

 /*-------------------------------------------------------------------------------------------------------------------------------
 |
 |  TbxCliToolsSelectRemoteScreen	:	Select the application which we wish to remotely control now.
 |
 |	in_hCliToolsHandle				:	Handle of the allocated TBX CLI library
 |	in_hRemoteHostScreen			:	Host handle to control.
 |
 |  Note							:	~
 |
 |  Return							:	TBX_RESULT_OK if succeeded, other error code otherwise
 |
 *------------------------------------------------------------------------------------------------------------------------------*/
//TBX_RESULT TbxCliToolsSelectRemoteScreen
//(
//	IN		TBX_CLI_TOOLS_HANDLE					in_hCliToolsHandle,
//	IN		TBX_ADAPTER_HANDLE						in_hRemoteHostScreen
//);


/*--------------------------------------------------------------------------------------------------------------------------------
 |  Function prototypes of helpers to parse configuration files
 *------------------------------------------------------------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------------------------------------------------------------
 |
 |  TbxCliToolsNetworkConfigParseLine	:	The application passes to this function all lines,
 |											one by one, from it's configuration file, and this
 |											function will 'consume' and parse the portion of
 |											the configuration within ".NETWORK_CONFIG" section.
 |
 |	in_pszLine							:	Line to parse
 |	out_pNetworkConfig					:	Network configuration structure to build.
 |											Make sure to memset to 0 before first call to this function
 |	out_szErrorString					:	In case of error, returns a string that explains the error
 |
 |  Note								:	out_pNetworkConfig must be memset to 0 before first
 |											call to this function.
 |
 |  Return								:	TBX_TRUE if line consumed, TBX_FALSE if line ignored.
 |
 *------------------------------------------------------------------------------------------------------------------------------*/
//TBX_BOOL TbxCliToolsNetworkConfigParseLine
//(
//	IN		PTBX_CHAR								in_pszLine,
//	OUT		PTBX_CLI_TOOLS_CONFIG_NETWORK_PARAMS	out_pNetworkConfig,
//	OUT		TBX_CHAR								out_szErrorString[ 256 ]
//);

/*-------------------------------------------------------------------------------------------------------------------------------
 |
 |  TbxCliToolsNetworkConfigCopyToTbxLibParams	:	This function applies the network configuration parsed
 |													by TbxCliToolsParseNetworkConfigLine to the host library
 |													parameters.
 |
 |	in_pNetworkConfig							:	Network configuration to copy to host lib parameters
 |	io_pLibPrams								:	Library parameters to complete with content from
 |													in_pNetworkConfig.
 |													Please note that this does not entirely fills io_pLibPrams,
 |													caller is responsible to fill missing fields.
 |													Fields filled are:
 |														szApplicationName
 |														szInterfaceIp0
 |														szInterfaceIp1
 |														un16TbxGwPort
 |														aun16ExtraTbxGwPort
 |														un32NbExtraGwAddr
 |														paExtraGwAddr
 |														NetworkRedundancyParams
 |
 |  Note										:	io_pLibPrams->paExtraGwAddr will point to
 |													in_pNetworkConfig->paExtraGwAddr, so make sure not
 |													to delete in_pNetworkConfig before io_pLibPrams.
 |
 |  Return										:	~
 |
 *------------------------------------------------------------------------------------------------------------------------------*/
//TBX_VOID TbxCliToolsNetworkConfigCopyToTbxLibParams
//(
//	IN		PTBX_CLI_TOOLS_CONFIG_NETWORK_PARAMS	in_pNetworkConfig,
//	IN_OUT	PTBX_LIB_PARAMS							io_pLibPrams
//);


/*-------------------------------------------------------------------------------------------------------------------------------
 |
 |  TbxCliToolsCliConfigParseLine		:	The application passes to this function all lines,
 |											one by one, from it's configuration file, and this
 |											function will 'consume' and parse the portion of
 |											the configuration within
 |											".LOG_FILE_CONFIG" and ".CLI_CONFIG" sections.
 |
 |	in_pszLine							:	Line to parse
 |	out_pCliConfig						:	CLI configuration structure to build.
 |											Make sure to memset to 0 before first call to this function
 |	out_szErrorString					:	In case of error, returns a string that explains the error
 |
 |  Note								:	out_pCliConfig must be memset to 0 before first
 |											call to this function.
 |
 |  Return								:	TBX_TRUE if line consumed, TBX_FALSE if line ignored.
 |
 *------------------------------------------------------------------------------------------------------------------------------*/
TBX_BOOL TbxCliToolsCliConfigParseLine
(
	IN		PTBX_CHAR								in_pszLine,
	OUT		PTBX_CLI_TOOLS_CONFIG_PARAMS			out_pCliConfig,
	OUT		TBX_CHAR								out_szErrorString[ 256 ]
);

/*-------------------------------------------------------------------------------------------------------------------------------
 |
 |  TbxCliToolsCliConfigCopyToTbxLibParams		:	This function applies the network configuration parsed
 |													by TbxCliToolsCliConfigParseLine to the CLI Tools Library
 |													parameters.
 |
 |	in_pCliConfig								:	CLI configuration to copy to host lib parameters
 |	io_pInitPrams								:	CLI Tools Library Init parameters to complete with content from
 |													in_pCliConfig.
 |													Please note that this does not entirely fills in_pCliConfig,
 |													caller is responsible to fill missing fields.
 |													Fields filled are those found in TBX_CLI_TOOLS_CONFIG_NETWORK_PARAMS.
 |
 |  Note										:	~
 |
 |  Return										:	~
 |
 *------------------------------------------------------------------------------------------------------------------------------*/
TBX_VOID TbxCliToolsCliConfigCopyToTbxLibParams
(
	IN		PTBX_CLI_TOOLS_CONFIG_PARAMS			in_pCliConfig,
	IN_OUT	PTBX_CLI_TOOLS_INIT_PARAMS				io_pInitPrams
);


/*-------------------------------------------------------------------------------------------------------------------------------
 |
 |  TbxCliToolsAnyConfigParseLine		:	Calls TbxCliToolsNetworkConfigParseLine and TbxCliToolsCliConfigParseLine
 |											in case one of both can parse this line.
 |
 |	in_pszLine							:	Line to parse
 |	out_pNetworkConfig					:	Network configuration structure to build.
 |											Make sure to memset to 0 before first call to this function
 |	out_pCliConfig						:	CLI configuration structure to build.
 |											Make sure to memset to 0 before first call to this function
 |	out_szErrorString					:	In case of error, returns a string that explains the error
 |
 |  Note								:	out_pNetworkConfig and out_pCliConfig must be memset to 0
 |											before first call to this function.
 |
 |  Return								:	TBX_TRUE if line consumed, TBX_FALSE if line ignored.
 |
 *------------------------------------------------------------------------------------------------------------------------------*/
//TBX_BOOL TbxCliToolsAnyConfigParseLine
//(
//	IN		PTBX_CHAR								in_pszLine,
//	OUT		PTBX_CLI_TOOLS_CONFIG_NETWORK_PARAMS	out_pNetworkConfig,
//	OUT		PTBX_CLI_TOOLS_CONFIG_PARAMS			out_pCliConfig,
//	OUT		TBX_CHAR								out_szErrorString[ 256 ]
//);




/*--------------------------------------------------------------------------------------------------------------------------------
 |  Macros
 *------------------------------------------------------------------------------------------------------------------------------*/

#define TBX_CLI_TOOLS_GET_KEY_CODE( _KeyDesc )		( _KeyDesc & TBX_CLI_TOOLS_KEY_CODE_MASK )
#define TBX_CLI_TOOLS_GET_KEY_MODIFIERS( _KeyDesc )	( _KeyDesc & TBX_CLI_TOOLS_KEY_MODIFIERs_MASK )

#ifdef WIN32
	#define vsnprintf _vsnprintf
	#define snprintf _snprintf
#endif


/*-------------------------------------------------------------------------------------------------------------------------------
 |
 |  TbxCliTools_vsnprintf		:	A version of vsnprintf that does not return -1
 |									when it overflows. Instead, it always returns
 |									the exact number of characters written in the string.
 |									It also always null-terminates the string
 |
 |	Arguments					:	Same as vsnprintf
 |
 |  Note						:	~
 |
 |  Return						:	~
 |
 *------------------------------------------------------------------------------------------------------------------------------*/

static __inline TBX_UINT32 TbxCliTools_vsnprintf
(
	OUT		PTBX_CHAR		out_pszOutString,
	IN		TBX_UINT32		in_un32BufLen,
	IN		const TBX_CHAR*	in_pszFormat,
	IN		va_list			in_Marker
)
{
	TBX_UINT32	un32Len;
	int			iLen;

	iLen = vsnprintf( out_pszOutString, in_un32BufLen, in_pszFormat, in_Marker );

	if( iLen < 0 )
		un32Len = (in_un32BufLen - 1);
	else if( (TBX_UINT32)iLen >= in_un32BufLen )
		un32Len = (in_un32BufLen - 1);
	else
		un32Len = (TBX_UINT32)iLen;

	/* Force NULL-termination */
	out_pszOutString[ in_un32BufLen - 1 ] = '\0';

	return un32Len;
}

/*-------------------------------------------------------------------------------------------------------------------------------
 |
 |  TbxCliTools_snprintf		:	A version of snprintf that does not return -1
 |									when it overflows. Instead, it always returns
 |									the exact number of characters written in the string.
 |									It also always null-terminates the string
 |
 |	Arguments					:	Same as snprintf
 |
 |  Note						:	~
 |
 |  Return						:	~
 |
 *------------------------------------------------------------------------------------------------------------------------------*/
static __inline TBX_UINT32 TbxCliTools_snprintf
(
	OUT		PTBX_CHAR		out_pszOutString,
	IN		TBX_UINT32		in_un32BufLen,
	IN		const TBX_CHAR*	in_pszFormat,
	...
)
{
	TBX_UINT32	un32Len;
	va_list		marker;

	va_start (marker, in_pszFormat);

	un32Len = TbxCliTools_vsnprintf( out_pszOutString, in_un32BufLen, in_pszFormat, marker );

	va_end (marker);

	return un32Len;
}


/*-------------------------------------------------------------------------------------------------------------------------------
 |
 |  TbxCliTools_stricmp			:	Case insensitive string compare
 |
 |	s1							:	First string to compare
 |	s2							:	Second string to compare
 |
 |  Note						:	~
 |
 |  Return						:	0 if equal, != 0 otherwise
 |
 *------------------------------------------------------------------------------------------------------------------------------*/
static __inline int TbxCliTools_stricmp
(
	IN const		char *s1,
	IN const		char *s2
)
{
	int		idx = 0;
	int		diff = 0;
	while ((s1[idx] != 0) && (s2[idx] != 0) && (diff == 0))
	{
		diff = toupper(s1[idx]) - toupper(s2[idx]);
		idx++;
	}
	if (diff == 0)
	{
		if (s1[idx] != 0)
			return 1;
		else if (s2[idx] != 0)
			return -1;
	}
	return diff;
}

/*-------------------------------------------------------------------------------------------------------------------------------
 |
 |  TbxCliTools_strincmp			:	Case insensitive string compare
 |
 |	s1							:	First string to compare
 |	s2							:	Second string to compare
 |	len							:	Max length to compare
 |
 |  Note						:	~
 |
 |  Return						:	0 if equal, != 0 otherwise
 |
 *------------------------------------------------------------------------------------------------------------------------------*/
static __inline int TbxCliTools_strincmp
(
	IN const		char *s1,
	IN const		char *s2,
	IN const		int len
)
{
	int		idx = 0;
	int		diff = 0;
	int		remain = len;
	while ((s1[idx] != 0) && (s2[idx] != 0) && (diff == 0) && (remain-- != 0))
	{
		diff = toupper(s1[idx]) - toupper(s2[idx]);
		idx++;
	}
	if (diff == 0)
	{
		if( len != 0 )
		{
			if( idx == len )
				return 0;	/* Compared all bytes requested */
			else if (s1[idx] != 0)
				return 1;
			else if (s2[idx] != 0)
				return -1;
		}
	}
	return diff;
}



/*-------------------------------------------------------------------------------------------------------------------------------
 |
 |  TBXCliToolsValidatePrintfFormat	:	Validate that the arguments for fprintf are valid,
 |										including NULL pointers passed to %s, or other similar errors.
 |										This can avoid crashes due to bad traces!
 |
 |	format							:	Format of the trace (same as first argument of printf)
 |	args							:	Argument list for format
 |
 |  Note							:	~
 |									
 |  Return							:	0 if equal, != 0 otherwise
 |
 *------------------------------------------------------------------------------------------------------------------------------*/
int TBXCliToolsValidatePrintfFormat
( 
	IN		const char*			format,
	IN		va_list				args
);

/* Function to convert key to string */
static __inline TBX_VOID TBX_CLI_TOOLS_KEY_TO_STRING
(
	IN		TBX_INT					in_KeyPressed,
	OUT		TBX_CHAR				out_szString[ 64 ]
)
{
	TBX_INT	KeyPressedNoModifier = in_KeyPressed;

	KeyPressedNoModifier &= ~TBX_CLI_TOOLS_KEY_MODIFIERs_MASK;
	KeyPressedNoModifier &= ~TBX_CLI_TOOLS_KEY_OTHER_INFO_MASK;

	/* Print modifiers */
	sprintf
	(
		out_szString,
		"%s%s%s",
		(in_KeyPressed & TBX_CLI_TOOLS_KEY_CTRL_MODIFIER_MASK) ? "Control-" : "",
		(in_KeyPressed & TBX_CLI_TOOLS_KEY_SHIFT_MODIFIER_MASK) ? "Shift-" : "",
		(in_KeyPressed & TBX_CLI_TOOLS_KEY_ALT_MODIFIER_MASK) ? "Alt-" : ""
	);

	switch( KeyPressedNoModifier )
	{
		case TBX_CLI_TOOLS_KEY_UP_ARROW_KEY:		strcat( out_szString, "Up-arrow" ); break;
		case TBX_CLI_TOOLS_KEY_DOWN_ARROW_KEY:		strcat( out_szString, "Down-arrow" ); break;
		case TBX_CLI_TOOLS_KEY_LEFT_ARROW_KEY:		strcat( out_szString, "Left-arrow" ); break;
		case TBX_CLI_TOOLS_KEY_RIGHT_ARROW_KEY:		strcat( out_szString, "Right-arrow" ); break;
		case TBX_CLI_TOOLS_KEY_PAGE_UP_KEY:			strcat( out_szString, "Page-up" ); break;
		case TBX_CLI_TOOLS_KEY_PAGE_DOWN_KEY:		strcat( out_szString, "Page-down" ); break;
		case TBX_CLI_TOOLS_KEY_HOME_KEY:			strcat( out_szString, "Home" ); break;
		case TBX_CLI_TOOLS_KEY_END_KEY:				strcat( out_szString, "End" ); break;
		case TBX_CLI_TOOLS_KEY_ESCAPE:				strcat( out_szString, "Escape" ); break;
		case TBX_CLI_TOOLS_KEY_CONTROL_C:			strcat( out_szString, "Control-C" ); break;
		case TBX_CLI_TOOLS_KEY_BACKSPACE:			strcat( out_szString, "Backspace" ); break;
		case TBX_CLI_TOOLS_KEY_INSERT:				strcat( out_szString, "Insert" ); break;
		case TBX_CLI_TOOLS_KEY_DELETE:				strcat( out_szString, "Delete" ); break;
		case TBX_CLI_TOOLS_KEY_PRINTSCREEN:			strcat( out_szString, "Print-screen" ); break;
		case TBX_CLI_TOOLS_KEY_WINDOWS_KEY:			strcat( out_szString, "Windows-key" ); break;
		case TBX_CLI_TOOLS_KEY_MOUSE_WHEEL_UP:		strcat( out_szString, "Mouse-wheel-up" ); break;
		case TBX_CLI_TOOLS_KEY_MOUSE_WHEEL_DOWN:	strcat( out_szString, "Mouse-wheel-down" ); break;
		case TBX_CLI_TOOLS_KEY_F1:					strcat( out_szString, "F1" ); break;
		case TBX_CLI_TOOLS_KEY_F2:					strcat( out_szString, "F2" ); break;
		case TBX_CLI_TOOLS_KEY_F3:					strcat( out_szString, "F3" ); break;
		case TBX_CLI_TOOLS_KEY_F4:					strcat( out_szString, "F4" ); break;
		case TBX_CLI_TOOLS_KEY_F5:					strcat( out_szString, "F5" ); break;
		case TBX_CLI_TOOLS_KEY_F6:					strcat( out_szString, "F6" ); break;
		case TBX_CLI_TOOLS_KEY_F7:					strcat( out_szString, "F7" ); break;
		case TBX_CLI_TOOLS_KEY_F8:					strcat( out_szString, "F8" ); break;
		case TBX_CLI_TOOLS_KEY_F9:					strcat( out_szString, "F9" ); break;
		case TBX_CLI_TOOLS_KEY_F10:					strcat( out_szString, "F10" ); break;
		case TBX_CLI_TOOLS_KEY_F11:					strcat( out_szString, "F11" ); break;
		case TBX_CLI_TOOLS_KEY_F12:					strcat( out_szString, "F12" ); break;
		case TBX_CLI_TOOLS_KEY_TAB:					strcat( out_szString, "Tab" ); break;

		default:
		{
			if( isprint( (int)KeyPressedNoModifier ) )
			{
				TBX_CHAR	szChar[ 16 ];
				sprintf( szChar, "%c", (char)KeyPressedNoModifier );
				strcat( out_szString, szChar );
			}
			else
			{
				TBX_CHAR	szChar[ 16 ];
				sprintf( szChar, "0x%X", (int)KeyPressedNoModifier );
				strcat( out_szString, szChar );
			}
		} break;
	}

	return;
}


/*--------------------------------------------------------------------------------------------------------------------------------
 |  C++ support
 *------------------------------------------------------------------------------------------------------------------------------*/
#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __TBX_CLI_TOOLS_H__ */

