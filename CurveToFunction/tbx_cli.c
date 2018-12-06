/*--------------------------------------------------------------------------------------------------------------------------------
 |
 |	Project:    	VoiceLink TBX sample (common cli)
 |
 |	Filename:   	tbx_cli.c
 |
 |	Copyright:  	TelcoBridges 2002-2003, All Rights Reserved
 |
 |	Description:	This file contains the client interface functions
 |
 |	Notes:      	Tabs = 4
 |
 *-------------------------------------------------------------------------------------------------------------------------------
 |
 |	Revision:   	$Revision: 145601 $
 |
 *------------------------------------------------------------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------------------------------------------------------------
 |  Includes
 *------------------------------------------------------------------------------------------------------------------------------*/

#ifdef WIN32
#include "windows.h"
#endif

/* System includes */
#ifdef MQX
#include <mqx.h>
#include <fio.h>
#include <telnet.h>
#else
#include <stdio.h>
#endif
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>
#include <ctype.h>
#if defined( WIN32 ) && !defined( OSE )
#include <conio.h>
#include <io.h>
#include <fcntl.h>
#include <process.h>
#elif defined( OSE )
#include <conio.h>
#define _TIME_T_DEFINED
#include <io.h>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#elif defined( MQX )
#else /* OSE */
#include <termios.h>
#include <unistd.h>
#endif /* WIN32 && !OSE */

/* Local includes */
#include "tbx_id.h"
#include "tbx_result.h"
#include "tbx_os_wrapper.h"
//#include "tbx_std.h"
#include "tbx_cli.h"


//#pragma once


/*--------------------------------------------------------------------------------------------------------------------------------
 |  Forward declarations
 *------------------------------------------------------------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------------------------------------------------------------
 |  Defines
 *------------------------------------------------------------------------------------------------------------------------------*/
#ifdef WIN32
#define pipe( _aPipe ) _pipe( _aPipe, 512, O_BINARY )
#endif

/*--------------------------------------------------------------------------------------------------------------------------------
 |  Types
 *------------------------------------------------------------------------------------------------------------------------------*/

typedef enum _TBX_PIPE
{
	TBX_PIPE_READ	= 0,
	TBX_PIPE_WRITE	= 1

} TBX_PIPE, *PTBX_PIPE;


typedef struct _STDIN_HANDLER_PARAMS
{
	PFCT_STDIN_CALLBACK	pFct;
	void*				pCtx;
	FILE*				hFileToPlay;
	TBX_BOOL			fInit;

	//TBX_THREAD			StdinThread;
	HANDLE			StdinThread;
	TBX_BOOL			fQuit;
	int					stdIn;

} STDIN_HANDLER_PARAMS, *PSTDIN_HANDLER_PARAMS;


/*--------------------------------------------------------------------------------------------------------------------------------
 |  Global variables
 *------------------------------------------------------------------------------------------------------------------------------*/
STDIN_HANDLER_PARAMS	StdinHandlerCtx = { 0, 0, 0, 0 };

#if defined(VXWORKS) && (CPU==SIMNT)
extern FILE *			g_pConsoleOutputStream;
#endif


/*--------------------------------------------------------------------------------------------------------------------------------
 |  Macros
 *------------------------------------------------------------------------------------------------------------------------------*/

#ifdef WIN32
#ifndef vsnprintf
	#define vsnprintf _vsnprintf
#endif
#ifndef snprintf
	#define snprintf _snprintf
#endif
#endif

/*--------------------------------------------------------------------------------------------------------------------------------
 |  Function Prototypes
 *------------------------------------------------------------------------------------------------------------------------------*/
int TBXCli_vfprintf_file
(
	IN		FILE*		file,
	IN		const char*	fmt,
	IN		va_list		argptr
);
TBX_INT
TB640KeypadRemap ( TBX_INT in_nChar );

/*--------------------------------------------------------------------------------------------------------------------------------
 |  Implementation
 *------------------------------------------------------------------------------------------------------------------------------*/


/*-------------------------------------------------------------------------------------------------------------------------------
 |
 |  TB640KeypadRemap:	This function adjust display pointers for a UpArrow
 |
 |  Note			:	~
 |
 |  Return          :	Key pressed
 |
 *------------------------------------------------------------------------------------------------------------------------------*/
TBX_INT
TB640KeypadRemap ( TBX_INT in_nChar )
{
	/*---------------------------------------------------------------------------------------------------------------------------
	 |  Code section

		if( in_nChar == 0xE0 || in_nChar == 0x00 ) /* Edit Keypad remapping  WIN32 */
		{
			in_nChar = TBXCliGetch();
			switch (in_nChar)
			{
				case 0x51: /* PGDWN */
					in_nChar = '3'; /* remap to keypad page down */
					break;
				case 0x49: /* PGUP */
					in_nChar = '9'; /* remap to keypad page up */
					break;
				case 0x47: /* HOME */
					in_nChar = '7'; /* remap to keypad home */
					break;
				case 0x4F: /* END */
					in_nChar = '1'; /* remap to keypad end */
					break;
				case 0x48: /* UPARROW */
					in_nChar = '8'; /* remap to keypad up arrow */
					break;
				case 0x50: /* DWNARROW */
					in_nChar = '2'; /* remap to keypad down arrow */
					break;
				case 0x4d: /* RGTHARROW */
					in_nChar = '6'; /* remap to keypad rigth arrow */
					break;
				case 0x4b: /* LFTARROW */
					in_nChar = '4'; /* remap to keypad left arrow */
					break;
				default:
					break;
			}
		}

		if( in_nChar == 0x1B /* ESC */ ) /* Edit Keypad remapping Solaris */
		{
			/* Escape sequence is "ESC[6~" where 6 is the key */
			in_nChar = TBXCliGetch(); /* [ */
			in_nChar = TBXCliGetch(); /* key */
			switch (in_nChar)
			{
				case '6': /* PGDWN */
					in_nChar = '3'; /* remap to keypad page down */
					TBXCliGetch(); /* ~ */
					break;
				case '5': /* PGUP */
					in_nChar = '9'; /* remap to keypad page up */
					TBXCliGetch(); /* ~ */
					break;
				case '1': /* HOME */
					in_nChar = '7'; /* remap to keypad home */
					TBXCliGetch(); /* ~ */
					break;
				case '4': /* END */
					in_nChar = '1'; /* remap to keypad end */
					TBXCliGetch(); /* ~ */
					break;
				case 'A': /* UPARROW */
					in_nChar = '8'; /* remap to keypad up arrow */
					break;
				case 'B': /* DWNARROW */
					in_nChar = '2'; /* remap to keypad down arrow */
					break;
				case 'C': /* RGTHARROW */
					in_nChar = '6'; /* remap to keypad right arrow */
					break;
				case 'D': /* LFTARROW */
					in_nChar = '4'; /* remap to keypad left arrow */
					break;
				default:
				break;
				}
		}


		in_nChar = toupper (in_nChar);

		/* End of the code (skip to cleanup) */
		//TBX_EXIT_SUCCESS (in_nChar);


	return;
}


#if defined (VXWORKS)

#include "ioLib.h"

#define	TBXCLI_VXWORKS_STDIN_BUF_LEN		512
static volatile TBX_UINT32					g_un32StdinWriteIdx = 0;
static volatile TBX_UINT32					g_un32StdinReadIdx = 0;
static volatile TBX_BYTE					g_abyStdin [TBXCLI_VXWORKS_STDIN_BUF_LEN];


/*-------------------------------------------------------------------------------------------------------------------------------
 |
 |  TBXCliVxworksStdin:	This thread is used to simulate asyncronous stdin
 |
 |  ~		        :	No arguments used
 |
 |  Note			:	THIS IMPLEMENTATION IS FOR VXWORKS
 |
 |  Return          :	TBX_RESULT_OK
 |						TBX_RESULT_FAIL
 |
 *------------------------------------------------------------------------------------------------------------------------------*/
TB_C_FUNC_PROTOTYPE int TBXCliVxworksStdin( int	in_Fd )
{
	volatile TBX_BOOL	fStdinContinue;
	TBX_CHAR			nChar;
	TBX_INT				nValue;
	TBX_UINT32			un32NewStdinWriteIdx;

	/*---------------------------------------------------------------------------------------------------------------------------
	 |  Code section
	 *--------------------------------------------------------------------------------------------------------------------------*/
	CODE
	{
		/* Initialize local variables */
		fStdinContinue = TBX_TRUE;

		/* Remove the 'LINE configuration' of the terminal */
		(void) ioctl (in_Fd, FIOGETOPTIONS, &nValue);
		(void) ioctl (in_Fd, FIOSETOPTIONS, (nValue & ~(OPT_LINE)));

		/* Loop infinitely until killed */
		while (fStdinContinue)
		{
			/* Read a character (blocking) */
			nChar = getc(stdin);

			/* Insert it into the array */
			un32NewStdinWriteIdx = ((g_un32StdinWriteIdx + 1) % TBXCLI_VXWORKS_STDIN_BUF_LEN);
			if (g_un32StdinReadIdx != un32NewStdinWriteIdx)
			{
				g_abyStdin [g_un32StdinWriteIdx] = (TBX_BYTE)nChar;
				g_un32StdinWriteIdx = un32NewStdinWriteIdx;
			}
		}

		/* return 1 if the keyboard was hit, or 0 if it was not hit */
		TBX_EXIT_SUCCESS (0);
	}

	/*---------------------------------------------------------------------------------------------------------------------------
	 |  Error handling section
	 *--------------------------------------------------------------------------------------------------------------------------*/
	ERROR_HANDLING
	{
	}

	/*---------------------------------------------------------------------------------------------------------------------------
	 |  Cleanup section
	 *--------------------------------------------------------------------------------------------------------------------------*/
	CLEANUP
	{

	}

	RETURN_WITH_TYPE (TBX_INT);

}

/*-------------------------------------------------------------------------------------------------------------------------------
 |
 |  TBXCliSetEcho	:	This function enables or disables terminal input echo
 |
 |  in_fEnable      :	Enable?
 |
 |  Note			:	~
 |
 |  Return          :	TBX_RESULT_OK
 |						TBX_RESULT_FAIL
 |
 *------------------------------------------------------------------------------------------------------------------------------*/
TBX_BOOL TBXCliSetEcho ( TBX_BOOL in_fEnable )
{
	return in_fEnable;
}

/*-------------------------------------------------------------------------------------------------------------------------------
 |
 |  TBXCliKeyboardHit:	This function returns whether or not a key was pressed
 |
 |  ~		        :	No arguments used
 |
 |  Note			:	THIS IMPLEMENTATION IS FOR VXWORKS
 |
 |  Return          :	TBX_RESULT_OK
 |						TBX_RESULT_FAIL
 |
 *------------------------------------------------------------------------------------------------------------------------------*/
TBX_INT TBXCliKeyboardHit (void)
{
	/*---------------------------------------------------------------------------------------------------------------------------
	 |  Code section
	 *--------------------------------------------------------------------------------------------------------------------------*/
	CODE
	{
		/* return 1 if the keyboard was hit, or 0 if it was not hit */
		TBX_EXIT_SUCCESS ((g_un32StdinWriteIdx!=g_un32StdinReadIdx)?TBX_TRUE:TBX_FALSE);
	}

	/*---------------------------------------------------------------------------------------------------------------------------
	 |  Error handling section
	 *--------------------------------------------------------------------------------------------------------------------------*/
	ERROR_HANDLING
	{
	}

	/*---------------------------------------------------------------------------------------------------------------------------
	 |  Cleanup section
	 *--------------------------------------------------------------------------------------------------------------------------*/
	CLEANUP
	{

	}

	RETURN_WITH_TYPE (TBX_INT);
}


/*-------------------------------------------------------------------------------------------------------------------------------
 |
 |  TBXCliGetch			:	This function returns a pressed key,
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
TBX_INT  TBXCliGetch (void)
{
	TBX_INT				c;
	TBX_UINT32			un32NewStdinReadIdx;

	/*---------------------------------------------------------------------------------------------------------------------------
	 |  Code section
	 *--------------------------------------------------------------------------------------------------------------------------*/
	CODE
	{
		/* Initialize local variables */
		c = 0;

		/* Calculate new read pointer */
		un32NewStdinReadIdx = ((g_un32StdinReadIdx + 1) % TBXCLI_VXWORKS_STDIN_BUF_LEN);

		/* Make sure there is a character (shouldn't never block) */
		while (g_un32StdinWriteIdx==g_un32StdinReadIdx)
		{
			taskDelay(1);
		}

		/* Read next character */
		c = (TBX_INT)g_abyStdin [g_un32StdinReadIdx];
		g_un32StdinReadIdx = un32NewStdinReadIdx;

		/* return the charcter */
		TBX_EXIT_SUCCESS (c);
	}

	/*---------------------------------------------------------------------------------------------------------------------------
	 |  Error handling section
	 *--------------------------------------------------------------------------------------------------------------------------*/
	ERROR_HANDLING
	{
	}

	/*---------------------------------------------------------------------------------------------------------------------------
	 |  Cleanup section
	 *--------------------------------------------------------------------------------------------------------------------------*/
	CLEANUP
	{

	}

	RETURN_WITH_TYPE (TBX_INT);
}


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
TBX_VOID TBXCliHome ( TBX_VOID )
{
	#if (CPU==SIMNT)

		if (g_pConsoleOutputStream)
		{
			fprintf (g_pConsoleOutputStream, HOME);
			fflush (g_pConsoleOutputStream);
		}

	#else

		printf (HOME);

	#endif
}


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
TBX_VOID TBXCliCls ( TBX_VOID )
{
	#if (CPU==SIMNT)

		if (g_pConsoleOutputStream)
		{
			fprintf (g_pConsoleOutputStream, CLEARSCR);
			fflush (g_pConsoleOutputStream);
		}

	#else

		printf (CLEARSCR);

	#endif
}

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
TBX_VOID TBXCliClsNoHome ( TBX_VOID )
{
	#if (CPU==SIMNT)

		if (g_pConsoleOutputStream)
		{
			fprintf (g_pConsoleOutputStream, CLEARLINE);
			fflush (g_pConsoleOutputStream);
		}

	#else

		printf (CLEARLINE);

	#endif
}

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
TBX_VOID TBXCliClearLine ( TBX_VOID )
{
	TBXCliClsNoHome();
}


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
TBX_VOID TBXCliClearLineNoHome ()
{
	#if (CPU==SIMNT)

		if (g_pConsoleOutputStream)
		{
			fprintf (g_pConsoleOutputStream, CLEARLINEEND);
			fflush (g_pConsoleOutputStream);
		}

	#else

		printf (CLEARLINEEND);

	#endif
}

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
TBX_UINT32 TBXCliGetScreenWidth ()
{
	TBX_INT32	n32ScreenWidth = 0;

	#ifdef TIOCGWINSZ
	{
		struct winsize ws;

		if (ioctl (1, TIOCGWINSZ, &ws) != -1)
		{
			if (ws.ws_col != 0)
			{
				n32ScreenWidth	= (TBX_UINT32)(ws.ws_col - 1);
			}
		}
	}
	#else
	#ifdef TIOCGSIZE
	{
		struct ttysize ts;

		if (ioctl (1, TIOCGSIZE, &ts) != -1)
		{
			if (ts.ts_cols != 0)
			{
				n32ScreenWidth	= (TBX_UINT32)(ts.ts_cols - 1);
			}
		}
	}
	#endif /* TIOCGSIZE */
	#endif /* TIOCGWINSZ */

	if( n32ScreenWidth < 0 )
		n32ScreenWidth = 0;
	return (TBX_UINT32)n32ScreenWidth;
}

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
TBX_UINT32 TBXCliGetScreenHeight ()
{
	TBX_INT32	n32ScreenHeight = 0;

	#ifdef TIOCGWINSZ
	{
		struct winsize ws;

		if (ioctl (1, TIOCGWINSZ, &ws) != -1)
		{
			if (ws.ws_row != 0)
			{
				n32ScreenHeight = (TBX_UINT32)(ws.ws_row - 1);
			}
		}
	}
	#else
	#ifdef TIOCGSIZE
	{
		struct ttysize ts;

		if (ioctl (1, TIOCGSIZE, &ts) != -1)
		{
			if (ts.ts_lines != 0)
			{
				n32ScreenHeight = (TBX_UINT32)(ts.ts_lines - 1);
			}
		}
	}
	#endif /* TIOCGSIZE */
	#endif /* TIOCGWINSZ */

	if( n32ScreenHeight < 0 )
		n32ScreenHeight = 0;
	return (TBX_UINT32)n32ScreenHeight;
}


#if (CPU==SIMNT)
/*-------------------------------------------------------------------------------------------------------------------------------
 |
 |  TBXCli_vfprintf	:	This function handle ANSI escape sequence printing
 |
 |	Args			:	see vfprintf
 |
 |  Return          :	see vfprintf
 |
 *------------------------------------------------------------------------------------------------------------------------------*/
int
TBXCli_vfprintf( FILE *file , const char *fmt, va_list argptr)
{
	FILE *fout;
	int	nbWritten;

	if ((file != stdout) && (file != stderr))
		fout = file;
	else if (g_pConsoleOutputStream)
		fout = g_pConsoleOutputStream;
	else
		fout = stdout;

	nbWritten = vfprintf (fout, fmt, argptr);
	fflush(fout);
	return nbWritten;
}

/*-------------------------------------------------------------------------------------------------------------------------------
 |
 |  TBXCli_fprintf	:	This function handle ANSI escape sequence printing
 |
 |	Args			:	see fprintf
 |
 |  Return          :	see fprintf
 |
 *------------------------------------------------------------------------------------------------------------------------------*/

int
TBXCli_fprintf( FILE * file, const char *fmt, ... )
{
	va_list	argptr;
	int		written;

    va_start(argptr, fmt);
	written = TBXCli_vfprintf( file, fmt, argptr );
    va_end(argptr);

	return written;
}

/*-------------------------------------------------------------------------------------------------------------------------------
 |
 |  TBXCli_printf	:	This function handle ANSI escape sequence printing
 |
 |	Args			:	see fprintf
 |
 |  Return          :	see fprintf
 |
 *------------------------------------------------------------------------------------------------------------------------------*/

int
TBXCli_printf( const char *fmt, ... )
{
	va_list	argptr;
	int		written;

    va_start(argptr, fmt);
	written = TBXCli_vfprintf( stdout, fmt, argptr );
    va_end(argptr);

	return written;
}


#endif /* SIMNT */


#elif defined ( MQX )


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
TBX_VOID TBXCliSetWindowTitle ( PTBX_CHAR in_pszTitle )
{
	/* Not supported */
}


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
TBX_BOOL TBXCliSetEcho ( TBX_BOOL in_fEnable )
{
	_mqx_uint	flags;
	TBX_BOOL	fEcho;
	ioctl(stdin, IO_IOCTL_SERIAL_GET_FLAGS, &flags);
	fEcho = ( ( flags & IO_SERIAL_ECHO ) ? TBX_TRUE : TBX_FALSE );
	if( in_fEnable )
		flags |= IO_SERIAL_ECHO;
	else
		flags &= ~IO_SERIAL_ECHO;
	ioctl(stdin, IO_IOCTL_SERIAL_SET_FLAGS, &flags);
	
	return fEcho;
}

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
TBX_INT TBXCliKeyboardHit (void)
{
	fstatus( stdin ) ? 1 : 0;
}


/*-------------------------------------------------------------------------------------------------------------------------------
 |
 |  TBXCliGetch			:	This function returns a pressed key,
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
TBX_INT  TBXCliGetch (void)
{
	return fgetc( stdin );
}


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
TBX_VOID TBXCliHome ( TBX_VOID )
{
	printf( HOME );
}


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
TBX_VOID TBXCliCls ( TBX_VOID )
{
	printf( CLEARSCR );
}

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
TBX_VOID TBXCliClsNoHome ( TBX_VOID )
{
	/* Clear the remaining of the line */
	printf( CLEARLINE );

	/* Clear the remaining lines of the screen */
	/* Not implemented */
}

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
TBX_VOID TBXCliClearLine ( TBX_VOID )
{
	printf( CLEARLINE );
}


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
TBX_VOID TBXCliClearLineNoHome ()
{
	printf( CLEARLINEEND );
}

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
TBX_UINT32 TBXCliGetScreenWidth ()
{
	_mqx_uint	result;
	_mqx_uint	un32Width;
	result = ioctl(stdin, IO_IOCTL_TELNET_GET_TERM_WIDTH, &un32Width);
	if( result == MQX_OK && un32Width > 80 )
	{
		return (TBX_UINT32)un32Width;
	}
	else
	{
		return 100;
	}
}

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
TBX_UINT32 TBXCliGetScreenHeight ()
{
	_mqx_uint	result;
	_mqx_uint	un32Height;
	result = ioctl(stdin, IO_IOCTL_TELNET_GET_TERM_HEIGHT, &un32Height);
	if( result == MQX_OK && un32Height > 24 )
	{
		return (TBX_UINT32)(un32Height - 1);
	}
	else
	{
		return 40;
	}
}

int
TBXCli_vfprintf( FILE *file , const char *fmt, va_list argptr)
{
	TBX_CHAR	szBuf[ 1024 ];

	vsnprintf( szBuf, sizeof(szBuf), fmt, argptr );
	szBuf[sizeof(szBuf)-1] = '\0';
	fwrite( szBuf, 1, strlen(szBuf), stdout );
}

/*-------------------------------------------------------------------------------------------------------------------------------
 |
 |  TBXCli_fprintf	:	This function handle ANSI escape sequence printing
 |
 |	Args			:	see fprintf
 |
 |  Return          :	see fprintf
 |
 *------------------------------------------------------------------------------------------------------------------------------*/

int
TBXCli_fprintf( FILE * file, const char *fmt, ... )
{
	va_list	argptr;
	int		written;

    va_start(argptr, fmt);
	written = TBXCli_vfprintf( file, fmt, argptr );
    va_end(argptr);

	return written;
}

/*-------------------------------------------------------------------------------------------------------------------------------
 |
 |  TBXCli_printf	:	This function handle ANSI escape sequence printing
 |
 |	Args			:	see fprintf
 |
 |  Return          :	see fprintf
 |
 *------------------------------------------------------------------------------------------------------------------------------*/

int
TBXCli_printf( const char *fmt, ... )
{
	va_list	argptr;
	int		written;

    va_start(argptr, fmt);
	written = TBXCli_vfprintf( stdout, fmt, argptr );
    va_end(argptr);

	return written;
}

#elif !defined (WIN32)

#define TERMINAL_TITLE_BEGIN	"\033]2;"
#define TERMINAL_TITLE_END		"\007"

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
TBX_VOID TBXCliSetWindowTitle ( PTBX_CHAR in_pszTitle )
{
	TBX_CHAR	szTitle[ 512 ];

	snprintf
	(
		szTitle,
		sizeof(szTitle),
		TERMINAL_TITLE_BEGIN "%s" TERMINAL_TITLE_END,
		in_pszTitle
	);
	szTitle[ sizeof(szTitle) - 1 ] = '\0';
	printf( "%s", szTitle );
}


/*-------------------------------------------------------------------------------------------------------------------------------
 |
 |  TBXCliSetEcho	:	This function enables or disables terminal input echo
 |
 |  in_fEnable      :	Enable?
 |
 |  Note			:	~
 |
 |  Return          :	TBX_RESULT_OK
 |						TBX_RESULT_FAIL
 |
 *------------------------------------------------------------------------------------------------------------------------------*/
TBX_BOOL TBXCliSetEcho ( TBX_BOOL in_fEnable )
{
	TBX_INT				fd;
	struct termios		oterm;
	TBX_BOOL			fEcho;

	/* Initialize local variables */
	fd = 0;

	/* get the terminal settings */
	tcgetattr(fd, &oterm);

	fEcho = ((oterm.c_lflag & ECHO) ? TBX_TRUE : TBX_FALSE);
	
	if( in_fEnable )
	{
		oterm.c_lflag |= ECHO;
	}
	else
	{
		oterm.c_lflag &= ~ECHO;
	}
	tcsetattr(fd, TCSANOW, &oterm);
	
	return fEcho;
}

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
TBX_INT TBXCliKeyboardHit (void)
{
	TBX_INT				fd;
	TBX_INT				c;
	struct termios		term;
	struct termios		oterm;

	/*---------------------------------------------------------------------------------------------------------------------------
	 |  Code section
	 *--------------------------------------------------------------------------------------------------------------------------*/
	CODE
	{
		/* Initialize local variables */
		fd = 0;
		c = 0;

		/* get the terminal settings */
		tcgetattr(fd, &oterm);

		/* get a copy of the settings, which we modify */
		memcpy(&term, &oterm, sizeof(term));

		/* put the terminal in non-canonical mode, and
		   peak if a character is present */
		term.c_lflag = term.c_lflag & (!ICANON);
		term.c_cc[VMIN] = 0;
		term.c_cc[VTIME] = 0;
		tcsetattr(fd, TCSANOW, &term);

		/* get input - timeout after 0.1 seconds or
		 when one character is read. If timed out
		 getchar() returns -1, otherwise it returns
		 the character */
		c=getchar();

		/* reset the terminal to original state */
		tcsetattr(fd, TCSANOW, &oterm);

		/* if we retrieved a character, put it back on
		 the input stream */
		if (c != -1)
			ungetc(c, stdin);

		/* return 1 if the keyboard was hit, or 0 if it was not hit */
		TBX_EXIT_SUCCESS ((c!=-1)?1:0);
	}

	/*---------------------------------------------------------------------------------------------------------------------------
	 |  Error handling section
	 *--------------------------------------------------------------------------------------------------------------------------*/
	ERROR_HANDLING
	{
	}

	/*---------------------------------------------------------------------------------------------------------------------------
	 |  Cleanup section
	 *--------------------------------------------------------------------------------------------------------------------------*/
	CLEANUP
	{

	}

	RETURN_WITH_TYPE (TBX_INT);
}


/*-------------------------------------------------------------------------------------------------------------------------------
 |
 |  TBXCliGetch			:	This function returns a pressed key,
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
TBX_INT  TBXCliGetch (void)
{
	TBX_INT				c;
	TBX_INT				fd;
	struct termios		term;
	struct termios		oterm;

	/*---------------------------------------------------------------------------------------------------------------------------
	 |  Code section
	 *--------------------------------------------------------------------------------------------------------------------------*/
	CODE
	{
		/* Initialize local variables */
		fd = 0;

		/* get the terminal settings */
		tcgetattr(fd, &oterm);

		/* get a copy of the settings, which we modify */
		memcpy(&term, &oterm, sizeof(term));

		/* put the terminal in non-canonical mode, any
		 reads will wait until a character has been
		 pressed. This function will not time out */
		term.c_lflag = term.c_lflag & (!ICANON);
		term.c_cc[VMIN] = 1;
		term.c_cc[VTIME] = 0;
		tcsetattr(fd, TCSANOW, &term);

		/* get a character. c is the character */
		c=getchar();

		/* reset the terminal to its original state */
		tcsetattr(fd, TCSANOW, &oterm);

		/* return the charcter */
		TBX_EXIT_SUCCESS (c);
	}

	/*---------------------------------------------------------------------------------------------------------------------------
	 |  Error handling section
	 *--------------------------------------------------------------------------------------------------------------------------*/
	ERROR_HANDLING
	{
	}

	/*---------------------------------------------------------------------------------------------------------------------------
	 |  Cleanup section
	 *--------------------------------------------------------------------------------------------------------------------------*/
	CLEANUP
	{

	}

	RETURN_WITH_TYPE (TBX_INT);
}


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
TBX_VOID TBXCliHome ( TBX_VOID )
{
	printf( HOME );
}


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
TBX_VOID TBXCliCls ( TBX_VOID )
{
	printf( CLEARSCR );
}

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
TBX_VOID TBXCliClsNoHome ( TBX_VOID )
{
	/* Clear the remaining of the line */
	printf( CLEARLINE );

	/* Clear the remaining lines of the screen */
	/* Not implemented */
}

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
TBX_VOID TBXCliClearLine ( TBX_VOID )
{
	printf( CLEARLINE );
}


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
TBX_VOID TBXCliClearLineNoHome ()
{
	printf( CLEARLINEEND );
}

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
TBX_UINT32 TBXCliGetScreenWidth ()
{
	TBX_INT32	n32ScreenWidth = 0;

	#ifdef TIOCGWINSZ
	{
		struct winsize ws;

		if (ioctl (1, TIOCGWINSZ, &ws) != -1)
		{
			if (ws.ws_col != 0)
			{
				n32ScreenWidth	= (TBX_UINT32)(ws.ws_col - 1);
			}
		}
	}
	#else
	#ifdef TIOCGSIZE
	{
		struct ttysize ts;

		if (ioctl (1, TIOCGSIZE, &ts) != -1)
		{
			if (ts.ts_cols != 0)
			{
				n32ScreenWidth	= (TBX_UINT32)(ts.ts_cols - 1);
			}
		}
	}
	#endif /* TIOCGSIZE */
	#endif /* TIOCGWINSZ */

	if( n32ScreenWidth < 0 )
		n32ScreenWidth = 0;
	return (TBX_UINT32)n32ScreenWidth;
}

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
TBX_UINT32 TBXCliGetScreenHeight ()
{
	TBX_INT32	n32ScreenHeight = 0;

	#ifdef TIOCGWINSZ
	{
		struct winsize ws;

		if (ioctl (1, TIOCGWINSZ, &ws) != -1)
		{
			if (ws.ws_row != 0)
			{
				n32ScreenHeight = (TBX_UINT32)(ws.ws_row - 1);
			}
		}
	}
	#else
	#ifdef TIOCGSIZE
	{
		struct ttysize ts;

		if (ioctl (1, TIOCGSIZE, &ts) != -1)
		{
			if (ts.ts_lines != 0)
			{
				n32ScreenHeight = (TBX_UINT32)(ts.ts_lines - 1);
			}
		}
	}
	#endif /* TIOCGSIZE */
	#endif /* TIOCGWINSZ */

	if( n32ScreenHeight < 0 )
		n32ScreenHeight = 0;
	return (TBX_UINT32)n32ScreenHeight;
}



#else /* #ifndef WIN32 */


static void GetConsoleCursorPosition(HANDLE hCon, COORD *pCoord)
{
	CONSOLE_SCREEN_BUFFER_INFO csbi;

	GetConsoleScreenBufferInfo( hCon, &csbi);
	*pCoord = csbi.dwCursorPosition;
}

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
TBX_VOID TBXCliSetWindowTitle ( PTBX_CHAR in_pszTitle )
{
	SetConsoleTitle( in_pszTitle );
}


/*-------------------------------------------------------------------------------------------------------------------------------
 |
 |  TBXCliSetEcho	:	This function enables or disables terminal input echo
 |
 |  in_fEnable      :	Enable?
 |
 |  Note			:	~
 |
 |  Return          :	TBX_RESULT_OK
 |						TBX_RESULT_FAIL
 |
 *------------------------------------------------------------------------------------------------------------------------------*/
TBX_BOOL TBXCliSetEcho ( TBX_BOOL in_fEnable )
{
	return in_fEnable;
}

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
TBX_INT TBXCliKeyboardHit (void)
{
	return _kbhit ();
}


/*-------------------------------------------------------------------------------------------------------------------------------
 |
 |  TBXCliGetch	:	This function returns a pressed key,
 |								without remapping of escape sequences
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
TBX_INT  TBXCliGetch (void)
{
	TBX_INT		c;

	/* Get the key pressed */
	c = _getch ();

	return c;
}


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
TBX_VOID TBXCliHome ( TBX_VOID )
{
	COORD			coordScreen = { 0, 0 };
	HANDLE			hStdout;

	hStdout = GetStdHandle (STD_OUTPUT_HANDLE);
	SetConsoleCursorPosition( hStdout, coordScreen );
}


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
TBX_VOID TBXCliCls ( TBX_VOID )
{
	HANDLE						hStdout;
	COORD						coordScreen = { 0, 0 };
	DWORD						cCharsWritten;
	CONSOLE_SCREEN_BUFFER_INFO	csbi;
	DWORD						dwConSize;

	hStdout = GetStdHandle (STD_OUTPUT_HANDLE);
	if( !GetConsoleScreenBufferInfo( hStdout, &csbi ))
	  return;
	dwConSize = csbi.dwSize.X * csbi.dwSize.Y;

	if( !FillConsoleOutputCharacter( hStdout, (TCHAR) ' ',
	  dwConSize, coordScreen, &cCharsWritten ))
	  return;

	if( !GetConsoleScreenBufferInfo( hStdout, &csbi ))
	  return;

	if( !FillConsoleOutputAttribute( hStdout, csbi.wAttributes,
	  dwConSize, coordScreen, &cCharsWritten ))
	  return;

	SetConsoleCursorPosition( hStdout, coordScreen );
}



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
TBX_VOID TBXCliClsNoHome ( TBX_VOID )
{
	HANDLE						hStdout;
	COORD						coordScreen = { 0, 0 };
	DWORD						cCharsWritten;
	CONSOLE_SCREEN_BUFFER_INFO	csbi;
	DWORD						dwConSize;

	hStdout = GetStdHandle (STD_OUTPUT_HANDLE);
	if( !GetConsoleScreenBufferInfo( hStdout, &csbi ))
	  return;
	dwConSize = csbi.dwSize.X * csbi.dwSize.Y;

	GetConsoleCursorPosition( hStdout, &coordScreen );

	if( coordScreen.X <= csbi.dwSize.X && coordScreen.Y <= csbi.dwSize.Y )
	{

		if( !FillConsoleOutputCharacter( hStdout, (TCHAR) ' ',
		  dwConSize, coordScreen, &cCharsWritten ))
		  return;

		if( !GetConsoleScreenBufferInfo( hStdout, &csbi ))
		  return;

		if( !FillConsoleOutputAttribute( hStdout, csbi.wAttributes,
		  dwConSize, coordScreen, &cCharsWritten ))
		  return;

		coordScreen.X = coordScreen.Y = 0;
		SetConsoleCursorPosition( hStdout, coordScreen );
	}


}

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
TBX_VOID TBXCliClearLine ( TBX_VOID )
{
	HANDLE						hStdout;
	COORD						coordScreen = { 0, 0 };
	DWORD						cCharsWritten;
	CONSOLE_SCREEN_BUFFER_INFO	csbi;
	DWORD						dwConSize;

	hStdout = GetStdHandle (STD_OUTPUT_HANDLE);
	if( !GetConsoleScreenBufferInfo( hStdout, &csbi ))
	  return;
	dwConSize = csbi.dwSize.X;

	GetConsoleCursorPosition( hStdout, &coordScreen );
	coordScreen.X = 0;

	if( !FillConsoleOutputCharacter( hStdout, (TCHAR) ' ',
	  dwConSize, coordScreen, &cCharsWritten ))
	  return;

	if( !GetConsoleScreenBufferInfo( hStdout, &csbi ))
	  return;

	if( !FillConsoleOutputAttribute( hStdout, csbi.wAttributes,
	  dwConSize, coordScreen, &cCharsWritten ))
	  return;

	SetConsoleCursorPosition( hStdout, coordScreen );
}

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
TBX_VOID TBXCliClearLineNoHome ()
{
	HANDLE						hStdout;
	COORD						coordScreen = { 0, 0 };
	CONSOLE_SCREEN_BUFFER_INFO	csbi;

	hStdout = GetStdHandle (STD_OUTPUT_HANDLE);
	GetConsoleCursorPosition( hStdout, &coordScreen );

	if( !GetConsoleScreenBufferInfo( hStdout, &csbi ))
	  return;

	if( coordScreen.X < (SHORT)csbi.dwSize.X )
	{
		DWORD	dwNbToFill = (DWORD)(csbi.dwSize.X - coordScreen.X);
		DWORD	cCharsWritten;


		if( !FillConsoleOutputCharacter( hStdout, (TCHAR)' ',
		  dwNbToFill, coordScreen, &cCharsWritten ))
		  return;
	}
}



/*-------------------------------------------------------------------------------------------------------------------------------
 |
 |  TBXCli_vfprintf	:	This function handle ANSI escape sequence printing
 |
 |	Args			:	see vfprintf
 |
 |  Return          :	see vfprintf
 |
 *------------------------------------------------------------------------------------------------------------------------------*/

int TBXCli_vfprintf_output( const char *fmt, va_list argptr)
{
    HANDLE 			handle = NULL;
    unsigned long	written = 0;
    unsigned long	writtenTotal = 0;

	/* Buffer to format the string into */
    char			ibuf[2048];

	/* Pointers to scan through ibuf */
    char*			p;
    char*			q;

	/* String length of p */
	unsigned long	un32Strlen;

	handle = GetStdHandle(STD_OUTPUT_HANDLE);

	/* Parse the buffer and arguments into final string */
	{
		int			iLen;

		iLen = vsnprintf( ibuf, sizeof(ibuf) - 1, fmt, argptr );

		/* Calculate actual string length */
		if( iLen == -1 )
			un32Strlen = sizeof(ibuf) - 1;
		else
			un32Strlen = (TBX_UINT32)iLen;

		/* Force NULL-termination */
		ibuf[ sizeof(ibuf) - 1 ] = '\0';

	}

	/* Search and skip any color codes */
	p = ibuf;
	while ((q = strchr(p, 0x1B /* ESC */ )) != NULL)
	{
		short color = 0;

		if( q-p )
		{
			WriteConsole(handle, p, q-p, &written, 0); /* Write text between two escape sequences */
			writtenTotal += written;
		}

		un32Strlen -= q-p;	/* Update string length with what's left */
		p = q;

		while (*p && *p != 'm' )
		{
			p++;
			un32Strlen--;

			if( !(*p) || *p == '[' || *p == ';' )
				continue;

			/* Home */
			if( *p == HOME_CHAR )
			{
				TBXCliHome();
				p++;
				un32Strlen--;
				break;
			}

			/* Clear Line */
			if( *p == CLEARLINE_CHAR )
			{
				TBXCliClearLine();
				p++;
				un32Strlen--;
				break;
			}

			/* Clear Screen */
			if( p[0] == CLEARSCR_CHAR0 && p[1] == CLEARSCR_CHAR0 )
			{
				TBXCliCls();
				p += 2;
				un32Strlen -= 2;
				break;
			}

			switch( *p )
			{
				case '1':
				{
					p++;
					un32Strlen--;
					/* 1: Foreground Attribute: High */
					color |= FOREGROUND_INTENSITY;
				} break;

				case '2':
				{
					p++;
					un32Strlen--;
					switch( *p )
					{
						case '0':
						{
							p++;
							un32Strlen--;
							/* 2: Foreground color: Gray (hack) */
							color |= FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE;
						} break;
					}
				} break;

				case '3':
				{
					p++;
					un32Strlen--;
					switch( *p )
					{
						case '0':
						{
							p++;
							un32Strlen--;
							/* 30: Foreground color: Black */
							/* Not supported */
						} break;

						case '1':
						{
							p++;
							un32Strlen--;
							/* 31: Foreground color: Red */
							color |= FOREGROUND_RED | FOREGROUND_INTENSITY;
						} break;

						case '2':
						{
							p++;
							un32Strlen--;
							/* 32: Foreground color: Green */
							color |= FOREGROUND_GREEN | FOREGROUND_INTENSITY;
						} break;

						case '3':
						{
							p++;
							un32Strlen--;
							/* 33: Foreground color: Yellow */
							color |= FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY;
						} break;

						case '4':
						{
							p++;
							un32Strlen--;
							/* 34: Foreground color: Blue */
							color |= FOREGROUND_BLUE | FOREGROUND_INTENSITY;
						} break;

						case '5':
						{
							p++;
							un32Strlen--;
							/* 35: Foreground color: Magenta */
							color |= FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_INTENSITY;
						} break;

						case '6':
						{
							p++;
							un32Strlen--;
							/* 36: Foreground color: Cyan */
							color |= FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY;
						} break;

						case '7':
						{
							p++;
							un32Strlen--;
							/* 37: Foreground color: White */
							color |= FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY;
						} break;

						case '8':
						{
							p++;
							un32Strlen--;
							/* 38: Foreground color: Not supported */
						} break;

						case '9':
						{
							p++;
							un32Strlen--;
							/* 39: Foreground color: Not supported */
						} break;

						default:
						{
							/* 3: Foreground Attribute: High */
							color |= FOREGROUND_INTENSITY;
						} break;
					}
				} break;

				case '4':
				{
					p++;
					un32Strlen--;
					switch( *p )
					{
						case '0':
						{
							p++;
							un32Strlen--;
							/* 40: Foreground color: Black */
							/* Not supported */
						} break;

						case '1':
						{
							p++;
							un32Strlen--;
							/* 41: Background color: Red */
							color |= BACKGROUND_RED;
						} break;

						case '2':
						{
							p++;
							un32Strlen--;
							/* 42: Background color: Green */
							color |= BACKGROUND_GREEN;
						} break;

						case '3':
						{
							p++;
							un32Strlen--;
							/* 43: Background color: Yellow */
							color |= BACKGROUND_RED | BACKGROUND_GREEN;
						} break;

						case '4':
						{
							p++;
							un32Strlen--;
							/* 44: Background color: Blue */
							color |= BACKGROUND_BLUE;
						} break;

						case '5':
						{
							p++;
							un32Strlen--;
							/* 45: Background color: Magenta */
							color |= BACKGROUND_RED | BACKGROUND_BLUE;
						} break;

						case '6':
						{
							p++;
							un32Strlen--;
							/* 46: Background color: Cyan */
							color |= BACKGROUND_GREEN | BACKGROUND_BLUE;
						} break;

						case '7':
						{
							p++;
							un32Strlen--;
							/* 47: Background color: White */
							color |= BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_BLUE;
						} break;

						case '8':
						{
							p++;
							un32Strlen--;
							/* 48: Background color: Not supported */
						} break;

						case '9':
						{
							p++;
							un32Strlen--;
							/* 49: Background color: Not supported */
						} break;

						default:
						{
							/* 4: Foregroud Attribute: Underscore */
							color |= COMMON_LVB_UNDERSCORE;
						} break;
					}
				} break;

				case '5':
				{
					p++;
					un32Strlen--;
					/* 5: Foreground Attribute: Blink */
					/* Not supported */
				} break;

				case '7':
				{
					p++;
					un32Strlen--;
					/* 7: Foreground Attribute: Revrs */
					color |= COMMON_LVB_REVERSE_VIDEO;
				} break;

				case '8':
				{
					p++;
					un32Strlen--;
					/* 8: Foreground Attribute: Invisible */
					/* Not supported */
				} break;

				default:
				{
					/* Other color, use white as default */
					color |= FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE;
				} break;
			}

			/* Skip remainder of color code */
			while ( *p != ';' && *p != 'm' && *p )
			{
				p++;
				un32Strlen--;
			}
		}

		/* Modify Console attributes only if this is a modify command */
		if( *p == 'm' )
		{
			static short s_LastColor = 0;
			if( color != s_LastColor )
			{
				s_LastColor = color;
				SetConsoleTextAttribute(handle, color);
			}
			p++;
			un32Strlen--;
		}
	}

	if (*p)
	{
		WriteConsole(handle, p, un32Strlen, &written, 0);
		writtenTotal += written;
	}

    return writtenTotal;
}





int
TBXCli_vfprintf( FILE *file , const char *fmt, va_list argptr)
{
    if (!file || file == stdout || file == stderr)
	{
		return TBXCli_vfprintf_output( fmt, argptr );
	}
	else
	{
		return TBXCli_vfprintf_file( file, fmt, argptr );
	}
}


/*-------------------------------------------------------------------------------------------------------------------------------
 |
 |  TBXCli_fprintf	:	This function handle ANSI escape sequence printing
 |
 |	Args			:	see fprintf
 |
 |  Return          :	see fprintf
 |
 *------------------------------------------------------------------------------------------------------------------------------*/

int
TBXCli_fprintf( FILE * file, const char *fmt, ... )
{
	va_list	argptr;
	int		written;

    va_start(argptr, fmt);
	written = TBXCli_vfprintf( file, fmt, argptr );
    va_end(argptr);

	return written;
}

/*-------------------------------------------------------------------------------------------------------------------------------
 |
 |  TBXCli_printf	:	This function handle ANSI escape sequence printing
 |
 |	Args			:	see fprintf
 |
 |  Return          :	see fprintf
 |
 *------------------------------------------------------------------------------------------------------------------------------*/

int
TBXCli_printf( const char *fmt, ... )
{
	va_list	argptr;
	int		written;

    va_start(argptr, fmt);
	written = TBXCli_vfprintf( stdout, fmt, argptr );
    va_end(argptr);

	return written;
}






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
TBX_UINT32 TBXCliGetScreenWidth ()
{
	CONSOLE_SCREEN_BUFFER_INFO	csbi;
	HANDLE						hStdout;

	hStdout = GetStdHandle (STD_OUTPUT_HANDLE);
	if( !GetConsoleScreenBufferInfo( hStdout, &csbi ))
	  return 0;

	return (TBX_UINT32)csbi.srWindow.Right - csbi.srWindow.Left;
}

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
TBX_UINT32 TBXCliGetScreenHeight ()
{
	CONSOLE_SCREEN_BUFFER_INFO	csbi;
	HANDLE						hStdout;

	hStdout = GetStdHandle (STD_OUTPUT_HANDLE);
	if( !GetConsoleScreenBufferInfo( hStdout, &csbi ))
	  return 0;

	return (TBX_UINT32)csbi.srWindow.Bottom - csbi.srWindow.Top;
}

#endif /* #ifndef WIN32 */

int TBXCli_vfprintf_file
(
	IN		FILE*		file,
	IN		const char*	fmt,
	IN		va_list		argptr
)
{
    char			ibuf[2048];
	int				iStrlen;

	/* Render the string into the buffer */
	iStrlen = TBXCli_vsnprintf
	(
		ibuf,
		sizeof(ibuf) - 1,
		fmt,
		argptr
	);

	return fwrite( ibuf, 1, iStrlen, file );
}

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
)
{
	TBX_UINT32	un32Strlen;

	/* Buffer to format the string into */
    char			ibuf[2048];

	/* Parse the buffer and arguments into final string */
	{

		vsnprintf( ibuf, sizeof(ibuf) - 1, fmt, argptr );
		/* Force NULL-termination */
		ibuf[ sizeof(ibuf) - 1 ] = '\0';

	}

	/* Copy to output buffer, while removing the color codes */
	un32Strlen = TBXCli_strncpy
	(
		out_pszBuffer,
		ibuf,
		in_un32MaxLen
	);

    return (int)un32Strlen;
}


int TBXCli_snprintf
(
	OUT		char*			out_pszBuffer,
	IN		unsigned long	in_un32MaxLen,
	IN		const char*		fmt,
	IN		...
)
{
	va_list	argptr;
	int		written;

    va_start(argptr, fmt);
	written = TBXCli_vsnprintf( out_pszBuffer, in_un32MaxLen, fmt, argptr );
    va_end(argptr);

	return written;
}



/*-------------------------------------------------------------------------------------------------------------------------------
 |
 |  TBXCliGetchRemapped:	This function returns a pressed key
 |
 |  ~					:	No arguments used
 |
 |  Note				:	~
 |
 |  Return				:	TBX_RESULT_OK
 |							TBX_RESULT_FAIL
 |
 *------------------------------------------------------------------------------------------------------------------------------*/
TBX_INT  TBXCliGetchRemapped (void)
{
	TBX_INT		c;

	/* Get the key pressed */
	c = TBXCliGetch();

	/* Remap special keys */
	c = TB640KeypadRemap(c);

	return c;
}


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
)
{
    unsigned long	writtenTotal = 0;

	/* Pointers to scan through in_pszString */
    const char*		p;
    char*			q;

	/* String length of p */
	unsigned long	un32Strlen	= strlen( in_pszString );

	/* Buffer to cumulate data to write to file to avoid multiple calls to fwrite */
	unsigned long	un32OutStrlen = 0;


	/* Search and skip any color codes */
	p = in_pszString;
	while ((q = strchr(p, 0x1B /* ESC */ )) != NULL)
	{
		/* Append to output buffer */
		if( q-p )
		{
			unsigned long un32ToCopy = in_un32MaxLen - un32OutStrlen - 1;
			if( un32ToCopy > (TBX_UINT32)(q-p) )
				un32ToCopy = (TBX_UINT32)(q-p);
			memcpy
			(
				out_pszString + un32OutStrlen,
				p,
				un32ToCopy
			);
			un32OutStrlen += un32ToCopy;
			writtenTotal += un32ToCopy;
		}

		un32Strlen -= q-p;	/* Update string length with what's left */
		p = q;

		while (*p && *p != 'm' )
		{
			p++;
			un32Strlen--;
		}

		if( *p == 'm' )
		{
			p++;
			un32Strlen--;
		}
	}

	if (*p)
	{
		unsigned long un32ToCopy = in_un32MaxLen - un32OutStrlen - 1;
		if( un32ToCopy > un32Strlen )
			un32ToCopy = un32Strlen;

		/* Append to output buffer */
		memcpy
		(
			out_pszString + un32OutStrlen,
			p,
			un32ToCopy
		);
		un32OutStrlen += un32ToCopy;
		writtenTotal += un32ToCopy;
	}

	/* Enforce NULL-terminating character */
	out_pszString[ un32OutStrlen ] = '\0';

    return writtenTotal;
}

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
TBX_UINT32 TBXCli_Strlen (const TBX_CHAR* in_pszString)
{
	TBX_UINT32	un32Strlen	= 0;
	TBX_UINT32	un32Index	= 0;

	while( in_pszString[ un32Index ] != '\0' )
	{
		if( in_pszString[ un32Index ] == '\033' )
		{
			/* This is an escape character. Skip it */
			un32Index++;
			while
			(
				( in_pszString[ un32Index ] != '\0' ) &&
				( in_pszString[ un32Index ] != 'm' )
			)
			{
				un32Index++;
			}
		}
		else
		{
			un32Strlen++;
		}
		un32Index++;
	}

	return un32Strlen;
}


/*-------------------------------------------------------------------------------------------------------------------------------
 |
 |  TBXCli STDIN Functions
 |
 *------------------------------------------------------------------------------------------------------------------------------*/

#if !defined(OSE) && !defined(VXWORKS) && !defined(MQX)

/*-------------------------------------------------------------------------------------------------------------------------------
 |
 |  TbxCliStdinRedirThread	:	Thread that handles Stdin redirections
 |
 |	pParams			:	Thread Parameters
 |
 |  Return          :	~
 |
 *------------------------------------------------------------------------------------------------------------------------------*/

static TBX_RESULT __stdcall
TbxCliStdinRedirThread(
  IN	PTBX_VOID	in_pParams )
{
	PSTDIN_HANDLER_PARAMS	pParams = in_pParams;
	int 					file_pipes[2];

	pipe(file_pipes);

	pParams->stdIn = dup( fileno(stdin) );
	dup2( file_pipes[TBX_PIPE_READ], fileno(stdin) );
	close( file_pipes[TBX_PIPE_READ] );

	while( !pParams->fQuit )
	{
		int		nRead;
		char    buf[ 513 ];
#ifndef WIN32
		fd_set	fdRead;
		struct timeval timeout;

		FD_ZERO(&fdRead);
		FD_SET(pParams->stdIn, &fdRead);
		timeout.tv_usec = 100 * 1000;
		timeout.tv_sec = 0;
		if( !select( pParams->stdIn+1, &fdRead, NULL, NULL, &timeout) )
			continue;
#endif /* #ifndef WIN32 */
		nRead = read(pParams->stdIn, buf, 512);
		if( nRead > 0 && nRead < 512 )
		{
			buf[nRead] = '\0';
			write( file_pipes[TBX_PIPE_WRITE], buf, nRead);
			if( pParams->pFct )
				pParams->pFct( pParams->pCtx, buf, nRead ); /* User Callback */

			/* File that needs to be played as STDIN is handled here */
			if( pParams->hFileToPlay )
			{
				while( (nRead = read(fileno(pParams->hFileToPlay), buf, 512)) > 0 )
				{
					if( nRead > 0 && nRead < 512 )
						write( file_pipes[TBX_PIPE_WRITE], buf, nRead);
					else
						break;
				}
				fclose(  pParams->hFileToPlay );
				pParams->hFileToPlay = NULL;
			}
		}
	}

	/* pParams->stdIn was close by the destroy call */
	close( file_pipes[TBX_PIPE_WRITE] );

	return TBX_RESULT_OK;
}

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
  IN	PTBX_VOID				pCtx )
{
	PSTDIN_HANDLER_PARAMS	pParams = &StdinHandlerCtx;

	if( !pParams->fInit )
	{
		pParams->pCtx 		= pCtx;
		pParams->pFct 		= pFct;
		pParams->hFileToPlay = NULL;
		
		//TBX_THREAD_CREATE(&pParams->StdinThread, TbxCliStdinRedirThread, pParams );

		pParams->StdinThread = (HANDLE)_beginthreadex
		(
			NULL,                        		/* default security attributes */
			0,                           		/* use default stack size */
			TbxCliStdinRedirThread,			/* thread function */
			(void*)(pParams),			 		/* argument to thread function */
			0,                           		/* use default creation flags */
			NULL								/* returns the thread identifier */
		);




		pParams->fInit		= TBX_TRUE;
	}
}

/*-------------------------------------------------------------------------------------------------------------------------------
 |
 |  TBXCliStdinHandlerDestroy:	Functions that is used to destroy Stdin Monitoring and Redirection
 |
 |  Return          :	~
 |
 *------------------------------------------------------------------------------------------------------------------------------*/

TBX_VOID
TBXCliStdinHandlerDestroy( TBX_VOID )
{
	PSTDIN_HANDLER_PARAMS	pParams = &StdinHandlerCtx;

	pParams->fQuit = TBX_TRUE;

	//TBX_THREAD_DESTROY( &pParams->StdinThread );
}

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
  FILE*					hFile )
{
	TBX_CHAR				szPrompt[4];
	PSTDIN_HANDLER_PARAMS	pParams = &StdinHandlerCtx;

	if( pParams->fInit )
	{
		pParams->hFileToPlay = hFile;
		fprintf(stdout,"Press Enter to start playing file\n");
		szPrompt[0] = '\0';
		fgets( szPrompt, 2, stdin );
	}
}
#else

TBX_VOID
TBXCliStdinHandlerCreate(
  IN	PFCT_STDIN_CALLBACK		pFct,
  IN	PTBX_VOID				pCtx )
{
}

TBX_VOID
TBXCliStdinHandlerDestroy( TBX_VOID )
{
}

TBX_VOID
TBXCliStdinPlayFile(
  FILE*					hFile )
{
}

#endif /* #if !defined(OSE) && !defined(VXWORKS) */
