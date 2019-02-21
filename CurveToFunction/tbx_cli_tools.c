/*--------------------------------------------------------------------------------------------------------------------------------
 |
 |	Project:    	TBX sample common sour code
 |
 |	Filename:   	tbx_cli_tools.c
 |
 |	Copyright:  	TelcoBridges 2002-2014, All Rights Reserved
 |
 |	Description:	This file contains the prototypes of a set of functions
 |					used to build command-line tools with menus, command prompt
 |					and scrollable log file.
 |
 |	Notes:      	Tabs = 4
 |
 *-------------------------------------------------------------------------------------------------------------------------------
 |
 |	Revision:   	$Revision: 150907 $
 |
 *------------------------------------------------------------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------------------------------------------------------------
 |  Includes
 *------------------------------------------------------------------------------------------------------------------------------*/
#include "tbx_cli_tools_private.h"
#include <process.h>

/*--------------------------------------------------------------------------------------------------------------------------------
 |  Forward declarations
 *------------------------------------------------------------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------------------------------------------------------------
 |  Defines
 *------------------------------------------------------------------------------------------------------------------------------*/
#define TBX_CLI_TOOLS_LINE_BUFFER_DEBUG	0
#define TBX_CLI_TOOLS_LINE_MAX_SIZE	2048


/*--------------------------------------------------------------------------------------------------------------------------------
 |  Types
 *------------------------------------------------------------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------------------------------------------------------------
 |  Global variables
 *------------------------------------------------------------------------------------------------------------------------------*/
TBX_BOOL	g_fTbxCliToolsReady				= TBX_FALSE;
#ifdef TBX_CLI_TOOLS_USED_CLEARLINE
PTBX_CHAR	g_pszEndline					= CLEARLINEEND;
#else
PTBX_CHAR	g_pszEndline					= "";
#endif
PTBX_CHAR	g_pszTrunkatedEndline			= ESC F_BLACK AND_COLOR B_WHITE END_COLOR TBX_CLI_TOOLS_LINE_OVERFLOW_SYMBOL DEFAULT_COLOR;
TBX_UINT32	g_un32EndlineStrlen				= 0;
TBX_UINT32	g_un32TrunkatedEndlineLen		= 0;
TBX_UINT32	g_un32TrunkatedEndlineStrlen	= 0;


#if defined(VXWORKS)
	extern				int	consoleFd;
	TB_C_FUNC_PROTOTYPE	int TBXCliVxworksStdin( void * );
	TBX_THREAD			g_AsyncStdinThread				= TBX_THREAD_INVALID;

	#if (CPU==SIMNT)
		FILE *			g_pConsoleOutputStream 			= NULL;
	#endif
#endif


/*--------------------------------------------------------------------------------------------------------------------------------
 |  Macros
 *------------------------------------------------------------------------------------------------------------------------------*/


int
TbxCliTools_strnicmp(
  IN 	const char *psz1,
  IN 	const char *psz2,
  IN 	int n )
{
	while( toupper(*psz1) == toupper(*psz2) && n )
	{
		if (*psz1 == 0)
		  return 0;

		psz1++;
		psz2++;
		n--;
	}

	if( n == 0 )
		return 0;

	return (toupper(*psz1) - toupper(*psz2));
}

char *
TbxCliTools_stristr(
  IN	char *psz1,
  IN	const char *psz2 )
{
	int		first;
	long 	len;

    if ( (first = *psz2++) != 0) {
		first = toupper(first);
        len = strlen(psz2);
        do {
            do {
                if ( *psz1 == 0) {
                    return NULL;
                }
            } while (toupper(*psz1++) != first);
        } while (TbxCliTools_strnicmp(psz1, psz2, len) != 0);
        psz1--;
    }
    return psz1;
}

/*--------------------------------------------------------------------------------------------------------------------------------
 |  Function Prototypes
 *------------------------------------------------------------------------------------------------------------------------------*/
static TBX_RESULT __stdcall TbxCliToolsCliThread( PTBX_VOID in_pCliToolsCtx );

static TBX_VOID TbxCliToolsLogSetCheckpointPrivate
(
	IN		PTBX_CLI_TOOLS_CLI_CONTEXT					in_pCliToolsCtx,
  IN		PTBX_CLI_TOOLS_CUSTOM_CHECKPOINT_PARAMS		in_pCustomCheckpointParams
);

static void TbxCliToolsLogFileSplitPrivate
(
	IN		PTBX_CLI_TOOLS_CLI_CONTEXT				pCliToolsCtx,
	IN		TBX_BOOL								fLogFileFull
);


TBX_BOOL TbxCliToolsIsSomeoneWatching
(
	IN		TBX_CLI_TOOLS_HANDLE					in_hCliToolsHandle
)
{
	PTBX_CLI_TOOLS_CLI_CONTEXT	pCliToolsCtx			= (PTBX_CLI_TOOLS_CLI_CONTEXT)in_hCliToolsHandle;
	TBX_BOOL					fSomeoneWatching		= TBX_FALSE;

	//if( pCliToolsCtx )
	//{
	//	TBX_MUTEX_GET (pCliToolsCtx->Mutex);
	//	fSomeoneWatching = TbxCliToolsIsSomeoneWatchingInternal( pCliToolsCtx );
	//	TBX_MUTEX_GIV (pCliToolsCtx->Mutex);
	//}
	return fSomeoneWatching;
}

/*--------------------------------------------------------------------------------------------------------------------------------
 |  Implementation
 *------------------------------------------------------------------------------------------------------------------------------*/
/*-------------------------------------------------------------------------------------------------------------------------------
 |
 |  TBXCliToolsLine_fprintf		:	Print a line on the screen, and on the remote screen too
 |
 |  in_pCliToolsCtx				:	Context of this library
 |	in_pszLine					:	Line to print, or NULL to just skip line if no relevant changes found
 |								
 |  Note						:	~
 |								
 |  Return						:	~
 |
 *------------------------------------------------------------------------------------------------------------------------------*/
TBX_VOID TBXCliToolsLine_fprintf
(
  IN		PTBX_CLI_TOOLS_CLI_CONTEXT			in_pCliToolsCtx,
  IN		PTBX_CHAR							in_pszLine
)
{
	if( in_pCliToolsCtx->un32RemoteScreenCurrentIdx < TBX_CLI_TOOLS_MAX_REMOTE_SCREEN_HOSTS )
	{
		/* We're actually controlling a remote screen. Don't display our own screen for now. */
	}
	else
	{
		if( in_pszLine )
		{
			if( !in_pCliToolsCtx->fActuallyRemoteControlled )
			{
				if( !in_pCliToolsCtx->Params.fDisableTerminalOutput )
					TBXCli_fprintf( stdout, DEFAULT_COLOR "%s\n", in_pszLine );
			}

			if( in_pCliToolsCtx->pEvtRemoteScreen )
			{
				/* Append to string */
				in_pCliToolsCtx->pEvtRemoteScreen->un32EvtRemoteScreenStrlen += TbxCliTools_snprintf
				(
					in_pCliToolsCtx->pEvtRemoteScreen->szScreen + in_pCliToolsCtx->pEvtRemoteScreen->un32EvtRemoteScreenStrlen,
					in_pCliToolsCtx->un32EvtRemoteScreenMaxStrlen - in_pCliToolsCtx->pEvtRemoteScreen->un32EvtRemoteScreenStrlen,
					"%s\r\n",
					in_pszLine
				);
			}
		}
		else
		{
			/* No update on this line, just go to next line */
			if( !in_pCliToolsCtx->fActuallyRemoteControlled )
			{
				if( !in_pCliToolsCtx->Params.fDisableTerminalOutput )
					TBXCli_fprintf( stdout, DEFAULT_COLOR "\r\n" );
			}
			else if( in_pCliToolsCtx->pEvtRemoteScreen )
			{
				/* Append to string */
				in_pCliToolsCtx->pEvtRemoteScreen->un32EvtRemoteScreenStrlen += TbxCliTools_snprintf
				(
					in_pCliToolsCtx->pEvtRemoteScreen->szScreen + in_pCliToolsCtx->pEvtRemoteScreen->un32EvtRemoteScreenStrlen,
					in_pCliToolsCtx->un32EvtRemoteScreenMaxStrlen - in_pCliToolsCtx->pEvtRemoteScreen->un32EvtRemoteScreenStrlen,
					"\r\n"
				);
			}
		}
	}

	return;
}

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
)
{
	TBX_RESULT							Result;
	PTBX_CLI_TOOLS_CLI_CONTEXT			pCliToolsCtx;
	TBX_UINT32							un32Index;

	/*---------------------------------------------------------------------------------------------------------------------------
	 |  Code section
	 *--------------------------------------------------------------------------------------------------------------------------*/
	CODE
	{
		/* Initialize local variables */
		Result				= TBX_RESULT_OK;
		pCliToolsCtx		= NULL;

		/* Validate parameters */
		Result = TbxCliToolsValidateParams (NULL, in_pParams);
		if (TBX_RESULT_FAILURE (Result))
		{
			TBX_EXIT_ERROR( TBX_RESULT_INVALID_PARAM, 0, "Validate params failed" );
		}

		/* Initialize some global variables */
		if( !g_un32EndlineStrlen )
		{
			g_un32EndlineStrlen				= strlen( g_pszEndline );
			g_un32TrunkatedEndlineStrlen	= strlen( g_pszTrunkatedEndline );
			g_un32TrunkatedEndlineLen		= strlen( TBX_CLI_TOOLS_LINE_OVERFLOW_SYMBOL );
		}

		/* Allocate the context */
		pCliToolsCtx = (PTBX_CLI_TOOLS_CLI_CONTEXT)calloc( sizeof(*pCliToolsCtx), 1 );
		if( !pCliToolsCtx )
		{
			TBX_EXIT_ERROR( TBX_RESULT_OUT_OF_MEMORY, 0, "Could not allocate context!" );
		}
		pCliToolsCtx->un32StartTimestamp	= TBX_GET_TICK_PRECISE();
		pCliToolsCtx->un32CurrentTimestamp	= TBX_GET_TICK_PRECISE();

		pCliToolsCtx->fPrintAllowed = TBX_TRUE;

		/* Check if this application is launched by the watchdog in service mode */
		//{
		//	PTBX_CHAR pszVal;

		//	pszVal = getenv( TBX_ENV_VAR_NAME_SERVICE_MODE_ENV );
		//	if( pszVal || !TBX_IS_FOREGROUND_MODE() )
		//	{
		//		/* This application must run in service mode */
		//		in_pParams->fDisableTerminalOutput	= TBX_TRUE;
		//		in_pParams->fDisableTerminalInput	= TBX_TRUE;
		//	}
		//}

		/* Store the parameters */
		pCliToolsCtx->Params = *in_pParams;


		/* Allocate 2 display buffer */
		pCliToolsCtx->pDisplayBuf = (PTBX_CLI_TOOLS_DISPLAY_BUFFER)calloc( sizeof(*pCliToolsCtx->pDisplayBuf), 1 );
		if( !pCliToolsCtx->pDisplayBuf )
		{
			TBX_EXIT_ERROR( TBX_RESULT_OUT_OF_MEMORY, 0, "Could not allocate display buffer!" );
		}
		/* Allocate and clear display buffer */
		for( un32Index = 0; un32Index < in_pParams->un32MaxScreenHeight; un32Index++ )
		{
			PTBX_CLI_TOOLS_DISPLAY_LINE_PRIVATE pTempLine = &( pCliToolsCtx->pDisplayBuf->aLines[ un32Index ] );
			pTempLine->pszLine = (PTBX_CHAR)
				malloc( in_pParams->un32MaxScreenWidth + 4 );
			if( !pTempLine->pszLine )
			{
				TBX_EXIT_ERROR (TBX_RESULT_OUT_OF_MEMORY, 0, "Failed to allocate log buffer.");
			}
			pTempLine->pszLine[0] = '\0';
		}

		pCliToolsCtx->pPrevDisplayBuf = (PTBX_CLI_TOOLS_DISPLAY_BUFFER)calloc( sizeof(*pCliToolsCtx->pPrevDisplayBuf), 1 );
		if( !pCliToolsCtx->pPrevDisplayBuf )
		{
			TBX_EXIT_ERROR( TBX_RESULT_OUT_OF_MEMORY, 0, "Could not allocate display buffer!" );
		}
		/* Allocate and clear display buffer */
		for( un32Index = 0; un32Index < in_pParams->un32MaxScreenHeight; un32Index++ )
		{
			PTBX_CLI_TOOLS_DISPLAY_LINE_PRIVATE pTempLine = &( pCliToolsCtx->pPrevDisplayBuf->aLines[ un32Index ] );

			pTempLine->pszLine = (PTBX_CHAR)
				malloc( in_pParams->un32MaxScreenWidth + 4 );
			if( !pTempLine->pszLine )
			{
				TBX_EXIT_ERROR (TBX_RESULT_OUT_OF_MEMORY, 0, "Failed to allocate log buffer.");
			}
			pTempLine->pszLine[0] = '\0';
		}
		pCliToolsCtx->un32SelectedLineToBlink = 0xFFFFFFFF;

		/* Allocate the command input buffers, allocate one more to detect overflows */
		pCliToolsCtx->CmdInput.Public.paszPrompt = (PTBX_CLI_TOOLS_DISPLAY_LINE)calloc
			( in_pParams->un32MaxPromptLines + 1, sizeof(*pCliToolsCtx->CmdInput.Public.paszPrompt) );
		if( !pCliToolsCtx->CmdInput.Public.paszPrompt )
		{
			TBX_EXIT_ERROR( TBX_RESULT_OUT_OF_MEMORY, 0, "Could not allocate command input buffers!" );
		}

		pCliToolsCtx->CmdInput.Public.paszUserInput = (PTBX_CLI_TOOLS_DISPLAY_LINE)calloc
			( in_pParams->un32MaxPromptLines + 1, sizeof(*pCliToolsCtx->CmdInput.Public.paszUserInput) );
		if( !pCliToolsCtx->CmdInput.Public.paszUserInput )
		{
			TBX_EXIT_ERROR( TBX_RESULT_OUT_OF_MEMORY, 0, "Could not allocate command input buffers!" );
		}


		/* Allocate and circular log buffer */
		pCliToolsCtx->paCircularLog = (PTBX_CLI_TOOLS_DISPLAY_LINE_PRIVATE)calloc
		(
			pCliToolsCtx->Params.un32LogBufferMaxLines,
			sizeof( *pCliToolsCtx->paCircularLog )
		);
		if( !pCliToolsCtx->paCircularLog )
		{
			TBX_EXIT_ERROR (TBX_RESULT_OUT_OF_MEMORY, 0, "Failed to allocate log buffer.");
		}

		/* Allocate and clear circular log buffer */
		#if TBX_CLI_TOOLS_LINE_BUFFER_DEBUG
		{
			/* Allocate individual buffers for each line */
			for( un32Index = 0; un32Index < pCliToolsCtx->Params.un32LogBufferMaxLines; un32Index++ )
			{
				PTBX_CLI_TOOLS_DISPLAY_LINE_PRIVATE pTempLine = &( pCliToolsCtx->paCircularLog[ un32Index ] );
				pTempLine->pszLine = (PTBX_CHAR)
					malloc( in_pParams->un32MaxScreenWidth + 4 );
				if( !pTempLine->pszLine )
				{
					TBX_EXIT_ERROR (TBX_RESULT_OUT_OF_MEMORY, 0, "Failed to allocate log buffer.");
				}
				pTempLine->pszLine[0] = '\0';
			}
		}
		#else
		{
			TBX_UINT32	un32BufSizePerLine	= in_pParams->un32MaxScreenWidth + 4;

			/* Allocate one big buffer, and split it into lines */
			pCliToolsCtx->paCircularLog[ 0 ].pszLine = (PTBX_CHAR)
				malloc( pCliToolsCtx->Params.un32LogBufferMaxLines * un32BufSizePerLine );
			if( !pCliToolsCtx->paCircularLog[ 0 ].pszLine )
			{
				TBX_EXIT_ERROR (TBX_RESULT_OUT_OF_MEMORY, 0, "Failed to allocate log buffer.");
			}

			for( un32Index = 0; un32Index < pCliToolsCtx->Params.un32LogBufferMaxLines; un32Index++ )
			{
				pCliToolsCtx->paCircularLog[ un32Index ].pszLine =
					pCliToolsCtx->paCircularLog[ 0 ].pszLine +
					( un32Index * un32BufSizePerLine );
				pCliToolsCtx->paCircularLog[ un32Index ].pszLine[0] = '\0';
			}
		}
		#endif

		/* Allocate the escape chars flags array */
		pCliToolsCtx->pafEscapeChars = (PTBX_UINT8)
			malloc( in_pParams->un32MaxScreenWidth + 4 );
		if( !pCliToolsCtx->pafEscapeChars )
		{
			TBX_EXIT_ERROR (TBX_RESULT_OUT_OF_MEMORY, 0, "Failed to allocate log buffer.");
		}

		/* Allocate the array of circular log checkpoints */
		pCliToolsCtx->pafCircularLogCheckpoints = (PTBX_UINT8)calloc
		(
			pCliToolsCtx->Params.un32LogBufferMaxLines,
			sizeof( *pCliToolsCtx->pafCircularLogCheckpoints )
		);
		if( !pCliToolsCtx->pafCircularLogCheckpoints )
		{
			TBX_EXIT_ERROR (TBX_RESULT_OUT_OF_MEMORY, 0, "Failed to allocate log checkpoints.");
		}

		/* Allocate tmp buffer to build printed strings */
		pCliToolsCtx->pszTmpLine = (PTBX_CHAR)malloc( TBX_CLI_TOOLS_TMP_LINE_MAX_SIZE );
		if( !pCliToolsCtx->pszTmpLine )
		{
			TBX_EXIT_ERROR (TBX_RESULT_OUT_OF_MEMORY, 0, "Failed to allocate tmp line.");
		}
		pCliToolsCtx->pszTmpLine[0] = '\0';

		/* Create display semaphore */
		TBX_MUTEX_CREATE (&pCliToolsCtx->Mutex);

		TBX_MUTEX_GET (pCliToolsCtx->Mutex);

		pCliToolsCtx->un32RemoteScreenCurrentIdx = 0xFFFFFFFF;	/* Not actually monitoring remote host */
		//if( pCliToolsCtx->Params.hTbxLib )
		//{
		//	/* Start using hostlib with this tbx_cli_tools instance */
		//	TBXCliToolsRemoteStartUseHostlib( pCliToolsCtx );
		//}

		/* Open log file (perform initial log file rotation) */
		TbxCliToolsLogFilesRotation( pCliToolsCtx );

		/* Set an initial checkpoint */
		TbxCliToolsLogSetCheckpointPrivate( pCliToolsCtx, NULL );

		/* Enable mouse input */
		#if defined WIN32
		if( !in_pParams->fDisableTerminalInput )
		{
			HANDLE						hStdIn;
			TBX_BOOL					bSuccess;
			DWORD						dwStdInMode;

			hStdIn = GetStdHandle (STD_INPUT_HANDLE);

			bSuccess = GetConsoleMode(hStdIn, &dwStdInMode);
			if( !bSuccess )
			{
				TbxCliToolsLogPrint( (TBX_CLI_TOOLS_HANDLE)pCliToolsCtx, TRACE_LEVEL_1, "", "While enabling mouse input: Failed to GetConsoleMode\n" );
			}

			bSuccess = SetConsoleMode(hStdIn, dwStdInMode | ENABLE_MOUSE_INPUT);
			if( !bSuccess )
			{
				TbxCliToolsLogPrint( (TBX_CLI_TOOLS_HANDLE)pCliToolsCtx, TRACE_LEVEL_1, "", "While enabling mouse input: Failed to SetConsoleMode\n" );
			}
		}
		#endif

		/* Read some environment variables that override the parameters */
		{
			PTBX_CHAR	pszValue;
			TBX_UINT32	un32MinRefreshDelay = TBX_CLI_TOOLS_CLI_MIN_DELAY_BETWEEN_REFRESH_MS;
			TBX_UINT32	un32MaxRefreshDelay = TBX_CLI_TOOLS_CLI_MAX_DELAY_BETWEEN_REFRESH_MS;

			/* Get min refresh delay */
			pszValue = getenv( "TBX_CLI_MIN_REFRESH_MS" );
			if( pszValue != NULL )
			{
				un32MinRefreshDelay = strtoul( pszValue, NULL, 0 );

				if( un32MinRefreshDelay < TBX_CLI_TOOLS_CLI_MIN_DELAY_BETWEEN_REFRESH_MS )
				{
					TbxCliToolsLogPrint
					(
						(TBX_CLI_TOOLS_HANDLE)pCliToolsCtx, 	TRACE_LEVEL_ERROR, NULL,
						"WARNING: Invalid env. var. value (%s ms is too small) for TBX_CLI_MIN_REFRESH_MS. Will ignore (and use %u ms).\n",
						pszValue,
						(unsigned int)in_pParams->un32MinRefreshDelay
					);
					un32MinRefreshDelay = in_pParams->un32MinRefreshDelay;
				}
				else if( un32MinRefreshDelay > 10000 )
				{
					TbxCliToolsLogPrint
					(
						(TBX_CLI_TOOLS_HANDLE)pCliToolsCtx, 	TRACE_LEVEL_ERROR, NULL,
						"WARNING: Invalid env. var. value (%s ms is too big) for TBX_CLI_MIN_REFRESH_MS. Will ignore (and use %u ms).\n",
						pszValue,
						(unsigned int)in_pParams->un32MinRefreshDelay
					);
					un32MinRefreshDelay = in_pParams->un32MinRefreshDelay;
				}
				pCliToolsCtx->Params.un32MinRefreshDelay = un32MinRefreshDelay;
			}


			/* Get min refresh delay */
			pszValue = getenv( "TBX_CLI_MAX_REFRESH_MS" );
			if( pszValue != NULL )
			{
				un32MaxRefreshDelay = strtoul( pszValue, NULL, 0 );

				if( un32MaxRefreshDelay < TBX_CLI_TOOLS_CLI_MAX_DELAY_BETWEEN_REFRESH_MS )
				{
					TbxCliToolsLogPrint
					(
						(TBX_CLI_TOOLS_HANDLE)pCliToolsCtx, 	TRACE_LEVEL_ERROR, NULL,
						"WARNING: Invalid env. var. value (%s ms is too small) for TBX_CLI_MAX_REFRESH_MS. Will ignore (and use %u ms).\n",
						pszValue,
						(unsigned int)in_pParams->un32MaxRefreshDelay
					);
					un32MaxRefreshDelay = in_pParams->un32MaxRefreshDelay;
				}
				else if( un32MaxRefreshDelay > 10000 )
				{
					TbxCliToolsLogPrint
					(
						(TBX_CLI_TOOLS_HANDLE)pCliToolsCtx, 	TRACE_LEVEL_ERROR, NULL,
						"WARNING: Invalid env. var. value (%s ms is too big) for TBX_CLI_MAX_REFRESH_MS. Will ignore (and use %u ms).\n",
						pszValue,
						(unsigned int)in_pParams->un32MaxRefreshDelay
					);
					un32MaxRefreshDelay = in_pParams->un32MaxRefreshDelay;
				}
				pCliToolsCtx->Params.un32MaxRefreshDelay = un32MaxRefreshDelay;
			}
		}

		/* Open a file to redirect all output */
		#if defined(VXWORKS)
		{
			TBX_INT				fd;

			#if (CPU==SIMNT)
				/* Output is redirected to a file to get ANSI color codes and movement commands */
				g_pConsoleOutputStream = fopen (TBX_CLI_TOOLS_VXWORKS_SIM_OUTPUT_FILENAME, "w");
				if (!g_pConsoleOutputStream)
				{
					TBX_EXIT_ERROR (TBX_RESULT_ACCESS_DENIED, 0, "Unable to open Vxworks simulator tty output file");
				}
				else
				{
					int		termoptions;
					printf ("+-------------------------------------------------------------------+\n");
					printf ("| VxWorks Simulator TTY                                             |\n");
					printf ("+-------------------------------------------------------------------+\n");
					printf ("|  View the tty output using a shell running the following command: |\n");
					printf ("|     tail -f " TBX_CLI_TOOLS_VXWORKS_SIM_OUTPUT_FILENAME " | cat                             |\n");
					printf ("+-------------------------------------------------------------------+\n\n");
				}
			#endif

			/* Get our stdin file descriptor */
			fd = ioTaskStdGet(0, STD_IN);

			/* Create a separate thread to emulate a non-blocking stdin */
			TBX_THREAD_CREATE_VHPRIO (&g_AsyncStdinThread, TBXCliVxworksStdin, fd);
			if (g_AsyncStdinThread == TBX_THREAD_INVALID)
			{
				TBX_EXIT_ERROR (TBX_RESULT_OUT_OF_RESOURCE, 0, "Unable to create Vxworks async stdin thread");
			}

		}
		#endif

		g_fTbxCliToolsReady = TBX_TRUE;

		/* End of the code (skip to cleanup) */
		TBX_EXIT_SUCCESS (TBX_RESULT_OK);
	}

	/*---------------------------------------------------------------------------------------------------------------------------
	 |  Error handling section
	 *--------------------------------------------------------------------------------------------------------------------------*/
	ERROR_HANDLING
	{
		fprintf( stderr,
			"TbxCliToolsInit: %s (0x%08X %s:%d)\n",
			TBX_ERROR_DESCRIPTION,
			(int)TBX_ERROR_RESULT,
			TBX_ERROR_FILE,
			TBX_ERROR_LINE );

		if( pCliToolsCtx )
		{
			TbxCliToolsTerm( (TBX_CLI_TOOLS_HANDLE)pCliToolsCtx );
		}
		pCliToolsCtx = NULL;
	}

	/*---------------------------------------------------------------------------------------------------------------------------
	 |  Cleanup section
	 *--------------------------------------------------------------------------------------------------------------------------*/
	CLEANUP
	{
		if( out_phCliToolsHandle ) *out_phCliToolsHandle = (TBX_CLI_TOOLS_HANDLE)pCliToolsCtx;

		if( pCliToolsCtx && pCliToolsCtx->Mutex )
		{
			TBX_MUTEX_GIV (pCliToolsCtx->Mutex);
		}
	}

	RETURN;
}

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
)
{
	PTBX_CLI_TOOLS_CLI_CONTEXT			pCliToolsCtx;

	/*---------------------------------------------------------------------------------------------------------------------------
	 |  Code section
	 *--------------------------------------------------------------------------------------------------------------------------*/
	CODE
	{
		/* Initialize local variables */
		pCliToolsCtx		= (PTBX_CLI_TOOLS_CLI_CONTEXT)in_hCliToolsHandle;
		if( !pCliToolsCtx )
		{
			TBX_EXIT_ERROR( TBX_RESULT_INVALID_CONTEXT, 0, "Invalid library handle" );
		}

		TBX_MUTEX_GET (pCliToolsCtx->Mutex);

		/* Launch the CLI Thread */
		pCliToolsCtx->CliThreadContext.fRunning		= TBX_FALSE;
		pCliToolsCtx->CliThreadContext.fDone		= TBX_FALSE;
		pCliToolsCtx->CliThreadContext.fContinue	= TBX_TRUE;
		pCliToolsCtx->CliThreadContext.hThread		= TBX_THREAD_INVALID;

		if( pCliToolsCtx->Params.fLowPrioryThread )
		{
			//TBX_THREAD_CREATE_LPRIO
			//(
			//	&pCliToolsCtx->CliThreadContext.hThread,
			//	TbxCliToolsCliThread,
			//	pCliToolsCtx
			//);

			pCliToolsCtx->CliThreadContext.hThread = (HANDLE)_beginthreadex
			(
				NULL,                        		/* default security attributes */
				0,                           		/* use default stack size */
				TbxCliToolsCliThread,			/* thread function */
				(void*)(pCliToolsCtx),			 		/* argument to thread function */
				0,                           		/* use default creation flags */
				NULL								/* returns the thread identifier */
			);

			SetThreadPriority(pCliToolsCtx->CliThreadContext.hThread, THREAD_MODE_BACKGROUND_END);


	/*		_beginthreadex(NULL,0,&TbxCliToolsCliThread,NULL,0,pCliToolsCtx->CliThreadContext.hThread);
			TBX_THREAD_CREATE
			(
				&pCliToolsCtx->CliThreadContext.hThread,
				TbxCliToolsCliThread,
				pCliToolsCtx
			);*/
		}

		/* End of the code (skip to cleanup) */
		TBX_EXIT_SUCCESS (TBX_RESULT_OK);
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
		if( pCliToolsCtx && pCliToolsCtx->Mutex )
			TBX_MUTEX_GIV (pCliToolsCtx->Mutex);
	}

	RETURN;
}

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
)
{
	PTBX_CLI_TOOLS_CLI_CONTEXT			pCliToolsCtx;

	/*---------------------------------------------------------------------------------------------------------------------------
	 |  Code section
	 *--------------------------------------------------------------------------------------------------------------------------*/
	CODE
	{
		/* Initialize local variables */
		pCliToolsCtx		= (PTBX_CLI_TOOLS_CLI_CONTEXT)in_hCliToolsHandle;
		if( !pCliToolsCtx )
		{
			TBX_EXIT_ERROR( TBX_RESULT_INVALID_CONTEXT, 0, "Invalid library handle" );
		}

		TBX_MUTEX_GET (pCliToolsCtx->Mutex);

		/* Stop the CLI thread */
		if( pCliToolsCtx->CliThreadContext.hThread )
		{
			pCliToolsCtx->CliThreadContext.fContinue	= TBX_FALSE;
			pCliToolsCtx->fGenerateRemoteScreen			= TBX_FALSE;


			/* Wait until the stats thread has terminated */
			while( !pCliToolsCtx->CliThreadContext.fDone )
			{
				TBX_MUTEX_GIV (pCliToolsCtx->Mutex);
				TBX_SLEEP_MS( 10 );
				TBX_MUTEX_GET (pCliToolsCtx->Mutex);
			}

			if( !pCliToolsCtx->Params.fDisableTerminalOutput )
			{
				/* Clear the screen, now that this library is no more responsible to draw */
				TBXCliCls();
			}
		}
		if( pCliToolsCtx->CliThreadContext.hThread != TBX_THREAD_INVALID )
		{
			//TBX_THREAD_DESTROY( &pCliToolsCtx->CliThreadContext.hThread );
			pCliToolsCtx->CliThreadContext.hThread = TBX_THREAD_INVALID;
		}

		/* End of the code (skip to cleanup) */
		TBX_EXIT_SUCCESS (TBX_RESULT_OK);
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
		if( pCliToolsCtx && pCliToolsCtx->Mutex )
			TBX_MUTEX_GIV (pCliToolsCtx->Mutex);
	}

	RETURN;
}


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
)
{
	PTBX_CLI_TOOLS_CLI_CONTEXT			pCliToolsCtx;
	TBX_UINT32							un32Index;

	/*---------------------------------------------------------------------------------------------------------------------------
	 |  Code section
	 *--------------------------------------------------------------------------------------------------------------------------*/
	CODE
	{
		/* Initialize local variables */
		pCliToolsCtx		= (PTBX_CLI_TOOLS_CLI_CONTEXT)in_hCliToolsHandle;
		if( !pCliToolsCtx )
		{
			TBX_EXIT_ERROR( TBX_RESULT_INVALID_CONTEXT, 0, "Invalid library handle" );
		}

		TBX_MUTEX_GET (pCliToolsCtx->Mutex);

		g_fTbxCliToolsReady = TBX_FALSE;

		/* Close the tty file */
		#if defined(VXWORKS)
		{
			TBX_INT				fd;

			/* Put back shell in TERMINAL mode */
			fd = ioTaskStdGet(0, STD_IN);
			(void) ioctl (fd, FIOSETOPTIONS, OPT_TERMINAL);

			/* Create a separate thread to emulate a non-blocking stdin */
			if (g_AsyncStdinThread != TBX_THREAD_INVALID)
			{
				TBX_THREAD_DESTROY (&g_AsyncStdinThread);
				g_AsyncStdinThread = TBX_THREAD_INVALID;
			}

			#if (CPU==SIMNT)
				if (g_pConsoleOutputStream)
				{
					printf ("+-------------------------------------------------------------------+\n");
					printf ("| VxWorks Simulator TTY is now closed!                              |\n");
					printf ("+-------------------------------------------------------------------+\n");

					fclose(g_pConsoleOutputStream);
					g_pConsoleOutputStream = NULL;
				}
			#endif
		}
		#endif

		TbxCliToolsStop( in_hCliToolsHandle );
		
		/* Detach from hostlib, if appropriate */
		//TBXCliToolsRemoteStopUseHostlib( pCliToolsCtx );

		/* Destroy display output semaphore */
		TBX_MUTEX_DESTROY (&pCliToolsCtx->Mutex);

		/* Flush the disk write buffer to disk */
		TbxCliToolsFlushDiskWriteBuffer( pCliToolsCtx, TBX_TRUE );

		#ifdef TBX_CLI_TOOLS_USE_LOG_FILE
		/* Free the disk write buffer */
		if( pCliToolsCtx->pszDiskWriteBuffer )
		{
			free( pCliToolsCtx->pszDiskWriteBuffer );
			pCliToolsCtx->pszDiskWriteBuffer = NULL;
		}
		#endif

		/* Free temporary line */
		if( pCliToolsCtx->pszTmpLine )
		{
			free( pCliToolsCtx->pszTmpLine );
			pCliToolsCtx->pszTmpLine = NULL;
		}

		/* Free the log checkpoints */
		if( pCliToolsCtx->pafCircularLogCheckpoints )
		{
			free( pCliToolsCtx->pafCircularLogCheckpoints );
			pCliToolsCtx->pafCircularLogCheckpoints = NULL;
		}

		/* Free flags array */
		if( pCliToolsCtx->pafEscapeChars )
		{
			free( pCliToolsCtx->pafEscapeChars );
			pCliToolsCtx->pafEscapeChars = NULL;
		}

		/* Free the log buffer */
		if( pCliToolsCtx->paCircularLog )
		{
			#if TBX_CLI_TOOLS_LINE_BUFFER_DEBUG
			{
				/* Free individually allocated buffer lines */
				for( un32Index = 0; un32Index < pCliToolsCtx->Params.un32LogBufferMaxLines; un32Index++ )
				{
					if( pCliToolsCtx->paCircularLog[ un32Index ].pszLine )
					{
						free( pCliToolsCtx->paCircularLog[ un32Index ].pszLine );
						pCliToolsCtx->paCircularLog[ un32Index ].pszLine = NULL;
					}
				}
			}
			#else
			{
				/* Free the one buffer line */
				free( pCliToolsCtx->paCircularLog[ 0 ].pszLine );

				/* Make each line pointer NULL */
				for( un32Index = 0; un32Index < pCliToolsCtx->Params.un32LogBufferMaxLines; un32Index++ )
				{
					if( pCliToolsCtx->paCircularLog[ un32Index ].pszLine )
					{
						pCliToolsCtx->paCircularLog[ un32Index ].pszLine = NULL;
					}
				}
			}
			#endif

			free( pCliToolsCtx->paCircularLog );
			pCliToolsCtx->paCircularLog = NULL;
		}

		/* Free the command prompt buffers */
		if( pCliToolsCtx->CmdInput.Public.paszUserInput )
		{
			free( pCliToolsCtx->CmdInput.Public.paszUserInput );
			pCliToolsCtx->CmdInput.Public.paszUserInput = NULL;
		}
		if( pCliToolsCtx->CmdInput.Public.paszPrompt )
		{
			free( pCliToolsCtx->CmdInput.Public.paszPrompt );
			pCliToolsCtx->CmdInput.Public.paszPrompt = NULL;
		}

		/* Free the display buffers */
		if( pCliToolsCtx->pPrevDisplayBuf )
		{
			PTBX_CLI_TOOLS_DISPLAY_BUFFER pTempDisplay = pCliToolsCtx->pPrevDisplayBuf;

			for( un32Index = 0; un32Index < pCliToolsCtx->Params.un32MaxScreenHeight; un32Index++ )
			{
				if( pTempDisplay->aLines[ un32Index ].pszLine )
				{
					free( pTempDisplay->aLines[ un32Index ].pszLine );
					pTempDisplay->aLines[ un32Index ].pszLine = NULL;
				}
			}

			free( pCliToolsCtx->pPrevDisplayBuf );
			pCliToolsCtx->pPrevDisplayBuf = NULL;
		}

		if( pCliToolsCtx->pDisplayBuf )
		{
			PTBX_CLI_TOOLS_DISPLAY_BUFFER pTempDisplay = pCliToolsCtx->pDisplayBuf;

			for( un32Index = 0; un32Index < pCliToolsCtx->Params.un32MaxScreenHeight; un32Index++ )
			{
				if( pTempDisplay->aLines[ un32Index ].pszLine )
				{
					free( pTempDisplay->aLines[ un32Index ].pszLine );
					pTempDisplay->aLines[ un32Index ].pszLine = NULL;
				}
			}

			free( pCliToolsCtx->pDisplayBuf );
			pCliToolsCtx->pDisplayBuf = NULL;
		}

		free( pCliToolsCtx );
		pCliToolsCtx = NULL;

		/* End of the code (skip to cleanup) */
		TBX_EXIT_SUCCESS (TBX_RESULT_OK);
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

	RETURN;
}

/*-------------------------------------------------------------------------------------------------------------------------------
 |
 |  TbxCliToolsGetParams			:	Get the current TBX CLI Library parameters.
 |
 |	in_hCliToolsHandle				:	Handle of the allocated TBX CLI library
 |	out_pParams						:	Returns current parameters
 |	out_pun32CurrentWidth			:	Current CLI width
 |	out_pun32CurrentHeight			:	Current CLI height
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
)
{
	PTBX_CLI_TOOLS_CLI_CONTEXT pCliToolsCtx	= (PTBX_CLI_TOOLS_CLI_CONTEXT)in_hCliToolsHandle;
	if( !pCliToolsCtx )
	{
		return TBX_RESULT_INVALID_CONTEXT;
	}

	TBX_MUTEX_GET (pCliToolsCtx->Mutex);
	if( out_pParams )
	{
		*out_pParams = pCliToolsCtx->Params;
	}
	if( out_pun32CurrentWidth ) *out_pun32CurrentWidth		= pCliToolsCtx->pDisplayBuf->un32CurrentWidth;
	if( out_pun32CurrentHeight ) *out_pun32CurrentHeight	= pCliToolsCtx->pDisplayBuf->un32CurrentHeight;
	TBX_MUTEX_GIV (pCliToolsCtx->Mutex);

	return TBX_RESULT_OK;
}


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
)
{
	PTBX_CLI_TOOLS_CLI_CONTEXT			pCliToolsCtx;
	TBX_BOOL							fClearScreen;

	/*---------------------------------------------------------------------------------------------------------------------------
	 |  Code section
	 *--------------------------------------------------------------------------------------------------------------------------*/
	CODE
	{
		/* Initialize local variables */
		pCliToolsCtx		= (PTBX_CLI_TOOLS_CLI_CONTEXT)in_hCliToolsHandle;
		fClearScreen		= TBX_FALSE;

		if( !pCliToolsCtx )
		{
			TBX_EXIT_ERROR( TBX_RESULT_INVALID_CONTEXT, 0, "Invalid library handle" );
		}

		TBX_MUTEX_GET (pCliToolsCtx->Mutex);

		/* Validate if non-changeable parameters were changed */
		if
		(

			(in_pParams->un32MaxScreenWidth != pCliToolsCtx->Params.un32MaxScreenWidth)			||
			(in_pParams->un32MaxScreenHeight != pCliToolsCtx->Params.un32MaxScreenHeight)		||
			(in_pParams->un32MaxPromptLines > pCliToolsCtx->Params.un32MaxPromptLines)			||
			(in_pParams->fLowPrioryThread != pCliToolsCtx->Params.fLowPrioryThread)				||
			(in_pParams->un32LogBufferMaxLines != pCliToolsCtx->Params.un32LogBufferMaxLines)
		)
		{
			TBX_EXIT_ERROR( TBX_RESULT_NOT_SUPPORTED, 0, "Parameter change not supported" );
		}

		/* Apply new parameters (those that can be changed live) */
		if( strcmp( in_pParams->szLogFileName, pCliToolsCtx->Params.szLogFileName ) != 0 )
		{
			/* Force log file rotation */
			strcpy( pCliToolsCtx->Params.szLogFileName, in_pParams->szLogFileName );
			TbxCliToolsLogFilesRotation( pCliToolsCtx );
		}

		//if( in_pParams->hTbxLib != pCliToolsCtx->Params.hTbxLib )
		//{
		//	/* Detach from previous hostlib */
		//	if( pCliToolsCtx->Params.hTbxLib )
		//	{
		//		TBXCliToolsRemoteStopUseHostlib( pCliToolsCtx );
		//	}
		//}

		/* Check if this application is launched by the watchdog in service mode */
		//{
		//	PTBX_CHAR pszVal;

		//	pszVal = getenv( TBX_ENV_VAR_NAME_SERVICE_MODE_ENV );
		//	if( pszVal || !TBX_IS_FOREGROUND_MODE() )
		//	{
		//		/* This application must run in service mode */
		//		in_pParams->fDisableTerminalOutput	= TBX_TRUE;
		//		in_pParams->fDisableTerminalInput	= TBX_TRUE;
		//	}
		//}

		if( in_pParams->fDisableTerminalOutput == TBX_TRUE
			&&
			pCliToolsCtx->Params.fDisableTerminalOutput == TBX_FALSE  )
		{
			/* Clear the screen before disabling terminal output */
			fClearScreen = TBX_TRUE;
		}

		/* Save new parameters */
		pCliToolsCtx->Params = *in_pParams;
		
		//if( pCliToolsCtx->Params.hTbxLib && !pCliToolsCtx->fUseHostlib )
		//{
		//	/* Start using the hostlib */
		//	TBXCliToolsRemoteStartUseHostlib( pCliToolsCtx );
		//}

		if( fClearScreen )
		{
			/* Clear the screen before disabling terminal output */
			TBXCliCls();
		}

		/* End of the code (skip to cleanup) */
		TBX_EXIT_SUCCESS (TBX_RESULT_OK);
	}

	/*---------------------------------------------------------------------------------------------------------------------------
	 |  Error handling section
	 *--------------------------------------------------------------------------------------------------------------------------*/
	ERROR_HANDLING
	{
		if (pCliToolsCtx)
		{
			TbxCliToolsLogPrint
			(
				(TBX_CLI_TOOLS_HANDLE)pCliToolsCtx,
				TRACE_LEVEL_ERROR, NULL,
				"TbxCliToolsSetParams: %s, result 0x%08X\n",
				TBX_ERROR_DESCRIPTION,
				(int)TBX_ERROR_RESULT
			);
		}
	}

	/*---------------------------------------------------------------------------------------------------------------------------
	 |  Cleanup section
	 *--------------------------------------------------------------------------------------------------------------------------*/
	CLEANUP
	{
		if( pCliToolsCtx && pCliToolsCtx->Mutex )
			TBX_MUTEX_GIV (pCliToolsCtx->Mutex);
	}

	RETURN;
}


/*-------------------------------------------------------------------------------------------------------------------------------
 |
 |  TbxCliToolsValidateParams		:	Validates the TBX CLI Library parameters.
 |
 |	in_hCliToolsHandle				:	Handle of the allocated TBX CLI library
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
)
{
	PTBX_CLI_TOOLS_CLI_CONTEXT			pCliToolsCtx;

	/*---------------------------------------------------------------------------------------------------------------------------
	 |  Code section
	 *--------------------------------------------------------------------------------------------------------------------------*/
	CODE
	{
		pCliToolsCtx		= (PTBX_CLI_TOOLS_CLI_CONTEXT)in_hCliToolsHandle;

		/* Validate parameters */
		if( in_pParams->un32MaxScreenWidth > TBX_CLI_TOOLS_CLI_SCREEN_WIDTH_MAX_PRIVATE )
		{
			TbxCliToolsLogPrint
			(
				(TBX_CLI_TOOLS_HANDLE)pCliToolsCtx, TRACE_LEVEL_ERROR, NULL,
				"un32MaxScreenWidth is too big (%u). Will use %u instead.\n",
				(unsigned int)in_pParams->un32MaxScreenWidth,
				(unsigned int)TBX_CLI_TOOLS_CLI_SCREEN_WIDTH_MAX_PRIVATE
			);
			in_pParams->un32MaxScreenWidth = TBX_CLI_TOOLS_CLI_SCREEN_WIDTH_MAX_PRIVATE;
		}

		if( in_pParams->un32MaxScreenHeight > TBX_CLI_TOOLS_CLI_SCREEN_HEIGHT_MAX )
		{
			TbxCliToolsLogPrint
			(
				(TBX_CLI_TOOLS_HANDLE)pCliToolsCtx, TRACE_LEVEL_ERROR, NULL,
				"un32MaxScreenHeight is too big (%u). Will use %u instead.\n",
				(unsigned int)in_pParams->un32MaxScreenHeight,
				(unsigned int)TBX_CLI_TOOLS_CLI_SCREEN_HEIGHT_MAX
			);
			in_pParams->un32MaxScreenHeight = TBX_CLI_TOOLS_CLI_SCREEN_HEIGHT_MAX;
		}

		if( in_pParams->un32MinScreenWidth < TBX_CLI_TOOLS_CLI_SCREEN_WIDTH_MIN )
		{
			TbxCliToolsLogPrint
			(
				(TBX_CLI_TOOLS_HANDLE)pCliToolsCtx, TRACE_LEVEL_ERROR, NULL,
				"un32MinScreenWidth is too small (%u). Will use %u instead.\n",
				(unsigned int)in_pParams->un32MinScreenWidth,
				(unsigned int)TBX_CLI_TOOLS_CLI_SCREEN_WIDTH_MIN
			);
			in_pParams->un32MinScreenWidth = TBX_CLI_TOOLS_CLI_SCREEN_WIDTH_MIN;
		}

		if( in_pParams->un32MinScreenHeight < TBX_CLI_TOOLS_CLI_SCREEN_HEIGHT_MIN )
		{
			TbxCliToolsLogPrint
			(
				(TBX_CLI_TOOLS_HANDLE)pCliToolsCtx, TRACE_LEVEL_ERROR, NULL,
				"un32MinScreenHeight is too small (%u). Will use %u instead.\n",
				(unsigned int)in_pParams->un32MinScreenHeight,
				(unsigned int)TBX_CLI_TOOLS_CLI_SCREEN_HEIGHT_MIN
			);
			in_pParams->un32MinScreenHeight = TBX_CLI_TOOLS_CLI_SCREEN_HEIGHT_MIN;
		}

		if( in_pParams->un32MaxPromptLines > TBX_CLI_TOOLS_CLI_MAX_PROMPT_LINES )
		{
			TbxCliToolsLogPrint
			(
				(TBX_CLI_TOOLS_HANDLE)pCliToolsCtx, TRACE_LEVEL_ERROR, NULL,
				"un32MaxPromptLines is too big (%u). Will use %u instead.\n",
				(unsigned int)in_pParams->un32MaxPromptLines,
				(unsigned int)TBX_CLI_TOOLS_CLI_MAX_PROMPT_LINES
			);
			in_pParams->un32MaxPromptLines = TBX_CLI_TOOLS_CLI_MAX_PROMPT_LINES;
		}

		if( in_pParams->un32LogBufferMaxLines < 2*in_pParams->un32MinScreenHeight )
		{
			TbxCliToolsLogPrint
			(
				(TBX_CLI_TOOLS_HANDLE)pCliToolsCtx, TRACE_LEVEL_ERROR, NULL,
				"un32LogBufferMaxLines is too small (%u). Will use %u instead.\n",
				(unsigned int)in_pParams->un32LogBufferMaxLines,
				(unsigned int)TBX_CLI_TOOLS_CLI_MAX_LOG_BUFFER_LINES
			);
			in_pParams->un32LogBufferMaxLines = 2*in_pParams->un32MinScreenHeight;
		}

		if( in_pParams->un32LogBufferMaxLines > TBX_CLI_TOOLS_CLI_MAX_LOG_BUFFER_LINES )
		{
			TbxCliToolsLogPrint
			(
				(TBX_CLI_TOOLS_HANDLE)pCliToolsCtx, TRACE_LEVEL_ERROR, NULL,
				"un32LogBufferMaxLines is too big (%u). Will use %u instead.\n",
				(unsigned int)in_pParams->un32LogBufferMaxLines,
				(unsigned int)TBX_CLI_TOOLS_CLI_MAX_LOG_BUFFER_LINES
			);
			in_pParams->un32LogBufferMaxLines = TBX_CLI_TOOLS_CLI_MAX_LOG_BUFFER_LINES;
		}

		if( in_pParams->un32MinRefreshDelay < TBX_CLI_TOOLS_CLI_MIN_DELAY_BETWEEN_REFRESH_MS )
		{
			TbxCliToolsLogPrint
			(
				(TBX_CLI_TOOLS_HANDLE)pCliToolsCtx, TRACE_LEVEL_ERROR, NULL,
				"un32MinRefreshDelay is too small (%u ms). Will use %u ms instead.\n",
				(unsigned int)in_pParams->un32MinRefreshDelay,
				(unsigned int)TBX_CLI_TOOLS_CLI_MIN_DELAY_BETWEEN_REFRESH_MS
			);
			in_pParams->un32MinRefreshDelay = TBX_CLI_TOOLS_CLI_MIN_DELAY_BETWEEN_REFRESH_MS;
		}

		if( in_pParams->un32MaxRefreshDelay > TBX_CLI_TOOLS_CLI_MAX_DELAY_BETWEEN_REFRESH_MS )
		{
			TbxCliToolsLogPrint
			(
				(TBX_CLI_TOOLS_HANDLE)pCliToolsCtx, TRACE_LEVEL_ERROR, NULL,
				"un32MaxRefreshDelay is too big (%u ms). Will use %u ms instead.\n",
				(unsigned int)in_pParams->un32MaxRefreshDelay,
				(unsigned int)TBX_CLI_TOOLS_CLI_MAX_DELAY_BETWEEN_REFRESH_MS
			);
			in_pParams->un32MaxRefreshDelay = TBX_CLI_TOOLS_CLI_MAX_DELAY_BETWEEN_REFRESH_MS;
		}

		if( (in_pParams->un32MinClsDelay > 0) && (in_pParams->un32MinClsDelay < TBX_CLI_TOOLS_CLI_MIN_DELAY_BETWEEN_CLS_SEC) )
		{
			TbxCliToolsLogPrint
			(
				(TBX_CLI_TOOLS_HANDLE)pCliToolsCtx, TRACE_LEVEL_ERROR, NULL,
				"un32MinClsDelay is too small (%u ms). Will use %u ms instead.\n",
				(unsigned int)in_pParams->un32MinClsDelay,
				(unsigned int)TBX_CLI_TOOLS_CLI_MIN_DELAY_BETWEEN_CLS_SEC
			);
			in_pParams->un32MinClsDelay = TBX_CLI_TOOLS_CLI_MIN_DELAY_BETWEEN_CLS_SEC;
		}

		#ifdef TBX_CLI_TOOLS_USE_LOG_FILE
		if( in_pParams->un32MaxLogFileSize < TBX_CLI_TOOLS_CLI_MAX_LOG_FILE_SIZE_MINVALUE )
		{
			TbxCliToolsLogPrint
			(
				(TBX_CLI_TOOLS_HANDLE)pCliToolsCtx, TRACE_LEVEL_ERROR, NULL,
				"un32MaxLogFileSize is too small (%u). Will use %u instead.\n",
				(unsigned int)in_pParams->un32MaxLogFileSize,
				(unsigned int)TBX_CLI_TOOLS_CLI_MAX_LOG_FILE_SIZE_MINVALUE
			);
			in_pParams->un32MaxLogFileSize = TBX_CLI_TOOLS_CLI_MAX_LOG_FILE_SIZE_MINVALUE;
		}
		#endif

		/* End of the code (skip to cleanup) */
		TBX_EXIT_SUCCESS (TBX_RESULT_OK);
	}

	/*---------------------------------------------------------------------------------------------------------------------------
	 |  Error handling section
	 *--------------------------------------------------------------------------------------------------------------------------*/
	ERROR_HANDLING
	{
		if (pCliToolsCtx)
		{
			TbxCliToolsLogPrint
			(
				(TBX_CLI_TOOLS_HANDLE)pCliToolsCtx,
				TRACE_LEVEL_ERROR, NULL,
				"TbxCliToolsValidateParams: %s, result 0x%08X\n",
				TBX_ERROR_DESCRIPTION,
				(int)TBX_ERROR_RESULT
			);
		}
		else
		{
			fprintf( stderr,
				"TbxCliToolsValidateParams: %s (0x%08X %s:%d)\n",
				TBX_ERROR_DESCRIPTION,
				(int)TBX_ERROR_RESULT,
				TBX_ERROR_FILE,
				TBX_ERROR_LINE );
		}
	}

	/*---------------------------------------------------------------------------------------------------------------------------
	 |  Cleanup section
	 *--------------------------------------------------------------------------------------------------------------------------*/
	CLEANUP
	{
	}

	RETURN;
}



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
)
{
	PTBX_CLI_TOOLS_CLI_CONTEXT			pCliToolsCtx;
	TBX_BOOL							fHandled;

	/*---------------------------------------------------------------------------------------------------------------------------
	 |  Code section
	 *--------------------------------------------------------------------------------------------------------------------------*/
	CODE
	{
		/* Initialize local variables */
		pCliToolsCtx		= (PTBX_CLI_TOOLS_CLI_CONTEXT)in_hCliToolsHandle;
		fHandled			= TBX_FALSE;

		if( !pCliToolsCtx )
		{
			TBX_EXIT_ERROR( TBX_RESULT_INVALID_CONTEXT, 0, "Invalid library handle" );
		}

		/* WARNING: We don't lock the semaphore in this function.
					We assume all sub-functions called here do lock the semaphore */

		switch( in_KeyPressed )
		{
			/* Display window page down */
			case TBX_CLI_TOOLS_DEFAULT_KEY_LOG_SCROLL_PAGE_DOWN:
			{
				TbxCliToolsLogScroll
				(
					in_hCliToolsHandle,
					TBX_FALSE,
					TBX_CLI_TOOLS_SCROLL_PAGE
				);
				fHandled = TBX_TRUE;
			} break;

			/* Display window page up */
			case TBX_CLI_TOOLS_DEFAULT_KEY_LOG_SCROLL_PAGE_UP:
			{
				TbxCliToolsLogScroll
				(
					in_hCliToolsHandle,
					TBX_TRUE,
					TBX_CLI_TOOLS_SCROLL_PAGE
				);
				fHandled = TBX_TRUE;
			} break;

			/* Display window home */
			case TBX_CLI_TOOLS_DEFAULT_KEY_LOG_SCROLL_BEGIN:
			{
				TbxCliToolsLogScroll
				(
					in_hCliToolsHandle,
					TBX_TRUE,
					TBX_CLI_TOOLS_SCROLL_END
				);
				fHandled = TBX_TRUE;
			} break;

			/* Display k line */
			case 107:    //char  'k'
			{
				//pCliToolsCtx->

				pCliToolsCtx->CmdInput.Public.CmdType = 8;

				strcpy( pCliToolsCtx->CmdInput.Public.paszPrompt[0].szLine,		"Reload TBConfig.txt? (Y/N): " );
				strcpy( pCliToolsCtx->CmdInput.Public.paszUserInput[0].szLine,	"N" );

				fHandled = TBX_TRUE;
			} break;

			/* Display window end */
			case TBX_CLI_TOOLS_DEFAULT_KEY_LOG_SCROLL_END:
			{
				TbxCliToolsLogScroll
				(
					in_hCliToolsHandle,
					TBX_FALSE,
					TBX_CLI_TOOLS_SCROLL_END
				);
				fHandled = TBX_TRUE;
			} break;

			/* Display window move up */
			case TBX_CLI_TOOLS_DEFAULT_KEY_LOG_SCROLL_UP:
			{
				TbxCliToolsLogScroll
				(
					in_hCliToolsHandle,
					TBX_TRUE,
					TBX_CLI_TOOLS_SCROLL_SPEED_DEFAULT
				);
				fHandled = TBX_TRUE;
			} break;

			/* Display window move down */
			case TBX_CLI_TOOLS_DEFAULT_KEY_LOG_SCROLL_DOWN:
			{
				TbxCliToolsLogScroll
				(
					in_hCliToolsHandle,
					TBX_FALSE,
					TBX_CLI_TOOLS_SCROLL_SPEED_DEFAULT
				);
				fHandled = TBX_TRUE;
			} break;

			/* Display window move up */
			case TBX_CLI_TOOLS_DEFAULT_KEY_LOG_SCROLL_UP_MOUSE:
			{
				TbxCliToolsLogScroll
				(
					in_hCliToolsHandle,
					TBX_TRUE,
					TBX_CLI_TOOLS_SCROLL_WHEEL_SPEED_DEFAULT
				);
				fHandled = TBX_TRUE;
			} break;

			/* Display window move down */
			case TBX_CLI_TOOLS_DEFAULT_KEY_LOG_SCROLL_DOWN_MOUSE:
			{
				TbxCliToolsLogScroll
				(
					in_hCliToolsHandle,
					TBX_FALSE,
					TBX_CLI_TOOLS_SCROLL_WHEEL_SPEED_DEFAULT
				);
				fHandled = TBX_TRUE;
			} break;

			/* Display window move left */
			case TBX_CLI_TOOLS_DEFAULT_KEY_LOG_SCROLL_LEFT:
			{
				TbxCliToolsLogScrollHorizontal
				(
					in_hCliToolsHandle,
					TBX_TRUE,
					TBX_CLI_TOOLS_SCROLL_SPEED_DEFAULT
				);
				fHandled = TBX_TRUE;
			} break;

//			case TBX_CLI_TOOLS_DEFAULT_KEY_LOG_SCROLL_LEFTMOST:
			case TBX_CLI_TOOLS_DEFAULT_KEY_LOG_SCROLL_LEFTMOST_2:
			{
				TbxCliToolsLogScrollHorizontal
				(
					in_hCliToolsHandle,
					TBX_TRUE,
					0xFFFFFFFF
				);
				fHandled = TBX_TRUE;
			} break;

			/* Display window move right */
			case TBX_CLI_TOOLS_DEFAULT_KEY_LOG_SCROLL_RIGHT:
			{
				TbxCliToolsLogScrollHorizontal
				(
					in_hCliToolsHandle,
					TBX_FALSE,
					TBX_CLI_TOOLS_SCROLL_SPEED_DEFAULT
				);
				fHandled = TBX_TRUE;
			} break;

			/* Display window move left */
			case TBX_CLI_TOOLS_DEFAULT_KEY_LOG_SCROLL_LEFT_MOUSE:
			{
				TbxCliToolsLogScrollHorizontal
				(
					in_hCliToolsHandle,
					TBX_TRUE,
					TBX_CLI_TOOLS_SCROLL_WHEEL_SPEED_DEFAULT
				);
				fHandled = TBX_TRUE;
			} break;

			/* Display window move right */
			case TBX_CLI_TOOLS_DEFAULT_KEY_LOG_SCROLL_RIGHT_MOUSE:
			{
				TbxCliToolsLogScrollHorizontal
				(
					in_hCliToolsHandle,
					TBX_FALSE,
					TBX_CLI_TOOLS_SCROLL_WHEEL_SPEED_DEFAULT
				);
				fHandled = TBX_TRUE;
			} break;

			case TBX_CLI_TOOLS_DEFAULT_CHECKPOINT_SET:
			{
				/* Add a checkpoint in the scrolling log */
				TbxCliToolsLogSetCheckpoint( in_hCliToolsHandle );
				fHandled = TBX_TRUE;
			} break;

			case TBX_CLI_TOOLS_DEFAULT_REFRESH_SCREEN:
			{
				TbxCliToolsNeedCls( in_hCliToolsHandle );
				fHandled = TBX_TRUE;
			} break;

		}


		/* End of the code (skip to cleanup) */
		TBX_EXIT_SUCCESS (TBX_RESULT_OK);
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

	return fHandled;
}

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
)
{
	PTBX_CLI_TOOLS_CLI_CONTEXT			pCliToolsCtx;
	TBX_UINT32							un32PushIndex;

	/*---------------------------------------------------------------------------------------------------------------------------
	 |  Code section
	 *--------------------------------------------------------------------------------------------------------------------------*/
	CODE
	{
		/* Initialize local variables */
		pCliToolsCtx		= (PTBX_CLI_TOOLS_CLI_CONTEXT)in_hCliToolsHandle;

		if( !pCliToolsCtx )
		{
			TBX_EXIT_ERROR( TBX_RESULT_INVALID_CONTEXT, 0, "Invalid library handle" );
		}

		TBX_MUTEX_GET (pCliToolsCtx->Mutex);

		/* Validate that there is space in the FIFO */
		if( pCliToolsCtx->un32ForcedKeysFifoCount >= TBX_CLI_TOOLS_MAX_FORCED_KEYS )
		{
			TBX_EXIT_ERROR( TBX_RESULT_BUFFER_TOO_SMALL, 0, "Too many keys in the fifo!" );
		}

		un32PushIndex = pCliToolsCtx->un32ForcedKeysFifoPopIndex + pCliToolsCtx->un32ForcedKeysFifoCount;
		un32PushIndex %= TBX_CLI_TOOLS_MAX_FORCED_KEYS;

		pCliToolsCtx->aForcedKeysFifo[ un32PushIndex ] = in_KeyPressed;
		pCliToolsCtx->un32ForcedKeysFifoCount++;

		/* End of the code (skip to cleanup) */
		TBX_EXIT_SUCCESS (TBX_RESULT_OK);
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
		if( pCliToolsCtx && pCliToolsCtx->Mutex )
			TBX_MUTEX_GIV (pCliToolsCtx->Mutex);
	}

	RETURN;
}


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
)
{
	PTBX_CLI_TOOLS_CLI_CONTEXT pCliToolsCtx	= (PTBX_CLI_TOOLS_CLI_CONTEXT)in_hCliToolsHandle;
	if( pCliToolsCtx )
	{
		pCliToolsCtx->fRefreshRequired = TBX_TRUE;
	}
}


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
)
{
	PTBX_CLI_TOOLS_CLI_CONTEXT pCliToolsCtx	= (PTBX_CLI_TOOLS_CLI_CONTEXT)in_hCliToolsHandle;
	if( pCliToolsCtx )
	{
		pCliToolsCtx->fClsRequired = TBX_TRUE;
	}
}

/*-------------------------------------------------------------------------------------------------------------------------------
 |
 |  TbxCliToolsLogClearBuffer		:	Tells the TBX CLI Tools library that a clear screen is required.
 |
 |	in_hCliToolsHandle				:	Handle of the allocated TBX CLI library
 |
 |  Note							:	Some parameters can't be changed live
 |
 |  Return							:	~
 |
 *------------------------------------------------------------------------------------------------------------------------------*/
TBX_VOID TbxCliToolsLogClearBuffer
(
	IN		TBX_CLI_TOOLS_HANDLE					in_hCliToolsHandle
)
{
	TBX_UINT32	un32Index;

	PTBX_CLI_TOOLS_CLI_CONTEXT pCliToolsCtx	= (PTBX_CLI_TOOLS_CLI_CONTEXT)in_hCliToolsHandle;
	if( pCliToolsCtx )
	{
		TBX_MUTEX_GET (pCliToolsCtx->Mutex);

		/* Reinitialise log buffer */
		pCliToolsCtx->un32CircularLogNextLineIndex = 0;
		pCliToolsCtx->un32CircularLogCurrentNbLines = 0;
		pCliToolsCtx->un32LastLogDisplayNbLines = 0;
		pCliToolsCtx->un32SelectedLineToBlink = 0xFFFFFFFF;
		pCliToolsCtx->un32ScrollPosition = 0;
		pCliToolsCtx->un32ScrollPositionMax = 0;

		if (pCliToolsCtx->pafCircularLogCheckpoints)
		{
			/* Remove all checkpoints */
			memset (pCliToolsCtx->pafCircularLogCheckpoints, 0, pCliToolsCtx->Params.un32LogBufferMaxLines * sizeof( *pCliToolsCtx->pafCircularLogCheckpoints));
		}

		/* Clear all log lines */
		for( un32Index = 0; un32Index < pCliToolsCtx->Params.un32LogBufferMaxLines; un32Index++ )
		{
			pCliToolsCtx->paCircularLog[ un32Index ].pszLine[0] = '\0';
		}

		/* Put back initial checkpoint */
		TbxCliToolsLogSetCheckpointPrivate (pCliToolsCtx, NULL);

		pCliToolsCtx->fRefreshRequired = TBX_TRUE;

		TBX_MUTEX_GIV (pCliToolsCtx->Mutex);
	}
}


/*-------------------------------------------------------------------------------------------------------------------------------
 |
 |  TbxCliToolsLogScroll			:	This function scrolls up/down the log in the log window
 |
 |	in_hCliToolsHandle				:	Handle of the allocated TBX CLI library
 |	in_fUp							:	Scroll UP or Down
 |	in_un32Nb						:	Number of lines to scroll.
 |											(TBX_UINT32)-1	=> Scroll to end
 |											(TBX_UINT32)-2	=> Scroll one "page"
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
)
{
	PTBX_CLI_TOOLS_CLI_CONTEXT pCliToolsCtx	= (PTBX_CLI_TOOLS_CLI_CONTEXT)in_hCliToolsHandle;
	if( pCliToolsCtx )
	{
		TBX_UINT32	un32ElapsedMs;

		TBX_MUTEX_GET (pCliToolsCtx->Mutex);

		/* Update scroll speed */
		un32ElapsedMs = pCliToolsCtx->un32CurrentTimestamp - pCliToolsCtx->un32ScrollSpeedTimestamp;
		un32ElapsedMs *= TBX_MSEC_PER_TICKS;
		pCliToolsCtx->un32ScrollSpeedTimestamp = pCliToolsCtx->un32CurrentTimestamp;

		if( in_un32Nb != (TBX_UINT32)-1 )
		{
			/* Update scroll speed */
			if( un32ElapsedMs < 100 )
			{
				/* User pressed key very close */
				if( pCliToolsCtx->un32ScrollSpeedAccelerateTimestamp == 0 )
				{
					pCliToolsCtx->un32ScrollSpeedAccelerateTimestamp = pCliToolsCtx->un32CurrentTimestamp;
					pCliToolsCtx->un32ScrollSpeed = 1;
				}
				else
				{
					TBX_UINT32	un32ElapsedSinceAccelStart = pCliToolsCtx->un32CurrentTimestamp - pCliToolsCtx->un32ScrollSpeedAccelerateTimestamp;
					un32ElapsedSinceAccelStart *= TBX_MSEC_PER_TICKS;

					if( un32ElapsedSinceAccelStart > TBX_CLI_TOOLS_SCROLL_ACCELERATE_DELAY_MS )
					{
						TBX_UINT32	un32MaxFactor = TBX_CLI_TOOLS_SCROLL_ACCELERATE_MAX_FACTOR;
						if( in_un32Nb == (TBX_UINT32)-2 )
						{
							un32MaxFactor = TBX_CLI_TOOLS_SCROLL_ACCELERATE_MAX_FACTOR_PAGE_UP;
						}

						un32ElapsedSinceAccelStart -= TBX_CLI_TOOLS_SCROLL_ACCELERATE_DELAY_MS;
						un32ElapsedSinceAccelStart += TBX_CLI_TOOLS_SCROLL_ACCELERATE_PERIOD_MS;
						if( un32ElapsedSinceAccelStart >= un32MaxFactor*TBX_CLI_TOOLS_SCROLL_ACCELERATE_PERIOD_MS )
						{
							un32ElapsedSinceAccelStart = un32MaxFactor*TBX_CLI_TOOLS_SCROLL_ACCELERATE_PERIOD_MS;
						}
						pCliToolsCtx->un32ScrollSpeed =
							(un32ElapsedSinceAccelStart * un32ElapsedSinceAccelStart) / (TBX_CLI_TOOLS_SCROLL_ACCELERATE_PERIOD_MS*TBX_CLI_TOOLS_SCROLL_ACCELERATE_PERIOD_MS);
						if( !pCliToolsCtx->un32ScrollSpeed )
							pCliToolsCtx->un32ScrollSpeed = 1;
					}
					else
					{
						pCliToolsCtx->un32ScrollSpeed = 1;
					}
				}
			}
			else
			{
				/* User stopped pressing key rapidly */
				pCliToolsCtx->un32ScrollSpeedAccelerateTimestamp = 0;
				pCliToolsCtx->un32ScrollSpeed = 1;
			}
		}
		else
		{
			pCliToolsCtx->un32ScrollSpeed = 1;
		}

		if( in_fUp )
		{
			if( in_un32Nb == (TBX_UINT32)-1 )
			{
				pCliToolsCtx->un32ScrollPosition = 0xFFFFFFFF;
			}
			else if( in_un32Nb == (TBX_UINT32)-2 )
			{
				pCliToolsCtx->un32ScrollPosition += pCliToolsCtx->un32NbLogLinesDisplayed * pCliToolsCtx->un32ScrollSpeed;
			}
			else
			{
				pCliToolsCtx->un32ScrollPosition += in_un32Nb * pCliToolsCtx->un32ScrollSpeed;
			}

			if( pCliToolsCtx->un32ScrollPosition > pCliToolsCtx->un32ScrollPositionMax )
				pCliToolsCtx->un32ScrollPosition = pCliToolsCtx->un32ScrollPositionMax;
		}
		else
		{
			if( in_un32Nb == (TBX_UINT32)-1 )
			{
				pCliToolsCtx->un32ScrollPosition = 0;
			}
			else if( in_un32Nb == (TBX_UINT32)-2 )
			{
				if( pCliToolsCtx->un32ScrollPosition > pCliToolsCtx->un32NbLogLinesDisplayed * pCliToolsCtx->un32ScrollSpeed )
					pCliToolsCtx->un32ScrollPosition -= pCliToolsCtx->un32NbLogLinesDisplayed * pCliToolsCtx->un32ScrollSpeed;
				else
					pCliToolsCtx->un32ScrollPosition = 0;
			}
			else
			{
				if( pCliToolsCtx->un32ScrollPosition > (in_un32Nb * pCliToolsCtx->un32ScrollSpeed) )
					pCliToolsCtx->un32ScrollPosition -= in_un32Nb * pCliToolsCtx->un32ScrollSpeed;
				else
					pCliToolsCtx->un32ScrollPosition = 0;
			}
		}

		pCliToolsCtx->fRefreshRequired = TBX_TRUE;

		TBX_MUTEX_GIV (pCliToolsCtx->Mutex);
	}
}

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
)
{
	PTBX_CLI_TOOLS_CLI_CONTEXT pCliToolsCtx	= (PTBX_CLI_TOOLS_CLI_CONTEXT)in_hCliToolsHandle;
	if( pCliToolsCtx )
	{
		TBX_UINT32	un32ElapsedMs;

		TBX_MUTEX_GET (pCliToolsCtx->Mutex);

		/* Update scroll speed */
		un32ElapsedMs = pCliToolsCtx->un32CurrentTimestamp - pCliToolsCtx->un32ScrollSpeedTimestamp;
		un32ElapsedMs *= TBX_MSEC_PER_TICKS;
		pCliToolsCtx->un32ScrollSpeedTimestamp = pCliToolsCtx->un32CurrentTimestamp;

		if( in_un32Nb != (TBX_UINT32)-1 )
		{
			/* Update scroll speed */
			if( un32ElapsedMs < 100 )
			{
				/* User pressed key very close */
				if( pCliToolsCtx->un32ScrollSpeedAccelerateTimestamp == 0 )
				{
					pCliToolsCtx->un32ScrollSpeedAccelerateTimestamp = pCliToolsCtx->un32CurrentTimestamp;
					pCliToolsCtx->un32ScrollSpeed = 1;
				}
				else
				{
					TBX_UINT32	un32ElapsedSinceAccelStart = pCliToolsCtx->un32CurrentTimestamp - pCliToolsCtx->un32ScrollSpeedAccelerateTimestamp;
					un32ElapsedSinceAccelStart *= TBX_MSEC_PER_TICKS;

					if( un32ElapsedSinceAccelStart > TBX_CLI_TOOLS_SCROLL_ACCELERATE_DELAY_MS )
					{
						TBX_UINT32	un32MaxFactor = TBX_CLI_TOOLS_SCROLL_ACCELERATE_MAX_FACTOR;
						if( in_un32Nb == (TBX_UINT32)-2 )
						{
							un32MaxFactor = TBX_CLI_TOOLS_SCROLL_ACCELERATE_MAX_FACTOR_PAGE_UP;
						}

						un32ElapsedSinceAccelStart -= TBX_CLI_TOOLS_SCROLL_ACCELERATE_DELAY_MS;
						un32ElapsedSinceAccelStart += TBX_CLI_TOOLS_SCROLL_ACCELERATE_PERIOD_MS;
						if( un32ElapsedSinceAccelStart >= un32MaxFactor*TBX_CLI_TOOLS_SCROLL_ACCELERATE_PERIOD_MS )
						{
							un32ElapsedSinceAccelStart = un32MaxFactor*TBX_CLI_TOOLS_SCROLL_ACCELERATE_PERIOD_MS;
						}
						pCliToolsCtx->un32ScrollSpeed =
							(un32ElapsedSinceAccelStart * un32ElapsedSinceAccelStart) / (TBX_CLI_TOOLS_SCROLL_ACCELERATE_PERIOD_MS*TBX_CLI_TOOLS_SCROLL_ACCELERATE_PERIOD_MS);
						if( !pCliToolsCtx->un32ScrollSpeed )
							pCliToolsCtx->un32ScrollSpeed = 1;
					}
					else
					{
						pCliToolsCtx->un32ScrollSpeed = 1;
					}
				}
			}
			else
			{
				/* User stopped pressing key rapidly */
				pCliToolsCtx->un32ScrollSpeedAccelerateTimestamp = 0;
				pCliToolsCtx->un32ScrollSpeed = 1;
			}
		}
		else
		{
			pCliToolsCtx->un32ScrollSpeed = 1;
		}

		if( in_fLeft )
		{
			if( pCliToolsCtx->un32ScrollPositionHor > (in_un32Nb * pCliToolsCtx->un32ScrollSpeed) )
				pCliToolsCtx->un32ScrollPositionHor -= in_un32Nb * pCliToolsCtx->un32ScrollSpeed;
			else
				pCliToolsCtx->un32ScrollPositionHor = 0;
		}
		else
		{
			pCliToolsCtx->un32ScrollPositionHor += in_un32Nb * pCliToolsCtx->un32ScrollSpeed;
		}

		pCliToolsCtx->fRefreshRequired = TBX_TRUE;

		TBX_MUTEX_GIV (pCliToolsCtx->Mutex);
	}
}


/*-------------------------------------------------------------------------------------------------------------------------------
 |
 |  TbxCliToolsLogSetCheckpoint		:	This function set a log checkpoint to the current
 |										log position.
 |
 |  in_pCliToolsCtx					:	Context of this library
 |
 |  Note							:	~
 |
 |  Return							:	TBX_RESULT_OK
 |										TBX_RESULT_FAIL
 |
 *------------------------------------------------------------------------------------------------------------------------------*/
static TBX_VOID TbxCliToolsLogSetCheckpointPrivate
(
  IN		PTBX_CLI_TOOLS_CLI_CONTEXT				in_pCliToolsCtx,
  IN		PTBX_CLI_TOOLS_CUSTOM_CHECKPOINT_PARAMS	in_pCustomCheckpointParams
)
{
	PTBX_CLI_TOOLS_CUSTOM_CHECKPOINT_PARAMS		pCustomCheckpointParams = in_pCustomCheckpointParams;
	TBX_CLI_TOOLS_CUSTOM_CHECKPOINT_PARAMS		CustomCheckpointParams;
	TBX_CHAR									szCheckpointString[ 256 ];
	TBX_UINT32									un32LineToMark;

	if( !pCustomCheckpointParams )
	{
		pCustomCheckpointParams = &CustomCheckpointParams;

		memset( &CustomCheckpointParams, 0, sizeof( CustomCheckpointParams ) );

		snprintf
		(
			szCheckpointString,
			sizeof(szCheckpointString),
			"\n*************** CHECKPOINT %d *****************\n\n",
			(int)in_pCliToolsCtx->un32CheckpointCounter
		);
		szCheckpointString[ sizeof(szCheckpointString) - 1 ] = '\0';
		CustomCheckpointParams.pszCheckpointString	= szCheckpointString;
		CustomCheckpointParams.un32LinesOffset		= 3;	/* Just printed 3 lines */
	}

	if( pCustomCheckpointParams->pszCheckpointString && *pCustomCheckpointParams->pszCheckpointString )
	{
		TbxCliToolsLogPrint
		(
			(TBX_CLI_TOOLS_HANDLE)in_pCliToolsCtx, TRACE_LEVEL_ALWAYS, FMAGEN,
			pCustomCheckpointParams->pszCheckpointString
		);
	}

	if( in_pCliToolsCtx->un32CircularLogNextLineIndex )
	{
		un32LineToMark = 
		(
			in_pCliToolsCtx->un32CircularLogNextLineIndex
			+ in_pCliToolsCtx->Params.un32LogBufferMaxLines
			- pCustomCheckpointParams->un32LinesOffset - 1
		) % in_pCliToolsCtx->Params.un32LogBufferMaxLines;
	}
	else
	{
		un32LineToMark = 
		(
			in_pCliToolsCtx->Params.un32LogBufferMaxLines
			+ in_pCliToolsCtx->Params.un32LogBufferMaxLines
			- pCustomCheckpointParams->un32LinesOffset - 1
		) % in_pCliToolsCtx->Params.un32LogBufferMaxLines;
	}

	in_pCliToolsCtx->pafCircularLogCheckpoints[ un32LineToMark ] = TBX_TRUE;
	in_pCliToolsCtx->un32CheckpointCounter++;
	in_pCliToolsCtx->fRefreshRequired = TBX_TRUE;
}

TBX_VOID TbxCliToolsLogSetCheckpoint
(
	IN		TBX_CLI_TOOLS_HANDLE					in_hCliToolsHandle
)
{
	PTBX_CLI_TOOLS_CLI_CONTEXT pCliToolsCtx	= (PTBX_CLI_TOOLS_CLI_CONTEXT)in_hCliToolsHandle;
	if( pCliToolsCtx )
	{
		TBX_MUTEX_GET (pCliToolsCtx->Mutex);
		{
			TbxCliToolsLogSetCheckpointPrivate( pCliToolsCtx, NULL );
		}
		TBX_MUTEX_GIV (pCliToolsCtx->Mutex);
	}
}

TBX_VOID TbxCliToolsLogSetCustomCheckpoint
(
	IN		TBX_CLI_TOOLS_HANDLE					in_hCliToolsHandle,
	IN		PTBX_CLI_TOOLS_CUSTOM_CHECKPOINT_PARAMS	in_pCustomCheckpointParams
)
{
	PTBX_CLI_TOOLS_CLI_CONTEXT pCliToolsCtx	= (PTBX_CLI_TOOLS_CLI_CONTEXT)in_hCliToolsHandle;
	if( pCliToolsCtx )
	{
		TBX_MUTEX_GET (pCliToolsCtx->Mutex);
		{
			TbxCliToolsLogSetCheckpointPrivate( pCliToolsCtx,in_pCustomCheckpointParams );
		}
		TBX_MUTEX_GIV (pCliToolsCtx->Mutex);
	}
}

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
)
{
	PTBX_CLI_TOOLS_CLI_CONTEXT pCliToolsCtx	= (PTBX_CLI_TOOLS_CLI_CONTEXT)in_hCliToolsHandle;
	if( pCliToolsCtx )
	{
		TBX_MUTEX_GET (pCliToolsCtx->Mutex);
		{
			TBX_UINT32	un32Checkpoint	= pCliToolsCtx->un32ScrollPosition;
			TBX_UINT32	un32LoopCount	= 0;
			TBX_UINT32	un32CircularLogIndex;
			TBX_UINT32	un32MoveUp		= 0;
			TBX_BOOL	fMove		= TBX_TRUE;

			/* Make sure we align the checkpoint to center of screen */
			un32MoveUp = pCliToolsCtx->un32LastLogDisplayNbLines / 2;

			if( in_fNext )
			{
				if( un32Checkpoint > pCliToolsCtx->Params.un32LogBufferMaxLines )
					un32Checkpoint = pCliToolsCtx->Params.un32LogBufferMaxLines;

				/* Convert checkpoint index (scroll position) into position in log array */
				un32CircularLogIndex =
					pCliToolsCtx->Params.un32LogBufferMaxLines +
					(pCliToolsCtx->un32CircularLogNextLineIndex - un32Checkpoint);
				un32CircularLogIndex += pCliToolsCtx->Params.un32LogBufferMaxLines - un32MoveUp;
				un32CircularLogIndex %= pCliToolsCtx->Params.un32LogBufferMaxLines;

				while( TBX_TRUE )
				{
					(un32CircularLogIndex)++;
					(un32CircularLogIndex) %= pCliToolsCtx->Params.un32LogBufferMaxLines;

					if( un32CircularLogIndex == pCliToolsCtx->un32CircularLogNextLineIndex )
					{
						/* Don't move pass end/start of log */
						fMove = TBX_FALSE;
						break;
					}
					if( pCliToolsCtx->pafCircularLogCheckpoints[ un32CircularLogIndex ] )
					{
						/* Found next checkpoint */
						pCliToolsCtx->un32SelectedLineToBlink =
							un32CircularLogIndex + 2;	/* +2 because the ***** CHECKPOINT ***** line is 2 after */
						pCliToolsCtx->un32SelectedLineToBlink %= pCliToolsCtx->Params.un32LogBufferMaxLines;
						break;
					}

					un32LoopCount++;
					if( un32LoopCount >= pCliToolsCtx->Params.un32LogBufferMaxLines )
					{
						un32CircularLogIndex	= (TBX_UINT32)-1;
						break;
					}
				}
			}
			else
			{
				if( un32Checkpoint > pCliToolsCtx->Params.un32LogBufferMaxLines )
					un32Checkpoint = pCliToolsCtx->Params.un32LogBufferMaxLines;

				/* Convert checkpoint index (scroll position) into position in log array */
				un32CircularLogIndex =
					pCliToolsCtx->Params.un32LogBufferMaxLines +
					(pCliToolsCtx->un32CircularLogNextLineIndex - un32Checkpoint);
				un32CircularLogIndex += pCliToolsCtx->Params.un32LogBufferMaxLines - un32MoveUp;
				un32CircularLogIndex %= pCliToolsCtx->Params.un32LogBufferMaxLines;


				while( TBX_TRUE )
				{
					if( un32CircularLogIndex )
						(un32CircularLogIndex)--;
					else
						un32CircularLogIndex = pCliToolsCtx->Params.un32LogBufferMaxLines - 1;

					if( un32CircularLogIndex == pCliToolsCtx->un32CircularLogNextLineIndex )
					{
						/* Don't move pass end/start of log */
						fMove = TBX_FALSE;
						break;
					}

					if( pCliToolsCtx->pafCircularLogCheckpoints[ un32CircularLogIndex ] )
					{
						/* Found prev checkpoint */
						/* Found next checkpoint */
						pCliToolsCtx->un32SelectedLineToBlink =
							un32CircularLogIndex + 2;	/* +2 because the ***** CHECKPOINT ***** line is 2 after */
						pCliToolsCtx->un32SelectedLineToBlink %= pCliToolsCtx->Params.un32LogBufferMaxLines;
						break;
					}

					un32LoopCount++;
					if( un32LoopCount >= pCliToolsCtx->Params.un32LogBufferMaxLines )
					{
						un32CircularLogIndex	= (TBX_UINT32)-1;
						break;
					}
				}
			}

			if( fMove )
			{
				if( un32CircularLogIndex == (TBX_UINT32)-1 )
				{
					un32Checkpoint = 0;
				}
				else
				{

					/* Convert circular buffer index into scroll position */
					un32Checkpoint =
						pCliToolsCtx->Params.un32LogBufferMaxLines +
						(pCliToolsCtx->un32CircularLogNextLineIndex - un32CircularLogIndex);
					(un32Checkpoint) %= pCliToolsCtx->Params.un32LogBufferMaxLines;

					/* Move up so the checkpoint is at the center of display  */
					if( un32Checkpoint > un32MoveUp )
						un32Checkpoint -= un32MoveUp;
					else
						un32Checkpoint = 0;
				}

				pCliToolsCtx->un32ScrollPosition = un32Checkpoint;

				if( pCliToolsCtx->un32ScrollPosition > pCliToolsCtx->un32ScrollPositionMax )
					pCliToolsCtx->un32ScrollPosition = pCliToolsCtx->un32ScrollPositionMax;
			}
		}
		pCliToolsCtx->fRefreshRequired = TBX_TRUE;
		TBX_MUTEX_GIV (pCliToolsCtx->Mutex);
	}
}


/*-------------------------------------------------------------------------------------------------------------------------------
 |
 |  TbxCliToolsLogGotoString		:	This function moves to log to the next/prev occurrence
 |										of the specified string.
 |
 |	in_pszToFind					:	String to find
 |  in_fNext						:	Get next checkpoint (else previous)
 |										to select line to blink.
 |  fMatchFirstCharOnly				:	Match only the lines that start with the specified string.
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
	IN		TBX_BOOL								in_fMatchFirstCharOnly
)
{
	PTBX_CLI_TOOLS_CLI_CONTEXT pCliToolsCtx	= (PTBX_CLI_TOOLS_CLI_CONTEXT)in_hCliToolsHandle;
	if( pCliToolsCtx )
	{
		TBX_MUTEX_GET (pCliToolsCtx->Mutex);
		{
			TBX_UINT32	un32Checkpoint			= pCliToolsCtx->un32ScrollPosition;
			TBX_UINT32	un32LoopCount			= 0;
			TBX_UINT32	un32CircularLogIndex	= 0;
			TBX_UINT32	un32MoveUp				= 0;
			TBX_BOOL	fMove					= TBX_TRUE;
			PTBX_CHAR	pszFoundString			= NULL;

			/* Make sure we align the checkpoint to center of screen */
			un32MoveUp = pCliToolsCtx->un32LastLogDisplayNbLines / 2;

			if( in_fNext )
			{
				if( un32Checkpoint > pCliToolsCtx->Params.un32LogBufferMaxLines )
					un32Checkpoint = pCliToolsCtx->Params.un32LogBufferMaxLines;

				/* Convert checkpoint index (scroll position) into position in log array */
				un32CircularLogIndex =
					pCliToolsCtx->Params.un32LogBufferMaxLines +
					(pCliToolsCtx->un32CircularLogNextLineIndex - un32Checkpoint);
				un32CircularLogIndex += pCliToolsCtx->Params.un32LogBufferMaxLines - un32MoveUp;
				un32CircularLogIndex %= pCliToolsCtx->Params.un32LogBufferMaxLines;

				while( TBX_TRUE )
				{
					(un32CircularLogIndex)++;
					(un32CircularLogIndex) %= pCliToolsCtx->Params.un32LogBufferMaxLines;

					if( un32CircularLogIndex == pCliToolsCtx->un32CircularLogNextLineIndex )
					{
						/* Don't move pass end/start of log */
						fMove = TBX_FALSE;
						break;
					}
					pszFoundString = TbxCliTools_stristr( pCliToolsCtx->paCircularLog[ un32CircularLogIndex ].pszLine, in_pszToFind );
					if
					(
						(
							in_fMatchFirstCharOnly &&
							pszFoundString == pCliToolsCtx->paCircularLog[ un32CircularLogIndex ].pszLine
						)
						||
						(
							!in_fMatchFirstCharOnly &&
							pszFoundString != NULL
						)
					)
					{
						/* Found string */
						pCliToolsCtx->un32SelectedLineToBlink = un32CircularLogIndex;
						break;
					}

					un32LoopCount++;
					if( un32LoopCount >= pCliToolsCtx->Params.un32LogBufferMaxLines )
					{
						un32CircularLogIndex	= (TBX_UINT32)-1;
						break;
					}
				}
			}
			else
			{
				if( un32Checkpoint > pCliToolsCtx->Params.un32LogBufferMaxLines )
					un32Checkpoint = pCliToolsCtx->Params.un32LogBufferMaxLines;

				/* Convert checkpoint index (scroll position) into position in log array */
				un32CircularLogIndex =
					pCliToolsCtx->Params.un32LogBufferMaxLines +
					(pCliToolsCtx->un32CircularLogNextLineIndex - un32Checkpoint);
				un32CircularLogIndex += pCliToolsCtx->Params.un32LogBufferMaxLines - un32MoveUp;
				un32CircularLogIndex %= pCliToolsCtx->Params.un32LogBufferMaxLines;


				while( TBX_TRUE )
				{
					if( un32CircularLogIndex )
						(un32CircularLogIndex)--;
					else
						un32CircularLogIndex = pCliToolsCtx->Params.un32LogBufferMaxLines - 1;

					if( un32CircularLogIndex == pCliToolsCtx->un32CircularLogNextLineIndex )
					{
						/* Don't move pass end/start of log */
						fMove = TBX_FALSE;
						break;
					}

					pszFoundString = TbxCliTools_stristr( pCliToolsCtx->paCircularLog[ un32CircularLogIndex ].pszLine, in_pszToFind );
					if
					(
						(
							in_fMatchFirstCharOnly &&
							pszFoundString != NULL &&
							strcmp( pszFoundString, pCliToolsCtx->paCircularLog[ un32CircularLogIndex ].pszLine ) == 0
						)
						||
						(
							!in_fMatchFirstCharOnly &&
							pszFoundString != NULL
						)
					)
					{
						/* Found string */
						pCliToolsCtx->un32SelectedLineToBlink = un32CircularLogIndex;
						break;
					}

					un32LoopCount++;
					if( un32LoopCount >= pCliToolsCtx->Params.un32LogBufferMaxLines )
					{
						un32CircularLogIndex	= (TBX_UINT32)-1;
						break;
					}
				}
			}

			if( fMove )
			{
				if( un32CircularLogIndex == (TBX_UINT32)-1 )
				{
					un32Checkpoint = 0;
				}
				else
				{

					/* Convert circular buffer index into scroll position */
					un32Checkpoint =
						pCliToolsCtx->Params.un32LogBufferMaxLines +
						(pCliToolsCtx->un32CircularLogNextLineIndex - un32CircularLogIndex);
					(un32Checkpoint) %= pCliToolsCtx->Params.un32LogBufferMaxLines;

					/* Move up so the checkpoint is at the center of display  */
					if( un32Checkpoint > un32MoveUp )
						un32Checkpoint -= un32MoveUp;
					else
						un32Checkpoint = 0;
				}

				pCliToolsCtx->un32ScrollPosition = un32Checkpoint;

				if( pCliToolsCtx->un32ScrollPosition > pCliToolsCtx->un32ScrollPositionMax )
					pCliToolsCtx->un32ScrollPosition = pCliToolsCtx->un32ScrollPositionMax;
			}
		}
		pCliToolsCtx->fRefreshRequired = TBX_TRUE;
		TBX_MUTEX_GIV (pCliToolsCtx->Mutex);
	}
}

/*--------------------------------------------------------------------------------------------------------------------------------
 |
 |  TbxCliToolsCliThread		:	Command-line thread.
 |
 |  in_pCliToolsCtx				:	Thread context
 |
 |  Note						:	~
 |
 |  Return						:	~
 |
 *------------------------------------------------------------------------------------------------------------------------------*/
static TBX_RESULT __stdcall   TbxCliToolsCliThread( PTBX_VOID in_pCliToolsCtx )
{
	PTBX_CLI_TOOLS_CLI_CONTEXT					pCliToolsCtx;
	TBX_UINT32									un32ElapsedMs;
	TBX_UINT32									un32PrevWidth;
	TBX_UINT32									un32PrevHeight;
	TBX_UINT32									un32LastClsTimestamp;
	TBX_UINT32									un32LastNonEmptyRefreshTimestamp;
	time_t										LastLogRotationTime;
	TBX_BOOL									fConsoleInput;
	TBX_BOOL									fKbHitForced;
	TBX_BOOL									fLastRefreshDone;
	TBX_BOOL									fFirstRun;
	TBX_BOOL									fFirstRunPhase2;
	PTBX_CLI_TOOLS_DISPLAY_BUFFER				pDisplayBuffer;
	TBX_BOOL									fEcho;

	/*---------------------------------------------------------------------------------------------------------------------------
	 |  Code section
	 *--------------------------------------------------------------------------------------------------------------------------*/
	CODE
	{
		/* Initialize local variables */
		pCliToolsCtx				= in_pCliToolsCtx;
		un32PrevWidth				= 0;
		un32PrevHeight				= 0;
		un32LastClsTimestamp		= TBX_GET_TICK_PRECISE();
		un32LastNonEmptyRefreshTimestamp	= 0;
		LastLogRotationTime			= 0;
		fLastRefreshDone			= TBX_FALSE;
		fFirstRun					= TBX_FALSE;

		//fFirstRun					= TBX_TRUE;
		fFirstRunPhase2				= TBX_FALSE;
		pDisplayBuffer				= NULL;
		fEcho						= TBX_TRUE;


		/* Disable terminal input echo */
		fEcho = TBXCliSetEcho( TBX_FALSE );

		TBX_MUTEX_GET (pCliToolsCtx->Mutex);

		/* Thread is now running */
		pCliToolsCtx->CliThreadContext.fRunning = TBX_TRUE;

		//pCliToolsCtx->fPrintAllowed = TBX_TRUE;       //LY  : orininal not allow print 


		if( !pCliToolsCtx->Params.fDisableTerminalOutput )
			TBXCliCls();

		while( pCliToolsCtx->CliThreadContext.fContinue || !fLastRefreshDone )
		{
			TBX_UINT32	un32SleepTimeMs = 20;

			//if( pCliToolsCtx->Params.fDisableTerminalInput )
			//{
			//	if( !pCliToolsCtx->fActuallyRemoteControlled )
			//	{
			//		un32SleepTimeMs = 200;
			//	}
			//	if( strcmp( TBX_PLATFORM, "ppc-linux" ) == 0 )
			//	{
			//		un32SleepTimeMs = 200;
			//	}
			//}

			TBX_MUTEX_GIV (pCliToolsCtx->Mutex);
			{
				pCliToolsCtx->fHostlibInUse = TBX_FALSE;
				
				/* Sleep 10ms */
				TBX_SLEEP_MS( un32SleepTimeMs );
			}

			TBX_MUTEX_GET (pCliToolsCtx->Mutex);

			/* Clear any remote hosts from which we did not receive a screen update recently */
			//TBXCliToolsRemoteTestRemoteHostTimeout( pCliToolsCtx );

			pCliToolsCtx->un32CurrentTimestamp = TBX_GET_TICK_PRECISE();

			if( !pCliToolsCtx->CliThreadContext.fContinue )
			{
				if( fLastRefreshDone )
					break;
				else
					fLastRefreshDone = TBX_TRUE;
			}

			if( LastLogRotationTime != time( NULL ) )
			{
				LastLogRotationTime = time( NULL );
				TbxCliToolsLogFilesRotation( pCliToolsCtx);
			}

			/* Compute elapsed time since last display refresh */
			un32ElapsedMs = pCliToolsCtx->un32CurrentTimestamp - pCliToolsCtx->un32LastRefreshTimestamp;
			un32ElapsedMs *= TBX_MSEC_PER_TICKS;

			/* Avoid refresh if minimum allowed delay is not required */
			{
				TBX_UINT32	un32MaxRefreshDelayToUse		= pCliToolsCtx->Params.un32MaxRefreshDelay;
				TBX_UINT32	un32MinRefreshDelayToUse		= pCliToolsCtx->Params.un32MinRefreshDelay;
				TBX_UINT32	un32MinKeyPressedRefreshDelay	= TBX_CLI_TOOLS_CLI_MIN_DELAY_BETWEEN_REFRESH_MS;

				if( pCliToolsCtx->fGenerateRemoteScreen )
				{
					if( pCliToolsCtx->fActuallyRemoteControlled )
					{
						/* We're actually sending our screen to a remote application.
						   Let's reduce the maximum refresh rate a bit... */
						if( un32MinRefreshDelayToUse < 200 )
						{
							un32MinRefreshDelayToUse = 200;
						}
						if( un32MaxRefreshDelayToUse < 1000 )
						{
							un32MaxRefreshDelayToUse = 1000;
						}
						un32MinKeyPressedRefreshDelay = 100;
					}
					else
					{
						/* We're generating a screen to be remotely controlled, but we're not
						   actually remote controlled. Are we displaying to the screen? */
						if( pCliToolsCtx->Params.fDisableTerminalOutput )
						{
							/* Ok... we don't print to screen, we don't print to remote, let's
							   reduce our print rate to a minimum */
							un32MinRefreshDelayToUse		= TBX_CLI_TOOLS_CLI_MAX_DELAY_BETWEEN_REFRESH_MS;
							un32MinKeyPressedRefreshDelay	= 100;
						}
					}
				}
				if( un32MaxRefreshDelayToUse < un32MinRefreshDelayToUse )
					un32MaxRefreshDelayToUse = un32MinRefreshDelayToUse;

				/* Force refresh if too much time has elapsed */
				if( un32ElapsedMs >= un32MaxRefreshDelayToUse )
				{
					/* Too much time since last refresh, force refresh. */
					pCliToolsCtx->fRefreshRequired = TBX_TRUE;
				}

				/* Prevent refresh if too little time has elapsed */
				if( un32ElapsedMs < un32MinRefreshDelayToUse )
				{
					/* Too little time since last refresh. Don't refresh now. */
					pCliToolsCtx->fRefreshRequired = TBX_FALSE;
				}

				/* Force refresh if key was pressed (even if minimum delay was not respected, this is the rule!) */
				if( pCliToolsCtx->fKeyPressedRefreshRequired && (un32ElapsedMs >= un32MinKeyPressedRefreshDelay) )
				{
					pCliToolsCtx->fRefreshRequired = TBX_TRUE;
				}
				/* Force refresh if requested by a tbx_cli_tools_remote */
				if( pCliToolsCtx->fRemoteRefreshRequested )
				{
					pCliToolsCtx->fRefreshRequired = TBX_TRUE;
				}
			}
			
			if( fLastRefreshDone || pCliToolsCtx->fRefreshRequired )
			{
				TBX_UINT32			un32Line;
				TBX_UINT32			un32DisplayBufLineToBlink	= 0xFFFFFFFF;
				TBX_UINT32			un32RemoteClsElapsedMs		= 0xFFFFFFFF;

				pDisplayBuffer = pCliToolsCtx->pDisplayBuf;

				/* Return to start of display buffer */
				pDisplayBuffer->un32NbLines			= 0;

				/* Clear refresh flag */
				pCliToolsCtx->fRefreshRequired					= TBX_FALSE;
				pCliToolsCtx->fKeyPressedRefreshRequired		= TBX_FALSE;
				pCliToolsCtx->un32LastRefreshTimestamp			= pCliToolsCtx->un32CurrentTimestamp;

				/* Check if we should use the remote controlling screen's width, or our own width */
				if( pCliToolsCtx->un32RemoteClsTimestamp )
				{
					un32RemoteClsElapsedMs = TBX_GET_TICK_PRECISE() - pCliToolsCtx->un32RemoteClsTimestamp;
					un32RemoteClsElapsedMs *= TBX_MSEC_PER_TICKS;
				}

				/* Test if we're actually remote controlled */
				if( pCliToolsCtx->fGenerateRemoteScreen && un32RemoteClsElapsedMs < TBX_CLI_TOOLS_MAX_REMOTE_SCREEN_UPDATE_DELAY_MS )
				{
					pCliToolsCtx->fActuallyRemoteControlled			= TBX_TRUE;
				}
				else
				{
					pCliToolsCtx->fActuallyRemoteControlled			= TBX_FALSE;
				}

				//if( TbxCliToolsIsSomeoneWatchingInternal( pCliToolsCtx ) )
				if(1)
				{
					/* Get current display buffer size */
					{
						TBX_UINT32	un32CurrentWidth		= pDisplayBuffer->un32CurrentWidth;
						TBX_UINT32	un32CurrentHeight		= pDisplayBuffer->un32CurrentHeight;

						if( pCliToolsCtx->fActuallyRemoteControlled )
						{
							pDisplayBuffer->un32CurrentWidth	= pCliToolsCtx->un32RemoteCurrentWidth;
							pDisplayBuffer->un32CurrentHeight	= pCliToolsCtx->un32RemoteCurrentHeight;
						}
						else if( pCliToolsCtx->Params.fDisableTerminalOutput )
						{
							/* We're running in background, and not controlled by tbx_cli_tools_remote. Let's use
							   a default width and height */
							pDisplayBuffer->un32CurrentWidth	= pCliToolsCtx->Params.un32MaxScreenWidth;
							pDisplayBuffer->un32CurrentHeight	= pCliToolsCtx->Params.un32MaxScreenHeight;
						}
						else
						{

							pDisplayBuffer->un32CurrentWidth	= TBXCliGetScreenWidth();
							pDisplayBuffer->un32CurrentHeight	= TBXCliGetScreenHeight();
						}

						/* Validate screen width */
						if( pDisplayBuffer->un32CurrentWidth	< pCliToolsCtx->Params.un32MinScreenWidth )
							pDisplayBuffer->un32CurrentWidth	= pCliToolsCtx->Params.un32MinScreenWidth;
						if( pDisplayBuffer->un32CurrentHeight	< pCliToolsCtx->Params.un32MinScreenHeight )
							pDisplayBuffer->un32CurrentHeight	= pCliToolsCtx->Params.un32MinScreenHeight;
						if( pDisplayBuffer->un32CurrentWidth	> pCliToolsCtx->Params.un32MaxScreenWidth )
							pDisplayBuffer->un32CurrentWidth	= pCliToolsCtx->Params.un32MaxScreenWidth;
						if( pDisplayBuffer->un32CurrentHeight	> pCliToolsCtx->Params.un32MaxScreenHeight )
							pDisplayBuffer->un32CurrentHeight	= pCliToolsCtx->Params.un32MaxScreenHeight;

						if
						(
							( pDisplayBuffer->un32CurrentWidth != un32CurrentWidth ) ||
							( pDisplayBuffer->un32CurrentHeight != un32CurrentHeight )
						)
						{
							pCliToolsCtx->fClsRequired = TBX_TRUE;
						}
					}


					if( fFirstRun || fFirstRunPhase2 )
					{
						PTBX_CHAR		aszTelcoBridgesLogo[]		= TBX_CLI_TOOLS_TELCOBRIDGES_LOGO;
						TBX_UINT32		un32TelcoBridgesLogoLines;
						PTBX_CHAR		pszLine						= NULL;
						TBX_UINT32		un32LineCount				= 0;
						TBX_UINT32		un32SquareLineCount			= 0;
						TBX_BOOL		fFirstLine					= TBX_TRUE;
						TBX_UINT32		un32NbTestLines				= 0;

						un32TelcoBridgesLogoLines = sizeof(aszTelcoBridgesLogo) / sizeof(*aszTelcoBridgesLogo);

						/* Display a warning to the user that ask to set the console to sufficient width/height. */
						if
						(
							( un32PrevWidth != pDisplayBuffer->un32CurrentWidth )	||
							( un32PrevHeight != pDisplayBuffer->un32CurrentHeight )	||
							( pCliToolsCtx->fClsRequired == TBX_TRUE )
						)
						{
							TBXCliCls();
							pCliToolsCtx->fClsRequired = TBX_FALSE;
						}
						else
						{
							TBXCliHome();
						}
						un32PrevWidth	= pDisplayBuffer->un32CurrentWidth;
						un32PrevHeight	= pDisplayBuffer->un32CurrentHeight;

						TBXCli_fprintf( stdout, "******************************************************************************\n" );
						un32LineCount++;
						TBXCli_fprintf( stdout, "* WARNING: THIS SOFTWARE REQUIRES THAT THE CURRENT WINDOW IS CONFIGURED WITH *\n" );
						un32LineCount++;
						TBXCli_fprintf( stdout, "*          AT LEAST %u COLUMNS and %u LINES.                                 *\n",
							(unsigned int)(pCliToolsCtx->Params.un32MinScreenWidth+1),
							(unsigned int)(pCliToolsCtx->Params.un32MinScreenHeight+1)
						);
						un32LineCount++;
						TBXCli_fprintf( stdout, "* This warning is printed the first run of this software in this directory   *\n" );
						un32LineCount++;
						TBXCli_fprintf( stdout, "******************************************************************************\n" );
						un32LineCount++;
						TBXCli_fprintf( stdout, "\n" );
						un32LineCount++;

						if( fFirstRun )
						{
							TBXCli_fprintf( stdout, "If your setup is correct, the red line below should fit one line:\n" );
							un32LineCount++;

							un32NbTestLines = un32LineCount + 1;
						}
						else
						{
							TBXCli_fprintf( stdout, "Press any key to continue to main application...\n" );
							un32LineCount++;
							TBXCli_fprintf( stdout, "If your setup is correct, you should now see a stable red square below:\n" );
							un32LineCount++;

							un32NbTestLines = pCliToolsCtx->Params.un32MinScreenHeight;
						}

						for( ; un32LineCount < un32NbTestLines; un32LineCount++ )
						{
							TBX_BOOL	fLastLine			= (un32LineCount == un32NbTestLines - 1);
							TBX_UINT32	un32TbLogoLineIdx;

							if( un32SquareLineCount >= 2 )
							{
								un32TbLogoLineIdx = un32SquareLineCount - 2;
							}
							else
							{
								un32TbLogoLineIdx = 0xFFFFFFFF;
							}

							/* Display empty line (with * at beginning and end, or filled with **** for first/last line) */
							pszLine = (PTBX_CHAR)malloc( pCliToolsCtx->Params.un32MinScreenWidth + 64 );
							if( pszLine )
							{
								TBX_UINT32	un32TelcoBridgesLogoChar	= 0;
								TBX_UINT32	un32Index					= 0;
								TBX_UINT32	un32Strlen					= 0;
								PTBX_CHAR	pszLogoLine					= "";
								TBX_UINT32	un32LogoLineStrlen			= 0;

								if( un32TbLogoLineIdx < un32TelcoBridgesLogoLines )
								{
									pszLogoLine			= aszTelcoBridgesLogo[ un32TbLogoLineIdx ];
									un32LogoLineStrlen	= strlen( pszLogoLine );
								}

								for( un32Strlen = 0; un32Strlen < pCliToolsCtx->Params.un32MinScreenWidth; un32Strlen++ )
								{
									TBX_BOOL fFirstChar = (un32Strlen == 0);
									TBX_BOOL fLastChar  = (un32Strlen == (pCliToolsCtx->Params.un32MinScreenWidth-1));

									if( fFirstLine || fLastLine || fFirstChar || fLastChar )
									{
										pszLine[ un32Index ] = '*';
										un32Index++;
									}
									else
									{
										if( un32TbLogoLineIdx < un32TelcoBridgesLogoLines )
										{
											if( un32TelcoBridgesLogoChar < un32LogoLineStrlen )
											{
												if( pszLogoLine[ un32TelcoBridgesLogoChar ] == '\033' )
												{
													while
													(
														( pszLogoLine[ un32TelcoBridgesLogoChar ] != '\0' ) &&
														( pszLogoLine[ un32TelcoBridgesLogoChar ] != 'm' )
													)
													{
														pszLine[ un32Index ] = pszLogoLine[ un32TelcoBridgesLogoChar ];
														un32Index++;
														un32TelcoBridgesLogoChar++;
													}
													pszLine[ un32Index ] = pszLogoLine[ un32TelcoBridgesLogoChar ];
													un32Index++;
													un32Strlen--;
													un32TelcoBridgesLogoChar++;
												}
												else
												{
													pszLine[ un32Index ] = pszLogoLine[ un32TelcoBridgesLogoChar ];
													un32Index++;
													un32TelcoBridgesLogoChar++;
												}

												if( un32TelcoBridgesLogoChar == un32LogoLineStrlen )
												{
													pszLine[ un32Index ] = '\0';
													strcat( pszLine, FRED );
													un32Index += strlen( FRED );
												}
											}
											else
											{
												pszLine[ un32Index ] = ' ';
												un32Index++;
											}
										}
										else
										{
											pszLine[ un32Index ] = ' ';
											un32Index++;
										}
									}
								}
								pszLine[ un32Index ] = '\n';
								un32Index++;
								pszLine[ un32Index ] = '\0';

								TBXCli_fprintf( stdout, FRED "%s" DEFAULT_COLOR, pszLine );

								free( pszLine );
							}

							fFirstLine = TBX_FALSE;
							un32SquareLineCount++;
						}

						if( fFirstRun )
						{
							TBXCli_fprintf( stdout, "Press any key to test the screen height...\n" );
							un32LineCount++;
						}

						pCliToolsCtx->fRefreshRequired			= TBX_FALSE;
						pCliToolsCtx->un32LastRefreshTimestamp	= pCliToolsCtx->un32CurrentTimestamp;
						pCliToolsCtx->un32RefreshCounter++;

					}     // end of IF FirstRun
					else
					{
						pCliToolsCtx->fFirstRunDone = TBX_TRUE;

						/* Call the application callback to display the menu in the print buffer */
						if( pCliToolsCtx->Params.pFctDisplayMenu )
						{
							PFCT_TBX_CLI_TOOLS_DISPLAY_MENU_CALLBACK	pFctDisplayMenu = pCliToolsCtx->Params.pFctDisplayMenu;
							PTBX_VOID									pCtxDisplayMenu = pCliToolsCtx->Params.pCtxDisplayMenu;

							/* Allow printing while we ask application to display menu */
							pCliToolsCtx->fPrintAllowed = TBX_TRUE;

							TBX_MUTEX_GIV (pCliToolsCtx->Mutex);
							pFctDisplayMenu
							(
								(TBX_CLI_TOOLS_HANDLE)pCliToolsCtx,
								pCtxDisplayMenu
							);
							TBX_MUTEX_GET (pCliToolsCtx->Mutex);

							pCliToolsCtx->fPrintAllowed = TBX_FALSE;
						}

						if( !pCliToolsCtx->Params.fDisableTerminalOutput || pCliToolsCtx->fGenerateRemoteScreen )
						{
							/* Complete the display buffer with logs */
							TbxCliToolsGetLogDisplay
							(
								pCliToolsCtx,
								&un32DisplayBufLineToBlink
							);
							
							if( pCliToolsCtx->Params.un32MaxPromptLines )
							{
								/* Continue filling display buffer with user prompt */
								TbxCliToolsCliUserPromptDisplay( pCliToolsCtx );
							}

							/* Prepare display buffer for printf (remove extra endlines, padd with
							   spaces for screen width, etc.) */
							TbxCliToolsDisplayBufferCleanup( pCliToolsCtx );
						}

						/*
						 * Now really print the display buffer
						*/
						if( !pCliToolsCtx->Params.fDisableTerminalOutput || pCliToolsCtx->fGenerateRemoteScreen )
						{
							TBX_UINT32	un32NbEmptyLines = 0;
							TBX_UINT32	un32TotalNbNonEmptyLines = 0;
							TBX_BOOL	fScreenCleared;
							TBX_UINT32	un32LastClsElapsedMs;
							TBX_UINT32	un32MinClsDelayMs = 1000*pCliToolsCtx->Params.un32MinClsDelay;

							if
							(
								pCliToolsCtx->un32RemoteScreenCurrentIdx < TBX_CLI_TOOLS_MAX_REMOTE_SCREEN_HOSTS ||
								pCliToolsCtx->fActuallyRemoteControlled
							)
							{
								/* We're remote controlled. Enforce cls every TBX_CLI_TOOLS_MAX_REMOTE_CLS_DELAY_MS */
								un32MinClsDelayMs = TBX_CLI_TOOLS_MAX_REMOTE_CLS_DELAY_MS;
							}

							un32LastClsElapsedMs = TBX_GET_TICK_PRECISE() - un32LastClsTimestamp;
							un32LastClsElapsedMs *= TBX_MSEC_PER_TICKS;

							if
							(
								( un32PrevWidth != pDisplayBuffer->un32CurrentWidth )	||
								( un32PrevHeight != pDisplayBuffer->un32CurrentHeight )	||
								( pCliToolsCtx->fClsRequired == TBX_TRUE ) ||
								(
									(un32MinClsDelayMs > 0) &&
									(un32LastClsElapsedMs > un32MinClsDelayMs)
								)
							)
							{
								un32LastClsTimestamp		= TBX_GET_TICK_PRECISE();
								//TBXCliToolsCls( pCliToolsCtx );
								TBXCliCls();
								fScreenCleared	= TBX_TRUE;
								pCliToolsCtx->fClsRequired = TBX_FALSE;
							}
							else
							{
								if( !pCliToolsCtx->Params.fDisableTerminalOutput )
									TBXCliHome();
								
								if( pCliToolsCtx->un32RemoteScreenCurrentIdx < TBX_CLI_TOOLS_MAX_REMOTE_SCREEN_HOSTS )
								{
									/* We are actually controlling a remote application. Make sure to send a "polling"
									   every TBX_CLI_TOOLS_MAX_REMOTE_SCREEN_UPDATE_DELAY_MS/2 */
									TBX_UINT32			un32RemotePollElapsedMs		= 0xFFFFFFFF;
									if( pCliToolsCtx->un32RemotePollTimestamp )
									{
										un32RemotePollElapsedMs = TBX_GET_TICK() - pCliToolsCtx->un32RemotePollTimestamp;
										un32RemotePollElapsedMs *= TBX_MSEC_PER_TICKS;
									}

									if( un32RemotePollElapsedMs >= TBX_CLI_TOOLS_MAX_REMOTE_SCREEN_UPDATE_DELAY_MS/2 )
									{
										//TBXCliToolsRemoteSendClsRequest( in_pCliToolsCtx, TBX_FALSE, TBX_TRUE );
										pCliToolsCtx->un32RemotePollTimestamp = TBX_GET_TICK();
									}
								}

								fScreenCleared	= TBX_FALSE;
							}
							un32PrevWidth	= pDisplayBuffer->un32CurrentWidth;
							un32PrevHeight	= pDisplayBuffer->un32CurrentHeight;

							for( un32Line = 0; un32Line < pDisplayBuffer->un32NbLines; un32Line++ )
							{
								TBX_BOOL	fLinesDiffer = TBX_TRUE;

								/* Print only if different from previous (or if we cleared screen) */
								TBX_UINT32	fBlinkOff = TBX_FALSE;
								if
								(
									un32Line == un32DisplayBufLineToBlink ||
									pDisplayBuffer->afLinesBlink[ un32Line ]
								)
								{
									if
									(
										(pCliToolsCtx->un32BlinkCounter & 0x1) &&
										(pCliToolsCtx->Params.un32MinRefreshDelay <= 500)	/* Don't blink if min refresh is too slow */
									)
									{
										fBlinkOff = TBX_TRUE;
									}
								}
								if( fBlinkOff )
								{
									memset
									(
										pDisplayBuffer->aLines[ un32Line ].pszLine,
										' ',
										pCliToolsCtx->Params.un32MaxScreenWidth
									);
									pDisplayBuffer->aLines[ un32Line ].pszLine[ pDisplayBuffer->un32CurrentWidth ] = '\0';
								}
								if( !fScreenCleared )
								{
									fLinesDiffer = strcmp
									(
										pDisplayBuffer->aLines[ un32Line ].pszLine,
										pCliToolsCtx->pPrevDisplayBuf->aLines[ un32Line ].pszLine
									);
								}

								if( fLinesDiffer )
								{
									un32TotalNbNonEmptyLines++;
									un32LastNonEmptyRefreshTimestamp = TBX_GET_TICK();

									/* Print all accumulated empty lines */
									while( un32NbEmptyLines )
									{
										TBXCliToolsLine_fprintf( pCliToolsCtx, NULL );
										un32NbEmptyLines--;
									}
									TBXCliToolsLine_fprintf( pCliToolsCtx, pDisplayBuffer->aLines[ un32Line ].pszLine );
								}
								else
								{
									/* Line did not change, just go to next line */
									un32NbEmptyLines++;
								}
							}

							//if( pCliToolsCtx->fActuallyRemoteControlled && un32TotalNbNonEmptyLines == 0 )
							//{
							//	/* Once in a while, we are sending only empty refresh, force at least one empty line
							//	   so we send a message to remotely controlling application */
							//	un32ElapsedMs = (TBX_GET_TICK() - un32LastNonEmptyRefreshTimestamp) * TBX_MSEC_PER_TICKS;
							//	if( un32ElapsedMs >= TBX_CLI_TOOLS_MAX_REMOTE_REFRESH_DELAY_MS / 2 )
							//	{
							//		TBXCliToolsLine_fprintf( pCliToolsCtx, NULL );
							//		un32LastNonEmptyRefreshTimestamp = TBX_GET_TICK();
							//	}
							//}

							//if( pCliToolsCtx->fActuallyRemoteControlled &&
							//	!pCliToolsCtx->Params.fDisableTerminalOutput)
							//{
							//	/* Instead of the standard screen, print the 'actually remote controlled' trace */
							//	TBXCli_fprintf
							//	(
							//		stdout, 
							//		FYELLOW "Now remotely controlled by %s.%s\n",
							//		pCliToolsCtx->RemoteControllingHostInfo.szHostName,
							//		pCliToolsCtx->RemoteControllingHostInfo.szApplicationName
							//	);
							//}

							pCliToolsCtx->un32RefreshCounter++;

							/* Swap the current and previous display buffers */
							{
								PTBX_CLI_TOOLS_DISPLAY_BUFFER pbySwapTmp	= pCliToolsCtx->pDisplayBuf;
								pCliToolsCtx->pDisplayBuf					= pCliToolsCtx->pPrevDisplayBuf;
								pCliToolsCtx->pPrevDisplayBuf				= pbySwapTmp;
								pDisplayBuffer								= pCliToolsCtx->pDisplayBuf;
							}

							/* Post the built remote refresh message */
							//TBXCliToolsRemoteRefreshSend( pCliToolsCtx, fScreenCleared );
						}

						/* Clear lines */
						for( un32Line = 0; un32Line < pDisplayBuffer->un32NbLines; un32Line++ )
						{
							pDisplayBuffer->aLines[ un32Line ].pszLine[0]	= '\0';
							pDisplayBuffer->afLinesBlink[ un32Line ]			= TBX_FALSE;
						}
					}
				}
				else
				{
					/* Nobody's watching. Don't waste time generating a display, just build blank screen */
					//TBXCliToolsCls( pCliToolsCtx );
					//TBXCliToolsRemoteRefreshSend( pCliToolsCtx, TBX_TRUE );
				}
			

			}

			un32ElapsedMs = pCliToolsCtx->un32CurrentTimestamp - pCliToolsCtx->un32BlinkTimestamp;
			un32ElapsedMs *= TBX_MSEC_PER_TICKS;
			if( un32ElapsedMs >= pCliToolsCtx->Params.un32MaxRefreshDelay )
			{
				pCliToolsCtx->un32BlinkCounter++;
				pCliToolsCtx->un32BlinkTimestamp	= pCliToolsCtx->un32CurrentTimestamp;
				pCliToolsCtx->fRefreshRequired		= TBX_TRUE;
			}

			fConsoleInput	= TBX_FALSE;
			fKbHitForced	= TBX_FALSE;
			if( pCliToolsCtx->un32ForcedKeysFifoCount )
			{
				/* We have some 'forced' key pressed in the fifo... consume them */
				fKbHitForced = TBX_TRUE;
			}
			else
			{
				if( !pCliToolsCtx->Params.fDisableTerminalInput )
				{
					/* Is there a key stroke ? */
					TBX_MUTEX_GIV (pCliToolsCtx->Mutex);
					fConsoleInput = TBXCliToolsCheckConsoleInput();
					TBX_MUTEX_GET (pCliToolsCtx->Mutex);
				}
				else
				{
					fConsoleInput = TBX_FALSE;
				}
			}
			if( fConsoleInput || fKbHitForced )
			{
				TBX_INT	nOption;
				TBX_INT	nOptionNoInfo;

				if( fKbHitForced && pCliToolsCtx->un32ForcedKeysFifoCount )
				{
					/* Pop the key from the 'forced' fifo */
					nOption			= pCliToolsCtx->aForcedKeysFifo[ pCliToolsCtx->un32ForcedKeysFifoPopIndex ];
					pCliToolsCtx->un32ForcedKeysFifoCount--;
					pCliToolsCtx->un32ForcedKeysFifoPopIndex++;
					pCliToolsCtx->un32ForcedKeysFifoPopIndex %= TBX_CLI_TOOLS_MAX_FORCED_KEYS;
				}
				else
				{
					/* Check what key was pressed */
					TBX_MUTEX_GIV (pCliToolsCtx->Mutex);
					nOption = TbxCliToolsGetch( pCliToolsCtx );
					TBX_MUTEX_GET (pCliToolsCtx->Mutex);
				}
				nOptionNoInfo	= nOption & ~TBX_CLI_TOOLS_KEY_OTHER_INFO_MASK;

				if( nOptionNoInfo != 0 )     // LY :  if user input something 
				{
					pCliToolsCtx->fKeyPressedRefreshRequired	= TBX_TRUE;

					if
					(
						( pCliToolsCtx->un32RemoteScreenCurrentIdx < TBX_CLI_TOOLS_MAX_REMOTE_SCREEN_HOSTS ) ||
						pCliToolsCtx->szRemoteScreenHostName[0]
					)
					{
						/* We're actually controlling a remote host. Simply pass any key
						   to that remote host, but keep track if escape is pressed twice rapidly */
						if
						(
//							( nOptionNoInfo == TBX_CLI_TOOLS_KEY_CTRL_MODIFIER_MASK ) ||
							( nOptionNoInfo == TBX_CLI_TOOLS_KEY_ESCAPE )
						)
						{
							TBX_UINT32	un32ElaspedMs;

							if( pCliToolsCtx->un32LastEscapePressedTimestamp )
							{
								un32ElaspedMs = TBX_GET_TICK_PRECISE() - pCliToolsCtx->un32LastEscapePressedTimestamp;
								un32ElaspedMs *= TBX_MSEC_PER_TICKS;

								if( un32ElaspedMs < 250 )
								{
									/* Stop watching this host */
									//TBXCliToolsRemoteControlStop( pCliToolsCtx, TBX_TRUE );
									pCliToolsCtx->un32LastEscapePressedTimestamp	= 0;
								}
								else
								{
									pCliToolsCtx->un32LastEscapePressedTimestamp = TBX_GET_TICK_PRECISE();
								}
							}
							else
							{
								pCliToolsCtx->un32LastEscapePressedTimestamp = TBX_GET_TICK_PRECISE();
							}
						}

						//if( pCliToolsCtx->un32RemoteScreenCurrentIdx < TBX_CLI_TOOLS_MAX_REMOTE_SCREEN_HOSTS )
						//{
						//	TBXCliToolsRemoteSendRemoteKey( in_pCliToolsCtx, nOption, 0 /* Send to current controlled host */ );
						//}
					}
					else
					{
						if( nOption == TBX_CLI_TOOLS_KEY_CONTROL_C )
						{
							/* Cancel user input */
							pCliToolsCtx->CmdInput.Public.CmdType = 0;
						}

						if( fFirstRun )
						{
							un32LastClsTimestamp	= TBX_GET_TICK_PRECISE();
							fFirstRunPhase2			= TBX_TRUE;
							fFirstRun				= TBX_FALSE;
						}
						else if( fFirstRunPhase2 )
						{
							fFirstRunPhase2 = TBX_FALSE;
						}
						else if( !fFirstRunPhase2 && pCliToolsCtx->CmdInput.Public.CmdType == 0 )
						{
							TBX_UINT32	un32Index;

							/* Clear the command input structure */
							pCliToolsCtx->CmdInput.Public.un32CurrentPromptIdx	= 0;
							pCliToolsCtx->CmdInput.fUserEnteredValue			= TBX_FALSE;
							pCliToolsCtx->CmdInput.fEnteredValueValid			= TBX_TRUE;
							pCliToolsCtx->CmdInput.un32EnteredValueTimestamp	= pCliToolsCtx->un32CurrentTimestamp;

							/* We have allocated an extra PromptLines to detect application overflow */
							for( un32Index = 0; un32Index < pCliToolsCtx->Params.un32MaxPromptLines + 1; un32Index++ )
							{
								pCliToolsCtx->CmdInput.Public.paszPrompt[un32Index].szLine[0] = '\0';
								pCliToolsCtx->CmdInput.Public.paszUserInput[un32Index].szLine[0] = '\0';
							}

							/* Call function to handle menu choice */
							if( pCliToolsCtx->Params.pFctHandleMenuChoice )
							{
								PFCT_TBX_CLI_TOOLS_MENU_CHOICE_CALLBACK		pFctHandleMenuChoice = pCliToolsCtx->Params.pFctHandleMenuChoice;
								PTBX_VOID									pCtxHandleMenuChoice = pCliToolsCtx->Params.pCtxHandleMenuChoice;

								TBX_MUTEX_GIV (pCliToolsCtx->Mutex);
								pFctHandleMenuChoice
								(
									(TBX_CLI_TOOLS_HANDLE)pCliToolsCtx,
									pCtxHandleMenuChoice,
									nOption,
									&pCliToolsCtx->CmdInput.Public
								);
								TBX_MUTEX_GET (pCliToolsCtx->Mutex);

								if
								(
									(pCliToolsCtx->CmdInput.Public.paszPrompt[pCliToolsCtx->Params.un32MaxPromptLines].szLine[0] != '\0') ||
									(pCliToolsCtx->CmdInput.Public.paszUserInput[pCliToolsCtx->Params.un32MaxPromptLines].szLine[0] != '\0')
								)
								{
									/* Buffer overflow. */
									TbxCliToolsLogPrint
									(
										(TBX_CLI_TOOLS_HANDLE)pCliToolsCtx,
										TRACE_LEVEL_ERROR, NULL,
										"TbxCliToolsCliThread: Command (%u) exceed maximum prompt size (%u)!!! Increase this value\n",
										nOption,
										pCliToolsCtx->Params.un32MaxPromptLines
									);
								}

								if
								(
									(pCliToolsCtx->CmdInput.Public.CmdType != 0	)&&
									(pCliToolsCtx->CmdInput.Public.paszPrompt[0].szLine[0] == '\0')
								)
								{
									/* No parameters to this command, apply now. */
									TbxCliToolsUserInputValidate( in_pCliToolsCtx, '\n', 1, TBX_TRUE );
								}
							}
							else
							{

								TbxCliToolsDefaultKeysHandler((TBX_CLI_TOOLS_HANDLE)pCliToolsCtx,nOption);
							}
						}
						else
						{
						

							/* Gather the user input for current menu choice */
							TbxCliToolsGatherUserInput( pCliToolsCtx, nOptionNoInfo );
						}
					}
				}
			}
		}


		/* End of the code (skip to cleanup) */
		TBX_EXIT_SUCCESS (TBX_RESULT_OK);
	}

	/*---------------------------------------------------------------------------------------------------------------------------
	 |  Error handling section
	 *--------------------------------------------------------------------------------------------------------------------------*/
	ERROR_HANDLING
	{
		TbxCliToolsLogPrint
		(
			(TBX_CLI_TOOLS_HANDLE)pCliToolsCtx,
			TRACE_LEVEL_ERROR, NULL,
			"TbxCliToolsCliThread: %s, result 0x%08X\n",
			TBX_ERROR_DESCRIPTION,
			(int)TBX_ERROR_RESULT
		);
	}

	/*---------------------------------------------------------------------------------------------------------------------------
	 |  Cleanup section
	 *--------------------------------------------------------------------------------------------------------------------------*/
	CLEANUP
	{
		pCliToolsCtx->CliThreadContext.fRunning		= TBX_FALSE;
		pCliToolsCtx->CliThreadContext.fDone		= TBX_TRUE;

		pCliToolsCtx->fHostlibInUse = TBX_FALSE;

		/* Restore terminal input echo */
		TBXCliSetEcho( fEcho );

		TBX_MUTEX_GIV (pCliToolsCtx->Mutex);
	}

	RETURN;
}






/*-------------------------------------------------------------------------------------------------------------------------------
 |
 |  TbxCliToolsLogPrintLock		:	Lock the log print for atomic prints.
 |
 |	in_hCliToolsHandle				:	Handle of the allocated TBX CLI library
 |
 |  Note						:	~
 |
 |  Return						:	~
 |
 *------------------------------------------------------------------------------------------------------------------------------*/
void TbxCliToolsLogPrintLock
(
	IN		TBX_CLI_TOOLS_HANDLE					in_hCliToolsHandle
)
{
	PTBX_CLI_TOOLS_CLI_CONTEXT			pCliToolsCtx = (PTBX_CLI_TOOLS_CLI_CONTEXT)in_hCliToolsHandle;

	if( pCliToolsCtx )
	{
		TBX_MUTEX_GET (pCliToolsCtx->Mutex);
	}
}

/*-------------------------------------------------------------------------------------------------------------------------------
 |
 |  TbxCliToolsLogPrintUnlock	:	Unlock the log print for atomic prints.
 |
 |	in_hCliToolsHandle				:	Handle of the allocated TBX CLI library
 |
 |  Note						:	~
 |
 |  Return						:	~
 |
 *------------------------------------------------------------------------------------------------------------------------------*/
void TbxCliToolsLogPrintUnlock
(
	IN		TBX_CLI_TOOLS_HANDLE					in_hCliToolsHandle
)
{
	PTBX_CLI_TOOLS_CLI_CONTEXT			pCliToolsCtx = (PTBX_CLI_TOOLS_CLI_CONTEXT)in_hCliToolsHandle;

	if( pCliToolsCtx )
	{
		TBX_MUTEX_GIV (pCliToolsCtx->Mutex);
	}
}



/*-------------------------------------------------------------------------------------------------------------------------------
 |
 |  TbxCliToolsPrint		:	Print to screen buffer.
 |
 |
 |	in_hCliToolsHandle		:	Handle of the allocated TBX CLI library
 |  in_pszString			:	Format for print (same as fprintf)
 |	...						:	Variable number of arguments for specified format (same as fprintf)
 |
 |  Note					:	~
 |
 |  Return					:	~
 |
 *------------------------------------------------------------------------------------------------------------------------------*/
void TbxCliToolsvPrintPrivate
(
	IN		PTBX_CLI_TOOLS_CLI_CONTEXT				pCliToolsCtx,
	IN		TBX_BOOL								in_fNewLineDefaultColor,
	IN		PTBX_CHAR								in_pszString,
	IN		va_list									in_Marker
)
{
	int							iFormatValid			= 1;
	PTBX_CHAR					pszTmpBuffer;
	TBX_CHAR					szTmpBuffer[TBX_CLI_TOOLS_LINE_MAX_SIZE];
	PTBX_CLI_TOOLS_DISPLAY_BUFFER pDisplayBuffer = pCliToolsCtx->pDisplayBuf;

	if( pCliToolsCtx->Params.fDisableTerminalOutput && !pCliToolsCtx->fGenerateRemoteScreen )
		return;	/* Useless to print things while terminal output is disabled */
	
	#if defined WIN32
	#if defined TBX_DEBUG || defined DEBUG || defined _DEBUG || defined DBG
	{
		/* Validate the passed arguments */
		iFormatValid = TBXCliToolsValidatePrintfFormat( in_pszString, in_Marker );
	}
	#endif
	#elif !defined HPUX
	#if defined SOLARIS8 || LINUX || defined TBX_DEBUG || defined DEBUG || defined _DEBUG || defined DBG
	{
		va_list 	marker_copy;

		/* Validate the passed arguments */
		va_copy (marker_copy, in_Marker);
		iFormatValid = TBXCliToolsValidatePrintfFormat( in_pszString, marker_copy );
        va_end (marker_copy);
	}
	#endif
	#endif
	
	if ( !iFormatValid )
	{
		/* copy the bug message in buffer */
		sprintf( szTmpBuffer, "BUG: Invalid 'printf' style format passed to TbxCliToolsPrint, format is '%s'\n",
			in_pszString );

	}
	else
	{
		/* copy the string in the buffer */
		szTmpBuffer[TBX_CLI_TOOLS_LINE_MAX_SIZE-1] = '\0';
		TbxCliTools_vsnprintf( szTmpBuffer, sizeof(szTmpBuffer)-1, in_pszString, in_Marker );
	}

	if( pDisplayBuffer->un32NbLines < pDisplayBuffer->un32CurrentHeight )
	{
		PTBX_CHAR			pszEndString;

		pszTmpBuffer	= szTmpBuffer;
		pszEndString	= strstr( pszTmpBuffer, "\n" );
		while( pszEndString )
		{
			/* String has end-line in middle. Print partial */
			*pszEndString = '\0';

			Strncat
			(
				pDisplayBuffer->aLines[ pDisplayBuffer->un32NbLines ].pszLine,
				pszTmpBuffer,
				pCliToolsCtx->Params.un32MaxScreenWidth
			);

			*pszEndString = '\n';
			pszTmpBuffer = pszEndString + 1;

			pDisplayBuffer->un32NbLines++;
			if( pDisplayBuffer->un32NbLines >= pDisplayBuffer->un32CurrentHeight )
				break;

			/* Search next endline in string */
			pszEndString	= strstr( pszTmpBuffer, "\n" );
		}
		if( pszTmpBuffer && pDisplayBuffer->un32NbLines < pDisplayBuffer->un32CurrentHeight )
		{
			PTBX_CHAR		pszLine = pDisplayBuffer->aLines[ pDisplayBuffer->un32NbLines ].pszLine;
			
			if (in_fNewLineDefaultColor && strlen(pszLine) == 0)
			{
				PTBX_CHAR		pszColor = DEFAULT_COLOR;
				strcpy (pszLine, pszColor);
			}

			/* Print last part of the string */
			Strncat
			(
				pszLine,
				pszTmpBuffer,
				pCliToolsCtx->Params.un32MaxScreenWidth
			);
		}
	}

	return;
}

void TbxCliToolsPrint
(
	IN		TBX_CLI_TOOLS_HANDLE					in_hCliToolsHandle,
	IN		PTBX_CHAR								in_pszString,
	...
)
{
	PTBX_CLI_TOOLS_CLI_CONTEXT	pCliToolsCtx = (PTBX_CLI_TOOLS_CLI_CONTEXT)in_hCliToolsHandle;

	if( pCliToolsCtx )
	{
		TBX_MUTEX_GET (pCliToolsCtx->Mutex);
		if( pCliToolsCtx->fPrintAllowed )
		{
			va_list	marker;
			va_start (marker, in_pszString);

			TbxCliToolsvPrintPrivate( pCliToolsCtx, TBX_TRUE, in_pszString, marker );

			va_end (marker);
		}
		else
		{
			TbxCliToolsLogPrint
			(
				in_hCliToolsHandle,
				TRACE_LEVEL_ERROR, NULL,
				"TbxCliToolsPrint: ERROR: Application trying to print while not allowed\n"
			);
		}
		TBX_MUTEX_GIV (pCliToolsCtx->Mutex);
	}
}


void TbxCliToolsvPrint
(
	IN		TBX_CLI_TOOLS_HANDLE					in_hCliToolsHandle,
	IN		PTBX_CHAR								in_pszString,
	IN		va_list									in_Marker
)
{
	PTBX_CLI_TOOLS_CLI_CONTEXT	pCliToolsCtx = (PTBX_CLI_TOOLS_CLI_CONTEXT)in_hCliToolsHandle;

	if( pCliToolsCtx )
	{
		TBX_MUTEX_GET (pCliToolsCtx->Mutex);
		if( pCliToolsCtx->fPrintAllowed )
		{
			TbxCliToolsvPrintPrivate( pCliToolsCtx, TBX_TRUE, in_pszString, in_Marker );
		}
		else
		{
			TbxCliToolsLogPrint
			(
				in_hCliToolsHandle,
				TRACE_LEVEL_ERROR, NULL,
				"TbxCliToolsPrint: ERROR: Application trying to print while not allowed\n"
			);
		}
		TBX_MUTEX_GIV (pCliToolsCtx->Mutex);
	}
}



/*-------------------------------------------------------------------------------------------------------------------------------
 |
 |  TbxCliToolsPrintFillLine:	Fill a line with the specified character.
 |
 |
 |	in_hCliToolsHandle		:	Handle of the allocated TBX CLI library
 |  in_cFillChar			:	Character to fill with
 |	in_un32HighlightPos		:	Position of a character to highlight
 |
 |  Note					:	~
 |
 |  Return					:	~
 |
 *------------------------------------------------------------------------------------------------------------------------------*/
void TbxCliToolsPrintFillLinePrivate
(
	IN		PTBX_CLI_TOOLS_CLI_CONTEXT				pCliToolsCtx,
	IN		TBX_CHAR								in_cFillChar,
	IN		TBX_UINT32								in_un32HighlightPos
)
{
	if( pCliToolsCtx->pDisplayBuf->un32NbLines < pCliToolsCtx->pDisplayBuf->un32CurrentHeight )
	{
		TBX_UINT32	un32CurrentWidth = pCliToolsCtx->pDisplayBuf->un32CurrentWidth;
		TBX_UINT32	un32Strlen = strlen( pCliToolsCtx->pDisplayBuf->aLines[ pCliToolsCtx->pDisplayBuf->un32NbLines ].pszLine );
		TBX_UINT32	un32XPos;

		if( un32CurrentWidth > (pCliToolsCtx->Params.un32MaxScreenWidth - g_un32TrunkatedEndlineStrlen - 1) )
		{
			un32CurrentWidth = pCliToolsCtx->Params.un32MaxScreenWidth - g_un32TrunkatedEndlineStrlen - 1;
		}

		un32XPos = un32Strlen;

		if( in_un32HighlightPos < un32CurrentWidth )
		{
			if( un32XPos <= in_un32HighlightPos )
			{
				#ifdef WIN32
					TBX_CHAR	cHighlightChar	= ((TBX_CHAR)(TBX_UINT8)223);
				#else
					TBX_CHAR	cHighlightChar	= '=';
				#endif
				if( un32XPos <= in_un32HighlightPos )
				{
					memset
					(
						pCliToolsCtx->pDisplayBuf->aLines[ pCliToolsCtx->pDisplayBuf->un32NbLines ].pszLine + un32XPos,
						in_cFillChar,
						in_un32HighlightPos - un32XPos
					);
					un32XPos += in_un32HighlightPos;
				}
				/* Set the highlight character */
				pCliToolsCtx->pDisplayBuf->aLines[ pCliToolsCtx->pDisplayBuf->un32NbLines ].pszLine[ un32XPos ] =
					cHighlightChar;
				un32XPos++;
				if( un32XPos < un32CurrentWidth )
				{
					pCliToolsCtx->pDisplayBuf->aLines[ pCliToolsCtx->pDisplayBuf->un32NbLines ].pszLine[ un32XPos ] =
						cHighlightChar;
					un32XPos++;
				}
				if( un32XPos < un32CurrentWidth )
				{
					pCliToolsCtx->pDisplayBuf->aLines[ pCliToolsCtx->pDisplayBuf->un32NbLines ].pszLine[ un32XPos ] =
						cHighlightChar;
					un32XPos++;
				}
			}
		}

		if( un32XPos < un32CurrentWidth )
		{
			memset
			(
				pCliToolsCtx->pDisplayBuf->aLines[ pCliToolsCtx->pDisplayBuf->un32NbLines ].pszLine + un32XPos,
				in_cFillChar,
				un32CurrentWidth - un32XPos
			);
		}
		pCliToolsCtx->pDisplayBuf->aLines[ pCliToolsCtx->pDisplayBuf->un32NbLines ].pszLine[ un32CurrentWidth ] = '\0';
		pCliToolsCtx->pDisplayBuf->un32NbLines++;
	}
}

void TbxCliToolsPrintFillLine
(
	IN		TBX_CLI_TOOLS_HANDLE					in_hCliToolsHandle,
	IN		TBX_CHAR								in_cFillChar
)
{
	PTBX_CLI_TOOLS_CLI_CONTEXT	pCliToolsCtx = (PTBX_CLI_TOOLS_CLI_CONTEXT)in_hCliToolsHandle;

	if( pCliToolsCtx )
	{
		TBX_MUTEX_GET (pCliToolsCtx->Mutex);
		if( pCliToolsCtx->fPrintAllowed )
		{
			TbxCliToolsPrintFillLinePrivate( pCliToolsCtx, in_cFillChar, 0xFFFFFFFF );
		}
		else
		{
			TbxCliToolsLogPrint
			(
				in_hCliToolsHandle,
				TRACE_LEVEL_ERROR, NULL,
				"TbxCliToolsPrintFillLine: ERROR: Application trying to print while not allowed\n"
			);
		}
		TBX_MUTEX_GIV (pCliToolsCtx->Mutex);
	}
}

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
)
{
	PTBX_CLI_TOOLS_CLI_CONTEXT	pCliToolsCtx			= (PTBX_CLI_TOOLS_CLI_CONTEXT)in_hCliToolsHandle;

	if( pCliToolsCtx )
	{
		TBX_MUTEX_GET (pCliToolsCtx->Mutex);
		if( pCliToolsCtx->fPrintAllowed )
		{
			pCliToolsCtx->pDisplayBuf->afLinesBlink[ pCliToolsCtx->pDisplayBuf->un32NbLines ] = TBX_TRUE;
		}
		else
		{
			TbxCliToolsLogPrint
			(
				in_hCliToolsHandle,
				TRACE_LEVEL_ERROR, NULL,
				"TbxCliToolsGetRemainingLines: ERROR: Application not actually drawing screen\n"
			);
		}
		TBX_MUTEX_GIV (pCliToolsCtx->Mutex);
	}

	return;
}

TBX_BOOL TbxCliToolsLineWrap
(
	IN		TBX_CLI_TOOLS_HANDLE					in_hCliToolsHandle,
	IN		TBX_BOOL								in_fLineWrap
)
{
	PTBX_CLI_TOOLS_CLI_CONTEXT	pCliToolsCtx			= (PTBX_CLI_TOOLS_CLI_CONTEXT)in_hCliToolsHandle;
	TBX_BOOL					fPrevLineWrap			= TBX_TRUE;

	if( pCliToolsCtx )
	{
		TBX_MUTEX_GET (pCliToolsCtx->Mutex);
		{
			fPrevLineWrap			= !pCliToolsCtx->fNoWrap;
			pCliToolsCtx->fNoWrap	= !in_fLineWrap;
		}
		TBX_MUTEX_GIV (pCliToolsCtx->Mutex);
	}

	return fPrevLineWrap;
}

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
)
{
	PTBX_CLI_TOOLS_CLI_CONTEXT	pCliToolsCtx			= (PTBX_CLI_TOOLS_CLI_CONTEXT)in_hCliToolsHandle;
	TBX_UINT32					un32NbLinesRemaining	= 0;

	if( pCliToolsCtx )
	{
		TBX_MUTEX_GET (pCliToolsCtx->Mutex);
		if( pCliToolsCtx->fPrintAllowed )
		{
			if( pCliToolsCtx->pDisplayBuf->un32NbLines < pCliToolsCtx->pDisplayBuf->un32CurrentHeight )
			{
				un32NbLinesRemaining =
					pCliToolsCtx->pDisplayBuf->un32CurrentHeight -
					pCliToolsCtx->pDisplayBuf->un32NbLines;
			}
		}
		else
		{
			TbxCliToolsLogPrint
			(
				in_hCliToolsHandle,
				TRACE_LEVEL_ERROR, NULL,
				"TbxCliToolsGetRemainingLines: ERROR: Application not actually drawing screen\n"
			);
		}
		TBX_MUTEX_GIV (pCliToolsCtx->Mutex);
	}

	return un32NbLinesRemaining;
}

TBX_UINT32 TbxCliToolsGetRemainingChars
(
	IN		TBX_CLI_TOOLS_HANDLE					in_hCliToolsHandle
)
{
	PTBX_CLI_TOOLS_CLI_CONTEXT	pCliToolsCtx			= (PTBX_CLI_TOOLS_CLI_CONTEXT)in_hCliToolsHandle;
	TBX_UINT32					un32NbCharsRemaining	= 0;

	if( pCliToolsCtx )
	{
		TBX_MUTEX_GET (pCliToolsCtx->Mutex);
		if( pCliToolsCtx->fPrintAllowed )
		{
			if( pCliToolsCtx->pDisplayBuf->un32NbLines < pCliToolsCtx->pDisplayBuf->un32CurrentHeight )
			{
				if( pCliToolsCtx->pDisplayBuf->un32NbLines < TBX_CLI_TOOLS_CLI_SCREEN_HEIGHT_MAX )
				{
					PTBX_CLI_TOOLS_DISPLAY_LINE_PRIVATE	pLine = &( pCliToolsCtx->pDisplayBuf->aLines[ pCliToolsCtx->pDisplayBuf->un32NbLines ] );
					TBX_UINT32							un32Strlen;
					TBX_UINT32							un32StrlenNoColor;
					TBX_UINT32							un32RemainingInBuffer = 0;
					TBX_UINT32							un32RemainingInScreen = 0;

					un32Strlen			= strlen( pLine->pszLine );
					un32StrlenNoColor	= TBXCli_Strlen( pLine->pszLine );

					if( pCliToolsCtx->Params.un32MaxScreenWidth > un32Strlen )
					{
						un32RemainingInBuffer = pCliToolsCtx->Params.un32MaxScreenWidth - un32Strlen;
					}

					if( pCliToolsCtx->pDisplayBuf->un32CurrentWidth > un32StrlenNoColor )
					{
						un32RemainingInScreen = pCliToolsCtx->pDisplayBuf->un32CurrentWidth - un32StrlenNoColor;
					}

					un32NbCharsRemaining = TBX_MIN( un32RemainingInBuffer, un32RemainingInScreen );
				}
			}
		}
		else
		{
			TbxCliToolsLogPrint
			(
				in_hCliToolsHandle,
				TRACE_LEVEL_ERROR, NULL,
				"TbxCliToolsGetRemainingChars: ERROR: Application not actually drawing screen\n"
			);
		}
		TBX_MUTEX_GIV (pCliToolsCtx->Mutex);
	}

	return un32NbCharsRemaining;
}




static __inline PTBX_CHAR strchr_fast( PTBX_CHAR in_pszString, TBX_CHAR in_cChar )
{
	while( *in_pszString )
	{
		if( *in_pszString == in_cChar )
			return in_pszString;
		in_pszString++;
	}
	return NULL;
}

#define TBX_CLI_TOOLS_MAX_COLOR_CODE_LEN	16

static __inline TBX_VOID TbxCliToolsColorCat
(
	IN		TBX_CHAR		out_pszColor[TBX_CLI_TOOLS_MAX_COLOR_CODE_LEN],
	IN_OUT	PTBX_UINT32		io_pun32ColorLen,
	IN		TBX_CHAR		in_cChar
)
{
	if( (*io_pun32ColorLen+1) < TBX_CLI_TOOLS_MAX_COLOR_CODE_LEN )
	{
		out_pszColor[ (*io_pun32ColorLen)++ ]	= in_cChar;
		out_pszColor[ *io_pun32ColorLen ]		= '\0';
	}
}

static __inline TBX_BOOL TbxCliToolsIsWordWrapChar( TBX_CHAR in_cChar )
{	
	if( isalnum( (unsigned char)in_cChar ) )
		return TBX_FALSE;

	return TBX_TRUE;
}

static __inline PTBX_CHAR strcut_skipcolor( PTBX_CHAR in_pszString, TBX_UINT32 in_un32Char, TBX_CHAR out_pszColor[TBX_CLI_TOOLS_MAX_COLOR_CODE_LEN] )
{
	TBX_UINT32		un32NbChar		= 0;
	TBX_BOOL		fPrevCanSplit	= TBX_TRUE;
	PTBX_CHAR		pszCurrentWord	= NULL;

	while( *in_pszString )
	{
		if( TbxCliToolsIsWordWrapChar( *in_pszString ) )
		{
			fPrevCanSplit	= TBX_TRUE;
			pszCurrentWord	= NULL;
		}
		else
		{
			if( fPrevCanSplit )
			{
				/* Beginning of a word */
				pszCurrentWord = in_pszString;
			}
			fPrevCanSplit = TBX_FALSE;
		}

		if (un32NbChar == in_un32Char)
		{
			if( pszCurrentWord )
			{
				/* We are in the middle of a word. Don't split is relatively short */
				if( (in_pszString - pszCurrentWord) < 16 )
				{
					return pszCurrentWord;
				}
			}
			return in_pszString;
		}
		if( *in_pszString == '\033' )
		{
			TBX_UINT32 un32ColorIdx = 0;

			TbxCliToolsColorCat( out_pszColor, &un32ColorIdx, *in_pszString );

			/* Skip color */
			while
			(
				( *in_pszString != '\0' ) &&
				( *in_pszString != 'm' )
			)
			{
				in_pszString++;
				TbxCliToolsColorCat( out_pszColor, &un32ColorIdx, *in_pszString );
			}
			if ( *in_pszString == 'm' )
			{
				in_pszString++;
			}
			else
			{
				/* Color code incomplete. Ignore */
				out_pszColor[0] = '\0';
			}
		}
		else
		{
			in_pszString++;
			un32NbChar++;
		}
	}
	return NULL;
}


static __inline void TbxCliToolsLogPrintPrivate
(
	IN		PTBX_CLI_TOOLS_CLI_CONTEXT				pCliToolsCtx,
	IN		TRACE_LEVEL								in_TraceLevel,
	IN		TBX_BOOL								in_fPrintCircularLog,
	IN		TBX_BOOL								in_fPrintFile,
	IN		const TBX_CHAR*							in_pszTimeColor,
	IN		const TBX_CHAR*							in_pszColor,
	IN		const TBX_CHAR*							in_pszString,
	IN		va_list									in_Marker,
	IN		TBX_BOOL								in_fFlush
)
{
	PTBX_CHAR					pszLineToPrint			= NULL;
	PTBX_CHAR					pszLineToPrintNext		= NULL;
	TBX_UINT32					un32WrapLength			= TBX_CLI_TOOLS_LINE_MIN_WRAP_LENGTH;
	TBX_CHAR					cBkpChar				= 0;
	TBX_BOOL					fIsACutLine				= TBX_FALSE;
	TBX_CLI_TOOLS_TIMESTAMP		Timestamp;
	TBX_CHAR					szTime[ 64 ];
	TBX_CHAR					szCurrLineColor[TBX_CLI_TOOLS_MAX_COLOR_CODE_LEN];
	TBX_CHAR					szPrevLineColor[TBX_CLI_TOOLS_MAX_COLOR_CODE_LEN];
	TBX_CHAR					szCheck[2];
	int							iFormatValid			= 1;

	szCurrLineColor[0] = '\0';
	szPrevLineColor[0] = '\0';

	if( !in_fPrintCircularLog && !in_fPrintFile && pCliToolsCtx->CliThreadContext.fRunning )
		return;	/* Nothing to print */

	/* Get timestamp, and format time */
	TbxCliToolsGetTimestamp( &Timestamp );
	TbxCliToolsFormatTimePrivate( &Timestamp, szTime );

	if( pCliToolsCtx->pDisplayBuf->un32CurrentWidth == 0 )
	{
		pCliToolsCtx->pDisplayBuf->un32CurrentWidth = TBXCliGetScreenWidth();
	}

	/* Calculate the line wrap length */
	if( pCliToolsCtx->fNoWrap )
	{
		un32WrapLength = 0xFFFFFFFF;
	}
	else if( !pCliToolsCtx->fActuallyRemoteControlled && pCliToolsCtx->Params.fDisableTerminalOutput )
	{
		/* We're not remote controlled and don't output to screen => Don't wrap to 'current width', we don't really have one! */
		un32WrapLength = (pCliToolsCtx->Params.un32MaxScreenWidth-32);	/* -32 to leave room for date and time, and some color codes */
	}
	else
	{
		if( un32WrapLength < pCliToolsCtx->pDisplayBuf->un32CurrentWidth )
		{
			un32WrapLength = pCliToolsCtx->pDisplayBuf->un32CurrentWidth + pCliToolsCtx->un32ScrollPositionHor;
		}
		if( un32WrapLength > (pCliToolsCtx->Params.un32MaxScreenWidth-32) )
		{
			un32WrapLength = (pCliToolsCtx->Params.un32MaxScreenWidth-32);	/* -32 to leave room for date and time, and some color codes */
		}
	}

	if( !in_pszTimeColor )
		in_pszTimeColor = DEFAULT_COLOR;
	if( !in_pszColor )
		in_pszColor = DEFAULT_COLOR;

	{
		TBX_UINT32		un32Tmp;
		un32Tmp = 2;
		un32Tmp += strlen( szTime );
//		un32Tmp += strlen( in_pszTimeColor );
//		un32Tmp += strlen( in_pszColor );

		if( un32WrapLength > un32Tmp )
			un32WrapLength -= un32Tmp;
	}

	/* Validate the 'printf' style arguments */
	#if defined WIN32
	#if defined TBX_DEBUG || defined DEBUG || defined _DEBUG || defined DBG
	{
		/* Validate the passed arguments */
		iFormatValid = TBXCliToolsValidatePrintfFormat( in_pszString, in_Marker );

		if( !iFormatValid )
		{
			TbxCliTools_snprintf
			(
				pCliToolsCtx->pszTmpLine,
				TBX_CLI_TOOLS_TMP_LINE_MAX_SIZE,
				FRED "BUG: Invalid 'printf' style format passed to TbxCliToolsLogPrint, format is '%s'\n",
				in_pszString
			);
		}
	}
	#endif
	#elif !defined HPUX
	#if defined SOLARIS8 || LINUX || defined TBX_DEBUG || defined DEBUG || defined _DEBUG || defined DBG
	{
		TBX_CHAR    szTmpString[TBX_CLI_TOOLS_LINE_MAX_SIZE];
		va_list 	marker_copy;

		Strncpy( szTmpString, in_pszString, TBX_CLI_TOOLS_LINE_MAX_SIZE );

		/* Validate the passed arguments */
		va_copy (marker_copy, in_Marker);
		iFormatValid = TBXCliToolsValidatePrintfFormat( in_pszString, marker_copy );
        va_end (marker_copy);

		if( !iFormatValid )
		{
			TbxCliTools_snprintf
			(
				pCliToolsCtx->pszTmpLine,
				TBX_CLI_TOOLS_TMP_LINE_MAX_SIZE,
				FRED "BUG: Invalid 'printf' style format passed to TbxCliToolsLogPrint, format is '%s'\n",
				szTmpString
			);
		}
	}
	#endif
	#endif


	if( iFormatValid )
	{
		/* Format the string into a temporary buffer, parsing arguments */
		szCheck[0] = 0x7A;
		TbxCliTools_vsnprintf
		(
			pCliToolsCtx->pszTmpLine,
			TBX_CLI_TOOLS_TMP_LINE_MAX_SIZE,
			in_pszString,
			in_Marker
		);
		if (szCheck[0] != 0x7A)
			printf ("MemCorruption detected in TbxCliToolsLogPrintPrivate()\n");
	}

	/* Get pointer to line, and next line (if appropriate) */
	pszLineToPrint		= pCliToolsCtx->pszTmpLine;
	pszLineToPrintNext	= strchr_fast( pszLineToPrint, '\n' );
	if( !pszLineToPrintNext )
	{
		/* Line is not split. Maybe it needs wrap? */
		pszLineToPrintNext = strcut_skipcolor (pszLineToPrint, un32WrapLength, szCurrLineColor);
		if (pszLineToPrintNext)
		{
			cBkpChar = *pszLineToPrintNext;
		}
	}
	if( pszLineToPrintNext )
	{
#if 0		
		/* Check if line is too long */
		if( (TBX_UINT32)(pszLineToPrintNext - pszLineToPrint) > un32WrapLength )
		{
			pszLineToPrintNext = pszLineToPrint + un32WrapLength;
			cBkpChar = *pszLineToPrintNext;
		}
#endif		
		/* Cut the string here, no 'end-lines' are allowed. */
		*pszLineToPrintNext = '\0';
		/* Set pointer to beginning of next line */
		pszLineToPrintNext++;
		if( *pszLineToPrintNext == '\0' )
			pszLineToPrintNext = NULL;	/* There is nothing in 'next' line, just ignore it. */
	}

	while( pszLineToPrint )
	{
		#ifdef TBX_CLI_TOOLS_USE_LOG_FILE
		if( in_fPrintFile && pCliToolsCtx->szCurrentLogFilePath[0] )
		{
			if( !pCliToolsCtx->pszDiskWriteBuffer )
			{
				/* First time we need disk write buffer. Allocate the disk write buffer now. */
				pCliToolsCtx->pszDiskWriteBuffer = (PTBX_CHAR)malloc
				(
					TBX_CLI_TOOLS_DISK_BUFFER_SIZE
				);
				if( !pCliToolsCtx->pszDiskWriteBuffer )
				{
					TbxCliToolsLogPrint
					(
						(TBX_CLI_TOOLS_HANDLE)pCliToolsCtx, TRACE_LEVEL_ERROR, NULL,
						"Failed to allocate pszDiskWriteBuffer\n"
					);
				}
			}
		}

		if( in_fPrintFile && pCliToolsCtx->szCurrentLogFilePath[0] && pCliToolsCtx->pszDiskWriteBuffer )
		{
			/* Copy this string to the disk write buffer (removing color codes if appropriate) */
			if( pCliToolsCtx->Params.fLogKeepColorCodes )
			{
				pCliToolsCtx->un32DiskWriteBufferCurLen += TbxCliTools_snprintf
				(
					pCliToolsCtx->pszDiskWriteBuffer + pCliToolsCtx->un32DiskWriteBufferCurLen,
					TBX_CLI_TOOLS_DISK_BUFFER_SIZE - pCliToolsCtx->un32DiskWriteBufferCurLen,
					"%s%s%s" DEFAULT_COLOR,
					in_pszTimeColor,
					szTime,
					szPrevLineColor[0] ? szPrevLineColor : in_pszColor
				);
				
				{
					PTBX_CHAR	pszSrc		= pszLineToPrint;
					PTBX_CHAR	pszDst		= pCliToolsCtx->pszDiskWriteBuffer + pCliToolsCtx->un32DiskWriteBufferCurLen;
					TBX_UINT32	un32Left	= TBX_CLI_TOOLS_DISK_BUFFER_SIZE - pCliToolsCtx->un32DiskWriteBufferCurLen - 1;
					TBX_UINT32	un32Written = 0;

					while( un32Left && pszSrc[un32Written] )
					{
						pszDst[un32Written] = pszSrc[un32Written];
						un32Written++;
						un32Left--;
					}
					pszDst[un32Written] = '\0';

					pCliToolsCtx->un32DiskWriteBufferCurLen += un32Written;
				}
			}
			else
			{
				strcpy
				(
					pCliToolsCtx->pszDiskWriteBuffer + pCliToolsCtx->un32DiskWriteBufferCurLen,
					szTime
				);
				pCliToolsCtx->un32DiskWriteBufferCurLen += strlen( szTime );

				pCliToolsCtx->un32DiskWriteBufferCurLen += TBXCli_strncpy
				(
					pCliToolsCtx->pszDiskWriteBuffer + pCliToolsCtx->un32DiskWriteBufferCurLen,
					pszLineToPrint,
					TBX_CLI_TOOLS_DISK_BUFFER_SIZE - pCliToolsCtx->un32DiskWriteBufferCurLen - 1
				);
			}
			pCliToolsCtx->pszDiskWriteBuffer[ pCliToolsCtx->un32DiskWriteBufferCurLen ] = '\n';
			pCliToolsCtx->un32DiskWriteBufferCurLen++;
			pCliToolsCtx->pszDiskWriteBuffer[ pCliToolsCtx->un32DiskWriteBufferCurLen ] = '\0';

			TbxCliToolsFlushDiskWriteBuffer( pCliToolsCtx, in_fFlush );

			if( pCliToolsCtx->un32CurrentLogFileSize >= pCliToolsCtx->Params.un32MaxLogFileSize )
			{
				/* Split the log file now */
				TbxCliToolsLogFileSplitPrivate( pCliToolsCtx, TBX_TRUE );
			}
		}
		#endif

		if( !pCliToolsCtx->CliThreadContext.fRunning && !pCliToolsCtx->Params.fDisableTerminalOutput )
		{
			/* CLI Tools not started yet. Any log printed here will not draw to the screen.
			   To make sure errors get printed to the user, we can display them using standard
			   "printf" here. */
			TBXCli_fprintf( stdout, "%s\n" DEFAULT_COLOR, pszLineToPrint );
		}

		#if defined MQX
		{
			/* Also print to dbgprintf */
			TBX_CHAR	szBuffer[ 2048 ];
			TBXCli_strncpy( szBuffer, pszLineToPrint, sizeof(szBuffer) );
			dbgprintf( "%s\n", szBuffer );
		}
		#endif

		if( in_fPrintCircularLog )
		{
			TBX_UINT32					un32BufferOffset	= 0;
			PTBX_CHAR					pszLine				= pCliToolsCtx->paCircularLog[ pCliToolsCtx->un32CircularLogNextLineIndex ].pszLine;

			if( fIsACutLine )
			{
				/* Clear the date, in case it's a cut line */
				memset( pszLine, ' ', strlen(szTime) );
				un32BufferOffset += strlen(szTime);
				un32BufferOffset += sprintf( &(pszLine[ un32BufferOffset ]), "%s", szPrevLineColor[0] ? szPrevLineColor : in_pszColor );
			}
			else
			{
				un32BufferOffset += sprintf( &(pszLine[ un32BufferOffset ]), "%s", in_pszTimeColor );
				un32BufferOffset += sprintf( &(pszLine[ un32BufferOffset ]), "%s", szTime );
				un32BufferOffset += sprintf( &(pszLine[ un32BufferOffset ]), "%s", szPrevLineColor[0] ? szPrevLineColor : in_pszColor );
			}
			un32BufferOffset += TbxCliTools_snprintf
			(
				&( pszLine[ un32BufferOffset ] ),
				pCliToolsCtx->Params.un32MaxScreenWidth - un32BufferOffset - 6,
				"%s" DEFAULT_COLOR,
				pszLineToPrint
			);

			/* Terminate the string with DEFAULT_COLOR */
			if( cBkpChar == '\0' )
			{
				strcpy( &(pszLine[ un32BufferOffset ]), DEFAULT_COLOR );
			}


			/* Remove previous checkpoint on overwritten line */
			pCliToolsCtx->pafCircularLogCheckpoints[ pCliToolsCtx->un32CircularLogNextLineIndex ] = TBX_FALSE;

			/* Count this added line to log */
			if( pCliToolsCtx->un32CircularLogCurrentNbLines < pCliToolsCtx->Params.un32LogBufferMaxLines )
			{
				pCliToolsCtx->un32CircularLogCurrentNbLines++;
			}

			/* When adding lines to log, make scroll position "follow" so we continue
			   to display the same lines to user */
			if( pCliToolsCtx->un32ScrollPosition != 0 )
			{
				pCliToolsCtx->un32ScrollPosition++;

				if( pCliToolsCtx->un32ScrollPosition > pCliToolsCtx->un32ScrollPositionMax )
					pCliToolsCtx->un32ScrollPosition = pCliToolsCtx->un32ScrollPositionMax;
			}

			pCliToolsCtx->un32CircularLogNextLineIndex++;
			if( pCliToolsCtx->un32CircularLogNextLineIndex >= pCliToolsCtx->Params.un32LogBufferMaxLines )
			{
				pCliToolsCtx->fCircularLogFull = TBX_TRUE;
				pCliToolsCtx->un32CircularLogNextLineIndex = 0;
			}

			/* Need refresh here only if scroll is either top or bottom (otherwise, the added line
			   does not change the displayed lines) */
			if( pCliToolsCtx->un32ScrollPosition == 0 || pCliToolsCtx->un32ScrollPosition >= pCliToolsCtx->un32ScrollPositionMax )
				pCliToolsCtx->fRefreshRequired = TBX_TRUE;
		}

		if( cBkpChar && pszLineToPrintNext )
		{
			pszLineToPrintNext--;
			*pszLineToPrintNext = cBkpChar;
			cBkpChar = 0;

			/* Next line is the remainder of this cut line */
			fIsACutLine = TBX_TRUE;
		}
		else
		{
			fIsACutLine = TBX_FALSE;
		}

		if( !pszLineToPrintNext )
			break;

		/* Continue with next line */
		pszLineToPrint		= pszLineToPrintNext;
		pszLineToPrintNext	= strchr_fast( pszLineToPrint, '\n' );

		strcpy( szPrevLineColor, szCurrLineColor );
		if( !pszLineToPrintNext )
		{
			/* Line is not split. Maybe it needs wrap? */
			pszLineToPrintNext = strcut_skipcolor (pszLineToPrint, un32WrapLength, szCurrLineColor);
			if (pszLineToPrintNext)
			{
				cBkpChar = *pszLineToPrintNext;
			}
		}
		if( pszLineToPrintNext )
		{
#if 0			
			/* Check if line is too long */
			if( (TBX_UINT32)(pszLineToPrintNext - pszLineToPrint) > un32WrapLength )
			{
				pszLineToPrintNext = pszLineToPrint + un32WrapLength;
				cBkpChar = *pszLineToPrintNext;
			}
#endif			
			/* Cut the string here, no 'end-lines' are allowed. */
			*pszLineToPrintNext = '\0';
			/* Set pointer to beginning of next line */
			pszLineToPrintNext++;
			if( *pszLineToPrintNext == '\0' )
				pszLineToPrintNext = NULL;	/* There is nothing in 'next' line, just ignore it. */
		}
	}

	return;
}

/*-------------------------------------------------------------------------------------------------------------------------------
 |
 |  TbxCliToolsLogPrint		:	Log print function.
 |								Logs are stored in a circular buffer used to display in the
 |								command-line interface.
 |								Logs are also saved into the application's log file.
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
)
{
	PTBX_CLI_TOOLS_CLI_CONTEXT pCliToolsCtx = (PTBX_CLI_TOOLS_CLI_CONTEXT)in_hCliToolsHandle;

	if( pCliToolsCtx )
	{
		TBX_BOOL fPrintCircularLog	= ((TBX_UINT32)pCliToolsCtx->Params.TraceLevelStdout <= (TBX_UINT32)in_TraceLevel);
		TBX_BOOL fPrintFile			= ((TBX_UINT32)pCliToolsCtx->Params.TraceLevelLogFile <= (TBX_UINT32)in_TraceLevel);
		
		if( fPrintCircularLog || fPrintFile )
		{
			va_list		marker;
			va_start (marker, in_pszString);

			TBX_MUTEX_GET (pCliToolsCtx->Mutex);
			TbxCliToolsLogPrintPrivate
			(
				pCliToolsCtx,
				in_TraceLevel,
				fPrintCircularLog,
				fPrintFile,
				(in_TraceLevel == TRACE_LEVEL_ERROR) ? FRED : DEFAULT_COLOR,
				in_pszColor,
				in_pszString,
				marker,
				(in_TraceLevel == TRACE_LEVEL_ERROR) ? TBX_TRUE : !pCliToolsCtx->Params.fFlushLogOnlyOnError
			);
			TBX_MUTEX_GIV (pCliToolsCtx->Mutex);

			va_end (marker);
		}
	}
	else
	{
		va_list		marker;
		va_start (marker, in_pszString);
			
		vfprintf( stderr, in_pszString, marker );

		va_end (marker);
	}
}

void TbxCliToolsLogvPrint
(
	IN		TBX_CLI_TOOLS_HANDLE					in_hCliToolsHandle,
	IN		TRACE_LEVEL								in_TraceLevel,
	IN		const TBX_CHAR*							in_pszColor,
	IN		const TBX_CHAR*							in_pszString,
	IN		va_list									in_Marker
)
{
	PTBX_CLI_TOOLS_CLI_CONTEXT	pCliToolsCtx		= (PTBX_CLI_TOOLS_CLI_CONTEXT)in_hCliToolsHandle;
	if( pCliToolsCtx )
	{
		TBX_BOOL fPrintCircularLog	= ((TBX_UINT32)pCliToolsCtx->Params.TraceLevelStdout <= (TBX_UINT32)in_TraceLevel);
		TBX_BOOL fPrintFile			= ((TBX_UINT32)pCliToolsCtx->Params.TraceLevelLogFile <= (TBX_UINT32)in_TraceLevel);

		if( fPrintCircularLog || fPrintFile )
		{
			TBX_MUTEX_GET (pCliToolsCtx->Mutex);
			TbxCliToolsLogPrintPrivate
			(
				pCliToolsCtx,
				in_TraceLevel,
				fPrintCircularLog,
				fPrintFile,
				(in_TraceLevel == TRACE_LEVEL_ERROR) ? FRED : NULL,
				in_pszColor,
				in_pszString,
				in_Marker,
				(in_TraceLevel == TRACE_LEVEL_ERROR) ? TBX_TRUE : !pCliToolsCtx->Params.fFlushLogOnlyOnError
			);
			TBX_MUTEX_GIV (pCliToolsCtx->Mutex);
		}
	}
	else
	{
		vfprintf( stderr, in_pszString, in_Marker );
	}
}

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
)
{
	PTBX_CLI_TOOLS_CLI_CONTEXT	pCliToolsCtx	= (PTBX_CLI_TOOLS_CLI_CONTEXT)in_hCliToolsHandle;

	if( pCliToolsCtx )
	{
		TBX_MUTEX_GET (pCliToolsCtx->Mutex);
		TbxCliToolsLogFileSplitPrivate( pCliToolsCtx, TBX_FALSE );
		TBX_MUTEX_GIV (pCliToolsCtx->Mutex);
	}
}

static void TbxCliToolsLogFileSplitPrivate
(
	IN		PTBX_CLI_TOOLS_CLI_CONTEXT				pCliToolsCtx,
	IN		TBX_BOOL								fLogFileFull
)
{
	#ifdef TBX_CLI_TOOLS_USE_LOG_FILE
	TBX_CHAR					szCurrentLogFilePath[ 512 ];

	/* Increase the segment number */
	pCliToolsCtx->un32CurrentLogFileSegment++;
	pCliToolsCtx->un32CurrentLogFileSize = 0;

	strcpy( szCurrentLogFilePath, pCliToolsCtx->szCurrentLogFilePath );

	/* Print a trace to indicate why segment has been created */
	if( fLogFileFull )
	{
		TbxCliToolsLogPrint
		(
			(TBX_CLI_TOOLS_HANDLE)pCliToolsCtx, TRACE_LEVEL_ALWAYS, DEFAULT_COLOR,
			"Log file full: splitting log file '%s', new segment number %u\n",
			szCurrentLogFilePath,
			pCliToolsCtx->un32CurrentLogFileSegment
		);
	}
	else
	{
		TbxCliToolsLogPrint
		(
			(TBX_CLI_TOOLS_HANDLE)pCliToolsCtx, TRACE_LEVEL_ALWAYS, DEFAULT_COLOR,
			"Manually splitting log file '%s', new segment number %u\n",
			szCurrentLogFilePath,
			pCliToolsCtx->un32CurrentLogFileSegment
		);
	}

	/* Perform log file rotation now, with new segment. */
	TbxCliToolsLogFilesRotation( pCliToolsCtx );

	/* Print a trace to indicate why segment has been created */
	if( fLogFileFull )
	{
		TbxCliToolsLogPrint
		(
			(TBX_CLI_TOOLS_HANDLE)pCliToolsCtx, TRACE_LEVEL_ALWAYS, DEFAULT_COLOR,
			"Log file full: split log file, previous segment was '%s', new is '%s'\n",
			szCurrentLogFilePath,
			pCliToolsCtx->szCurrentLogFilePath
		);
	}
	else
	{
		TbxCliToolsLogPrint
		(
			(TBX_CLI_TOOLS_HANDLE)pCliToolsCtx, TRACE_LEVEL_ALWAYS, DEFAULT_COLOR,
			"Manually split log file, previous segment was '%s', new is '%s'\n",
			szCurrentLogFilePath,
			pCliToolsCtx->szCurrentLogFilePath
		);
	}
	#endif
}

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
)
{
	PTBX_CLI_TOOLS_CLI_CONTEXT pCliToolsCtx	= (PTBX_CLI_TOOLS_CLI_CONTEXT)in_hCliToolsHandle;
	if( !pCliToolsCtx )
	{
		return TBX_RESULT_INVALID_CONTEXT;
	}

	TBX_MUTEX_GET (pCliToolsCtx->Mutex);
	if( out_szLogFilePath )
	{
		#ifdef TBX_CLI_TOOLS_USE_LOG_FILE
			strcpy( out_szLogFilePath, pCliToolsCtx->szCurrentLogFilePath );
		#else
			out_szLogFilePath[0] = '\0';
		#endif
	}
	TBX_MUTEX_GIV (pCliToolsCtx->Mutex);

	return TBX_RESULT_OK;
}

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
	OUT		PTBX_CLI_TOOLS_TIMESTAMP				out_pTimestamp
)
{
	#if defined(WIN32)
		SYSTEMTIME			SystemTime;
		TBX_UINT32			un32PreciseTick;
		static SYSTEMTIME	sPrevSystemTime;
		static TBX_UINT32	sun32PrevPreciseTick;

		memset( &SystemTime, 0, sizeof(SystemTime) );
		un32PreciseTick = TBX_GET_TICK_PRECISE();
		GetLocalTime( &SystemTime );
		
		if( memcmp( &sPrevSystemTime, &SystemTime, sizeof(sPrevSystemTime) ) == 0 )
		{
			/* Same time as previous. Adjust milliseconds with more precision */
			TBX_UINT32	un32ExtraMs = un32PreciseTick - sun32PrevPreciseTick;
			if( un32ExtraMs )
			{
				SystemTime.wMilliseconds += (WORD)un32ExtraMs;
				if( SystemTime.wMilliseconds >= 1000 )
				{
					SystemTime.wMilliseconds -= 1000;
					SystemTime.wSecond += 1;
					if( SystemTime.wSecond >= 60 )
					{
						SystemTime.wSecond -= 60;
						SystemTime.wMinute += 1;
						if( SystemTime.wMinute >= 60 )
						{
							SystemTime.wMinute -= 60;
							SystemTime.wHour += 1;
						}
						if( SystemTime.wHour >= 24 )
						{
							SystemTime.wHour -= 24;
							SystemTime.wDay += 1;
							if( SystemTime.wDay >= 28 )
							{
								/* Hmmm... that would be more complicated to implement. Let's just round to last millisecond of previous day. */
								SystemTime.wDay -= 1;
								SystemTime.wHour = 23;
								SystemTime.wMinute = 59;
								SystemTime.wSecond = 59;
								SystemTime.wMilliseconds = 999;
							}
						}
					}
				}
			}
		}
		else
		{
			sPrevSystemTime = SystemTime;
			sun32PrevPreciseTick = un32PreciseTick;
		}

		SystemTimeToFileTime
		(
			&SystemTime,
			&out_pTimestamp->FileTime
		);

	#elif defined(VXWORKS)

		out_pTimestamp->Timeval.tv_sec = time (NULL);
	#elif defined MQX
		_time_get( &out_pTimestamp->Timestruct );
	#else
		struct timezone						TimeZone;

		gettimeofday( &out_pTimestamp->Timeval, &TimeZone );

	#endif

	return TBX_RESULT_OK;
}


/*-------------------------------------------------------------------------------------------------------------------------------
 |
 |  TbxCliToolsGetTimestampMs	:	Returns current timestamp in milliseconds since epoch.
 |
 |  Note						:	~
 |
 |  Return						:	Timestamp in milliseconds since epoch
 |
 *------------------------------------------------------------------------------------------------------------------------------*/
TBX_UINT64 TbxCliToolsGetTimestampMs()
{
	TBX_CLI_TOOLS_TIMESTAMP		Timestamp;

	TbxCliToolsGetTimestamp( &Timestamp );

	#if defined(WIN32)
	{
		ULARGE_INTEGER		LargeInteger;
		LargeInteger.LowPart = Timestamp.FileTime.dwLowDateTime;
		LargeInteger.HighPart = Timestamp.FileTime.dwHighDateTime;

		return (TBX_UINT64)(LargeInteger.QuadPart / 10000);
	}
	#elif defined MQX
	{
		return
		(
			(TBX_UINT64)Timestamp.Timestruct.SECONDS * 1000 +
			Timestamp.Timestruct.MILLISECONDS
		);
	}
	#else
	{
		return
		(
			(TBX_UINT64)Timestamp.Timeval.tv_sec * 1000 +
			Timestamp.Timeval.tv_usec / 1000
		);
	}
	#endif
}

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
)
{
	/* Parameter check */
	if( !in_pTimestamp1 || !in_pTimestamp2 || !out_pun32ElapsedTimeInMs )
	{
		return TBX_RESULT_FAIL;
	}

	#ifdef WIN32
	{
		ULARGE_INTEGER		TimeStamp1;
		ULARGE_INTEGER		TimeStamp2;

		TimeStamp1.LowPart = in_pTimestamp1->FileTime.dwLowDateTime;
		TimeStamp1.HighPart = in_pTimestamp1->FileTime.dwHighDateTime;
		TimeStamp2.LowPart = in_pTimestamp2->FileTime.dwLowDateTime;
		TimeStamp2.HighPart = in_pTimestamp2->FileTime.dwHighDateTime;

		if( TimeStamp1.QuadPart >= TimeStamp2.QuadPart )
		{
			/* Timestamp 1 is bigger */
			*out_pun32ElapsedTimeInMs = (TBX_UINT32) ( (TimeStamp1.QuadPart - TimeStamp2.QuadPart) / 10000 );
		}
		else
		{
			/* Timestamp 2 is bigger */
			*out_pun32ElapsedTimeInMs = (TBX_UINT32) ( (TimeStamp2.QuadPart - TimeStamp1.QuadPart) / 10000 );
		}
	}
	#elif defined MQX
	{
		TBX_UINT64		un64Timestamp1Ms;
		TBX_UINT64		un64Timestamp2Ms;

		un64Timestamp1Ms = (TBX_UINT64)in_pTimestamp1->Timestruct.SECONDS * 1000 +
							in_pTimestamp1->Timestruct.MILLISECONDS;
		un64Timestamp2Ms = (TBX_UINT64)in_pTimestamp2->Timestruct.SECONDS * 1000 +
							in_pTimestamp2->Timestruct.MILLISECONDS / 1000;

		if( un64Timestamp1Ms >= un64Timestamp2Ms )
		{
			/* Timestamp 1 is bigger */
			*out_pun32ElapsedTimeInMs = (TBX_UINT32) (un64Timestamp1Ms - un64Timestamp2Ms);
		}
		else
		{
			/* Timestamp 2 is bigger */
			*out_pun32ElapsedTimeInMs = (TBX_UINT32) (un64Timestamp2Ms - un64Timestamp1Ms);
		}
	}
	#else
	{
		TBX_UINT64		un64Timestamp1Ms;
		TBX_UINT64		un64Timestamp2Ms;

		un64Timestamp1Ms = (TBX_UINT64)in_pTimestamp1->Timeval.tv_sec * 1000 +
							in_pTimestamp1->Timeval.tv_usec / 1000;
		un64Timestamp2Ms = (TBX_UINT64)in_pTimestamp2->Timeval.tv_sec * 1000 +
							in_pTimestamp2->Timeval.tv_usec / 1000;

		if( un64Timestamp1Ms >= un64Timestamp2Ms )
		{
			/* Timestamp 1 is bigger */
			*out_pun32ElapsedTimeInMs = (TBX_UINT32) (un64Timestamp1Ms - un64Timestamp2Ms);
		}
		else
		{
			/* Timestamp 2 is bigger */
			*out_pun32ElapsedTimeInMs = (TBX_UINT32) (un64Timestamp2Ms - un64Timestamp1Ms);
		}
	}
	#endif


	return TBX_RESULT_OK;
}

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
)
{
	TBX_CHAR		szTimeBuffer[ 64 ];

	/* Format time */
	TbxCliToolsFormatTimePrivate( in_pTimestamp, szTimeBuffer );

	/* Copy to caller's buffer */
	Strncpy( out_pszBuffer, szTimeBuffer, in_un32BufferSize );
}



/*-------------------------------------------------------------------------------------------------------------------------------
 |
 |  TbxCliToolsFormatTimePrivate	:	Print current time.
 |
 |	in_pTimestamp					:	Timestamp to format
 |	out_pszBuffer					:	Buffer to print time to
 |
 |  Note							:	Buffer must hold at least 32 bytes
 |
 |  Return							:	~
 |
 *------------------------------------------------------------------------------------------------------------------------------*/
TBX_UINT32 TbxCliToolsFormatTimePrivate
(
	IN		PTBX_CLI_TOOLS_TIMESTAMP				in_pTimestamp,
	IN		PTBX_CHAR								out_pszBuffer
)
{
	PTBX_CHAR	pszDaySuffix = "th";

	#ifdef WIN32
		SYSTEMTIME			SystemTime;
		#ifdef TBX_CLI_TOOLS_PRINT_PRECISE_TIMESTAMP
		TBX_UINT32			un32TickPrecise = TBX_GET_TICK_PRECISE();
		static TBX_UINT32	un32TickPreciseFirst = 0;

		if( !un32TickPreciseFirst )
		{
			un32TickPreciseFirst = un32TickPrecise;
		}
		un32TickPrecise -= un32TickPreciseFirst;
		#endif

		FileTimeToSystemTime
		(
			&in_pTimestamp->FileTime,
			&SystemTime
		);

		if( SystemTime.wDay == 1 )
		{
			pszDaySuffix = "st";
		}
		else if( SystemTime.wDay == 2 )
		{
			pszDaySuffix = "nd";
		}
		else if( SystemTime.wDay == 3 )
		{
			pszDaySuffix = "rd";
		}

		#ifdef TBX_CLI_TOOLS_PRINT_PRECISE_TIMESTAMP
		return sprintf
		(
			out_pszBuffer,
			"%02u%s, %02u:%02u:%02u.%03u (%u.%03u) ",
			(unsigned int)SystemTime.wDay,
			pszDaySuffix,
			(unsigned int)SystemTime.wHour,
			(unsigned int)SystemTime.wMinute,
			(unsigned int)SystemTime.wSecond,
			(unsigned int)SystemTime.wMilliseconds,
			(unsigned int)(un32TickPrecise/1000),
			(unsigned int)(un32TickPrecise%1000)
		);
		#else
		return sprintf
		(
			out_pszBuffer,
			"%02u%s, %02u:%02u:%02u.%03u ",
			(unsigned int)SystemTime.wDay,
			pszDaySuffix,
			(unsigned int)SystemTime.wHour,
			(unsigned int)SystemTime.wMinute,
			(unsigned int)SystemTime.wSecond,
			(unsigned int)SystemTime.wMilliseconds
		);
		#endif
	#elif defined MQX
		DATE_STRUCT							DateStruct;
		
		_time_to_date( &in_pTimestamp->Timestruct, &DateStruct );

		if( DateStruct.DAY == 1 )
		{
			pszDaySuffix = "st";
		}
		else if( DateStruct.DAY == 2 )
		{
			pszDaySuffix = "nd";
		}
		else if( DateStruct.DAY == 3 )
		{
			pszDaySuffix = "rd";
		}

		return sprintf
		(
			out_pszBuffer,
			"%02u%s, %02u:%02u:%02u.%03u ",
			(unsigned int)DateStruct.DAY,
			pszDaySuffix,
			(unsigned int)DateStruct.HOUR,
			(unsigned int)DateStruct.MINUTE,
			(unsigned int)DateStruct.SECOND,
			(unsigned int)DateStruct.MILLISEC
		);
	#else
		struct tm							DateTimeStruct;

		/* Format current date */
		TBX_LOCALTIME( &in_pTimestamp->Timeval.tv_sec, &DateTimeStruct );

		if( DateTimeStruct.tm_mday == 1 )
		{
			pszDaySuffix = "st";
		}
		else if( DateTimeStruct.tm_mday == 2 )
		{
			pszDaySuffix = "nd";
		}
		else if( DateTimeStruct.tm_mday == 3 )
		{
			pszDaySuffix = "rd";
		}

		return sprintf
		(
			out_pszBuffer,
			"%u%s, %02u:%02u:%02u.%03u ",
			(unsigned int)DateTimeStruct.tm_mday,
			pszDaySuffix,
			(unsigned int)DateTimeStruct.tm_hour,
			(unsigned int)DateTimeStruct.tm_min,
			(unsigned int)DateTimeStruct.tm_sec,
			(unsigned int)(in_pTimestamp->Timeval.tv_usec / 1000)
		);
	#endif
}


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
)
{
	PTBX_CHAR	pszSelectedValue;

	/* Skip spaces */
	while( *in_pszLine && isspace( (unsigned char)*in_pszLine ) )
	{
		in_pszLine++;
	}

	/* Find the selected value from the list */
	pszSelectedValue = in_pszLine;
	while( *pszSelectedValue && *pszSelectedValue != TBX_CLI_TOOLS_POPUP_LIST_CURR_ELEM_START_CHAR )
	{
		pszSelectedValue++;
	}

	if( *pszSelectedValue == '\0' )
	{
		/* No 'current' element. Maybe we still have a selection list without a 'current'? In that case we consider first is selected... */
		pszSelectedValue = in_pszLine;
		while( *pszSelectedValue && *pszSelectedValue != TBX_CLI_TOOLS_POPUP_LIST_START_CHAR )
		{
			pszSelectedValue++;
		}

		if( *pszSelectedValue == TBX_CLI_TOOLS_POPUP_LIST_START_CHAR )
		{
			/* Skip the separator */
			pszSelectedValue++;
			
			/* Skip options */
			while
			(
				*pszSelectedValue == TBX_CLI_TOOLS_POPUP_LIST_SHORT_OPTION_CHAR ||
				*pszSelectedValue == TBX_CLI_TOOLS_POPUP_LIST_OPTION_NO_IMMEDIATE_VALIDATION
			)
			{
				pszSelectedValue++;
			}
		}
	}
	else
	{
		/* Skip the separator */
		pszSelectedValue++;
	}

	if( *pszSelectedValue != '\0' )
	{
		PTBX_CHAR	pszSelectedValueEnd;
		TBX_CHAR	cBkpChar;

		/* Skip white spaces */
		while( *pszSelectedValue && isspace( (unsigned char)*pszSelectedValue ) )
		{
			pszSelectedValue++;
		}

		/* Cut the string at the end of this value choice */
		pszSelectedValueEnd = pszSelectedValue;
		while
		( 
			*pszSelectedValueEnd && 
			*pszSelectedValueEnd != TBX_CLI_TOOLS_POPUP_LIST_SEPARATOR && 
			*pszSelectedValueEnd != TBX_CLI_TOOLS_POPUP_LIST_END_CHAR
		)
		{
			pszSelectedValueEnd++;
		}
		cBkpChar = *pszSelectedValueEnd;
		*pszSelectedValueEnd = '\0';

		strcpy( out_pszValue, pszSelectedValue );

		*pszSelectedValueEnd = cBkpChar;	/* Un-cut before returning */
	}
	else
	{
		/* Manually entered value (not a popup list) */
		strcpy( out_pszValue, in_pszLine );
	}
}

