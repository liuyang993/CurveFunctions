/*--------------------------------------------------------------------------------------------------------------------------------
 |
 |	Project:    	TBX sample common sour code
 |
 |	Filename:   	tbx_cli_tools_utils.c
 |
 |	Copyright:  	TelcoBridges 2002-2014, All Rights Reserved
 |
 |	Description:	This file contains utility functions for tbx_cli_tools.
 |
 |	Notes:      	Tabs = 4
 |
 *-------------------------------------------------------------------------------------------------------------------------------
 |
 |	Revision:   	$Revision: 154583 $
 |
 *------------------------------------------------------------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------------------------------------------------------------
 |  Includes
 *------------------------------------------------------------------------------------------------------------------------------*/
#include "tbx_cli_tools_private.h"
#include <time.h>

#define TBX_BUILD_NUMBER					0x02083F06		/* 2.8.63 RC6 */
#define TBX_BUILD_TAG						"RELEASE_V2_8_63_RC6"


/*--------------------------------------------------------------------------------------------------------------------------------
 |  Forward declarations
 *------------------------------------------------------------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------------------------------------------------------------
 |  Defines
 *------------------------------------------------------------------------------------------------------------------------------*/


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

static TBX_VOID TbxCliToolsGetPopupListChoice
(
  IN		TBX_CHAR			in_szPopupList[ TBX_CLI_TOOLS_CLI_LINE_WIDTH_MAX ],
  OUT		TBX_CHAR			out_pszSelected[ TBX_CLI_TOOLS_CLI_LINE_WIDTH_MAX ]
);
static PTBX_CHAR TbxCliToolsGetPopupListNextToken
(
  IN		PTBX_CHAR			in_szPopupListPartial,
  OUT		TBX_CHAR			out_pszElement[ TBX_CLI_TOOLS_CLI_LINE_WIDTH_MAX ]
);
static TBX_VOID TbxCliToolsPopupListNext
(
  IN		TBX_CHAR			in_szPopupList[ TBX_CLI_TOOLS_CLI_LINE_WIDTH_MAX ]
);
static TBX_VOID TbxCliToolsPopupListPrev
(
  IN		TBX_CHAR			in_szPopupList[ TBX_CLI_TOOLS_CLI_LINE_WIDTH_MAX ]
);


/*--------------------------------------------------------------------------------------------------------------------------------
 |  Implementation
 *------------------------------------------------------------------------------------------------------------------------------*/

static __inline void TbxCliToolsPrintPrivate
(
	IN		PTBX_CLI_TOOLS_CLI_CONTEXT				pCliToolsCtx,
	IN		PTBX_CHAR								in_pszString,
	IN		...
)
{
	va_list	marker;
	va_start (marker, in_pszString);

	TbxCliToolsvPrintPrivate( pCliToolsCtx, TBX_TRUE, in_pszString, marker );

	va_end (marker);
}

static __inline void TbxCliToolsPrintKeepColor
(
	IN		PTBX_CLI_TOOLS_CLI_CONTEXT				pCliToolsCtx,
	IN		PTBX_CHAR								in_pszString,
	IN		...
)
{
	va_list	marker;
	va_start (marker, in_pszString);

	TbxCliToolsvPrintPrivate( pCliToolsCtx, TBX_FALSE, in_pszString, marker );

	va_end (marker);
}

static __inline TBX_BOOL TbxCliToolsKeyNeedShiftModifierMark
(
	IN		TBX_INT			in_nOption
) 
{
	TBX_INT		nOption = in_nOption & TBX_CLI_TOOLS_KEY_CODE_MASK;

	if( nOption > 255 || !isprint( (unsigned char)nOption ) || isspace( (unsigned char)nOption ) )
	{
		/* Non-printable key requires shift modifier mask */
		return TBX_TRUE;
	}

	return TBX_FALSE;
}

/*-------------------------------------------------------------------------------------------------------------------------------
 |
 |   TbxCliToolsGetch	:	Function to read next input character, and remap
 |
 |  in_pCliToolsCtx		:	Context of this library
 |
 |  Note				:	~
 |
 |  Return				:	Key pressed
 |
 *------------------------------------------------------------------------------------------------------------------------------*/
TBX_INT  TbxCliToolsGetch( PTBX_CLI_TOOLS_CLI_CONTEXT in_pCliToolsCtx )
{
	TBX_INT						nOption;

	/*---------------------------------------------------------------------------------------------------------------------------
	 |  Code section
	 *--------------------------------------------------------------------------------------------------------------------------*/
	CODE
	{
		(void)in_pCliToolsCtx;
		#if defined WIN32
		{
			HANDLE						hStdIn;
			INPUT_RECORD				InputRecord;
			DWORD						n32NbEvents;
			TBX_BOOL					fKeyPressed;

			fKeyPressed = TBX_FALSE;
			hStdIn = GetStdHandle (STD_INPUT_HANDLE);

			nOption = 0;

			while( !fKeyPressed )
			{
				if( !PeekConsoleInput( hStdIn, &InputRecord, 1, &n32NbEvents ) ||
					n32NbEvents == 0 )
				{
					/* No key available or read error */
				  return 0;
				}

				if( !ReadConsoleInput( hStdIn, &InputRecord, 1, &n32NbEvents ) )
				{
					/* Read error */
					return 0;
				}
				switch( InputRecord.EventType )
				{
					case KEY_EVENT:
					{
						if( InputRecord.Event.KeyEvent.bKeyDown )
						{
							fKeyPressed = TBX_TRUE;

							/* Transcode to TbxCliTools constants */
							nOption = (unsigned char)InputRecord.Event.KeyEvent.uChar.AsciiChar;

							if( nOption == 0x1B )
							{
								nOption = TBX_CLI_TOOLS_KEY_ESCAPE;
							}
							else if( nOption == 3 )
							{
								nOption = TBX_CLI_TOOLS_KEY_CONTROL_C;
							}
							else if( nOption == 8 )
							{
								nOption = TBX_CLI_TOOLS_KEY_BACKSPACE;
							}
							else if( nOption == 0x00 ) /* Handle all special keys */
							{
								switch (InputRecord.Event.KeyEvent.wVirtualKeyCode)
								{
									case VK_NEXT: /* PGDWN */
										nOption = TBX_CLI_TOOLS_KEY_PAGE_DOWN_KEY;
										break;
									case VK_PRIOR: /* PGUP */
										nOption = TBX_CLI_TOOLS_KEY_PAGE_UP_KEY;
										break;
									case VK_HOME: /* HOME */
										nOption = TBX_CLI_TOOLS_KEY_HOME_KEY;
										break;
									case VK_END: /* END */
										nOption = TBX_CLI_TOOLS_KEY_END_KEY;
										break;
									case VK_UP: /* UPARROW */
										nOption = TBX_CLI_TOOLS_KEY_UP_ARROW_KEY;
										break;
									case VK_DOWN: /* DWNARROW */
										nOption = TBX_CLI_TOOLS_KEY_DOWN_ARROW_KEY;
										break;
									case VK_RIGHT: /* RGTHARROW */
										nOption = TBX_CLI_TOOLS_KEY_RIGHT_ARROW_KEY;
										break;
									case VK_LEFT: /* LFTARROW */
										nOption = TBX_CLI_TOOLS_KEY_LEFT_ARROW_KEY;
										break;
									case VK_INSERT: /* INSERT */
										nOption = TBX_CLI_TOOLS_KEY_INSERT;
										break;
									case VK_DELETE: /* DELETE */
										nOption = TBX_CLI_TOOLS_KEY_DELETE;
										break;
									case VK_LMENU:	/* Windows Key */
									case VK_RMENU:
										nOption = TBX_CLI_TOOLS_KEY_WINDOWS_KEY;
										break;
									case VK_F1:
										nOption = TBX_CLI_TOOLS_KEY_F1;
										break;
									case VK_F2:
										nOption = TBX_CLI_TOOLS_KEY_F2;
										break;
									case VK_F3:
										nOption = TBX_CLI_TOOLS_KEY_F3;
										break;
									case VK_F4:
										nOption = TBX_CLI_TOOLS_KEY_F4;
										break;
									case VK_F5:
										nOption = TBX_CLI_TOOLS_KEY_F5;
										break;
									case VK_F6:
										nOption = TBX_CLI_TOOLS_KEY_F6;
										break;
									case VK_F7:
										nOption = TBX_CLI_TOOLS_KEY_F7;
										break;
									case VK_F8:
										nOption = TBX_CLI_TOOLS_KEY_F8;
										break;
									case VK_F9:
										nOption = TBX_CLI_TOOLS_KEY_F9;
										break;
									case VK_F10:
										nOption = TBX_CLI_TOOLS_KEY_F10;
										break;
									case VK_F11:
										nOption = TBX_CLI_TOOLS_KEY_F11;
										break;
									case VK_F12:
										nOption = TBX_CLI_TOOLS_KEY_F12;
										break;

									default:
										nOption = 0;
										break;
								}
							}

							/* Add modifier keys */
							if( InputRecord.Event.KeyEvent.dwControlKeyState & SHIFT_PRESSED )
							{
								/* Don't set the shift modifier key for letters and typical printable characters */
								if( TbxCliToolsKeyNeedShiftModifierMark( nOption ) )
								{
									nOption |= TBX_CLI_TOOLS_KEY_SHIFT_MODIFIER_MASK;
								}
							}
							if( InputRecord.Event.KeyEvent.dwControlKeyState & LEFT_ALT_PRESSED ||
								InputRecord.Event.KeyEvent.dwControlKeyState & RIGHT_ALT_PRESSED )
							{
								nOption |= TBX_CLI_TOOLS_KEY_ALT_MODIFIER_MASK;
							}
							if( InputRecord.Event.KeyEvent.dwControlKeyState & LEFT_CTRL_PRESSED ||
								InputRecord.Event.KeyEvent.dwControlKeyState & RIGHT_CTRL_PRESSED )
							{
								nOption |= TBX_CLI_TOOLS_KEY_CTRL_MODIFIER_MASK;
							}
						}

					} break;

					case MOUSE_EVENT:
					{
						MOUSE_EVENT_RECORD*		pMouseEvent = &InputRecord.Event.MouseEvent;
//							TbxCliToolsLogPrint
//							(
//								(TBX_CLI_TOOLS_HANDLE)in_pCliToolsCtx, TRACE_LEVEL_ALWAYS, "",
//								"Mouse Button state: 0x%08X    Event:0x%08X   Position:0x%08X\n\n",
//								pMouseEvent->dwButtonState,
//								pMouseEvent->dwEventFlags,
//								pMouseEvent->dwMousePosition
//							);


						if( pMouseEvent->dwEventFlags & MOUSE_WHEELED )
						{
							fKeyPressed = TBX_TRUE;

							if( pMouseEvent->dwButtonState & 0x80000000 )
							{
								nOption = TBX_CLI_TOOLS_KEY_MOUSE_WHEEL_DOWN;
							}
							else
							{
								nOption = TBX_CLI_TOOLS_KEY_MOUSE_WHEEL_UP;
							}

							if( pMouseEvent->dwControlKeyState & SHIFT_PRESSED )
							{
								nOption |= TBX_CLI_TOOLS_KEY_SHIFT_MODIFIER_MASK;
							}
							if( pMouseEvent->dwControlKeyState & RIGHT_ALT_PRESSED ||
								pMouseEvent->dwControlKeyState & LEFT_ALT_PRESSED )
							{
								nOption |= TBX_CLI_TOOLS_KEY_ALT_MODIFIER_MASK;
							}
							if( pMouseEvent->dwControlKeyState & RIGHT_CTRL_PRESSED ||
								pMouseEvent->dwControlKeyState & LEFT_CTRL_PRESSED )
							{
								nOption |= TBX_CLI_TOOLS_KEY_CTRL_MODIFIER_MASK;
							}

						}
					}
					default:
					{
					}
				}	/* End of switch( EventType ) */
			}
		}
		#else
		{
			nOption = TBXCliGetch();

			#ifdef MQX
			if( nOption == -1 )
			{
				in_pCliToolsCtx->CliThreadContext.fContinue = TBX_FALSE;
				/* Simulate control-c to quit */
				nOption = TBX_CLI_TOOLS_KEY_CONTROL_C;
			}
			#endif

			if( nOption == 3 /* Control-C */ )
			{
				nOption = TBX_CLI_TOOLS_KEY_CONTROL_C;
			}
			else if( nOption == 0x1B /* ESC */ ) /* Edit Keypad remapping Solaris */
			{
				/* Escape sequence is "ESC[6~" where 6 is the key */
				if( TBXCliKeyboardHit() )
				{
					TBX_BOOL	fControlModifier;
					TBX_BOOL	fAltModifier;
					TBX_BOOL	fIsEscapeSequence	= TBX_TRUE;

					nOption = TBXCliGetch(); /* [ */

					/* Check if 'alt' has been pressed */
					if( nOption == 0x1B )
					{
						fAltModifier		= TBX_TRUE;

						/* Consume one more key here */
						if( TBXCliKeyboardHit() )
							nOption = TBXCliGetch(); /* key */
					}
					else
					{
						fAltModifier		= TBX_FALSE;
					}

					/* Check if 'control' has been pressed */
					if( nOption == 0x5B )
					{
						/* Control has NOT been pressed */
						fControlModifier	= TBX_FALSE;
					}
					else if( nOption == 0x4F )
					{
						/* Control has been pressed */
						fControlModifier	= TBX_TRUE;
					}
					else
					{
						/* It is ALT-and-non-escape-key (like "alt-x" for example) */
						fAltModifier		= TBX_TRUE;
						fControlModifier	= TBX_FALSE;
						fIsEscapeSequence	= TBX_FALSE;
					}

					if( fIsEscapeSequence )
					{
						if( TBXCliKeyboardHit() )
						{
							nOption = TBXCliGetch(); /* key */
							switch (nOption)
							{
								case '6': /* PGDWN */
									/* remap to keypad page down */
									nOption = TBX_CLI_TOOLS_KEY_PAGE_DOWN_KEY;
									TBXCliGetch(); /* ~ */
									break;
								case '5': /* PGUP */
									/* remap to keypad page up */
									nOption = TBX_CLI_TOOLS_KEY_PAGE_UP_KEY;
									TBXCliGetch(); /* ~ */
									break;
								case '1': /* HOME */
									/* remap to keypad home */
									nOption = TBX_CLI_TOOLS_KEY_HOME_KEY;
									TBXCliGetch(); /* ~ */
									break;
								case '4': /* END */
									/* remap to keypad end */
									nOption = TBX_CLI_TOOLS_KEY_END_KEY;
									TBXCliGetch(); /* ~ */
									break;
								case 'A': /* UPARROW */
									/* remap to keypad up arrow */
									nOption = TBX_CLI_TOOLS_KEY_UP_ARROW_KEY;
									break;
								case 'B': /* DWNARROW */
									/* remap to keypad down arrow */
									nOption = TBX_CLI_TOOLS_KEY_DOWN_ARROW_KEY;
									break;
								case 'C': /* RGTHARROW */
									/* remap to keypad right arrow */
									nOption = TBX_CLI_TOOLS_KEY_RIGHT_ARROW_KEY;
									break;
								case 'D': /* LFTARROW */
									/* remap to keypad left arrow */
									nOption = TBX_CLI_TOOLS_KEY_LEFT_ARROW_KEY;
									break;
								case 0x1B: /* ESCAPE */
									nOption = TBX_CLI_TOOLS_KEY_ESCAPE;
									break;
								case 0x5A: /* SHIFT-TAB */
									nOption = TBX_CLI_TOOLS_KEY_TAB | TBX_CLI_TOOLS_KEY_SHIFT_MODIFIER_MASK;
									break;
								default:
									nOption = ' '; /* Refresh if not known  */
								break;
							}
						}
						else
						{
							nOption = TBX_CLI_TOOLS_KEY_ESCAPE;
						}
					}

					if( fAltModifier )
					{
						nOption |= TBX_CLI_TOOLS_KEY_ALT_MODIFIER_MASK;
					}
					if( fControlModifier )
					{
						nOption |= TBX_CLI_TOOLS_KEY_CTRL_MODIFIER_MASK;
					}
				}
				else
				{
					nOption = TBX_CLI_TOOLS_KEY_ESCAPE;
				}
			}
			else if( nOption == 8 || nOption == 0x7F )
			{
				nOption = TBX_CLI_TOOLS_KEY_BACKSPACE;
			}
		}
		#endif

		/* End of the code (skip to cleanup) */
		TBX_EXIT_SUCCESS (nOption);
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
 |  TbxCliToolsGatherUserInput		:	This function gathers the user input strings
 |
 |  in_pCliToolsCtx					:	Context of the comand-line
 |	in_nOption						:	Key pressed
 |
 |  Note							:	~
 |
 |  Return							:	TBX_RESULT_OK
 |										TBX_RESULT_FAIL
 |
 *------------------------------------------------------------------------------------------------------------------------------*/
TBX_VOID TbxCliToolsGatherUserInput
(
  IN		PTBX_CLI_TOOLS_CLI_CONTEXT					in_pCliToolsCtx,
  IN		TBX_INT										in_nOption
)
{
	PTBX_CHAR		pszUserInput;
	pszUserInput	= in_pCliToolsCtx->CmdInput.Public.paszUserInput[ in_pCliToolsCtx->CmdInput.Public.un32CurrentPromptIdx ].szLine;

	/* Continue gathering input until end-line or escape is captured */
	switch( in_nOption )
	{
		case TBX_CLI_TOOLS_KEY_ESCAPE:
		{
			/* Cancel command input. */
			in_pCliToolsCtx->CmdInput.Public.CmdType = 0;
		} break;

		/* Display window page down */
		case TBX_CLI_TOOLS_KEY_PAGE_DOWN_KEY:
		{
			if( in_pCliToolsCtx->un32ScrollPosition > in_pCliToolsCtx->un32NbLogLinesDisplayed )
				in_pCliToolsCtx->un32ScrollPosition -= in_pCliToolsCtx->un32NbLogLinesDisplayed;
			else
				in_pCliToolsCtx->un32ScrollPosition = 0;
		} break;

		/* Display window page up */
		case TBX_CLI_TOOLS_KEY_PAGE_UP_KEY:
		{
			in_pCliToolsCtx->un32ScrollPosition += in_pCliToolsCtx->un32NbLogLinesDisplayed;
		} break;

		/* Display window home */
		case TBX_CLI_TOOLS_KEY_HOME_KEY:
		{
			in_pCliToolsCtx->un32ScrollPosition = 0xFFFFFFFF;
		} break;

		/* Display window end */
		case TBX_CLI_TOOLS_KEY_END_KEY:
		{
			in_pCliToolsCtx->un32ScrollPosition = 0;
		} break;


		case '\n':
		case '\r':
		{
			TbxCliToolsUserInputValidate( in_pCliToolsCtx, in_nOption, 1, TBX_TRUE );
		} break;

		case '\t':
		case TBX_CLI_TOOLS_KEY_DOWN_ARROW_KEY:
		{
			TbxCliToolsUserInputValidate( in_pCliToolsCtx, in_nOption, 1, TBX_FALSE );
		} break;

		case TBX_CLI_TOOLS_KEY_UP_ARROW_KEY:
		{
			TbxCliToolsUserInputValidate( in_pCliToolsCtx, in_nOption, -1, TBX_FALSE );
		} break;

		case TBX_CLI_TOOLS_KEY_RIGHT_ARROW_KEY:
		{
			if( pszUserInput[0] == TBX_CLI_TOOLS_POPUP_LIST_START_CHAR )
			{
				/* Change the selected element */
				TbxCliToolsPopupListNext( pszUserInput );
				if( in_pCliToolsCtx->fImmediateValidation )
				{
					TbxCliToolsUserInputValidate( in_pCliToolsCtx, in_nOption, 0, TBX_FALSE );
				}
			}
			else
			{
				in_pCliToolsCtx->CmdInput.fUserEnteredValue = TBX_TRUE;
			}
		} break;

		case TBX_CLI_TOOLS_KEY_LEFT_ARROW_KEY:
		{
			if( pszUserInput[0] == TBX_CLI_TOOLS_POPUP_LIST_START_CHAR )
			{
				/* Change the selected element */
				TbxCliToolsPopupListPrev( pszUserInput );
				if( in_pCliToolsCtx->fImmediateValidation )
				{
					TbxCliToolsUserInputValidate( in_pCliToolsCtx, in_nOption, 0, TBX_FALSE );
				}
			}
			else
			{
				in_pCliToolsCtx->CmdInput.fUserEnteredValue = TBX_TRUE;
			}
		} break;


		default:
		{
			TBX_UINT32	un32Strlen;

			if( !in_pCliToolsCtx->CmdInput.fUserEnteredValue )
			{
				/* First character typed by used. Clear default value. */
				*pszUserInput = '\0';
				in_pCliToolsCtx->CmdInput.fUserEnteredValue = TBX_TRUE;
			}

			un32Strlen		= strlen( pszUserInput );

			/* Append character to input buffer */
			switch( in_nOption )
			{
				case TBX_CLI_TOOLS_KEY_BACKSPACE:
				{
					if( un32Strlen )
					{
						pszUserInput[ un32Strlen - 1 ] = '\0';
					}
				} break;

				default:
				{
					if( un32Strlen + 1 < in_pCliToolsCtx->Params.un32MaxScreenWidth )
					{
						pszUserInput[ un32Strlen ]		= (TBX_CHAR)in_nOption;
						pszUserInput[ un32Strlen + 1 ]	= '\0';
					}
				} break;
			}
		} break;
	}

	return;
}





/*-------------------------------------------------------------------------------------------------------------------------------
 |
 |  TbxCliToolsUserInputValidate	:	This function validate a user input parameter
 |
 |  in_pCliToolsCtx					:	Context of the comand-line
 |	in_nOption						:	Key pressed
 |	in_n32Increment					:	Increment in input sequence:
 |											1:	Go to next question
 |											-1: Go to previous question
 |											0:  Stay in current question
 |	in_fTestAll						:	Test all inputs and try to apply command now
 |
 |  Note							:	~
 |
 |  Return							:	TBX_RESULT_OK
 |										TBX_RESULT_FAIL
 |
 *------------------------------------------------------------------------------------------------------------------------------*/
TBX_VOID TbxCliToolsUserInputValidate
(
  IN		PTBX_CLI_TOOLS_CLI_CONTEXT					in_pCliToolsCtx,
  IN		TBX_INT										in_nOption,
  IN		TBX_INT32									in_n32Increment,
  IN		TBX_BOOL									in_fTestAll
)
{
	do
	{
		in_pCliToolsCtx->CmdInput.un32EnteredValueTimestamp		= in_pCliToolsCtx->un32CurrentTimestamp;
		in_pCliToolsCtx->CmdInput.fEnteredValueValid			= TBX_TRUE;

		/* Validate the user input */
		if( in_pCliToolsCtx->Params.pFctValidateUserInput )           // if custome has own validate function ?  if have , use his own 
		{
			PFCT_TBX_CLI_TOOLS_VALIDATE_INPUT_CALLBACK	pFctValidateUserInput = in_pCliToolsCtx->Params.pFctValidateUserInput;
			PTBX_VOID									pCtxValidateUserInput = in_pCliToolsCtx->Params.pCtxValidateUserInput;

			TBX_MUTEX_GIV (in_pCliToolsCtx->Mutex);
			in_pCliToolsCtx->CmdInput.fEnteredValueValid = pFctValidateUserInput
			(
				(TBX_CLI_TOOLS_HANDLE)in_pCliToolsCtx,
				pCtxValidateUserInput,
				&in_pCliToolsCtx->CmdInput.Public
			);
			TBX_MUTEX_GET (in_pCliToolsCtx->Mutex);
		}

		if( in_pCliToolsCtx->CmdInput.fEnteredValueValid )
		{
			/* Continue with next prompted value */
			TBX_BOOL	fAllInputsReady = TBX_FALSE;

			if( in_n32Increment == 1 )
			{
				TBX_BOOL	fIncrementCmd	= TBX_FALSE;
				TBX_UINT32	un32Idx;

				for( un32Idx = in_pCliToolsCtx->CmdInput.Public.un32CurrentPromptIdx + 1; un32Idx < in_pCliToolsCtx->Params.un32MaxPromptLines; ++un32Idx )
				{
					if( in_pCliToolsCtx->CmdInput.Public.paszPrompt[ un32Idx ].szLine[0] != '\0' )
					{
						/* Try to find a non-null prompt */
						break;
					}
				}
				
				if( un32Idx == in_pCliToolsCtx->Params.un32MaxPromptLines )
				{
					fIncrementCmd = TBX_FALSE;
				}
				else
				{
					fIncrementCmd = TBX_TRUE;
				}
				
				if( in_nOption == '\n' || in_nOption == '\r' )
				{
					fIncrementCmd = TBX_TRUE;
				}

				if( fIncrementCmd )
				{
					in_pCliToolsCtx->CmdInput.Public.un32CurrentPromptIdx = un32Idx;
					in_pCliToolsCtx->CmdInput.fUserEnteredValue = TBX_FALSE;

					/* Check if all prompts have been filled */
					if( in_pCliToolsCtx->CmdInput.Public.un32CurrentPromptIdx >= in_pCliToolsCtx->Params.un32MaxPromptLines )
					{
						fAllInputsReady = TBX_TRUE;
					}
					else
					{
						if( in_pCliToolsCtx->CmdInput.Public.paszPrompt[ in_pCliToolsCtx->CmdInput.Public.un32CurrentPromptIdx ].szLine[0] == '\0' )
						{
							fAllInputsReady = TBX_TRUE;
						}
					}

					/* Make sure never increment past array size */
					if( in_pCliToolsCtx->CmdInput.Public.un32CurrentPromptIdx >= in_pCliToolsCtx->Params.un32MaxPromptLines )
						in_pCliToolsCtx->CmdInput.Public.un32CurrentPromptIdx = in_pCliToolsCtx->Params.un32MaxPromptLines - 1;
				}
			}
			else if ( in_n32Increment == -1 )
			{
				TBX_UINT32	un32Idx = 0;

				if( in_pCliToolsCtx->CmdInput.Public.un32CurrentPromptIdx > 0 )
				{
					for( un32Idx = in_pCliToolsCtx->CmdInput.Public.un32CurrentPromptIdx - 1; un32Idx > 0; --un32Idx )
					{
						if( in_pCliToolsCtx->CmdInput.Public.paszPrompt[ un32Idx ].szLine[0] != '\0' )
						{
							/* Try to find a non-null prompt */
							break;
						}
					}
				}
			
				in_pCliToolsCtx->CmdInput.Public.un32CurrentPromptIdx = un32Idx;
				in_pCliToolsCtx->CmdInput.fUserEnteredValue = TBX_FALSE;
			}

			if( fAllInputsReady )
			{
				/* User input capture is done. Perform action */
				if( in_pCliToolsCtx->Params.pFctApplyUserInput )
				{
					PFCT_TBX_CLI_TOOLS_APPLY_INPUT_CALLBACK		pFctApplyUserInput = in_pCliToolsCtx->Params.pFctApplyUserInput;
					PTBX_VOID									pCtxApplyUserInput = in_pCliToolsCtx->Params.pCtxApplyUserInput;
					TBX_MUTEX_GIV (in_pCliToolsCtx->Mutex);
					in_pCliToolsCtx->CmdInput.Public.un32CurrentPromptIdx = 0;	/* This index should not be used in callback */
					pFctApplyUserInput
					(
						(TBX_CLI_TOOLS_HANDLE)in_pCliToolsCtx,
						pCtxApplyUserInput,
						&in_pCliToolsCtx->CmdInput.Public
					);
					TBX_MUTEX_GET (in_pCliToolsCtx->Mutex);
					break;
				}
			}
			else
			{
				/* Continue gathering... */
			}
		}
		else
		{
			in_pCliToolsCtx->CmdInput.fUserEnteredValue = TBX_FALSE;
			break;
		}
	} while( in_fTestAll );

	return;
}



/*-------------------------------------------------------------------------------------------------------------------------------
 |
 |  TbxCliToolsCliUserPromptDisplay		:	This function displays the user prompt
 |
 |  in_pCliToolsCtx						:	Context of the comand-line
 |
 |  Note								:	~
 |
 |  Return								:	TBX_RESULT_OK
 |											TBX_RESULT_FAIL
 |
 *------------------------------------------------------------------------------------------------------------------------------*/
TBX_VOID TbxCliToolsCliUserPromptDisplay
(
  IN		PTBX_CLI_TOOLS_CLI_CONTEXT		in_pCliToolsCtx
)
{
	TBX_UINT32	un32NbPromptLinesPrinted = 0;

	TbxCliToolsPrintFillLinePrivate( in_pCliToolsCtx, '-', 0xFFFFFFFF );

	if( in_pCliToolsCtx->CmdInput.Public.CmdType == 0 )
	{
		if( !in_pCliToolsCtx->Params.fHidePrompt )
		{
			TbxCliToolsPrintPrivate( in_pCliToolsCtx, "Enter your command>_\n" );
			un32NbPromptLinesPrinted++;

			while( un32NbPromptLinesPrinted < in_pCliToolsCtx->Params.un32MaxPromptLines )
			{
				TbxCliToolsPrintPrivate( in_pCliToolsCtx, "\n" );
				un32NbPromptLinesPrinted++;
			}
		}
	}
	else
	{
		for( un32NbPromptLinesPrinted = 0; un32NbPromptLinesPrinted < in_pCliToolsCtx->Params.un32MaxPromptLines; un32NbPromptLinesPrinted++ )
		{
			PTBX_CHAR	pszColor;
			TBX_CHAR	szValueToDisplay[ 2 * TBX_CLI_TOOLS_CLI_LINE_WIDTH_MAX ];
			TBX_BOOL	fDisplayCursor = TBX_TRUE;

			/* Build the value to display */
			strcpy( szValueToDisplay, in_pCliToolsCtx->CmdInput.Public.paszUserInput[ un32NbPromptLinesPrinted ].szLine );
			if( szValueToDisplay[0] == TBX_CLI_TOOLS_POPUP_LIST_START_CHAR )
			{
				/* Value begins with TBX_CLI_TOOLS_POPUP_LIST_START_CHAR, thus must be handled as a 'list' of elements */
				PTBX_CHAR	pszListPtr = in_pCliToolsCtx->CmdInput.Public.paszUserInput[ un32NbPromptLinesPrinted ].szLine;
				TBX_CHAR	szCurrentValue[ TBX_CLI_TOOLS_CLI_LINE_WIDTH_MAX ];
				TBX_UINT32	un32ValueToDisplayStrlen = 0;
				TBX_BOOL	fShortDisplay = TBX_FALSE;

				szValueToDisplay[0] = '\0';
				fDisplayCursor		= TBX_FALSE;

				/* Get the current selected value in the list */
				TbxCliToolsGetPopupListChoice
				(
					in_pCliToolsCtx->CmdInput.Public.paszUserInput[ un32NbPromptLinesPrinted ].szLine,
					szCurrentValue
				);

				/* Set right color for blinking */
				if( un32NbPromptLinesPrinted == in_pCliToolsCtx->CmdInput.Public.un32CurrentPromptIdx )
				{
					if( in_pCliToolsCtx->un32BlinkCounter & 0x1 )
					{
						pszColor = ESC F_BLACK AND_COLOR B_YELLOW END_COLOR;
					}
					else
					{
						pszColor = ESC F_YELLOW AND_COLOR B_BLACK END_COLOR;
					}
				}
				else
				{
					pszColor = ESC F_BLACK AND_COLOR B_WHITE END_COLOR;
				}

				/* Scan for options */
				{
					PTBX_CHAR	pszListPtrBkp = pszListPtr;

					if( un32NbPromptLinesPrinted == in_pCliToolsCtx->CmdInput.Public.un32CurrentPromptIdx )
					{
						in_pCliToolsCtx->fImmediateValidation = TBX_TRUE;
					}
					
					while
					(
						*pszListPtr == TBX_CLI_TOOLS_POPUP_LIST_START_CHAR ||
						*pszListPtr == TBX_CLI_TOOLS_POPUP_LIST_SHORT_OPTION_CHAR ||
						*pszListPtr == TBX_CLI_TOOLS_POPUP_LIST_OPTION_NO_IMMEDIATE_VALIDATION
					)
					{
						if( *pszListPtr == TBX_CLI_TOOLS_POPUP_LIST_SHORT_OPTION_CHAR )
						{
							fShortDisplay = TBX_TRUE;
						}
						else if( *pszListPtr == TBX_CLI_TOOLS_POPUP_LIST_OPTION_NO_IMMEDIATE_VALIDATION )
						{
							if( un32NbPromptLinesPrinted == in_pCliToolsCtx->CmdInput.Public.un32CurrentPromptIdx )
							{
								in_pCliToolsCtx->fImmediateValidation = TBX_FALSE;
							}
						}
						pszListPtr++;
					}
					pszListPtr = pszListPtrBkp;
				}

				if( fShortDisplay )
				{
					TBX_UINT32		un32ElementCount = 0;
					TBX_UINT32		un32CurrentElementIndex = 0;

					/* Display only the active element */

					/*	Add arrow to indicate that there are other elements available
						before/after the selected element */
					while( TBX_TRUE )
					{
						TBX_CHAR	szElement[ TBX_CLI_TOOLS_CLI_LINE_WIDTH_MAX ];

						/* Get the next token */
						pszListPtr = TbxCliToolsGetPopupListNextToken
						(
							pszListPtr,
							szElement
						);
						if( szElement[0] == '\0' )
							break;	/* End of listing */

						un32ElementCount++;

						if( strcmp( szElement, szCurrentValue ) == 0 )
						{
							un32CurrentElementIndex = un32ElementCount;
						}
					}

					if( un32CurrentElementIndex != 1 )
					{
						/* Current element is not the first one. Add left arrow */
						un32ValueToDisplayStrlen += TbxCliTools_snprintf
						(
							szValueToDisplay + un32ValueToDisplayStrlen,
							sizeof( szValueToDisplay ) - un32ValueToDisplayStrlen,
							"%s<-" DEFAULT_COLOR " ",
							pszColor
						);
					}
					/* Add element */
					un32ValueToDisplayStrlen += TbxCliTools_snprintf
					(
						szValueToDisplay + un32ValueToDisplayStrlen,
						sizeof( szValueToDisplay ) - un32ValueToDisplayStrlen,
						"%s",
						szCurrentValue
					);
					if( un32CurrentElementIndex != un32ElementCount )
					{
						/* Current element is not the last one. Add right arrow */
						un32ValueToDisplayStrlen += TbxCliTools_snprintf
						(
							szValueToDisplay + un32ValueToDisplayStrlen,
							sizeof( szValueToDisplay ) - un32ValueToDisplayStrlen,
							" %s->" DEFAULT_COLOR,
							pszColor
						);
					}
				}
				else
				{
					/* Display the pop-up menu */
					while( TBX_TRUE )
					{
						TBX_CHAR	szElement[ TBX_CLI_TOOLS_CLI_LINE_WIDTH_MAX ];

						/* Get the next token */
						pszListPtr = TbxCliToolsGetPopupListNextToken
						(
							pszListPtr,
							szElement
						);
						if( szElement[0] == '\0' )
							break;	/* End of listing */

						if( strcmp( szElement, szCurrentValue ) == 0 )
						{
							un32ValueToDisplayStrlen += TbxCliTools_snprintf
							(
								szValueToDisplay + un32ValueToDisplayStrlen,
								sizeof( szValueToDisplay ) - un32ValueToDisplayStrlen,
								"%s%s" DEFAULT_COLOR " ",
								pszColor,
								szElement
							);
						}
						else
						{
							un32ValueToDisplayStrlen += TbxCliTools_snprintf
							(
								szValueToDisplay + un32ValueToDisplayStrlen,
								sizeof( szValueToDisplay ) - un32ValueToDisplayStrlen,
								"%s ",
								szElement
							);
						}
					}
				}
			}

			TbxCliToolsPrintPrivate
			(
				in_pCliToolsCtx,
				in_pCliToolsCtx->CmdInput.Public.paszPrompt[ un32NbPromptLinesPrinted ].szLine
			);
			if( in_pCliToolsCtx->CmdInput.fUserEnteredValue )/* When user entered value, blink cursor after user input */
			{
				TbxCliToolsPrintPrivate
				(
					in_pCliToolsCtx,
					szValueToDisplay
				);
			}

			if( fDisplayCursor && un32NbPromptLinesPrinted == in_pCliToolsCtx->CmdInput.Public.un32CurrentPromptIdx )
			{
				/* Append the blinking cursor */
				if( in_pCliToolsCtx->un32BlinkCounter & 0x1 )
				{
					TbxCliToolsPrintPrivate( in_pCliToolsCtx, "_" );
				}
				else
				{
					TbxCliToolsPrintPrivate( in_pCliToolsCtx, " " );
				}
			}

			if( !in_pCliToolsCtx->CmdInput.fUserEnteredValue )	/* When user did not enter value yet, blink cursor before default user input */
			{
				TbxCliToolsPrintPrivate
				(
					in_pCliToolsCtx,
					szValueToDisplay
				);
			}

			if( un32NbPromptLinesPrinted == in_pCliToolsCtx->CmdInput.Public.un32CurrentPromptIdx )
			{
				if( !in_pCliToolsCtx->CmdInput.fEnteredValueValid )
				{
					TBX_UINT32	un32ElapsedMs;

					/* Display indication that entered value is invalid */
					un32ElapsedMs = in_pCliToolsCtx->un32CurrentTimestamp - in_pCliToolsCtx->CmdInput.un32EnteredValueTimestamp;
					un32ElapsedMs *= TBX_MSEC_PER_TICKS;

					if( un32ElapsedMs > TBX_CLI_TOOLS_CLI_INVALID_VALUE_SHAKE_DELAY_MS )
					{
						in_pCliToolsCtx->CmdInput.fEnteredValueValid = TBX_TRUE;
					}
					else
					{
						/* Make the line "shake" to show user "no-no", invalid value */
						if( in_pCliToolsCtx->un32BlinkCounter & 0x1 )
						{
							TbxCliToolsPrintPrivate( in_pCliToolsCtx, FRED " *** Invalid ***" DEFAULT_COLOR );
						}
					}
				}
			}
			TbxCliToolsPrintPrivate( in_pCliToolsCtx, "\n" );
		}
	}
}


/*-------------------------------------------------------------------------------------------------------------------------------
 |
 |  TbxCliToolsGetLogDisplay		:	This function returns the log content.
 |
 |  in_pCliToolsCtx					:	Context of this library
 |	out_pun32DisplayBufLineToBlink	:	Returns the corresponding line to blink in in_pCliToolsCtx->pDisplayBuf
 |										(or (TBX_UINT32)-1 if line to blink is not actually displayed)
 |
 |  Note							:	~
 |
 |  Return							:	TBX_RESULT_OK
 |										TBX_RESULT_FAIL
 |
 *------------------------------------------------------------------------------------------------------------------------------*/
TBX_RESULT TbxCliToolsGetLogDisplay
(
	IN		PTBX_CLI_TOOLS_CLI_CONTEXT			in_pCliToolsCtx,
	OUT		PTBX_UINT32							out_pun32DisplayBufLineToBlink
)
{
	TBX_UINT32	un32NbLogLines;
	TBX_UINT32	un32FirstLineToDisplay;
	TBX_UINT32	un32CurrentCircularLine;
	TBX_UINT32	un32CursorLinePosition;

	/*---------------------------------------------------------------------------------------------------------------------------
	 |  Code section
	 *--------------------------------------------------------------------------------------------------------------------------*/
	CODE
	{
		if( in_pCliToolsCtx->Params.fDisplayLog )
		{
			TBX_BOOL	fActuallyScrolledToBottom	= TBX_FALSE;
			PTBX_CHAR	pszScrollBar				= "";

			/* Display separator */
			TbxCliToolsPrintFillLinePrivate( in_pCliToolsCtx, '=', 0xFFFFFFFF );
			
			if( out_pun32DisplayBufLineToBlink )	*out_pun32DisplayBufLineToBlink = (TBX_UINT32)-1;

			/* Compute the number of lines to display */
			if( in_pCliToolsCtx->pDisplayBuf->un32NbLines < (in_pCliToolsCtx->pDisplayBuf->un32CurrentHeight - 2) )
			{
				un32NbLogLines = in_pCliToolsCtx->pDisplayBuf->un32CurrentHeight - 2 - in_pCliToolsCtx->pDisplayBuf->un32NbLines;

				if( un32NbLogLines > (in_pCliToolsCtx->Params.un32MaxPromptLines+1) )
				{
					un32NbLogLines -= (in_pCliToolsCtx->Params.un32MaxPromptLines+1);
				}
				else
				{
					/* No room to display any log lines! */
					TBX_EXIT_SUCCESS (TBX_RESULT_OK);
				}

				in_pCliToolsCtx->un32LastLogDisplayNbLines = un32NbLogLines;
			}
			else
			{
				/* No room to display any log lines! */
				TBX_EXIT_SUCCESS (TBX_RESULT_OK);
			}

			/* Compute the new max scroll allowed */
			if( in_pCliToolsCtx->un32CircularLogCurrentNbLines > un32NbLogLines )
				in_pCliToolsCtx->un32ScrollPositionMax = in_pCliToolsCtx->un32CircularLogCurrentNbLines - un32NbLogLines;
			else
				in_pCliToolsCtx->un32ScrollPositionMax = 0;

			/* Adjust the scroll position according to max */
			if( in_pCliToolsCtx->un32ScrollPosition > in_pCliToolsCtx->un32ScrollPositionMax)
				in_pCliToolsCtx->un32ScrollPosition = in_pCliToolsCtx->un32ScrollPositionMax;

			/* Compute the first line to display */
			un32FirstLineToDisplay =
				in_pCliToolsCtx->un32CircularLogNextLineIndex +
				in_pCliToolsCtx->Params.un32LogBufferMaxLines -
				in_pCliToolsCtx->un32ScrollPosition -
				un32NbLogLines;
			un32FirstLineToDisplay %= in_pCliToolsCtx->Params.un32LogBufferMaxLines;


			/* Test if masked lines cause the scroll position to be forced 'up' */
			{
				TBX_UINT32	un32NbLogLinesTmp = un32NbLogLines;

				un32CurrentCircularLine = un32FirstLineToDisplay;
				while( un32NbLogLinesTmp )
				{
					#ifdef TBX_CLI_TOOLS_USE_LIVE_TRACE_LEVEL_MASKING
						/* This code has been deactivated, was preventing to browse a log
						   generated with very verbose trace level with actual verbose level low,
						   which is useful to capture only a short period with verbose trace level to
						   avoid being flooded with traces */
					/* Test if current line is masked or not */
					if
					(
						(TBX_UINT32)in_pCliToolsCtx->paCircularLog[ un32CurrentCircularLine ].TraceLevel >=
						(TBX_UINT32)in_pCliToolsCtx->Params.TraceLevelStdout
					)
					{
						/* This line is to be displayed */
						un32NbLogLinesTmp--;
					}
					else
					{
						/* This line is masked */
					}
					#else
						un32NbLogLinesTmp--;
					#endif
					un32CurrentCircularLine = (un32CurrentCircularLine + 1) % in_pCliToolsCtx->Params.un32LogBufferMaxLines;

					if( un32NbLogLinesTmp && un32CurrentCircularLine == in_pCliToolsCtx->un32CircularLogNextLineIndex )
					{
						/* Reached end of log. We must scroll-up temporarily */
						break;
					}
				}
				if
				(
					un32CurrentCircularLine == in_pCliToolsCtx->un32CircularLogNextLineIndex &&
					in_pCliToolsCtx->un32ScrollPosition < in_pCliToolsCtx->un32ScrollPositionMax
				)
				{
					/* Scroll up until we have included enough lines to display */
					while( un32NbLogLinesTmp )
					{
						/* Scroll up */
						in_pCliToolsCtx->un32ScrollPosition++;

						/* Compute (again) the first line to display */
						if( un32FirstLineToDisplay ) un32FirstLineToDisplay--;

						/* Test if current line is masked or not */
						#ifdef TBX_CLI_TOOLS_USE_LIVE_TRACE_LEVEL_MASKING
						if
						(
							(TBX_UINT32)in_pCliToolsCtx->paCircularLog[ un32FirstLineToDisplay ].TraceLevel >=
							(TBX_UINT32)in_pCliToolsCtx->Params.TraceLevelStdout
						)
						{
							/* This line is to be displayed */
							un32NbLogLinesTmp--;
						}
						else
						{
							/* This line is masked */
						}
						#else
							un32NbLogLinesTmp--;
						#endif

						if( un32NbLogLinesTmp && in_pCliToolsCtx->un32ScrollPosition >= in_pCliToolsCtx->un32ScrollPositionMax)
							break;	/* Can't scroll up anymore */
					}
				}

				if( un32NbLogLinesTmp )
				{
					/* Some lines will not be displayed. */
					un32NbLogLines -= un32NbLogLinesTmp;
				}

				/* Compute (again) the first line to display */
				un32FirstLineToDisplay =
					in_pCliToolsCtx->un32CircularLogNextLineIndex +
					in_pCliToolsCtx->Params.un32LogBufferMaxLines -
					in_pCliToolsCtx->un32ScrollPosition -
					un32NbLogLines;
				un32FirstLineToDisplay %= in_pCliToolsCtx->Params.un32LogBufferMaxLines;
			}


			/* Compute the cursor line position */
			if( in_pCliToolsCtx->un32ScrollPosition == 0 )
				un32CursorLinePosition = 2;
			else
				un32CursorLinePosition =
				4 +
				(
					2 * (un32NbLogLines-1) *
					in_pCliToolsCtx->un32ScrollPosition /
					in_pCliToolsCtx->un32ScrollPositionMax
				);


			in_pCliToolsCtx->un32NbLogLinesDisplayed = un32NbLogLines;

			/* Now that we know from where to where, copy these lines in the screen buffer */
			un32CurrentCircularLine = un32FirstLineToDisplay;
			while( un32NbLogLines )
			{
				/* Test if current line is masked or not */
				#ifdef TBX_CLI_TOOLS_USE_LIVE_TRACE_LEVEL_MASKING
				if
				(
					(TBX_UINT32)in_pCliToolsCtx->paCircularLog[ un32CurrentCircularLine ].TraceLevel >=
					(TBX_UINT32)in_pCliToolsCtx->Params.TraceLevelStdout
				)
				#endif
				{
					PTBX_CHAR	pszScrolledString	= NULL;
					TBX_UINT32	un32ScrollCharCount = 0;
					TBX_CHAR	szCurrentColor[ 16 ];
					TBX_UINT32	un32CurrentColorLen = 0;
					szCurrentColor[0] = '\0';
					pszScrollBar = "";

					/* display scroll bar line */
					if( in_pCliToolsCtx->un32ScrollPosition != 0 )
					{
						if( (un32CursorLinePosition-1)/2 == un32NbLogLines )
						{
							/* Display the cursor */
							if( un32CursorLinePosition & 0x1 )
								pszScrollBar = FRED "_" DEFAULT_COLOR "|";
							else
								pszScrollBar = FRED "-" DEFAULT_COLOR "|";
						}
						else
						{
							pszScrollBar = " |";
						}
					}
					else
					{
						fActuallyScrolledToBottom = TBX_TRUE;
						pszScrollBar = " |";
					}

					if( un32CurrentCircularLine == in_pCliToolsCtx->un32SelectedLineToBlink )
					{
						if( out_pun32DisplayBufLineToBlink ) *out_pun32DisplayBufLineToBlink = in_pCliToolsCtx->pDisplayBuf->un32NbLines;
					}

					/* Apply horizontal scrolling */
					pszScrolledString = in_pCliToolsCtx->paCircularLog[ un32CurrentCircularLine ].pszLine;
					while( un32ScrollCharCount < in_pCliToolsCtx->un32ScrollPositionHor )
					{
						if( *pszScrolledString == '\033'  )
						{
							un32CurrentColorLen = 0;

							/* It's a color code. Skip it entirely */
							while( *pszScrolledString && *pszScrolledString != 'm' )
							{
								if( un32CurrentColorLen + 1 < sizeof( szCurrentColor ) )
								{
									szCurrentColor[ un32CurrentColorLen++ ] = *pszScrolledString;
								}
								pszScrolledString++;
							}
							szCurrentColor[ un32CurrentColorLen++ ] = *pszScrolledString;
							szCurrentColor[ un32CurrentColorLen ]	= '\0';
							un32ScrollCharCount--;	/* Ignore this 'color' indication in the scrolling count */
						}

						if( *pszScrolledString ) pszScrolledString++;
						un32ScrollCharCount++;

						if( *pszScrolledString == '\0' )
							break;
					}

					TbxCliToolsPrintKeepColor
					(
						in_pCliToolsCtx,
						"%s%s%s\n",
						pszScrollBar,
						szCurrentColor,
						pszScrolledString
					);
					un32NbLogLines--;
				}
				un32CurrentCircularLine = (un32CurrentCircularLine + 1) % in_pCliToolsCtx->Params.un32LogBufferMaxLines;

				if( un32NbLogLines && un32CurrentCircularLine == in_pCliToolsCtx->un32CircularLogNextLineIndex )
				{
					/* Reached end of log. We must scroll-up temporarily */
					break;
				}
			}

#ifndef MQX
			/* Display current time */
			{
				TBX_CLI_TOOLS_TIMESTAMP		Timestamp;
				TBX_CHAR					szTime[ 64 ];
				TbxCliToolsGetTimestamp( &Timestamp );
				TbxCliToolsFormatTimePrivate( &Timestamp, szTime );

				if( fActuallyScrolledToBottom )
				{
					pszScrollBar = FGREEN "-" DEFAULT_COLOR "|";
				}
				else
				{
					pszScrollBar = " |";
				}

				TbxCliToolsPrintKeepColor
				(
					in_pCliToolsCtx,
					"%s" FWHITE "%s" DEFAULT_COLOR "\n",
					pszScrollBar,
					szTime
				);
				
			}
#endif
		}
		else
		{
			//TbxCliToolsPrintPrivate( in_pCliToolsCtx, "(hidden)\n" );
		}

		/* End of the code (skip to cleanup) */
		TBX_EXIT_SUCCESS (TBX_RESULT_OK);
	}

	/*---------------------------------------------------------------------------------------------------------------------------
	 |  Error handling section
	 *--------------------------------------------------------------------------------------------------------------------------*/
	ERROR_HANDLING
	{
		/* Print error message */
		TbxCliToolsLogPrint
		(
			(TBX_CLI_TOOLS_HANDLE)in_pCliToolsCtx,
			TRACE_LEVEL_ERROR, NULL,
			"%s (Result=0x%08X) %s:%d\n",
			TBX_ERROR_DESCRIPTION,
			TBX_ERROR_RESULT,
			__FILE__,
			TBX_ERROR_LINE
		);
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
 |  TbxCliToolsDisplayBufferCleanup	:	This function cleans-up the display buffer, make it ready to display
 |
 |  in_pCliToolsCtx					:	Context of this library
 |
 |  Note							:	~
 |
 |  Return							:	TBX_RESULT_OK
 |										TBX_RESULT_FAIL
 |
 *------------------------------------------------------------------------------------------------------------------------------*/
TBX_VOID TbxCliToolsDisplayBufferCleanup
(
	IN		PTBX_CLI_TOOLS_CLI_CONTEXT			in_pCliToolsCtx
)
{
	TBX_UINT32						un32Line;
	TBX_UINT32						un32Char;
	TBX_BOOL						fLineEnded;
	TBX_UINT32						un32CurrentWidth;
	PTBX_CLI_TOOLS_DISPLAY_BUFFER	pTempDisplayBuf = in_pCliToolsCtx->pDisplayBuf;

	un32CurrentWidth = pTempDisplayBuf->un32CurrentWidth;
	if( un32CurrentWidth > (in_pCliToolsCtx->Params.un32MaxScreenWidth - g_un32TrunkatedEndlineStrlen - 1) )
	{
		un32CurrentWidth = in_pCliToolsCtx->Params.un32MaxScreenWidth - g_un32TrunkatedEndlineStrlen - 1;
	}


	for( un32Line = 0; un32Line < pTempDisplayBuf->un32CurrentHeight; un32Line++ )
	{
		TBX_UINT32	un32CurrentWidthTmp = un32CurrentWidth;
		PTBX_CHAR	pszTempLine			= pTempDisplayBuf->aLines[ un32Line ].pszLine;

		fLineEnded = (un32Line >= pTempDisplayBuf->un32NbLines);

		memset
		(
			in_pCliToolsCtx->pafEscapeChars,
			0,
			in_pCliToolsCtx->Params.un32MaxScreenWidth * sizeof(*in_pCliToolsCtx->pafEscapeChars)
		);

		un32Char = 0;
		while( TBX_TRUE )
		{
			if( un32Char > un32CurrentWidthTmp )
			{
				pszTempLine[ un32Char ] = '\0';
				fLineEnded = TBX_TRUE;
			}

			if( fLineEnded )
			{
			}
			else if( pszTempLine[ un32Char ] == '\033'  )
			{
				in_pCliToolsCtx->pafEscapeChars[ un32Char ] = TBX_TRUE;

				/* This is a display color. Don't get "fooled" by it */
				while
				(
					( pszTempLine[ un32Char ] != '\0' ) &&
					( pszTempLine[ un32Char ]!= 'm' )
				)
				{
					un32Char++;

					/* Increment the line width to include this color code, but never go pass max length! */
					if( (un32CurrentWidthTmp + 1) < in_pCliToolsCtx->Params.un32MaxScreenWidth )
					{
						un32CurrentWidthTmp++;
					}
					else
					{
						if( un32Char > un32CurrentWidthTmp )
						{
							pszTempLine[ un32Char ] = '\0';
							fLineEnded = TBX_TRUE;
							break;
						}
					}

					in_pCliToolsCtx->pafEscapeChars[ un32Char ] = TBX_TRUE;
				}
				if( ( pszTempLine[ un32Char ] == 'm' ) )
				{
					/* Increment the line width to include this color code, but never go pass max length! */
					if( (un32CurrentWidthTmp + 1) < in_pCliToolsCtx->Params.un32MaxScreenWidth )
						un32CurrentWidthTmp++;
				}
			}
			else
			{
				/* Validate */
				switch( pszTempLine[ un32Char ] )
				{
					case '\0':
					{
						fLineEnded =  TBX_TRUE;
					} break;

					case '\n':
					{
						/* Remove extra enlines */
						pszTempLine[ un32Char ] = ' ';
					} break;

					case '\r':
					{
						/* Remove extra enlines */
						pszTempLine[ un32Char ] = ' ';
					} break;

					case '\t':
					{
						/* Replace tabs with spaces */
						pszTempLine[ un32Char ] = ' ';
					} break;
				}
			}

			if( fLineEnded )
			{
				if( un32CurrentWidthTmp >= un32Char )
				{
					PTBX_CHAR	pszEndlineString;

					/* Remove un-terminated escape characters at the end of line (if any) */
					while( un32Char )
					{
						if( in_pCliToolsCtx->pafEscapeChars[ un32Char ] )
							un32Char--;
						else
							break;
					}
#ifdef TBX_CLI_TOOLS_USED_CLEARLINE
					/* No need to padd when we use clearline as part of g_pszEndline */
#else
					if( un32CurrentWidthTmp > un32Char )
					{
						/* Fill end of line with spaces */
						memset
						(
							&( pszTempLine[ un32Char ] ),
							' ',
							un32CurrentWidthTmp - un32Char
						);
						un32Char = un32CurrentWidthTmp;
					}
#endif
					/* Make sure there is enough room in the buffer to store g_pszEndline */
					while( un32Char && (in_pCliToolsCtx->Params.un32MaxScreenWidth - un32Char <= strlen(g_pszEndline)) )
						un32Char--;

					/* Remove un-terminated escape characters at the end of line (if any) */
					while( un32Char )
					{
						if( in_pCliToolsCtx->pafEscapeChars[ un32Char ] )
							un32Char--;
						else
							break;
					}

					pszEndlineString = &( pszTempLine[ un32Char ] );
					strcpy( pszEndlineString, g_pszEndline );
				}
				else
				{
					PTBX_CHAR	pszWarningString;
					TBX_UINT32	un32ToReduce;
					TBX_UINT32	un32MaxCharPos;

					/* Line overflow. Print the overflowing warning! */

					/* Remove the last characters from the line, to override with the warning string */
					for( un32ToReduce = 0; un32ToReduce < g_un32TrunkatedEndlineLen + 1; un32ToReduce++ )
					{
						un32Char--;

						while( un32Char )
						{
							if( in_pCliToolsCtx->pafEscapeChars[ un32Char ] )
								un32Char--;
							else
								break;
						}
					}

					/* Make sure that the warning string will not overflow the buffer */
					un32MaxCharPos = in_pCliToolsCtx->Params.un32MaxScreenWidth - g_un32TrunkatedEndlineStrlen;
					while( un32Char >= un32MaxCharPos )
					{
						un32Char--;

						while( un32Char )
						{
							if( in_pCliToolsCtx->pafEscapeChars[ un32Char ] )
								un32Char--;
							else
								break;
						}
					}

					pszWarningString = &( pszTempLine[ un32Char ] );
					strcpy( pszWarningString, g_pszTrunkatedEndline );
				}
				break;
			}

			un32Char++;
		}
	}

	pTempDisplayBuf->un32NbLines = pTempDisplayBuf->un32CurrentHeight;
}





static __inline TBX_VOID TbxCliToolsLogFileNameBuild
(
	IN		PTBX_CLI_TOOLS_CLI_CONTEXT				in_pCliToolsCtx,
	IN		TBX_CHAR								in_szDate[ 64 ],
	IN		TBX_UINT32								in_un32SegmentNb,
	OUT		TBX_CHAR								out_szLogFilePath[ 512 ]
)
{
	if( in_un32SegmentNb == 0 )
	{
		sprintf
		(
			out_szLogFilePath,
			"%s_%s.log",
			in_pCliToolsCtx->Params.szLogFileName,
			in_szDate
		);
	}
	else
	{
		sprintf
		(
			out_szLogFilePath,
			"%s_%s_seg%03u.log",
			in_pCliToolsCtx->Params.szLogFileName,
			in_szDate,
			(unsigned int)in_un32SegmentNb
		);
	}
}

/*-------------------------------------------------------------------------------------------------------------------------------
 |
 |  TbxCliToolsLogFilesRotation	:	Rotates log file name every day.
 |
 |	pCliToolsCtx				:	Context of the allocated TBX CLI library
 |
 |  Note						:	~
 |
 |  Return						:	TBX_RESULT_OK
 |									TBX_RESULT_FAIL
 |									or others
 |
 *------------------------------------------------------------------------------------------------------------------------------*/
TBX_RESULT TbxCliToolsLogFilesRotation
(
	IN		PTBX_CLI_TOOLS_CLI_CONTEXT				pCliToolsCtx
)
{
	TBX_RESULT							Result			= TBX_RESULT_OK;
	#ifdef TBX_CLI_TOOLS_USE_LOG_FILE
	TBX_CHAR							szDate[ 64 ];
	TBX_CHAR							szLogFilePath[ 512 ];

	/* No logfile name supplied so don't log */
	if( *pCliToolsCtx->Params.szLogFileName == '\0' )
	{
		/* Prevent from trying to write to file */
		pCliToolsCtx->szCurrentLogFilePath[0] = '\0';
		pCliToolsCtx->pLogFile = NULL;

		return TBX_RESULT_OK;
	}

	/* Flush log file, in case there was some non-flushed data */
	if( pCliToolsCtx->pLogFile )
	{
		fflush( pCliToolsCtx->pLogFile );
	}

	if( pCliToolsCtx->Params.fDontRotateLogFile )
	{
		TBX_BOOL	fEndsWithLogExtension;

		strcpy( szLogFilePath, pCliToolsCtx->Params.szLogFileName );

		fEndsWithLogExtension =
		(
			!strcmp( szLogFilePath + strlen( szLogFilePath) - strlen( ".log" ), ".log" )
		);

		if( !fEndsWithLogExtension )
		{
			Strncat
			(
				szLogFilePath,
				".log",
				sizeof( szLogFilePath )
			);
		}
	}
	else
	{
		/* Build current date */
		#if defined MQX
		{
			TIME_STRUCT		Timestruct;
			DATE_STRUCT		DateStruct;
			
			_time_get( &Timestruct );
			_time_to_date( &Timestruct, &DateStruct );
			
			snprintf
			( 
				szDate,
				sizeof(szDate) - 1,
				"%u-%02u-%02u",
				(unsigned long)DateStruct.YEAR,
				(unsigned long)DateStruct.MONTH,
				(unsigned long)DateStruct.DAY
			);
			szDate[ sizeof( szDate ) - 1 ] = '\0';
		}
		#else
		{
			struct tm	DateTimeStruct;
			time_t		Time		= time( NULL );
			
			TBX_LOCALTIME( &Time, &DateTimeStruct );
			
			strftime( szDate, sizeof(szDate) - 1, "%Y-%m-%d", &DateTimeStruct );
			szDate[ sizeof( szDate ) - 1 ] = '\0';
		}
		#endif

		if( strcmp( szDate, pCliToolsCtx->szCurrentLogFileTime ) != 0 )
		{
			/* Date changed. Reset the file segment number. */
			pCliToolsCtx->un32CurrentLogFileSegment = 0;
			strcpy( pCliToolsCtx->szCurrentLogFileTime, szDate );
		}

		/* Build current log file path */
		TbxCliToolsLogFileNameBuild
		(
			pCliToolsCtx,
			szDate,
			pCliToolsCtx->un32CurrentLogFileSegment,
			szLogFilePath
		);
	}

	/* Test if time to open a different log file */
	if( strcmp( szLogFilePath, pCliToolsCtx->szCurrentLogFilePath ) != 0 )
	{
		TBX_BOOL	fPrintInitialTrace	= TBX_FALSE;
		FILE*		pNewFile			= NULL;

		if( !pCliToolsCtx->pLogFile )
		{
			/* First time this log is opened. Must find the highest available segment */
			TBX_UINT32	un32Segment = pCliToolsCtx->un32CurrentLogFileSegment;
			FILE*		pTestFile	= NULL;
			TBX_CHAR	szTestLogFilePath[ 512 ];

			do
			{
				if( pTestFile )
				{
					fclose( pTestFile );
					pTestFile = NULL;
				}
				un32Segment++;
				TbxCliToolsLogFileNameBuild
				(
					pCliToolsCtx,
					szDate,
					un32Segment,
					szTestLogFilePath
				);
				pTestFile = fopen( szTestLogFilePath, "rb" );
				if( pTestFile )
				{
					/* This segment exist. Increment current segment to this one. */
					pCliToolsCtx->un32CurrentLogFileSegment = un32Segment;
					strcpy( szLogFilePath, szTestLogFilePath );
				}
			} while( pTestFile );
		}
		else
		{
			/* Force flush latest traces for closed log file */
			TbxCliToolsFlushDiskWriteBuffer( pCliToolsCtx, TBX_TRUE );
		}

		/* don't append logs when in test mode */
		if( pCliToolsCtx->Params.fDontRotateLogFile )
		{
			pNewFile = fopen( szLogFilePath, "w" );
		}
		else
		{
			pNewFile = fopen( szLogFilePath, "a" );
		}
		if( pNewFile == NULL )
		{
			TbxCliToolsLogPrint
			(
				(TBX_CLI_TOOLS_HANDLE)pCliToolsCtx, TRACE_LEVEL_ERROR, FRED,
				"Failed to open log file '%s'!\n",
				szLogFilePath
			);
			Result = TBX_RESULT_ACCESS_DENIED;
		}
		else
		{
			strcpy( pCliToolsCtx->szCurrentLogFilePath, szLogFilePath );
			if( pCliToolsCtx->pLogFile )
			{
				fclose( pCliToolsCtx->pLogFile );
				pCliToolsCtx->pLogFile = NULL;
			}
			else
			{
				fPrintInitialTrace = TBX_TRUE;
			}
			pCliToolsCtx->pLogFile					= pNewFile;

			/* Retrieve actual file size */
			fseek( pCliToolsCtx->pLogFile, 0, SEEK_END );
			pCliToolsCtx->un32CurrentLogFileSize	= ftell( pCliToolsCtx->pLogFile );
		}

		/* Print initial trace */
		if( fPrintInitialTrace && pCliToolsCtx->pLogFile )
		{
		struct tm	DateTimeStruct;

			time_t		Time = time( NULL );
			TBX_CHAR	szDate[ 256 ];

			/* Build current date */
			TBX_LOCALTIME( &Time, &DateTimeStruct );
			strftime( szDate, sizeof(szDate) - 1, "%m/%d/%Y %H:%M:%S", &DateTimeStruct );
			szDate[ sizeof( szDate ) - 1 ] = '\0';

			TbxCliToolsLogPrint( (TBX_CLI_TOOLS_HANDLE)pCliToolsCtx, TRACE_LEVEL_ALWAYS, DEFAULT_COLOR, "******************************************************************************\n");
			TbxCliToolsLogPrint( (TBX_CLI_TOOLS_HANDLE)pCliToolsCtx, TRACE_LEVEL_ALWAYS, DEFAULT_COLOR, "* Application starting: %s\n", szDate);
			TbxCliToolsLogPrint( (TBX_CLI_TOOLS_HANDLE)pCliToolsCtx, TRACE_LEVEL_ALWAYS, DEFAULT_COLOR, "* Build number 0x%08X, Build tag: %s\n", TBX_BUILD_NUMBER, TBX_BUILD_TAG );
			TbxCliToolsLogPrint( (TBX_CLI_TOOLS_HANDLE)pCliToolsCtx, TRACE_LEVEL_ALWAYS, DEFAULT_COLOR, "******************************************************************************\n");
		}

		/* If there is a limit on the number of log files, we must delete all 'older' log files */
		if( pCliToolsCtx->Params.un32MaxLogFilesPerDay != 0 )
		{
			if( pCliToolsCtx->un32CurrentLogFileSegment > pCliToolsCtx->Params.un32MaxLogFilesPerDay )
			{
				TBX_UINT32	un32SegmentToDelete;
				for
				(
					un32SegmentToDelete = 1;	/* Never delete segment 0 */
					un32SegmentToDelete < (pCliToolsCtx->un32CurrentLogFileSegment - pCliToolsCtx->Params.un32MaxLogFilesPerDay);
					un32SegmentToDelete++
				)
				{
					FILE*		pFileToDelete	= NULL;
					TBX_CHAR	szLogFileToDeletePath[ 512 ];

					TbxCliToolsLogFileNameBuild
					(
						pCliToolsCtx,
						szDate,
						un32SegmentToDelete,
						szLogFileToDeletePath
					);
					/* Open this file for writing, truncate, to flush it's content. */
					pFileToDelete = fopen( szLogFileToDeletePath, "w" );
					if( pFileToDelete )
					{
						/* Write a comment in this log file to mention it has been deleted. */
						fprintf
						(
							pFileToDelete,
							"[This log file has been deleted. This application has been configured to have maximum %u log files of %u bytes per day]\n",
							(unsigned int)pCliToolsCtx->Params.un32MaxLogFilesPerDay,
							(unsigned int)pCliToolsCtx->Params.un32MaxLogFileSize
						);
						fclose( pFileToDelete );
					}
				}
			}
		}

		#ifndef WIN32	// On win32, deamon mode is irrelevant
		if( pCliToolsCtx->Params.fDisableTerminalInput )
		{
			/* We're running in "background" mode, and probably in "deamon" mode. In that mode,
			   we don't keep the log file opened all the time, we re-open it for every write to it,
			   because of the log-rotate tools that may remove our log file for backup.
			   Performance hit of re-opening for each write will not be too big, since we buffer
			   all writes for 1 second, or else up to 256KB. */
			if( pCliToolsCtx->pLogFile )
			{
				fclose( pCliToolsCtx->pLogFile );
				pCliToolsCtx->pLogFile = NULL;
			}
		}
		#endif
	}
	#endif
	return Result;
}


/*-------------------------------------------------------------------------------------------------------------------------------
 |
 |  TbxCliToolsFlushDiskWriteBuffer	:	Flush the temporary disk write buffer to disk.
 |
 |	pCliToolsCtx					:	Context of the allocated TBX CLI library
 |
 |  Note							:	~
 |
 |  Return							:	TBX_RESULT_OK
 |										TBX_RESULT_FAIL
 |										or others
 |
 *------------------------------------------------------------------------------------------------------------------------------*/
TBX_VOID TbxCliToolsFlushDiskWriteBuffer
(
	IN		PTBX_CLI_TOOLS_CLI_CONTEXT				pCliToolsCtx,
	IN		TBX_BOOL								in_fForced
)
{
	#ifdef TBX_CLI_TOOLS_USE_LOG_FILE
	TBX_UINT32	un32RemainingBytes;
	TBX_UINT32	un32ElapsedMs;

	if( !pCliToolsCtx->pszDiskWriteBuffer )
	{
		/* Log file not used */
		return;
	}

	un32RemainingBytes = TBX_CLI_TOOLS_DISK_BUFFER_SIZE - pCliToolsCtx->un32DiskWriteBufferCurLen;

	un32ElapsedMs = pCliToolsCtx->un32CurrentTimestamp - pCliToolsCtx->un32DiskWriteBufferTimestamp;
	un32ElapsedMs *= TBX_MSEC_PER_TICKS;

	if
	(
		in_fForced ||
		(un32RemainingBytes < TBX_CLI_TOOLS_TMP_LINE_MAX_SIZE)	||
		(un32ElapsedMs >= TBX_CLI_TOOLS_DISK_BUFFER_FLUSH_DELAY_MS)
	)
	{
		if( pCliToolsCtx->un32DiskWriteBufferCurLen )
		{
			TBX_BOOL fCloseAfterWrite = TBX_FALSE;

			if( pCliToolsCtx->pLogFile == NULL && pCliToolsCtx->szCurrentLogFilePath[0] )
			{
				/* Log file is not opened. We're probably running in mode where we
				   want to re-open upon each write. */
				fCloseAfterWrite = TBX_TRUE;

				pCliToolsCtx->pLogFile = fopen( pCliToolsCtx->szCurrentLogFilePath, "a" );
			}

			if( pCliToolsCtx->pLogFile )
			{
				/* Write to disk */
				pCliToolsCtx->un32CurrentLogFileSize += fwrite
				(
					pCliToolsCtx->pszDiskWriteBuffer,
					1,
					pCliToolsCtx->un32DiskWriteBufferCurLen,
					pCliToolsCtx->pLogFile
				);
				if( in_fForced )
				{
					fflush( pCliToolsCtx->pLogFile );
				}
			}

			if( pCliToolsCtx->pLogFile && fCloseAfterWrite )
			{
				fclose( pCliToolsCtx->pLogFile );
				pCliToolsCtx->pLogFile = NULL;
			}
		}
		pCliToolsCtx->un32DiskWriteBufferCurLen		= 0;
		pCliToolsCtx->un32DiskWriteBufferTimestamp	= pCliToolsCtx->un32CurrentTimestamp;
	}
	#endif
	return;
}

static TBX_VOID StringInsertChar
(
  IN		PTBX_CHAR			in_pszString,
  IN		TBX_CHAR			in_cChar,
  IN		TBX_UINT32			in_un32MaxLen
)
{
	PTBX_CHAR	pszString		= in_pszString;
	TBX_CHAR	cBackupChar;
	TBX_CHAR	cNextBackupChar;

	/* Insert TBX_CLI_TOOLS_POPUP_LIST_CURR_ELEM_START_CHAR before first element */
	cBackupChar = *pszString;
	*pszString	= in_cChar;
	while( TBX_TRUE )
	{
		pszString++;
		cNextBackupChar	= *pszString;
		*pszString		= cBackupChar;
		cBackupChar		= cNextBackupChar;

		if( (TBX_UINT32)(pszString - in_pszString) >= in_un32MaxLen )
		{
			/* Reached end of buffer */
			break;
		}
		if( *pszString == '\0' )
		{
			/* Reached end of string */
			break;
		}
	}
	*pszString = '\0';

	return;
}

static TBX_VOID StringRemoveChar
(
  IN		PTBX_CHAR			in_pszString
)
{
	PTBX_CHAR	pszString		= in_pszString;

	*pszString = *(pszString+1);
	while( *pszString )
	{
		pszString++;
		*pszString = *(pszString+1);
	}

	return;
}

/*-------------------------------------------------------------------------------------------------------------------------------
 |
 |  TbxCliToolsGetPopupListChoice		:	Function that finds the selected element from the
 |											popup menu list string.
 |
 |  in_szPopupList						:	List to retrieve the selected element from.
 |	out_pszSelected						:	Selected element within in_szPopupList
 |
 |  Note								:	The list is a string of the form
 |											{ elem1,elem2,[elem3],elem4 }
 |											The selected element is within brackets []
 |
 |  Return								:	~
 |
 *------------------------------------------------------------------------------------------------------------------------------*/
static TBX_VOID TbxCliToolsGetPopupListChoice
(
  IN		TBX_CHAR			in_szPopupList[ TBX_CLI_TOOLS_CLI_LINE_WIDTH_MAX ],
  OUT		TBX_CHAR			out_pszSelected[ TBX_CLI_TOOLS_CLI_LINE_WIDTH_MAX ]
)
{
	PTBX_CHAR	pszFound	= in_szPopupList;
	PTBX_CHAR	pszOut		= out_pszSelected;

	*pszOut = '\0';

	while( *pszFound && *pszFound != TBX_CLI_TOOLS_POPUP_LIST_CURR_ELEM_START_CHAR )
	{
		pszFound++;
	}
	if( *pszFound == TBX_CLI_TOOLS_POPUP_LIST_CURR_ELEM_START_CHAR )
	{
		pszFound++;

		while( *pszFound && *pszFound != TBX_CLI_TOOLS_POPUP_LIST_SEPARATOR && *pszFound != TBX_CLI_TOOLS_POPUP_LIST_END_CHAR )
		{
			*pszOut = *pszFound;
			pszFound++;
			pszOut++;
		}
		*pszOut = '\0';
	}
	else
	{
		/* There seems to be no selected element. Force select the first one */
		pszFound = in_szPopupList;

		/* Find the first element */
		while( *pszFound && *pszFound != TBX_CLI_TOOLS_POPUP_LIST_START_CHAR )
		{
			pszFound++;
		}
		if( *pszFound )
			pszFound++;	/* Skip the TBX_CLI_TOOLS_POPUP_LIST_START_CHAR */

		/* Skip the options */
		while
		(
			*pszFound == TBX_CLI_TOOLS_POPUP_LIST_SHORT_OPTION_CHAR ||
			*pszFound == TBX_CLI_TOOLS_POPUP_LIST_OPTION_NO_IMMEDIATE_VALIDATION
		)
		{
			pszFound++;
		}
		if( *pszFound )
		{
			StringInsertChar
			(
				pszFound,
				TBX_CLI_TOOLS_POPUP_LIST_CURR_ELEM_START_CHAR,
				TBX_CLI_TOOLS_CLI_LINE_WIDTH_MAX - (pszFound - in_szPopupList)
			);
		}
	}

	return;
}

/*-------------------------------------------------------------------------------------------------------------------------------
 |
 |  TbxCliToolsGetPopupListNextToken	:	Function that returns the next token from the popup list.
 |
 |  in_szPopupListPartial				:	Pointer to remainder of list to get next token.
 |	out_pszElement						:	Next token within in_szPopupList,
 |											or empty string if no more
 |
 |  Note								:	~
 |
 |  Return								:	Pointer to list after the returned token
 |
 *------------------------------------------------------------------------------------------------------------------------------*/
static PTBX_CHAR TbxCliToolsGetPopupListNextToken
(
  IN		PTBX_CHAR			in_szPopupListPartial,
  OUT		TBX_CHAR			out_pszElement[ TBX_CLI_TOOLS_CLI_LINE_WIDTH_MAX ]
)
{
	PTBX_CHAR	pszFound	= in_szPopupListPartial;
	PTBX_CHAR	pszOut		= out_pszElement;

	*pszOut = '\0';

	/* Find the next separator */
	while
	(
		*pszFound &&
		*pszFound != TBX_CLI_TOOLS_POPUP_LIST_START_CHAR &&
		*pszFound != TBX_CLI_TOOLS_POPUP_LIST_SEPARATOR &&
		*pszFound != TBX_CLI_TOOLS_POPUP_LIST_CURR_ELEM_START_CHAR
	)
	{
		pszFound++;
	}
	if( *pszFound )
		pszFound++;	/* Skip the separator */

	/* Skip options if any */
	while
	(
		*pszFound == TBX_CLI_TOOLS_POPUP_LIST_SHORT_OPTION_CHAR ||
		*pszFound == TBX_CLI_TOOLS_POPUP_LIST_OPTION_NO_IMMEDIATE_VALIDATION
	)
	{
		pszFound++;
	}

	if( *pszFound )
	{
		/* Skip whitespaces */
		while( *pszFound && isspace( (unsigned char)*pszFound) )
		{
			pszFound++;
		}

		/* Copy the element */
		while( *pszFound && *pszFound != TBX_CLI_TOOLS_POPUP_LIST_SEPARATOR && *pszFound != TBX_CLI_TOOLS_POPUP_LIST_END_CHAR )
		{
			/* Ignore TBX_CLI_TOOLS_POPUP_LIST_CURR_ELEM_START_CHAR */
			if( *pszFound != TBX_CLI_TOOLS_POPUP_LIST_CURR_ELEM_START_CHAR )
			{
				*pszOut = *pszFound;
				pszOut++;
			}
			pszFound++;
		}
		*pszOut = '\0';
	}

	/* Remove ending white spaces */
	if( *out_pszElement )
	{
		pszOut--;
		while( *pszOut && isspace( (unsigned char)*pszOut ) )
		{
			*pszOut = '\0';
			pszOut--;
		}
	}

	/* Return the pointer after the found ending separator */
	return pszFound;
}

/*-------------------------------------------------------------------------------------------------------------------------------
 |
 |  TbxCliToolsPopupListNext			:	Function that changes the actually selected popup
 |											element to the next in list.
 |
 |  in_szPopupList						:	List to move TBX_CLI_TOOLS_POPUP_LIST_CURR_ELEM_START_CHAR
 |											to the next element
 |
 |  Note								:	~
 |
 |  Return								:	Pointer to list after the returned token
 |
 *------------------------------------------------------------------------------------------------------------------------------*/
static TBX_VOID TbxCliToolsPopupListNext
(
  IN		TBX_CHAR			in_szPopupList[ TBX_CLI_TOOLS_CLI_LINE_WIDTH_MAX ]
)
{
	PTBX_CHAR	pszFound	= in_szPopupList;

	/* Find the currently selected element */
	while( *pszFound && *pszFound != TBX_CLI_TOOLS_POPUP_LIST_CURR_ELEM_START_CHAR )
	{
		pszFound++;
	}
	if( *pszFound == TBX_CLI_TOOLS_POPUP_LIST_CURR_ELEM_START_CHAR )
	{
		PTBX_CHAR	pszPrevSelectedElement = pszFound;
		TBX_CHAR	szElementDummy[ TBX_CLI_TOOLS_CLI_LINE_WIDTH_MAX ];

		/* 'Consume' current element */
		pszFound = TbxCliToolsGetPopupListNextToken
		(
			pszFound,
			szElementDummy
		);
		if( szElementDummy[0] == '\0' )
		{
			/* We were already on the last element. Can't continue */
		}
		else
		{
			/* 'Test' if there is a 'next' element */
			TbxCliToolsGetPopupListNextToken
			(
				pszFound,
				szElementDummy
			);
			if( szElementDummy[0] != '\0' )
			{
				if( *pszFound )
					pszFound++;	/* Skip the separator */

				/* Insert the new selection character here */
				StringInsertChar
				(
					pszFound,
					TBX_CLI_TOOLS_POPUP_LIST_CURR_ELEM_START_CHAR,
					TBX_CLI_TOOLS_CLI_LINE_WIDTH_MAX - (pszFound - in_szPopupList)
				);

				/* Remove the selection character */
				StringRemoveChar( pszPrevSelectedElement );
			}
		}
	}
}

/*-------------------------------------------------------------------------------------------------------------------------------
 |
 |  TbxCliToolsPopupListPrev			:	Function that changes the actually selected popup
 |											element to the prev in list.
 |
 |  in_szPopupList						:	List to move TBX_CLI_TOOLS_POPUP_LIST_CURR_ELEM_START_CHAR
 |											to the prev element
 |
 |  Note								:	~
 |
 |  Return								:	Pointer to list after the returned token
 |
 *------------------------------------------------------------------------------------------------------------------------------*/
static TBX_VOID TbxCliToolsPopupListPrev
(
  IN		TBX_CHAR			in_szPopupList[ TBX_CLI_TOOLS_CLI_LINE_WIDTH_MAX ]
)
{
	PTBX_CHAR	pszFound		= in_szPopupList;
	PTBX_CHAR	pszPrevElement	= NULL;

	/* List through separators */
	while( *pszFound )
	{
		while
		(
			*pszFound &&
			*pszFound != TBX_CLI_TOOLS_POPUP_LIST_START_CHAR &&
			*pszFound != TBX_CLI_TOOLS_POPUP_LIST_SEPARATOR
		)
		{
			pszFound++;
		}
		if( *pszFound )
			pszFound++;	/* Skip the separator */

		/* Skip options if any */
		while
		(
			*pszFound == TBX_CLI_TOOLS_POPUP_LIST_SHORT_OPTION_CHAR ||
			*pszFound == TBX_CLI_TOOLS_POPUP_LIST_OPTION_NO_IMMEDIATE_VALIDATION
		)
		{
			pszFound++;
		}

		if( *pszFound )
		{
			/* Skip whitespaces */
			while( *pszFound && isspace( (unsigned char)*pszFound ) )
			{
				pszFound++;
			}
			if( *pszFound == TBX_CLI_TOOLS_POPUP_LIST_CURR_ELEM_START_CHAR )
			{
				/* This is the selected element */
				if( pszPrevElement )
				{
					/* Remove TBX_CLI_TOOLS_POPUP_LIST_CURR_ELEM_START_CHAR from this element */
					StringRemoveChar( pszFound );

					/* Add TBX_CLI_TOOLS_POPUP_LIST_CURR_ELEM_START_CHAR before previous element */
					StringInsertChar
					(
						pszPrevElement,
						TBX_CLI_TOOLS_POPUP_LIST_CURR_ELEM_START_CHAR,
						TBX_CLI_TOOLS_CLI_LINE_WIDTH_MAX - (pszPrevElement - in_szPopupList)
					);
				}
				else
				{
					/* This is the first element, we cannot move selection to previous element */
				}
				break;
			}
			else
			{
				/* This is a non-selected element */
				pszPrevElement = pszFound;

				/* Skip options */
				while
				(
					*pszPrevElement == TBX_CLI_TOOLS_POPUP_LIST_SHORT_OPTION_CHAR ||
					*pszPrevElement == TBX_CLI_TOOLS_POPUP_LIST_OPTION_NO_IMMEDIATE_VALIDATION
				)
				{
					pszPrevElement++;
				}
			}
		}
		else
		{
			/* Reached end of string */
		}
	}

	return;
}


/*-------------------------------------------------------------------------------------------------------------------------------
 |
 |  TBXCliToolsCheckConsoleInput:	This function returns whether or not a console input (like keyboard or mouse)
 |									is waiting to be processed.
 |
 |  ~		        :	No arguments used
 |
 |  Note			:	~
 |
 |  Return          :	TBX_RESULT_OK
 |						TBX_RESULT_FAIL
 |
 *------------------------------------------------------------------------------------------------------------------------------*/
TBX_INT TBXCliToolsCheckConsoleInput (void)
{
	#ifdef WIN32
	{
		HANDLE						hStdIn;
		DWORD						dwNbEvents;
		INPUT_RECORD				Record;

		hStdIn = GetStdHandle (STD_INPUT_HANDLE);

		PeekConsoleInput( hStdIn, &Record, 1, &dwNbEvents );
		return (TBX_INT)dwNbEvents;
	}
	#else
	{
		return TBXCliKeyboardHit();
	}
	#endif
}


#define PAD_RIGHT 1
#define PAD_ZERO 2

int TBXCliToolsValidatePrintfFormat( const char *format, va_list args )
{
	register int width, maxwidth, pad, tmp;
	TBX_UINT64 tmp64;
	(void)tmp64;
	(void)tmp;
	
	if( !format )
		return 0;

	for (; *format != 0; ++format) {
		if (*format == '%') {
			int iIs64Bits = 0;
			++format;
			width = pad = 0;
			if (*format == '\0') break;
			if (*format == '%') goto out;
			if (*format == '-') {
				++format;
				pad = PAD_RIGHT;
			}
			while (*format == '0') {
				++format;
				pad |= PAD_ZERO;
			}
			
			/* width specifier */
			if( *format == '*' )
			{
				tmp = va_arg( args, int );
				++format;
			}
			else
			{
				for( ; *format >= '0' && *format <= '9'; ++format )
				{
					width *= 10;
					width += *format - '0';
				}
			}
			
			/* precision specifier */
			maxwidth = (int)(~0U >> 1); /* Doesn't work on CPUs that don't use two's complement. */
			if( *format == '.' )
			{
				++format;
				if( *format == '*')
				{
					tmp = va_arg( args, int );
					++format;
				}
				else
				{
					if( !(*format >= '1' && *format <= '9') )
						return 0;
					maxwidth = 0;
					for( ; *format >= '0' && *format <= '9'; ++format )
					{
						maxwidth *= 10;
						maxwidth += *format - '0';
					}
				}
			}

			/* length specifier */
			if( *format == 'h' )
			{
				++format;
				if( *format == 'h' )
					++format;
			}
			else if( *format == 'l' )
			{
				++format;
				if( *format == 'l' ) {
					iIs64Bits = 1;
					++format;
				}
			}
			else if( *format == 'j' || *format == 'z' || *format == 't' )
			{
				++format;
			}
			else if( *format == 'L' )
			{
				/* long double */
				long double ld;
				(void)ld;
				++format;
				if( *format != 'f' )
					return 0;
				++format;
				ld = va_arg( args, long double );
				continue;
			}
			
			/* format specifier */
#ifdef WIN32
			if( *format == 'I' ) {
				++format;
				if( *format != '6' ) {
					return 0;
				}
				++format;
				if( *format != '4' ) {
					return 0;
				}
				++format;

				iIs64Bits = 1;
			}
#endif
			if( *format == 's' || *format == 'S' ) {
				register char *s = (char *)va_arg( args, char* );
				if( !s ) return 0;	/* Return 0 if passed string is NULL */
				continue;
			}
			if( *format == 'c' || *format == 'C' ) {
				/* char are converted to int then pushed on the stack */
				tmp = va_arg( args, int );
				continue;
			}

			if( *format == 'n' ) {
				if( iIs64Bits )
					tmp64 = va_arg( args, TBX_UINT64 );
				else
					tmp = va_arg( args, int );
				continue;
			}

			if( *format == 'd' ) {
				if( iIs64Bits )
					tmp64 = va_arg( args, TBX_UINT64 );
				else
					tmp = va_arg( args, int );
				continue;
			}
			if( *format == 'i' ) {
				if( iIs64Bits )
					tmp64 = va_arg( args, TBX_UINT64 );
				else
					tmp = va_arg( args, int );
				continue;
			}
			if( *format == 'o' ) {
				if( iIs64Bits )
					tmp64 = va_arg( args, TBX_UINT64 );
				else
					tmp = va_arg( args, int );
				continue;
			}
			if( *format == 'u' ) {
				if( iIs64Bits )
					tmp64 = va_arg( args, TBX_UINT64 );
				else
					tmp = va_arg( args, int );
				continue;
			}
			if( *format == 'x' ) {
				if( iIs64Bits )
					tmp64 = va_arg( args, TBX_UINT64 );
				else
					tmp = va_arg( args, int );
				continue;
			}
			if( *format == 'X' ) {
				if( iIs64Bits )
					tmp64 = va_arg( args, TBX_UINT64 );
				else
					tmp = va_arg( args, int );
				continue;
			}


			if( *format == 'f' ) {
				if( iIs64Bits )
					tmp64 = va_arg( args, TBX_UINT64 );
				else
					tmp = (int)va_arg( args, double ); /* comp.lang.c FAQ list Question 15.10 */
				continue;
			}
			if( *format == 'e' ) {
				if( iIs64Bits )
					tmp64 = va_arg( args, TBX_UINT64 );
				else
					tmp = va_arg( args, int );
				continue;
			}
			if( *format == 'E' ) {
				if( iIs64Bits )
					tmp64 = va_arg( args, TBX_UINT64 );
				else
					tmp = va_arg( args, int );
				continue;
			}
			if( *format == 'g' ) {
				if( iIs64Bits )
					tmp64 = va_arg( args, TBX_UINT64 );
				else
					tmp = va_arg( args, int );
				continue;
			}
			if( *format == 'G' ) {
				if( iIs64Bits )
					tmp64 = va_arg( args, TBX_UINT64 );
				else
					tmp = va_arg( args, int );
				continue;
			}

			if( *format == 'p' ) {
				tmp = va_arg( args, TBX_ADDR );
				continue;
			}

			if( *format == 'I' ) {
				return 0;	/* the I may crash some application */
			}
			if( *format == 'S' ) {
				return 0;	/* the S may crash some application */
			}


		}
		else {
		out:
			;
		}
	}
	va_end( args );

	return 1;
}

