/*--------------------------------------------------------------------------------------------------------------------------------
 |
 |	Project:    	VoiceLink TBX sample (common cli)
 |
 |	Filename:   	tbx_cli.h
 |
 |	Copyright:  	TelcoBridges 2002-2003, All Rights Reserved
 |
 |	Description:	This file contains the client interface functions
 |
 |	Notes:      	Tabs = 4
 |
 *-------------------------------------------------------------------------------------------------------------------------------
 |
 |	Revision:   	$Revision: 142061 $
 |
 *------------------------------------------------------------------------------------------------------------------------------*/

#include "tbx_ostypes.h"
#include <stdio.h>
#include <stdlib.h>
//#include "tbx_os_wrapper.h"

/*--------------------------------------------------------------------------------------------------------------------------------
 |  Define header file
 *------------------------------------------------------------------------------------------------------------------------------*/
#ifndef __TBX_CLI_H__
#define __TBX_CLI_H__



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

#define ESC				"\033["

/* Ansi Control character suffixes */
#define HOME_CHAR			'H'
#define HOME_CHAR_STR		"H"
#define	CLEARLINE_CHAR		'1'
#define	CLEARLINE_CHAR_STR	"1"
#define	CLEARLINEEND_CHAR	"K"
#define CLEARSCR_CHAR0		'2'
#define CLEARSCR_CHAR1		'J'
#define CLEARSCR_CHAR		"2J"

#define	DEFAULT_COLOR	ESC END_COLOR	/* Reset to Default fg/bg color */
#define AND_COLOR		";"				/* To add multiple color definitions */
#define END_COLOR		"m"				/* To end color definitions */

/* Foreground colors values */
#define	F_BLACK		"30"
#define	F_RED		"31"
#define	F_GREEN		"32"
#define	F_YELLOW	"33"
#define	F_BLUE		"34"
#define	F_MAGEN		"35"
#define	F_CYAN		"36"
#define	F_WHITE		"37"
#ifdef WIN32
	#define	F_GREY		"20"
#else
	#define	F_GREY		F_WHITE
#endif

/* Background colors values */
#define	B_BLACK		"40"
#define	B_RED		"41"
#define	B_GREEN		"42"
#define	B_YELLOW	"43"
#define	B_BLUE		"44"
#define	B_MAGEN		"45"
#define	B_CYAN		"46"
#define	B_WHITE		"47"

/* Preset escape sequences - Change foreground colors only */
#define	FBLACK		ESC F_BLACK END_COLOR
#define	FRED		ESC F_RED END_COLOR
#define	FGREEN		ESC F_GREEN	END_COLOR
#define	FYELLOW		ESC F_YELLOW END_COLOR
#define	FBLUE		ESC F_BLUE END_COLOR
#define	FMAGEN		ESC F_MAGEN END_COLOR
#define	FCYAN		ESC F_CYAN END_COLOR
#define	FWHITE		ESC F_WHITE END_COLOR
#define	FGREY		ESC F_GREY END_COLOR

#define	BBLACK		ESC B_BLACK END_COLOR
#define	BRED		ESC B_RED END_COLOR
#define	BGREEN		ESC B_GREEN	END_COLOR
#define	BYELLOW		ESC B_YELLOW END_COLOR
#define	BBLUE		ESC B_BLUE END_COLOR
#define	BMAGEN		ESC B_MAGEN END_COLOR
#define	BCYAN		ESC B_CYAN END_COLOR
#define	BWHITE		ESC B_WHITE END_COLOR

/* Preset escape sequences - Change foreground and background colors */
#define	FBLACK_BWHITE		ESC B_WHITE AND_COLOR F_BLACK END_COLOR

/* Preset escape sequences */
#define HOME			ESC HOME_CHAR_STR
#define	CLEARLINE		ESC CLEARLINE_CHAR_STR
#define	CLEARLINEEND	ESC CLEARLINEEND_CHAR
#define CLEARSCR		ESC CLEARSCR_CHAR HOME

/* TBXCliStdin definitions */
typedef void (*PFCT_STDIN_CALLBACK)(void* pCtx, char* pBuff, int len );

/*--------------------------------------------------------------------------------------------------------------------------------
 |  Types
 *------------------------------------------------------------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------------------------------------------------------------
 |  Macros
 *------------------------------------------------------------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------------------------------------------------------------
 |  Function Prototypes
 *------------------------------------------------------------------------------------------------------------------------------*/

 /*-------------------------------------------------------------------------------------------------------------------------------
 |
 |  TBXCliSetWindowTitle	:	This function changes the window title
 |
 |  in_pszTitle				:	Window title to set
 |							
 |  Note					:	~
 |							
 |  Return					:	TBX_RESULT_OK
 |								TBX_RESULT_FAIL
 |
 *------------------------------------------------------------------------------------------------------------------------------*/
TBX_VOID TBXCliSetWindowTitle ( PTBX_CHAR in_pszTitle );

/*-------------------------------------------------------------------------------------------------------------------------------
 |
 |  TBXCliSetEcho	:	This function enables or disables terminal input echo
 |
 |  in_fEnable      :	Enable?
 |
 |  Note			:	~
 |
 |  Return          :	The previous echo setting.
 |
 *------------------------------------------------------------------------------------------------------------------------------*/
TBX_BOOL TBXCliSetEcho ( TBX_BOOL in_fEnable );

/*-------------------------------------------------------------------------------------------------------------------------------
 |
 |  TBXCliKeyboardHit:	This function returns whether or not a key was pressed
 |
 |  ~		        :	No arguments used
 |
 |  Note			:	~
 |
 |  Return          :	TBX_RESULT_OK
 |						TBX_RESULT_FAIL
 |
 *------------------------------------------------------------------------------------------------------------------------------*/
TBX_INT TBXCliKeyboardHit (void);

/*-------------------------------------------------------------------------------------------------------------------------------
 |
 |  TBXCliGetch:	This function returns a pressed key with key remapping
 |
 |  ~		        :	No arguments used
 |
 |  Note			:	~
 |
 |  Return          :	TBX_RESULT_OK
 |						TBX_RESULT_FAIL
 |
 *------------------------------------------------------------------------------------------------------------------------------*/
TBX_INT  TBXCliGetchRemapped (void);


/*-------------------------------------------------------------------------------------------------------------------------------
 |
 |  TBXCliGetch	:	This function returns a pressed key,
 |								without remapping of esacape sequences
 |								(in case it needs to be done manually by caller)
 |
 |  ~					:	No arguments used
 |
 |  Note				:	~
 |
 |  Return				:	TBX_RESULT_OK
 |							TBX_RESULT_FAIL
 |
 *------------------------------------------------------------------------------------------------------------------------------*/
TBX_INT  TBXCliGetch (void);

/*-------------------------------------------------------------------------------------------------------------------------------
 |
 |  TBXCliHome		:	This function returns the cursor to the position 0,0
 |
 |  ~		        :	No arguments used
 |
 |  Note			:	~
 |
 |  Return          :	No return value
 |
 *------------------------------------------------------------------------------------------------------------------------------*/
TBX_VOID TBXCliHome (void);

/*-------------------------------------------------------------------------------------------------------------------------------
 |
 |  TBXCliCls		:	This function clears the screen
 |
 |  ~		        :	No arguments used
 |
 |  Note			:	~
 |
 |  Return          :	No return value
 |
 *------------------------------------------------------------------------------------------------------------------------------*/
TBX_VOID TBXCliCls (void);

/*-------------------------------------------------------------------------------------------------------------------------------
 |
 |  TBXCliClsNoHome	:	This function clears the remaining part of the screen,
 |						(starting from current cursor position)
 |
 |  ~		        :	No arguments used
 |
 |  Note			:	~
 |
 |  Return          :	No return value
 |
 *------------------------------------------------------------------------------------------------------------------------------*/
TBX_VOID TBXCliClsNoHome (void);

/*-------------------------------------------------------------------------------------------------------------------------------
 |
 |  TBXCliClearLine	:	This function clears the current line
 |
 |  ~		        :	No arguments used
 |
 |  Note			:	~
 |
 |  Return          :	No return value
 |
 *------------------------------------------------------------------------------------------------------------------------------*/
TBX_VOID TBXCliClearLine (void);

/*-------------------------------------------------------------------------------------------------------------------------------
 |
 |  TBXCliClearLineNoHome	:	This function clears the remaining of the current line
 |
 |  ~						:	No arguments used
 |
 |  Note					:	~
 |
 |  Return					:	No return value
 |
 *------------------------------------------------------------------------------------------------------------------------------*/
TBX_VOID TBXCliClearLineNoHome (void);


/*-------------------------------------------------------------------------------------------------------------------------------
 |
 |  TBXCliGetScreenWidth	:	This function returns the current console width
 |								(or 0 when not supported)
 |
 |  ~						:	No arguments used
 |
 |  Note					:	~
 |
 |  Return					:	No return console width
 |
 *------------------------------------------------------------------------------------------------------------------------------*/
TBX_UINT32 TBXCliGetScreenWidth (void);

/*-------------------------------------------------------------------------------------------------------------------------------
 |
 |  TBXCliGetScreenHeight	:	This function returns the current console width
 |								(or 0 when not supported)
 |
 |  ~						:	No arguments used
 |
 |  Note					:	~
 |
 |  Return					:	No return console width
 |
 *------------------------------------------------------------------------------------------------------------------------------*/
TBX_UINT32 TBXCliGetScreenHeight (void);



/*-------------------------------------------------------------------------------------------------------------------------------
 |
 |  TBXCli_vfprintf	:	This function handle ANSI escape sequence printing
 |
 |	Args			:	see vfprintf
 |
 |  Return          :	see vfprintf
 |
 *------------------------------------------------------------------------------------------------------------------------------*/
int TBXCli_vfprintf( FILE *file , const char *fmt, va_list argptr);

/*-------------------------------------------------------------------------------------------------------------------------------
 |
 |  TBXCli_fprintf	:	This function handle ANSI escape sequence printing
 |
 |	Args			:	see fprintf
 |
 |  Return          :	see fprintf
 |
 *------------------------------------------------------------------------------------------------------------------------------*/

int TBXCli_fprintf( FILE * file, const char *fmt, ... );


/*-------------------------------------------------------------------------------------------------------------------------------
 |
 |  TBXCli_printf	:	This function handle ANSI escape sequence printing
 |
 |	Args			:	see printf
 |
 |  Return          :	see printf
 |
 *------------------------------------------------------------------------------------------------------------------------------*/
int TBXCli_printf( const char *fmt, ... );






/*-------------------------------------------------------------------------------------------------------------------------------
 |
 |  TBXCli_vsnprintf	:	This function renders string and removes ANSI escape sequences
 |
 |	Args				:	see vsnprintf
 |
 |  Return			    :	see vsnprintf
 |
 *------------------------------------------------------------------------------------------------------------------------------*/
int TBXCli_vsnprintf
(
	OUT		char*			out_pszBuffer,
	IN		unsigned long	in_un32MaxLen,
	IN		const char*		fmt,
	IN		va_list			argptr
);
int TBXCli_snprintf
(
	OUT		char*			out_pszBuffer,
	IN		unsigned long	in_un32MaxLen,
	IN		const char*		fmt,
	IN		...
);


/*-------------------------------------------------------------------------------------------------------------------------------
 |
 |  TBXCli_strncpy		:	This function copy a string and removes ANSI escape sequences
 |
 |	Args				:	see strcpy
 |
 |  Return			    :	Returns the copied string length (with ANDI escape sequences removed)
 |
 *------------------------------------------------------------------------------------------------------------------------------*/
TBX_UINT32 TBXCli_strncpy
(
	OUT		char*			out_pszString,
	IN		const char*		in_pszString,
	IN		unsigned long	in_un32MaxLen
);

/*-------------------------------------------------------------------------------------------------------------------------------
 |
 |  TBXCli_Strlen	:	This function returns the length of a string, but it "skips" (does not count)
 |						the escape codes such as color codes, so that it returns the actual string length
 |						seen on the shell display.
 |
 |	in_pszString	:	String to calcultate it's length
 |
 |  Return          :	The length of the string
 |
 *------------------------------------------------------------------------------------------------------------------------------*/
TBX_UINT32 TBXCli_Strlen (const TBX_CHAR* in_pszString);

/*-------------------------------------------------------------------------------------------------------------------------------
 |
 |  TBXCliStdinHandlerCreate:	Functions that is used to start Stdin Monitoring and Redirection
 |
 |	pFct			:	User Callback that will be called for each lines that were input on stdin.
 |	pCtx			:	User context for the Callback.
 |
 |  Return          :	~
 |
 *------------------------------------------------------------------------------------------------------------------------------*/

TBX_VOID
TBXCliStdinHandlerCreate(
  IN	PFCT_STDIN_CALLBACK		pFct,
  IN	PTBX_VOID				pCtx );

/*-------------------------------------------------------------------------------------------------------------------------------
 |
 |  TBXCliStdinHandlerDestroy:	Functions that is used to destroy Stdin Monitoring and Redirection
 |
 |  Return          :	~
 |
 *------------------------------------------------------------------------------------------------------------------------------*/

TBX_VOID
TBXCliStdinHandlerDestroy(void);

/*-------------------------------------------------------------------------------------------------------------------------------
 |
 |  TBXCliStdinPlayFile:	Functions that can be used to pass a file as stdin stream,
 |							After the file is finished, stdin is back to normal.
 |							TBXCliStdinHandlerCreate() must be called before this
 |
 |	hFile			:	File handle from which to feed the stdin.  This file handle is consumed
 |						by this fucntions, and must not be freed by the user.
 |
 |  Return          :	~
 |
 *------------------------------------------------------------------------------------------------------------------------------*/

TBX_VOID
TBXCliStdinPlayFile(
  FILE*					hFile );

/*--------------------------------------------------------------------------------------------------------------------------------
 |  C++ support
 *------------------------------------------------------------------------------------------------------------------------------*/
#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __TBX_CLI_H__ */

