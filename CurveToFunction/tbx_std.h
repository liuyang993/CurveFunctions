/*--------------------------------------------------------------------------------------------------------------------------------
 |
 |	Project:    	TBX products family
 |
 |	Filename:   	tbx_std.h
 |
 |	Copyright:  	TelcoBridges 2002-2003, All Rights Reserved
 |
 |	Description:	This header file contains the definitions for the coding standard.
 |
 |	Notes:			Tabs = 4
 |
 *--------------------------------------------------------------------------------------------------------------------------------
 |
 |	Revision:   	$Id: tbx_std.h 148592 2014-09-18 15:27:03Z rblanchet $
 |
 *------------------------------------------------------------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------------------------------------------------------------
 |  Define header file
 *------------------------------------------------------------------------------------------------------------------------------*/
#ifndef __TBX_STD_H__
#define __TBX_STD_H__


/*--------------------------------------------------------------------------------------------------------------------------------
 |  C++ support
 *------------------------------------------------------------------------------------------------------------------------------*/
#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */


#include <ctype.h>	/* For toupper/tolower */
#include <string.h>	/* For strcpy */

/*--------------------------------------------------------------------------------------------------------------------------------
 |  Defines
 *------------------------------------------------------------------------------------------------------------------------------*/

#if defined (__TBX_USE_PROFILING__)
#define CODE															\
	TBX_UINT32				__ErrorCode = (TBX_UINT32)TBX_RESULT_OK;	\
	TBX_UINT32				__nErrorLevel = 0;							\
	PTBX_CHAR				__szErrorDescription = NULL;				\
	TBX_INT					__nErrorLine = 0;							\
	TBX_PROFILING_START													\

#else
#define CODE															\
	TBX_UINT32				__ErrorCode = (TBX_UINT32)TBX_RESULT_OK;	\
	TBX_UINT32				__nErrorLevel = 0;							\
	PTBX_CHAR				__szErrorDescription = NULL;				\
	TBX_INT					__nErrorLine = 0;							\

#endif /* __TBX_USE_PROFILING__ */

#define ERROR_HANDLING													\
	__ErrorHandling_prevent_unreachable_codewarning:					\
	if (__ErrorCode != TBX_RESULT_OK)									\
	__ErrorHandling:

#define CLEANUP					\
	(void)__nErrorLevel;		\
	(void)__szErrorDescription;	\
	(void)__nErrorLine;			\
	goto __Cleanup;				\
	__Cleanup:

#if defined (__TBX_USE_PROFILING__)

#define RETURN													\
	TBX_PROFILING_END											\
	return (__ErrorCode)


#define RETURN_WITH_TYPE(__ResultCodeType)						\
	TBX_PROFILING_END											\
	return (__ResultCodeType)(__ErrorCode)


#define RETURN_VOID												\
	TBX_PROFILING_END											\
	return

#define RETURN_OTHER( __ToReturn )								\
	TBX_PROFILING_END											\
	return __ToReturn

#else

#define RETURN													\
	return (__ErrorCode)


#define RETURN_WITH_TYPE(__ResultCodeType)						\
	return (__ResultCodeType)(__ErrorCode)


#define RETURN_VOID												\
	return

#define RETURN_OTHER( __ToReturn )								\
	return __ToReturn

#endif /* __TBX_USE_PROFILING__ */


#define TBX_EXIT_SUCCESS_VOID		\
	{								\
		(void)__ErrorCode;			\
		(void)__szErrorDescription;	\
		(void)__nErrorLine;			\
		(void)__nErrorLevel;		\
		if (__nErrorLine == 1)		\
 		{	goto __ErrorHandling_prevent_unreachable_codewarning;}	\
		if (__nErrorLine )			\
 		{	goto __ErrorHandling;}	\
		goto __Cleanup;				\
	}


/*
 * Some useful inline string manipulation functions
*/

/* TelcoBridges "improved" version of strncpy:
  - Always NULL-terminates the string (even if can't store in_pszString completely in out_pszString)
  - Does not memset to 0 up to in_un32MaxLength bytes in outgoing buffer
*/
static __inline void tbx_strncpy( char* out_pszString, const char* in_pszString, unsigned int in_un32MaxLength )
{
	unsigned int un32Left;
	if( in_un32MaxLength > 0 )
		un32Left = in_un32MaxLength - 1;
	else
		un32Left = 0;
	while( un32Left-- )
	{
		char	cChar = *in_pszString++;
		*out_pszString++ = cChar;
		if( cChar == '\0' )
		{
			return;
		}
	}
	/* Force NULL-termination */
	*out_pszString = '\0';
}

/* Telcobridges "improved" version of strncat:
  - Always NULL-terminates the string (even if can't store in_pszString completely in out_pszString)
  - in_un32MaxLength represents the total length of out_pszString, so caller does not have to
    manually calculate how many bytes can still fit at the end of out_pszString.
	Caller simply passes sizeof( szBuffer ).
*/
static __inline void tbx_strncat( char* out_pszString, const char* in_pszString, unsigned int in_un32MaxLength )
{
	if( in_un32MaxLength != 0 )
	{
		const unsigned int un32Length = strlen(out_pszString);
		const unsigned int un32MaxCharacters = in_un32MaxLength - 1;
		strncat( out_pszString, in_pszString, un32MaxCharacters - un32Length );
		out_pszString[ un32MaxCharacters ] = '\0'; /* Probably useless */
	}
}

#ifdef TBX_STD_DEFINE_SNPRINTF

#ifdef WIN32
	#define vsnprintf _vsnprintf
	#define snprintf _snprintf
#endif

/* Modified version of vnsprintf that always NULL-terminate string, and returns the exact number of characters written,
   (by opposition to vnsprintf which can return -1, or greater than in_un32BufLen) */
static __inline unsigned int tbx_vsnprintf
(
	char*			out_pszOutString,
	unsigned int	in_un32BufLen,
	const char*		in_pszFormat,
	va_list			in_Marker
)
{
	unsigned int	un32Len;
	int			iLen;

	iLen = vsnprintf( out_pszOutString, in_un32BufLen, in_pszFormat, in_Marker );

	if( iLen < 0 )
		un32Len = (in_un32BufLen - 1);
	else if( (unsigned int)iLen >= in_un32BufLen )
		un32Len = (in_un32BufLen - 1);
	else
		un32Len = (unsigned int)iLen;

	/* Force NULL-termination */
	out_pszOutString[ in_un32BufLen - 1 ] = '\0';

	return un32Len;
}

/* See tbx_vsnprintf */
static __inline unsigned int tbx_snprintf
(
	char*			out_pszOutString,
	unsigned int	in_un32BufLen,
	const char*		in_pszFormat,
	...
)
{
	unsigned int	un32Len;
	va_list			marker;

	va_start (marker, in_pszFormat);

	un32Len = tbx_vsnprintf( out_pszOutString, in_un32BufLen, in_pszFormat, marker );

	va_end (marker);

	return un32Len;
}
#endif

/* Convert entire string to upper case */
static __inline void tbx_toupper( char* in_pszString )
{
	while( *in_pszString )
	{
		*in_pszString = (char)toupper( (int)*in_pszString );
		in_pszString++;
	}
}

/* Convert entire string to lower case */
static __inline void tbx_tolower( char* in_pszString )
{
	while( *in_pszString )
	{
		*in_pszString = (char)tolower( (int)*in_pszString );
		in_pszString++;
	}
}


/* Function that compares two strings ignoring upper or lower case differences */
static __inline int tbx_stricmp( const char * s1, const char* s2 )
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

/* Function that compares two strings ignoring upper or lower case differences */
static __inline int tbx_strnicmp( const char * s1, const char* s2, int n )
{
	while( toupper(*s1) == toupper(*s2) && n )
	{
		if (*s1 == 0)
		  return 0;

		s1++;
		s2++;
		n--;
	}

	if( n == 0 )
		return 0;

	return (toupper(*s1) - toupper(*s2));
}

/* Version of strrchr that does not return NULL
   and returns one character past found character */
static __inline char* tbx_strrchr( const char * in_pszStr, int in_c )
{
	char* pszPtr 	= (char *)in_pszStr;
	char* pszPtrChr = NULL;

	while( pszPtr[0] && (pszPtr - in_pszStr) < 256 )
	{
		while( pszPtr[0] && pszPtr[0] != in_c && (pszPtr - in_pszStr) < 256 )
			pszPtr++;

		if( pszPtr[0] == in_c && pszPtr[1] )
			pszPtrChr = &pszPtr[1];

		if( pszPtr[0] )
			pszPtr++;
	}

	return pszPtrChr ? pszPtrChr : (char*)in_pszStr;
}

/* Function that returns the file name from a path */
static __inline char* TbxFileNameFromPath( const char * in_pszFilePath )
{
	/* Check if it's a windows style path with back-slash */
	char* pszFileNameBackSlash;
	char* pszFileNameSlash;

	pszFileNameBackSlash	= tbx_strrchr( in_pszFilePath, '\\' );	/* Return one character passed the last back-slash */
	pszFileNameSlash		= tbx_strrchr( in_pszFilePath, '/' );	/* Return one character passed the last slash */

	/* Return the smallest string (the last found slash or backslash) */
	if( pszFileNameBackSlash > pszFileNameSlash )
		return pszFileNameBackSlash;
	return pszFileNameSlash;
}

/* Function that returns 1 if in_pszString ends with in_pszEnd */
static __inline int TbxStringEndsWith( const char * in_pszString, const char* in_pszEnd )
{
	size_t un32StrlenString;
	size_t un32StrlenEnd;

	un32StrlenString	= strlen( in_pszString );
	un32StrlenEnd		= strlen( in_pszEnd );

	if( un32StrlenString >= un32StrlenEnd )
	{
		if( strcmp( in_pszString + un32StrlenString - un32StrlenEnd, in_pszEnd ) == 0 )
		{
			/* Yes, ends with */
			return 1;
		}
	}

	return 0;
}

static __inline int TbxIsSpace( char in_cChar )
{
	if
	(
		( in_cChar == ' ' )	||
		( in_cChar == '\f' )	||
		( in_cChar == '\t' )	||
		( in_cChar == '\v' )	||
		( in_cChar == '\r' )	||
		( in_cChar == '\n' )
	)
	{
		return 1;
	}

	return 0;
}



/* Function to remove spaces and endlines at start or end of a string */
static __inline void TbxStringRemoveBlanks( char* in_pszString )
{
	char* pszString = in_pszString;
	char* pszBlank  = NULL;	/* Points to first consecutive Blank at end of string */

	/* Skip initial whitespaces */
	while( *pszString && TbxIsSpace( *pszString) )
	{
		pszString++;
	}

	if( pszString == in_pszString )
	{
		/* No spaces removed, just remove characters at the end */
		while( *pszString )
		{
			if( TbxIsSpace( *pszString) )
			{
				if( !pszBlank) pszBlank = pszString;
			}
			else
			{
				pszBlank = NULL;
			}
			pszString++;
		}
	}
	else
	{
		/* Need to 'shift' the string to remove initial characters */
		while( *pszString )
		{
			*in_pszString = *pszString;
			if( TbxIsSpace( *in_pszString) )
			{
				if( !pszBlank ) pszBlank = in_pszString;
			}
			else
			{
				pszBlank = NULL;
			}
			pszString++;
			in_pszString++;
		}
		*in_pszString = '\0';
	}

	if( pszBlank )
	{
		/* Truncate any Blank at the end */
		*pszBlank = '\0';
	}
}

/* Same as TbxStringRemoveBlanks, but does not waste time moving the data
 inside the string, instead returns a pointer to the new start of the string
 within io_pszString */
static __inline char* TbxStringRemoveBlanks2( char* io_pszString )
{
	/* Make sure it does not begin with a end-line */
	while( *io_pszString && TbxIsSpace( *io_pszString ) )
	{
		io_pszString++;
	}

	/* Make sure it does not end with a end-line */
	if( *io_pszString )
	{
		char*	pszEnd = io_pszString + strlen(io_pszString) - 1;

		while
		(
			( pszEnd > io_pszString ) &&
			TbxIsSpace( *pszEnd )
		)
		{
			*pszEnd = '\0';
			pszEnd--;
		}
	}

	return io_pszString;
}

/*
 * Obsolete names for string manipulation functions above (we have unified naming, below are old names)
*/

#define Strncpy tbx_strncpy
#define Strncat tbx_strncat
#define ToUpper tbx_toupper
#define ToLower tbx_tolower
#define TbxStricmp tbx_stricmp
#define STRRCHR tbx_strrchr



#define TBX_ERROR_RESULT			__ErrorCode
#define TBX_ERROR_DESCRIPTION		__szErrorDescription
#define TBX_ERROR_LINE				__nErrorLine
#define TBX_ERROR_LEVEL				__nErrorLevel
#define TBX_ERROR_FILE				(TbxFileNameFromPath(__FILE__))

/*--------------------------------------------------------------------------------------------------------------------------------
 |  Types
 *------------------------------------------------------------------------------------------------------------------------------*/
/*
 * Messages and error tracing level for log buffer / log file
*/
typedef enum _TBX_TRACE_LEVEL
{
	TBX_TRACE_LEVEL_0		= 0,
	TBX_TRACE_LEVEL_1		= 1,
	TBX_TRACE_LEVEL_2		= 2,
	TBX_TRACE_LEVEL_3		= 3,
	TBX_TRACE_LEVEL_4		= 4,
	TBX_TRACE_LEVEL_MAX		= 4,
	TBX_TRACE_LEVEL_ALWAYS	= 0x7FFFFFFD,
	TBX_TRACE_LEVEL_ERROR	= 0x7FFFFFFE,
	TBX_TRACE_LEVEL_DISABLE	= 0x7FFFFFFF		/* Use this value carefully */
} TBX_TRACE_LEVEL, *PTBX_TRACE_LEVEL;


/*--------------------------------------------------------------------------------------------------------------------------------
 |  Global variables
 *------------------------------------------------------------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------------------------------------------------------------
 |  Macros
 *------------------------------------------------------------------------------------------------------------------------------*/

#ifdef TBX_DEBUG
#ifndef TBX_ENABLE_ASSERT
#define TBX_ENABLE_ASSERT
#endif /* !TBX_ENABLE_ASSERT */
#endif /* TBX_DEBUG */

#ifdef TBX_ENABLE_ASSERT
#define TBX_ASSERT_HANDLER(_Condition, _ConditionStr) \
											((void)((_Condition) || (TBX_PLATFORM_ASSERT_HANDLER(_Condition, _ConditionStr), TBX_FALSE)))
#else /* TBX_ENABLE_ASSERT */
#define TBX_ASSERT_HANDLER(_Condition, _ConditionStr) \
											((void)0)
#endif /* TBX_ENABLE_ASSERT */

#define TBX_ASSERT(_Condition)				TBX_ASSERT_HANDLER(_Condition, #_Condition)
#define TBX_PRECONDITION(_Condition)		TBX_ASSERT_HANDLER(_Condition, #_Condition)
#define TBX_POSTCONDITION(_Condition)		TBX_ASSERT_HANDLER(_Condition, #_Condition)

#define TBX_EXIT_ERROR(_ErrorCode, _nErrorLevel, _szErrorDescription)		\
	{																		\
		__ErrorCode = (TBX_UINT32)_ErrorCode;								\
		__szErrorDescription = (PTBX_CHAR)_szErrorDescription;				\
		__nErrorLevel = (TBX_UINT32)_nErrorLevel;							\
		__nErrorLine = (TBX_INT)__LINE__;									\
		if (__ErrorCode != TBX_RESULT_OK )									\
 		{	goto __ErrorHandling_prevent_unreachable_codewarning;}			\
		goto __ErrorHandling;												\
	}

#define TBX_EXIT_SUCCESS(_ErrorCode)					\
	{													\
		__ErrorCode = (TBX_UINT32)_ErrorCode;			\
		(void)__szErrorDescription;						\
		(void)__nErrorLine;								\
		(void)__nErrorLevel;							\
		if (__nErrorLine == 1)								\
		{ 	goto __ErrorHandling_prevent_unreachable_codewarning;}	\
		if (__nErrorLine )								\
 		{	goto __ErrorHandling;}						\
		goto __Cleanup;									\
	}

#define TBX_EXIT_CLEANUP	TBX_EXIT_SUCCESS

/* Jumps to ERROR_HANDLING in case the result code is a failure */
#define TBX_EXIT_ON_ERROR( _Result, _Str ) \
	if( TBX_RESULT_FAILURE( (TBX_UINT32)_Result ) ) \
	{ \
		TBX_EXIT_ERROR( _Result, 0, _Str ); \
	}

/* Jumps to ERROR_HANDLING in case the passed pointer is NULL */
#define TBX_EXIT_ON_NULL( _ptr, _Result, _Str ) \
	if( !(_ptr) ) \
	{ \
		TBX_EXIT_ERROR( _Result, 0, _Str ); \
	}

/* Jumps to ERROR_HANDLING in case the passed expression is false */
#define TBX_EXIT_ON_FALSE( _pCondition, _Result, _Str ) \
	if( !(_pCondition) ) \
	{ \
		TBX_EXIT_ERROR( _Result, 0, _Str ); \
	}

#define TBX_MIN(a,b)        				((a) < (b) ? (a) : (b))
#define TBX_MAX(a,b)        				((a) < (b) ? (b) : (a))
#define TBX_ABS(a)							((a) >= 0 ? (a) : -(a))

/* Macro to align a value _Addr to the next multiple of _Align */
#define TBX_ALIGN(_Addr, _Align)		\
{										\
	if( (_Addr) % (_Align) )			\
	{									\
		(_Addr) -= (_Addr) % (_Align);	\
		(_Addr) += (_Align);			\
	}									\
}
#define TBX_ALIGN_LOW(_Addr, _Align)	\
{										\
	if( (_Addr) % (_Align) )			\
	{									\
		(_Addr) -= (_Addr) % (_Align);	\
	}									\
}

#define TBX_IS_A_CHAR( _c ) ((32 <= (_c)) && ((_c) < 127))

/*--------------------------------------------------------------------------------------------------------------------------------
 |  Function Prototypes
 *------------------------------------------------------------------------------------------------------------------------------*/

#ifdef __cplusplus

#define TB_C_FUNC_PROTOTYPE		extern "C"

#else

#define TB_C_FUNC_PROTOTYPE

#endif /* __cplusplus */



/*--------------------------------------------------------------------------------------------------------------------------------
 |  C++ support
 *------------------------------------------------------------------------------------------------------------------------------*/
#ifdef __cplusplus
}
#endif /* __cplusplus */

#ifdef TBX_STD_LEAN_AND_MEAN
#include <tbx_std_lw.h>
#endif

#endif /* __TBX_STD_H__ */
