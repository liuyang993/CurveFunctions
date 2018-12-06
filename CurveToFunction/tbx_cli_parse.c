/*--------------------------------------------------------------------------------------------------------------------------------
 |
 |	Project:    	TBX sample common sour code
 |
 |	Filename:   	tbx_cli_tools_parse.c
 |
 |	Copyright:  	TelcoBridges 2002-2007, All Rights Reserved
 |
 |	Description:	This file contains utility functions for tbx_cli_tools to parse configuration files.
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
#include "tbx_cli_tools_private.h"


/*--------------------------------------------------------------------------------------------------------------------------------
 |  Forward declarations
 *------------------------------------------------------------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------------------------------------------------------------
 |  Defines
 *------------------------------------------------------------------------------------------------------------------------------*/


#define TBX_CLI_TOOLS_NETWORK_SECTION_ENTER				".NETWORK"
#define TBX_CLI_TOOLS_LOG_FILE_SECTION_ENTER			".LOG_FILE_CONFIG"
#define TBX_CLI_TOOLS_CLI_SECTION_ENTER					".CLI_CONFIG"
#define TBX_CLI_TOOLS_SECTION_EXIT						".END"

#define TBX_CLI_TOOLS_NETWORK_TBX_GW_PORT				"TBX_GW_PORT"
#define TBX_CLI_TOOLS_NETWORK_APPLICATION_NAME			"application_name"
#define TBX_CLI_TOOLS_NETWORK_TBX_GW_ADDRESS_0			"TBX_GW_ADDRESS_0"
#define TBX_CLI_TOOLS_NETWORK_TBX_GW_ADDRESS_1			"TBX_GW_ADDRESS_1"
#define TBX_CLI_TOOLS_NETWORK_TBX_GW_REMOTE_ADDR		"TBX_GW_REMOTE_ADDR"
#define TBX_CLI_TOOLS_NETWORK_POLL_DELAY				"poll_delay_ms"
#define TBX_CLI_TOOLS_NETWORK_NETWORK_DOWN_DELAY		"network_down_delay_ms"
#define TBX_CLI_TOOLS_NETWORK_ADPATER_DOWN_DELAY		"adapter_down_delay_ms"
#define TBX_CLI_TOOLS_NETWORK_FLOW_CONTROL_MODE			"flow_control_mode"

#define TBX_CLI_TOOLS_CONFIG_TRACE_LEVEL_STDOUT			"trace_level_stdout"
#define TBX_CLI_TOOLS_CONFIG_TRACE_LEVEL_LOG_FILE		"trace_level_log_file"
#define TBX_CLI_TOOLS_CONFIG_LOG_FILE_PATH				"log_file_path"
#define TBX_CLI_TOOLS_CONFIG_MAX_LOG_FILE_SEG_SIZE		"max_log_file_segment_size"
#define TBX_CLI_TOOLS_CONFIG_MAX_LOG_FILE_SEG_PER_DAY	"max_log_file_segment_per_day"
#define TBX_CLI_TOOLS_CONFIG_LOG_KEEP_COLOR_CODES		"log_keep_color_codes"
#define TBX_CLI_TOOLS_CONFIG_DISABLE_TERMINAL_INPUT		"disable_terminal_input"
#define TBX_CLI_TOOLS_CONFIG_DISABLE_TERMINAL_OUTPUT	"disable_terminal_output"
#define TBX_CLI_TOOLS_CONFIG_SCREEN_BUFFER_LOG_LINES	"screen_buffer_log_lines"
#define TBX_CLI_TOOLS_CONFIG_SCREEN_REDRAW_MIN_DELAY_MS	"screen_redraw_min_delay_ms"
#define TBX_CLI_TOOLS_CONFIG_LOW_PRIORITY_CLI			"low_priority_cli"

/* Legacy keywords */
#define TBX_CLI_TOOLS_NETWORK_TBX_GW_PORT_LEGACY		"tbx_gw_port"
#define TBX_CLI_TOOLS_NETWORK_TBX_GW_PORT_LEGACY_2		"ipport"
#define TBX_CLI_TOOLS_NETWORK_TBX_GW_PORT_LEGACY_3		"serverport"
#define TBX_CLI_TOOLS_NETWORK_APPLICATION_NAME_LEGACY1	"manager_name"
#define TBX_CLI_TOOLS_NETWORK_APPLICATION_NAME_LEGACY2	"streamserver_name"



/*--------------------------------------------------------------------------------------------------------------------------------
 |  Types
 *------------------------------------------------------------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------------------------------------------------------------
 |  Global variables
 *------------------------------------------------------------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------------------------------------------------------------
 |  Macros
 *------------------------------------------------------------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------------------------------------------------------------
 |  Function Prototypes
 *------------------------------------------------------------------------------------------------------------------------------*/

static TBX_BOOL TbxCliToolsStringToBool
(
  IN		PTBX_CHAR					in_pszValue
);




TBX_BOOL TbxCliToolsCliConfigParseLine
(
	IN		PTBX_CHAR								in_pszLine,
	OUT		PTBX_CLI_TOOLS_CONFIG_PARAMS			out_pCliConfig,
	OUT		TBX_CHAR								out_szErrorString[ 256 ]
)
{
	TBX_BOOL							fHandled;

	/*---------------------------------------------------------------------------------------------------------------------------
	 |  Code section
	 *--------------------------------------------------------------------------------------------------------------------------*/
	CODE
	{
		/* Initialize local variables */
		fHandled			= TBX_FALSE;

		if( out_szErrorString ) *out_szErrorString = '\0';
		if( !out_pCliConfig )
		{
			TBX_EXIT_ERROR( TBX_RESULT_INVALID_CONTEXT, 0, "NULL parameter" );
		}

		if( out_pCliConfig->LogFileConfig.un32Reserved != 1 && out_pCliConfig->CliConfig.un32Reserved != 1 )
		{
			/* We're outside the ".NETWORK" section. Search for it */
			if( strstr( in_pszLine, TBX_CLI_TOOLS_LOG_FILE_SECTION_ENTER ) )
			{
				/* Now enter the section */
				memset( &out_pCliConfig->LogFileConfig, 0, sizeof(out_pCliConfig->LogFileConfig ) );
				out_pCliConfig->LogFileConfig.un32Reserved = 1;
				fHandled = TBX_TRUE;
				TBX_EXIT_SUCCESS (TBX_RESULT_OK);
			}
			else if( strstr( in_pszLine, TBX_CLI_TOOLS_CLI_SECTION_ENTER ) )
			{
				/* Now enter the section */
				memset( &out_pCliConfig->CliConfig, 0, sizeof(out_pCliConfig->CliConfig ) );
				out_pCliConfig->CliConfig.un32Reserved = 1;
				fHandled = TBX_TRUE;
				TBX_EXIT_SUCCESS (TBX_RESULT_OK);
			}
			else
			{
				PTBX_CHAR	pszFound;

				/* Make sure the line contains '=' */
				pszFound = strstr( in_pszLine, "=" );
				if( !pszFound )
				{
					/* Nothing to do with this line */
					TBX_EXIT_CLEANUP (TBX_RESULT_OK);
				}

				pszFound++;	/* Skip '=' */
				TbxStringRemoveBlanks( pszFound );

				if( strstr( in_pszLine, TBX_CLI_TOOLS_CONFIG_TRACE_LEVEL_STDOUT ) )
				{
					fHandled = TBX_TRUE;
					out_pCliConfig->LogFileConfig.TraceLevelStdout = (TRACE_LEVEL)strtoul( pszFound, NULL, 0 );
					if( out_pCliConfig->LogFileConfig.TraceLevelStdout > TRACE_LEVEL_MAX )
					{
						TBX_EXIT_ERROR (TBX_RESULT_INVALID_CONFIG, 0, TBX_CLI_TOOLS_CONFIG_TRACE_LEVEL_STDOUT " is invalid (too big)" );
					}
				}
				else if( strstr( in_pszLine, TBX_CLI_TOOLS_CONFIG_TRACE_LEVEL_LOG_FILE ) )
				{
					fHandled = TBX_TRUE;
					out_pCliConfig->LogFileConfig.TraceLevelLogFile = (TRACE_LEVEL)strtoul( pszFound, NULL, 0 );
					if( out_pCliConfig->LogFileConfig.TraceLevelLogFile > TRACE_LEVEL_MAX )
					{
						TBX_EXIT_ERROR (TBX_RESULT_INVALID_CONFIG, 0, TBX_CLI_TOOLS_CONFIG_TRACE_LEVEL_LOG_FILE " is invalid (too big)" );
					}
				}
				else if( strstr( in_pszLine, TBX_CLI_TOOLS_CONFIG_LOG_FILE_PATH ) )
				{
					fHandled = TBX_TRUE;
					Strncpy
					(
						out_pCliConfig->LogFileConfig.szLogFileName,
						pszFound,
						sizeof( out_pCliConfig->LogFileConfig.szLogFileName )
					);
				}
				else if( strstr( in_pszLine, TBX_CLI_TOOLS_CONFIG_DISABLE_TERMINAL_INPUT ) )
				{
					fHandled = TBX_TRUE;
					out_pCliConfig->CliConfig.fDisableTerminalInput = TbxCliToolsStringToBool( pszFound );
				}
				else if( strstr( in_pszLine, TBX_CLI_TOOLS_CONFIG_DISABLE_TERMINAL_OUTPUT ) )
				{
					fHandled = TBX_TRUE;
					out_pCliConfig->CliConfig.fDisableTerminalOutput = TbxCliToolsStringToBool( pszFound );
				}

			}
		}
		else
		{
			PTBX_CHAR	pszFound;

			/* Search for parameters */
			TbxStringRemoveBlanks( in_pszLine );
			if( in_pszLine[0] == '#' || in_pszLine[0] == '\n' || in_pszLine[0] == '\0' || in_pszLine[0] == '\r' )
			{
				/* Ignore this line */
				TBX_EXIT_SUCCESS (TBX_RESULT_OK);
			}

			if( strstr( in_pszLine, TBX_CLI_TOOLS_SECTION_EXIT ) )
			{
				/* Finished parsing */
				if( out_pCliConfig->LogFileConfig.un32Reserved )
				{
					out_pCliConfig->LogFileConfig.un32Reserved = 0xFFFFFFFF;
				}
				if( out_pCliConfig->CliConfig.un32Reserved )
				{
					out_pCliConfig->CliConfig.un32Reserved = 0xFFFFFFFF;
				}
				fHandled = TBX_TRUE;
				TBX_EXIT_SUCCESS (TBX_RESULT_OK);
			}

			/* Make sure the line contains '=' */
			pszFound = strstr( in_pszLine, "=" );
			if( !pszFound )
			{
				TBX_EXIT_ERROR( TBX_RESULT_INVALID_CONFIG, 0, "Found no '=' in config line")
			}

			pszFound++;	/* Skip '=' */
			TbxStringRemoveBlanks( pszFound );

			fHandled = TBX_TRUE;
			if( out_pCliConfig->LogFileConfig.un32Reserved == 1 )
			{
				if( strstr( in_pszLine, TBX_CLI_TOOLS_CONFIG_TRACE_LEVEL_STDOUT ) )
				{
					out_pCliConfig->LogFileConfig.TraceLevelStdout = (TRACE_LEVEL)strtoul( pszFound, NULL, 0 );
					if( out_pCliConfig->LogFileConfig.TraceLevelStdout > TRACE_LEVEL_MAX )
					{
						TBX_EXIT_ERROR (TBX_RESULT_INVALID_CONFIG, 0, TBX_CLI_TOOLS_CONFIG_TRACE_LEVEL_STDOUT " is invalid (too big)" );
					}
				}
				else if( strstr( in_pszLine, TBX_CLI_TOOLS_CONFIG_TRACE_LEVEL_LOG_FILE ) )
				{
					out_pCliConfig->LogFileConfig.TraceLevelLogFile = (TRACE_LEVEL)strtoul( pszFound, NULL, 0 );
					if( out_pCliConfig->LogFileConfig.TraceLevelLogFile > TRACE_LEVEL_MAX )
					{
						TBX_EXIT_ERROR (TBX_RESULT_INVALID_CONFIG, 0, TBX_CLI_TOOLS_CONFIG_TRACE_LEVEL_LOG_FILE " is invalid (too big)" );
					}
				}
				else if( strstr( in_pszLine, TBX_CLI_TOOLS_CONFIG_LOG_FILE_PATH ) )
				{
					Strncpy
					(
						out_pCliConfig->LogFileConfig.szLogFileName,
						pszFound,
						sizeof( out_pCliConfig->LogFileConfig.szLogFileName )
					);
				}
				else if( strstr( in_pszLine, TBX_CLI_TOOLS_CONFIG_MAX_LOG_FILE_SEG_SIZE ) )
				{
					out_pCliConfig->LogFileConfig.un32MaxLogFileSize = (TBX_UINT32)strtoul( pszFound, NULL, 0 );
					if( out_pCliConfig->LogFileConfig.un32MaxLogFileSize < 10*1024 )
					{
						TBX_EXIT_ERROR (TBX_RESULT_INVALID_CONFIG, 0, TBX_CLI_TOOLS_CONFIG_MAX_LOG_FILE_SEG_SIZE " is too small." );
					}
					if( out_pCliConfig->LogFileConfig.un32MaxLogFileSize > TBX_CLI_TOOLS_CLI_DEFAULT_MAX_LOG_FILE_SIZE )
					{
						TBX_EXIT_ERROR (TBX_RESULT_INVALID_CONFIG, 0, TBX_CLI_TOOLS_CONFIG_MAX_LOG_FILE_SEG_SIZE " is too big." );
					}
				}
				else if( strstr( in_pszLine, TBX_CLI_TOOLS_CONFIG_MAX_LOG_FILE_SEG_PER_DAY ) )
				{
					out_pCliConfig->LogFileConfig.un32MaxLogFilesPerDay = (TBX_UINT32)strtoul( pszFound, NULL, 0 );
					if( !out_pCliConfig->LogFileConfig.un32MaxLogFilesPerDay )
					{
						out_pCliConfig->LogFileConfig.un32MaxLogFilesPerDay = 1;
					}
				}
				else if( strstr( in_pszLine, TBX_CLI_TOOLS_CONFIG_LOG_KEEP_COLOR_CODES ) )
				{
					out_pCliConfig->LogFileConfig.fLogKeepColorCodes = TbxCliToolsStringToBool( pszFound );
				}
				else
				{
					fHandled = TBX_FALSE;
				}
			}
			else if( out_pCliConfig->CliConfig.un32Reserved == 1 )
			{
				if( strstr( in_pszLine, TBX_CLI_TOOLS_CONFIG_DISABLE_TERMINAL_INPUT ) )
				{
					out_pCliConfig->CliConfig.fDisableTerminalInput = TbxCliToolsStringToBool( pszFound );
				}
				else if( strstr( in_pszLine, TBX_CLI_TOOLS_CONFIG_DISABLE_TERMINAL_OUTPUT ) )
				{
					out_pCliConfig->CliConfig.fDisableTerminalOutput = TbxCliToolsStringToBool( pszFound );
				}
				else if( strstr( in_pszLine, TBX_CLI_TOOLS_CONFIG_SCREEN_BUFFER_LOG_LINES ) )
				{
					out_pCliConfig->CliConfig.un32LogBufferMaxLines = (TBX_UINT32)strtoul( pszFound, NULL, 0 );
					if( out_pCliConfig->CliConfig.un32LogBufferMaxLines < 500 )
					{
						out_pCliConfig->CliConfig.un32LogBufferMaxLines = 500;
					}
					else if( out_pCliConfig->CliConfig.un32LogBufferMaxLines > TBX_CLI_TOOLS_CLI_MAX_LOG_BUFFER_LINES )
					{
						TBX_EXIT_ERROR (TBX_RESULT_INVALID_CONFIG, 0, TBX_CLI_TOOLS_CONFIG_SCREEN_BUFFER_LOG_LINES " is too big." );
					}
				}
				else if( strstr( in_pszLine, TBX_CLI_TOOLS_CONFIG_SCREEN_REDRAW_MIN_DELAY_MS ) )
				{
					out_pCliConfig->CliConfig.un32MinRefreshDelay = (TBX_UINT32)strtoul( pszFound, NULL, 0 );
					if( out_pCliConfig->CliConfig.un32MinRefreshDelay < TBX_CLI_TOOLS_CLI_MIN_DELAY_BETWEEN_CLS_SEC )
					{
						out_pCliConfig->CliConfig.un32MinRefreshDelay = TBX_CLI_TOOLS_CLI_MIN_DELAY_BETWEEN_CLS_SEC;
					}
					else if( out_pCliConfig->CliConfig.un32MinRefreshDelay > 1000 )
					{
						out_pCliConfig->CliConfig.un32MinRefreshDelay = 1000;
					}
				}
				else if( strstr( in_pszLine, TBX_CLI_TOOLS_CONFIG_LOW_PRIORITY_CLI ) )
				{
					out_pCliConfig->CliConfig.fLowPrioryThread = TbxCliToolsStringToBool( pszFound );
				}
				else
				{
					fHandled = TBX_FALSE;
				}
			}
			else
			{
				/* Should not happen */
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
		TbxCliTools_snprintf
		(
			out_szErrorString,
			256,
			"TbxCliToolsCliConfigParseLine: %s (result 0x%08X), line '%s'\n",
			TBX_ERROR_DESCRIPTION,
			TBX_ERROR_RESULT,
			in_pszLine
		);
	}

	/*---------------------------------------------------------------------------------------------------------------------------
	 |  Cleanup section
	 *--------------------------------------------------------------------------------------------------------------------------*/
	CLEANUP
	{
	}

	return fHandled;
}



TBX_VOID TbxCliToolsCliConfigCopyToTbxLibParams
(
	IN		PTBX_CLI_TOOLS_CONFIG_PARAMS			in_pCliConfig,
	IN_OUT	PTBX_CLI_TOOLS_INIT_PARAMS				io_pInitPrams
)
{
	if( in_pCliConfig->LogFileConfig.un32Reserved != 0xFFFFFFFF )
	{
		/* Has not been parsed. Set parameters that have mandatory value. */
		if( !in_pCliConfig->LogFileConfig.un32MaxLogFileSize )
			in_pCliConfig->LogFileConfig.un32MaxLogFileSize		= 1024*1024*10;

		if( !in_pCliConfig->LogFileConfig.un32MaxLogFilesPerDay )
			in_pCliConfig->LogFileConfig.un32MaxLogFilesPerDay	= 100;
	}
	if( in_pCliConfig->CliConfig.un32Reserved != 0xFFFFFFFF )
	{
		/* Has not been parsed. Set parameters that have mandatory value. */

		if( !in_pCliConfig->CliConfig.un32LogBufferMaxLines )
			in_pCliConfig->CliConfig.un32LogBufferMaxLines		= 1000;

		if( !in_pCliConfig->CliConfig.un32MinRefreshDelay )
			in_pCliConfig->CliConfig.un32MinRefreshDelay		= 100;

		in_pCliConfig->CliConfig.fLowPrioryThread			= TBX_TRUE;
	}

	io_pInitPrams->TraceLevelStdout				= in_pCliConfig->LogFileConfig.TraceLevelStdout;
	io_pInitPrams->TraceLevelLogFile			= in_pCliConfig->LogFileConfig.TraceLevelLogFile;
	Strncpy
	(
		io_pInitPrams->szLogFileName,
		in_pCliConfig->LogFileConfig.szLogFileName,
		sizeof( io_pInitPrams->szLogFileName )
	);
	io_pInitPrams->un32MaxLogFileSize			= in_pCliConfig->LogFileConfig.un32MaxLogFileSize;
	io_pInitPrams->un32MaxLogFilesPerDay		= in_pCliConfig->LogFileConfig.un32MaxLogFilesPerDay;
	io_pInitPrams->fLogKeepColorCodes			= in_pCliConfig->LogFileConfig.fLogKeepColorCodes;

	io_pInitPrams->fDisableTerminalInput		= in_pCliConfig->CliConfig.fDisableTerminalInput;
	io_pInitPrams->fDisableTerminalOutput		= in_pCliConfig->CliConfig.fDisableTerminalOutput;
	io_pInitPrams->un32LogBufferMaxLines		= in_pCliConfig->CliConfig.un32LogBufferMaxLines;
	io_pInitPrams->un32MinRefreshDelay			= in_pCliConfig->CliConfig.un32MinRefreshDelay;
	io_pInitPrams->fLowPrioryThread				= in_pCliConfig->CliConfig.fLowPrioryThread;
}





static TBX_BOOL TbxCliToolsStringToBool
(
  IN		PTBX_CHAR					in_pszValue
)
{
	TBX_BOOL	fValue = TBX_FALSE;

	if( isdigit( (unsigned char)*in_pszValue ) )
	{
		fValue = (strtoul( in_pszValue, NULL, 0 ) ? TBX_TRUE : TBX_FALSE);
	}
	else if( TbxCliTools_strincmp( in_pszValue, "TRUE", strlen( "TRUE" ) ) == 0 )
	{
		fValue = TBX_TRUE;
	}
	else
	{
		fValue = TBX_FALSE;
	}

	return fValue;
}


