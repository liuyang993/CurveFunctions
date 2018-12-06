/*--------------------------------------------------------------------------------------------------------------------------------
 |
 |	Project:    	VoiceLink
 |
 |	Filename:   	tbx_result.h
 |
 |	Copyright:  	TelcoBridges 2002-2003, All Rights Reserved
 |
 |	Description:	This header file contains definitions concerning result codes.
 |
 |					  2bits    14bits   16bits
 |					+-------+---------+-----------+
 |					| Type  | Class   | ID number |
 |					+-------+---------+-----------+
 |					 \___________________________/
 |					             |
 |					             +--------- Result code (32 bits)
 |
 |					Type	: Result type identifier (success, error or warning)
 |					Class	: Module class identifier (trunk manager, CTbus manager, connection manager...)
 |					ID		: Result identifier relative to the module class and locally defined in the module itself.
 |
 |	Notes:			Tabs = 4
 |
 *--------------------------------------------------------------------------------------------------------------------------------
 |
 |	Revision:   	$Id: tbx_result.h 119321 2013-01-16 00:23:04Z atremblay $
 |
 *------------------------------------------------------------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------------------------------------------------------------
 |  Define header file
 *------------------------------------------------------------------------------------------------------------------------------*/
#ifndef __TBX_RESULT_H__
#define __TBX_RESULT_H__



/*--------------------------------------------------------------------------------------------------------------------------------
 |  C++ support
 *------------------------------------------------------------------------------------------------------------------------------*/
#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */


/*--------------------------------------------------------------------------------------------------------------------------------
 |  Some include dependencies
 *------------------------------------------------------------------------------------------------------------------------------*/
#include "tbx_id.h"
#include "tbx_ostypes.h"


/*--------------------------------------------------------------------------------------------------------------------------------
 |  Defines
 *------------------------------------------------------------------------------------------------------------------------------*/


/* Types of error codes */
#define TBX_RESULT_TYPE_SUCCESS					0x00
#define TBX_RESULT_TYPE_WARNING					0x01
#define TBX_RESULT_TYPE_ERROR					0x02
#define TBX_RESULT_TYPE_NOTUSED					0x03

/* Generic result identifier */
#define	TBX_RESULT_ID_OK						0x0000
#define TBX_RESULT_ID_FAIL						0x0001
#define TBX_RESULT_ID_FATAL_ERROR				0x0002
#define TBX_RESULT_ID_TIMEOUT					0x0003
#define TBX_RESULT_ID_MISMATCH					0x0004
#define TBX_RESULT_ID_ACCESS_DENIED				0x0005
#define TBX_RESULT_ID_OUT_OF_RANGE				0x0006
#define TBX_RESULT_ID_OUT_OF_MEMORY				0x0007
#define TBX_RESULT_ID_OUT_OF_RESOURCE			0x0008
#define TBX_RESULT_ID_NULL_PARAM				0x0009
#define TBX_RESULT_ID_INVALID_PARAM				0x000A
#define TBX_RESULT_ID_INVALID_CONTEXT			0x000B
#define TBX_RESULT_ID_INVALID_HANDLE			0x000C
#define TBX_RESULT_ID_INVALID_DEVICE			0x000D
#define TBX_RESULT_ID_INVALID_CONFIG			0x000E
#define TBX_RESULT_ID_INVALID_STATE				0x000F
#define TBX_RESULT_ID_INVALID_MESSAGE			0x0010
#define TBX_RESULT_ID_INVALID_OPERATION			0x0011
#define TBX_RESULT_ID_NOT_ENOUGH_MEMORY			0x0012
#define TBX_RESULT_ID_NOT_SUPPORTED				0x0013
#define TBX_RESULT_ID_NOT_INITIALIZED			0x0014
#define TBX_RESULT_ID_NOT_FOUND					0x0015
#define TBX_RESULT_ID_NOT_READY					0x0016
#define TBX_RESULT_ID_NOT_ACCESSIBLE			0x0017
#define TBX_RESULT_ID_NOT_RESPONDING			0x0018
#define TBX_RESULT_ID_NOT_IMPLEMENTED			0x0019
#define TBX_RESULT_ID_BUFFER_TOO_SMALL			0x001A
#define TBX_RESULT_ID_UNSUPPORTED_VERSION		0x0020
#define TBX_RESULT_ID_NOT_SUPPORTED_BY_LICENSE	0x0021
#define TBX_RESULT_ID_OPERATION_INCOMPLETE		0x0022
#define TBX_RESULT_ID_OPERATION_POSTPONED		0x0023

/*	Maximum number of result identifiers */
#define TBX_RESULT_RESULT_ID_MAX_COUNT			0x200


/*--------------------------------------------------------------------------------------------------------------------------------
 |  Types
 *------------------------------------------------------------------------------------------------------------------------------*/

/* Generic result data type */
typedef unsigned __int32		TBX_RESULT, *PTBX_RESULT;


/*--------------------------------------------------------------------------------------------------------------------------------
 |  Global variables
 *------------------------------------------------------------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------------------------------------------------------------
 |  Macros
 *------------------------------------------------------------------------------------------------------------------------------*/

/* Create result code common to all module class */
#define TBX_RESULT_CREATE_COMMON(__Type__, __Class__, __Id__)		\
  (TBX_UINT32)((((TBX_UINT32)(__Type__) & 0x00000003) << 30) |		\
  (((TBX_UINT32)(__Class__) & 0x00003FFF) << 16)|					\
  (((TBX_UINT32)(__Id__) & 0x0000FFFF) << 0))

/* Create result code */
#define TBX_RESULT_CREATE(__Type__, __Class__, __Id__)		\
  TBX_RESULT_CREATE_COMMON(__Type__, __Class__, (__Id__ + TBX_RESULT_RESULT_ID_MAX_COUNT))

/* Create result code with default method */
#define TBX_RESULT_CREATE_X								TBX_RESULT_CREATE_ENUM

/* Create result code with enum method */
#define TBX_RESULT_CREATE_ENUM( _1, _2, _3, _4, _5 ) 	_1 = TBX_RESULT_CREATE( _3, _4, _5 )

/* Create result with also Result string and description */
#define TBX_RESULT_CREATE_TEXT( _1, _2, _3, _4, _5 )	{ TBX_RESULT_CREATE( _3, _4, _5 ), #_1 ": " _2 }

/* Retrieve the class identifier from result code */
#define TBX_RESULT_GET_CLASS(__Result__)		((TBX_UINT32)((__Result__) & 0x3FFF0000) >> 16)

/* Retrieve the ID number from result code */
#define TBX_RESULT_GET_ID(__Result__)			((__Result__) & 0x0000FFFF)

/* Test the type bits of result code for success */
#define TBX_RESULT_SUCCESS(__Result__)			(((__Result__) & 0xC0000000) == 0x00000000)

/* Test the type bits of result code for failure */
#define TBX_RESULT_FAILURE(__Result__)			(((__Result__) & 0x80000000) == 0x80000000)

/* Common result codes */
#define	TBX_RESULT_OK						TBX_RESULT_CREATE_COMMON( TBX_RESULT_TYPE_SUCCESS,	TBX_ID_CLASS_TBX_COMMON,	TBX_RESULT_ID_OK )
#define TBX_RESULT_FAIL						TBX_RESULT_CREATE_COMMON( TBX_RESULT_TYPE_ERROR,	TBX_ID_CLASS_TBX_COMMON,	TBX_RESULT_ID_FAIL )
#define TBX_RESULT_FATAL_ERROR				TBX_RESULT_CREATE_COMMON( TBX_RESULT_TYPE_ERROR,	TBX_ID_CLASS_TBX_COMMON,	TBX_RESULT_ID_FATAL_ERROR )
#define TBX_RESULT_TIMEOUT					TBX_RESULT_CREATE_COMMON( TBX_RESULT_TYPE_ERROR,	TBX_ID_CLASS_TBX_COMMON,	TBX_RESULT_ID_TIMEOUT )
#define TBX_RESULT_MISMATCH					TBX_RESULT_CREATE_COMMON( TBX_RESULT_TYPE_ERROR,	TBX_ID_CLASS_TBX_COMMON,	TBX_RESULT_ID_MISMATCH )
#define TBX_RESULT_ACCESS_DENIED			TBX_RESULT_CREATE_COMMON( TBX_RESULT_TYPE_ERROR,	TBX_ID_CLASS_TBX_COMMON,	TBX_RESULT_ID_ACCESS_DENIED )
#define TBX_RESULT_OUT_OF_RANGE				TBX_RESULT_CREATE_COMMON( TBX_RESULT_TYPE_ERROR,	TBX_ID_CLASS_TBX_COMMON,	TBX_RESULT_ID_OUT_OF_RANGE )
#define TBX_RESULT_OUT_OF_MEMORY			TBX_RESULT_CREATE_COMMON( TBX_RESULT_TYPE_ERROR,	TBX_ID_CLASS_TBX_COMMON,	TBX_RESULT_ID_OUT_OF_MEMORY )
#define TBX_RESULT_OUT_OF_RESOURCE			TBX_RESULT_CREATE_COMMON( TBX_RESULT_TYPE_ERROR,	TBX_ID_CLASS_TBX_COMMON,	TBX_RESULT_ID_OUT_OF_RESOURCE )
#define TBX_RESULT_NULL_PARAM				TBX_RESULT_CREATE_COMMON( TBX_RESULT_TYPE_ERROR,	TBX_ID_CLASS_TBX_COMMON,	TBX_RESULT_ID_NULL_PARAM )
#define TBX_RESULT_INVALID_PARAM			TBX_RESULT_CREATE_COMMON( TBX_RESULT_TYPE_ERROR,	TBX_ID_CLASS_TBX_COMMON,	TBX_RESULT_ID_INVALID_PARAM )
#define TBX_RESULT_INVALID_CONTEXT			TBX_RESULT_CREATE_COMMON( TBX_RESULT_TYPE_ERROR,	TBX_ID_CLASS_TBX_COMMON,	TBX_RESULT_ID_INVALID_CONTEXT )
#define TBX_RESULT_INVALID_HANDLE			TBX_RESULT_CREATE_COMMON( TBX_RESULT_TYPE_ERROR,	TBX_ID_CLASS_TBX_COMMON,	TBX_RESULT_ID_INVALID_HANDLE )
#define TBX_RESULT_INVALID_DEVICE			TBX_RESULT_CREATE_COMMON( TBX_RESULT_TYPE_ERROR,	TBX_ID_CLASS_TBX_COMMON,	TBX_RESULT_ID_INVALID_DEVICE )
#define TBX_RESULT_INVALID_CONFIG			TBX_RESULT_CREATE_COMMON( TBX_RESULT_TYPE_ERROR,	TBX_ID_CLASS_TBX_COMMON,	TBX_RESULT_ID_INVALID_CONFIG )
#define TBX_RESULT_INVALID_STATE			TBX_RESULT_CREATE_COMMON( TBX_RESULT_TYPE_ERROR,	TBX_ID_CLASS_TBX_COMMON,	TBX_RESULT_ID_INVALID_STATE )
#define TBX_RESULT_INVALID_MESSAGE			TBX_RESULT_CREATE_COMMON( TBX_RESULT_TYPE_ERROR,	TBX_ID_CLASS_TBX_COMMON,	TBX_RESULT_ID_INVALID_MESSAGE )
#define TBX_RESULT_INVALID_OPERATION		TBX_RESULT_CREATE_COMMON( TBX_RESULT_TYPE_ERROR,	TBX_ID_CLASS_TBX_COMMON,	TBX_RESULT_ID_INVALID_OPERATION )
#define TBX_RESULT_NOT_ENOUGH_MEMORY		TBX_RESULT_CREATE_COMMON( TBX_RESULT_TYPE_ERROR,	TBX_ID_CLASS_TBX_COMMON,	TBX_RESULT_ID_NOT_ENOUGH_MEMORY )
#define TBX_RESULT_NOT_SUPPORTED			TBX_RESULT_CREATE_COMMON( TBX_RESULT_TYPE_ERROR,	TBX_ID_CLASS_TBX_COMMON,	TBX_RESULT_ID_NOT_SUPPORTED )
#define TBX_RESULT_NOT_INITIALIZED			TBX_RESULT_CREATE_COMMON( TBX_RESULT_TYPE_ERROR,	TBX_ID_CLASS_TBX_COMMON,	TBX_RESULT_ID_NOT_INITIALIZED )
#define TBX_RESULT_NOT_FOUND				TBX_RESULT_CREATE_COMMON( TBX_RESULT_TYPE_ERROR,	TBX_ID_CLASS_TBX_COMMON,	TBX_RESULT_ID_NOT_FOUND )
#define TBX_RESULT_NOT_READY				TBX_RESULT_CREATE_COMMON( TBX_RESULT_TYPE_ERROR,	TBX_ID_CLASS_TBX_COMMON,	TBX_RESULT_ID_NOT_READY )
#define TBX_RESULT_NOT_ACCESSIBLE			TBX_RESULT_CREATE_COMMON( TBX_RESULT_TYPE_ERROR,	TBX_ID_CLASS_TBX_COMMON,	TBX_RESULT_ID_NOT_ACCESSIBLE )
#define TBX_RESULT_NOT_RESPONDING			TBX_RESULT_CREATE_COMMON( TBX_RESULT_TYPE_ERROR,	TBX_ID_CLASS_TBX_COMMON,	TBX_RESULT_ID_NOT_RESPONDING )
#define TBX_RESULT_NOT_IMPLEMENTED			TBX_RESULT_CREATE_COMMON( TBX_RESULT_TYPE_ERROR,	TBX_ID_CLASS_TBX_COMMON,	TBX_RESULT_ID_NOT_IMPLEMENTED )
#define TBX_RESULT_BUFFER_TOO_SMALL			TBX_RESULT_CREATE_COMMON( TBX_RESULT_TYPE_ERROR,	TBX_ID_CLASS_TBX_COMMON,	TBX_RESULT_ID_BUFFER_TOO_SMALL )
#define TBX_RESULT_UNSUPPORTED_VERSION		TBX_RESULT_CREATE_COMMON( TBX_RESULT_TYPE_ERROR,	TBX_ID_CLASS_TBX_COMMON,	TBX_RESULT_ID_UNSUPPORTED_VERSION )
#define TBX_RESULT_NOT_SUPPORTED_BY_LICENSE	TBX_RESULT_CREATE_COMMON( TBX_RESULT_TYPE_ERROR,	TBX_ID_CLASS_TBX_COMMON,	TBX_RESULT_ID_NOT_SUPPORTED_BY_LICENSE )
#define TBX_RESULT_OPERATION_INCOMPLETE		TBX_RESULT_CREATE_COMMON( TBX_RESULT_TYPE_ERROR,	TBX_ID_CLASS_TBX_COMMON,	TBX_RESULT_ID_OPERATION_INCOMPLETE )
#define TBX_RESULT_OPERATION_POSTPONED		TBX_RESULT_CREATE_COMMON( TBX_RESULT_TYPE_ERROR,	TBX_ID_CLASS_TBX_COMMON,	TBX_RESULT_ID_OPERATION_POSTPONED )


/* Definition to include the common result codes into an enumeration type */
#define TBX_RESULT_ENUMERATE_COMMONS( __ProductName__, __ModuleName__, __ModuleClass__ ) \
	TBX_RESULT_CREATE_X( TB##__ProductName__##_RESULT_##__ModuleName__##_OK, 						"Success", 			TBX_RESULT_TYPE_SUCCESS, 	__ModuleClass__, TBX_RESULT_ID_OK - TBX_RESULT_RESULT_ID_MAX_COUNT ), \
	TBX_RESULT_CREATE_X( TB##__ProductName__##_RESULT_##__ModuleName__##_FAIL, 						"Fail", 			TBX_RESULT_TYPE_ERROR,		__ModuleClass__, TBX_RESULT_ID_FAIL - TBX_RESULT_RESULT_ID_MAX_COUNT ), \
	TBX_RESULT_CREATE_X( TB##__ProductName__##_RESULT_##__ModuleName__##_FATAL_ERROR,				"FatalError",		TBX_RESULT_TYPE_ERROR,		__ModuleClass__, TBX_RESULT_ID_FATAL_ERROR - TBX_RESULT_RESULT_ID_MAX_COUNT), \
	TBX_RESULT_CREATE_X( TB##__ProductName__##_RESULT_##__ModuleName__##_TIMEOUT,					"Timeout",			TBX_RESULT_TYPE_ERROR,		__ModuleClass__, TBX_RESULT_ID_TIMEOUT - TBX_RESULT_RESULT_ID_MAX_COUNT), \
	TBX_RESULT_CREATE_X( TB##__ProductName__##_RESULT_##__ModuleName__##_MISMATCH,					"MisMatch",			TBX_RESULT_TYPE_ERROR,		__ModuleClass__, TBX_RESULT_ID_MISMATCH - TBX_RESULT_RESULT_ID_MAX_COUNT), \
	TBX_RESULT_CREATE_X( TB##__ProductName__##_RESULT_##__ModuleName__##_ACCESS_DENIED,				"AccessDenied",		TBX_RESULT_TYPE_ERROR,		__ModuleClass__, TBX_RESULT_ID_ACCESS_DENIED - TBX_RESULT_RESULT_ID_MAX_COUNT), \
	TBX_RESULT_CREATE_X( TB##__ProductName__##_RESULT_##__ModuleName__##_OUT_OF_RANGE,				"OutOfRange",		TBX_RESULT_TYPE_ERROR,		__ModuleClass__, TBX_RESULT_ID_OUT_OF_RANGE - TBX_RESULT_RESULT_ID_MAX_COUNT), \
	TBX_RESULT_CREATE_X( TB##__ProductName__##_RESULT_##__ModuleName__##_OUT_OF_MEMORY,				"OutOfMemory",		TBX_RESULT_TYPE_ERROR,		__ModuleClass__, TBX_RESULT_ID_OUT_OF_MEMORY - TBX_RESULT_RESULT_ID_MAX_COUNT), \
	TBX_RESULT_CREATE_X( TB##__ProductName__##_RESULT_##__ModuleName__##_OUT_OF_RESOURCE,			"OutOfResource",	TBX_RESULT_TYPE_ERROR,		__ModuleClass__, TBX_RESULT_ID_OUT_OF_RESOURCE - TBX_RESULT_RESULT_ID_MAX_COUNT), \
	TBX_RESULT_CREATE_X( TB##__ProductName__##_RESULT_##__ModuleName__##_NULL_PARAM,				"NullParam", 		TBX_RESULT_TYPE_ERROR,		__ModuleClass__, TBX_RESULT_ID_NULL_PARAM - TBX_RESULT_RESULT_ID_MAX_COUNT), \
	TBX_RESULT_CREATE_X( TB##__ProductName__##_RESULT_##__ModuleName__##_INVALID_PARAM,				"InvalidParam",		TBX_RESULT_TYPE_ERROR,		__ModuleClass__, TBX_RESULT_ID_INVALID_PARAM - TBX_RESULT_RESULT_ID_MAX_COUNT), \
	TBX_RESULT_CREATE_X( TB##__ProductName__##_RESULT_##__ModuleName__##_INVALID_CONTEXT,			"InvalidContext",	TBX_RESULT_TYPE_ERROR,		__ModuleClass__, TBX_RESULT_ID_INVALID_CONTEXT - TBX_RESULT_RESULT_ID_MAX_COUNT), \
	TBX_RESULT_CREATE_X( TB##__ProductName__##_RESULT_##__ModuleName__##_INVALID_HANDLE,			"InvalidHandle", 	TBX_RESULT_TYPE_ERROR,		__ModuleClass__, TBX_RESULT_ID_INVALID_HANDLE - TBX_RESULT_RESULT_ID_MAX_COUNT),	\
	TBX_RESULT_CREATE_X( TB##__ProductName__##_RESULT_##__ModuleName__##_INVALID_DEVICE,			"InvalidDevice", 	TBX_RESULT_TYPE_ERROR,		__ModuleClass__, TBX_RESULT_ID_INVALID_DEVICE - TBX_RESULT_RESULT_ID_MAX_COUNT),	\
	TBX_RESULT_CREATE_X( TB##__ProductName__##_RESULT_##__ModuleName__##_INVALID_CONFIG,			"InvalidConfig", 	TBX_RESULT_TYPE_ERROR,		__ModuleClass__, TBX_RESULT_ID_INVALID_CONFIG - TBX_RESULT_RESULT_ID_MAX_COUNT),	\
	TBX_RESULT_CREATE_X( TB##__ProductName__##_RESULT_##__ModuleName__##_INVALID_STATE,				"InvalidState",		TBX_RESULT_TYPE_ERROR,		__ModuleClass__, TBX_RESULT_ID_INVALID_STATE - TBX_RESULT_RESULT_ID_MAX_COUNT), \
	TBX_RESULT_CREATE_X( TB##__ProductName__##_RESULT_##__ModuleName__##_INVALID_MESSAGE,			"InvalidMessage",	TBX_RESULT_TYPE_ERROR,		__ModuleClass__, TBX_RESULT_ID_INVALID_MESSAGE - TBX_RESULT_RESULT_ID_MAX_COUNT), \
	TBX_RESULT_CREATE_X( TB##__ProductName__##_RESULT_##__ModuleName__##_INVALID_OPERATION,			"InvalidOperation",	TBX_RESULT_TYPE_ERROR,		__ModuleClass__, TBX_RESULT_ID_INVALID_OPERATION - TBX_RESULT_RESULT_ID_MAX_COUNT), \
	TBX_RESULT_CREATE_X( TB##__ProductName__##_RESULT_##__ModuleName__##_NOT_ENOUGH_MEMORY,			"NotEnoughMemory",	TBX_RESULT_TYPE_ERROR,		__ModuleClass__, TBX_RESULT_ID_NOT_ENOUGH_MEMORY - TBX_RESULT_RESULT_ID_MAX_COUNT), \
	TBX_RESULT_CREATE_X( TB##__ProductName__##_RESULT_##__ModuleName__##_NOT_SUPPORTED,				"NotSupported",		TBX_RESULT_TYPE_ERROR,		__ModuleClass__, TBX_RESULT_ID_NOT_SUPPORTED - TBX_RESULT_RESULT_ID_MAX_COUNT), \
	TBX_RESULT_CREATE_X( TB##__ProductName__##_RESULT_##__ModuleName__##_NOT_INITIALIZED,			"NotInitialized",	TBX_RESULT_TYPE_ERROR,		__ModuleClass__, TBX_RESULT_ID_NOT_INITIALIZED - TBX_RESULT_RESULT_ID_MAX_COUNT), \
	TBX_RESULT_CREATE_X( TB##__ProductName__##_RESULT_##__ModuleName__##_NOT_FOUND,					"NotFound",			TBX_RESULT_TYPE_ERROR,		__ModuleClass__, TBX_RESULT_ID_NOT_FOUND - TBX_RESULT_RESULT_ID_MAX_COUNT), \
	TBX_RESULT_CREATE_X( TB##__ProductName__##_RESULT_##__ModuleName__##_NOT_READY,					"NotReady",			TBX_RESULT_TYPE_ERROR,		__ModuleClass__, TBX_RESULT_ID_NOT_READY - TBX_RESULT_RESULT_ID_MAX_COUNT), \
	TBX_RESULT_CREATE_X( TB##__ProductName__##_RESULT_##__ModuleName__##_NOT_ACCESSIBLE,			"NotAccessible", 	TBX_RESULT_TYPE_ERROR,		__ModuleClass__, TBX_RESULT_ID_NOT_ACCESSIBLE - TBX_RESULT_RESULT_ID_MAX_COUNT), \
	TBX_RESULT_CREATE_X( TB##__ProductName__##_RESULT_##__ModuleName__##_NOT_RESPONDING,			"NotResponding", 	TBX_RESULT_TYPE_ERROR,		__ModuleClass__, TBX_RESULT_ID_NOT_RESPONDING - TBX_RESULT_RESULT_ID_MAX_COUNT), \
	TBX_RESULT_CREATE_X( TB##__ProductName__##_RESULT_##__ModuleName__##_NOT_IMPLEMENTED,			"NotImplemented",	TBX_RESULT_TYPE_ERROR,		__ModuleClass__, TBX_RESULT_ID_NOT_IMPLEMENTED - TBX_RESULT_RESULT_ID_MAX_COUNT), \
	TBX_RESULT_CREATE_X( TB##__ProductName__##_RESULT_##__ModuleName__##_BUFFER_TOO_SMALL,			"BufferTootSmall",	TBX_RESULT_TYPE_ERROR,		__ModuleClass__, TBX_RESULT_ID_BUFFER_TOO_SMALL - TBX_RESULT_RESULT_ID_MAX_COUNT), \
	TBX_RESULT_CREATE_X( TB##__ProductName__##_RESULT_##__ModuleName__##_UNSUPPORTED_VERSION,		"UnsuppVersion",	TBX_RESULT_TYPE_ERROR,		__ModuleClass__, TBX_RESULT_ID_UNSUPPORTED_VERSION - TBX_RESULT_RESULT_ID_MAX_COUNT), \
	TBX_RESULT_CREATE_X( TB##__ProductName__##_RESULT_##__ModuleName__##_NOT_SUPPORTED_BY_LICENSE, 	"NotLicensed", 		TBX_RESULT_TYPE_ERROR,		__ModuleClass__, TBX_RESULT_ID_NOT_SUPPORTED_BY_LICENSE - TBX_RESULT_RESULT_ID_MAX_COUNT), \
	TBX_RESULT_CREATE_X( TB##__ProductName__##_RESULT_##__ModuleName__##_OPERATION_INCOMPLETE,		"OpIncomplete", 	TBX_RESULT_TYPE_ERROR,		__ModuleClass__, TBX_RESULT_ID_OPERATION_INCOMPLETE - TBX_RESULT_RESULT_ID_MAX_COUNT), \
	TBX_RESULT_CREATE_X( TB##__ProductName__##_RESULT_##__ModuleName__##_OPERATION_POSTPONED,		"OpPostponed", 		TBX_RESULT_TYPE_ERROR,		__ModuleClass__, TBX_RESULT_ID_OPERATION_POSTPONED - TBX_RESULT_RESULT_ID_MAX_COUNT)

/*--------------------------------------------------------------------------------------------------------------------------------
 |  Function Prototypes
 *------------------------------------------------------------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------------------------------------------------------------
 |  Macros
 *------------------------------------------------------------------------------------------------------------------------------*/


/*
 * Function to return a string that represents the value of TBX_RESULT.
 * This macro only returns a string for the basic results defined in tbx_result.h.
*/
static __inline PTBX_CHAR TBX_RESULT_TO_STRING( TBX_RESULT in_Result )
{
	switch( in_Result )
	{
		case TBX_RESULT_OK:							return (PTBX_CHAR)"TBX_RESULT_OK";
		case TBX_RESULT_FAIL:						return (PTBX_CHAR)"TBX_RESULT_FAIL";
		case TBX_RESULT_FATAL_ERROR:				return (PTBX_CHAR)"TBX_RESULT_FATAL_ERROR";
		case TBX_RESULT_TIMEOUT:					return (PTBX_CHAR)"TBX_RESULT_TIMEOUT";
		case TBX_RESULT_MISMATCH:					return (PTBX_CHAR)"TBX_RESULT_MISMATCH";
		case TBX_RESULT_ACCESS_DENIED:				return (PTBX_CHAR)"TBX_RESULT_ACCESS_DENIED";
		case TBX_RESULT_OUT_OF_RANGE:				return (PTBX_CHAR)"TBX_RESULT_OUT_OF_RANGE";
		case TBX_RESULT_OUT_OF_MEMORY:				return (PTBX_CHAR)"TBX_RESULT_OUT_OF_MEMORY";
		case TBX_RESULT_OUT_OF_RESOURCE:			return (PTBX_CHAR)"TBX_RESULT_OUT_OF_RESOURCE";
		case TBX_RESULT_NULL_PARAM:					return (PTBX_CHAR)"TBX_RESULT_NULL_PARAM";
		case TBX_RESULT_INVALID_PARAM:				return (PTBX_CHAR)"TBX_RESULT_INVALID_PARAM";
		case TBX_RESULT_INVALID_CONTEXT:			return (PTBX_CHAR)"TBX_RESULT_INVALID_CONTEXT";
		case TBX_RESULT_INVALID_HANDLE:				return (PTBX_CHAR)"TBX_RESULT_INVALID_HANDLE";
		case TBX_RESULT_INVALID_DEVICE:				return (PTBX_CHAR)"TBX_RESULT_INVALID_DEVICE";
		case TBX_RESULT_INVALID_CONFIG:				return (PTBX_CHAR)"TBX_RESULT_INVALID_CONFIG";
		case TBX_RESULT_INVALID_STATE:				return (PTBX_CHAR)"TBX_RESULT_INVALID_STATE";
		case TBX_RESULT_INVALID_MESSAGE:			return (PTBX_CHAR)"TBX_RESULT_INVALID_MESSAGE";
		case TBX_RESULT_INVALID_OPERATION:			return (PTBX_CHAR)"TBX_RESULT_INVALID_OPERATION";
		case TBX_RESULT_NOT_ENOUGH_MEMORY:			return (PTBX_CHAR)"TBX_RESULT_NOT_ENOUGH_MEMORY";
		case TBX_RESULT_NOT_SUPPORTED:				return (PTBX_CHAR)"TBX_RESULT_NOT_SUPPORTED";
		case TBX_RESULT_NOT_INITIALIZED:			return (PTBX_CHAR)"TBX_RESULT_NOT_INITIALIZED";
		case TBX_RESULT_NOT_FOUND:					return (PTBX_CHAR)"TBX_RESULT_NOT_FOUND";
		case TBX_RESULT_NOT_READY:					return (PTBX_CHAR)"TBX_RESULT_NOT_READY";
		case TBX_RESULT_NOT_ACCESSIBLE:				return (PTBX_CHAR)"TBX_RESULT_NOT_ACCESSIBLE";
		case TBX_RESULT_NOT_RESPONDING:				return (PTBX_CHAR)"TBX_RESULT_NOT_RESPONDING";
		case TBX_RESULT_NOT_IMPLEMENTED:			return (PTBX_CHAR)"TBX_RESULT_NOT_IMPLEMENTED";
		case TBX_RESULT_BUFFER_TOO_SMALL:			return (PTBX_CHAR)"TBX_RESULT_BUFFER_TOO_SMALL";
		case TBX_RESULT_UNSUPPORTED_VERSION:		return (PTBX_CHAR)"TBX_RESULT_UNSUPPORTED_VERSION";
		case TBX_RESULT_NOT_SUPPORTED_BY_LICENSE:	return (PTBX_CHAR)"TBX_RESULT_NOT_SUPPORTED_BY_LICENSE";
		case TBX_RESULT_OPERATION_INCOMPLETE:		return (PTBX_CHAR)"TBX_RESULT_OPERATION_INCOMPLETE";
		case TBX_RESULT_OPERATION_POSTPONED:		return (PTBX_CHAR)"TBX_RESULT_OPERATION_POSTPONED";
	}

	return (PTBX_CHAR)"";
}

/*
 * Function to return a string that represents the value of TBX_RESULT_ID_*.
 * This macro only returns a string for the basic results defined in tbx_result.h.
*/
static __inline PTBX_CHAR TBX_RESULT_ID_TO_STRING( TBX_UINT32 in_Result )
{
	TBX_UINT32	un32ResultId = TBX_RESULT_GET_ID( in_Result );

	switch( un32ResultId )
	{
		case TBX_RESULT_ID_OK:						return (PTBX_CHAR)"OK";
		case TBX_RESULT_ID_FAIL:					return (PTBX_CHAR)"FAIL";
		case TBX_RESULT_ID_FATAL_ERROR:				return (PTBX_CHAR)"FATAL_ERROR";
		case TBX_RESULT_ID_TIMEOUT:					return (PTBX_CHAR)"TIMEOUT";
		case TBX_RESULT_ID_MISMATCH:				return (PTBX_CHAR)"MISMATCH";
		case TBX_RESULT_ID_ACCESS_DENIED:			return (PTBX_CHAR)"ACCESS_DENIED";
		case TBX_RESULT_ID_OUT_OF_RANGE:			return (PTBX_CHAR)"OUT_OF_RANGE";
		case TBX_RESULT_ID_OUT_OF_MEMORY:			return (PTBX_CHAR)"OUT_OF_MEMORY";
		case TBX_RESULT_ID_OUT_OF_RESOURCE:			return (PTBX_CHAR)"OUT_OF_RESOURCE";
		case TBX_RESULT_ID_NULL_PARAM:				return (PTBX_CHAR)"NULL_PARAM";
		case TBX_RESULT_ID_INVALID_PARAM:			return (PTBX_CHAR)"INVALID_PARAM";
		case TBX_RESULT_ID_INVALID_CONTEXT:			return (PTBX_CHAR)"INVALID_CONTEXT";
		case TBX_RESULT_ID_INVALID_HANDLE:			return (PTBX_CHAR)"INVALID_HANDLE";
		case TBX_RESULT_ID_INVALID_DEVICE:			return (PTBX_CHAR)"INVALID_DEVICE";
		case TBX_RESULT_ID_INVALID_CONFIG:			return (PTBX_CHAR)"INVALID_CONFIG";
		case TBX_RESULT_ID_INVALID_STATE:			return (PTBX_CHAR)"INVALID_STATE";
		case TBX_RESULT_ID_INVALID_MESSAGE:			return (PTBX_CHAR)"INVALID_MESSAGE";
		case TBX_RESULT_ID_INVALID_OPERATION:		return (PTBX_CHAR)"INVALID_OPERATION";
		case TBX_RESULT_ID_NOT_ENOUGH_MEMORY:		return (PTBX_CHAR)"NOT_ENOUGH_MEMORY";
		case TBX_RESULT_ID_NOT_SUPPORTED:			return (PTBX_CHAR)"NOT_SUPPORTED";
		case TBX_RESULT_ID_NOT_INITIALIZED:			return (PTBX_CHAR)"NOT_INITIALIZED";
		case TBX_RESULT_ID_NOT_FOUND:				return (PTBX_CHAR)"NOT_FOUND";
		case TBX_RESULT_ID_NOT_READY:				return (PTBX_CHAR)"NOT_READY";
		case TBX_RESULT_ID_NOT_ACCESSIBLE:			return (PTBX_CHAR)"NOT_ACCESSIBLE";
		case TBX_RESULT_ID_NOT_RESPONDING:			return (PTBX_CHAR)"NOT_RESPONDING";
		case TBX_RESULT_ID_NOT_IMPLEMENTED:			return (PTBX_CHAR)"NOT_IMPLEMENTED";
		case TBX_RESULT_ID_BUFFER_TOO_SMALL:		return (PTBX_CHAR)"BUFFER_TOO_SMALL";
		case TBX_RESULT_ID_UNSUPPORTED_VERSION:		return (PTBX_CHAR)"UNSUPPORTED_VERSION";
		case TBX_RESULT_ID_NOT_SUPPORTED_BY_LICENSE:return (PTBX_CHAR)"NOT_SUPPORTED_BY_LICENSE";
		case TBX_RESULT_ID_OPERATION_INCOMPLETE:	return (PTBX_CHAR)"OPERATION_INCOMPLETE";
		case TBX_RESULT_ID_OPERATION_POSTPONED:		return (PTBX_CHAR)"OPERATION_POSTPONED";
	}

	return (PTBX_CHAR)"";
}


/*--------------------------------------------------------------------------------------------------------------------------------
 |  C++ support
 *------------------------------------------------------------------------------------------------------------------------------*/
#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif /* __TBX_RESULT_H__ */
