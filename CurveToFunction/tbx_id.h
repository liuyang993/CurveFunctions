/*--------------------------------------------------------------------------------------------------------------------------------
 |
 |	Project:    	TBX products family
 |
 |	Filename:   	tbx_id.h
 |
 |	Copyright:  	TelcoBridges 2002-2003, All Rights Reserved
 |
 |	Description:	This header file contains definitions concerning component class ID.
 |
 |					The class ID is defined as a 14 bits value.
 |					It is used in errors and messages 32 bits code
 |
 |					|<------ 32 bits Code ----->|
 |					| |<- 14 bits ->|<--- 16 bits --->|
 |					+-+-------------+-----------------+
 |					| |  Class ID   | Specific Number |
 |					+-+-------------+-----------------+
 |					 |
 |					 +--->	2 reserved bits
 |
 |					All classes must be reserved and declared here.
 |
 |	Notes:			Tabs = 4
 |
 *--------------------------------------------------------------------------------------------------------------------------------
 |
 |	Revision:   	$Id: tbx_id.h 111826 2012-06-12 13:20:28Z rblanchet $
 |
 *------------------------------------------------------------------------------------------------------------------------------*/

#include <string.h>

/*--------------------------------------------------------------------------------------------------------------------------------
 |  Define header file
 *------------------------------------------------------------------------------------------------------------------------------*/
#ifndef __TBX_ID_H__
#define __TBX_ID_H__


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

/* Definition of error code class ranges */
typedef enum _TBX_ID_CLASS_ALLOCATION
{
	/* Common classes */
	TBX_ID_CLASS_ALLOCATION_COMMON_BEGIN			= 0x0000,
	TBX_ID_CLASS_ALLOCATION_COMMON_END				= 0x01FF,

	/* TBX classes */
	TBX_ID_CLASS_ALLOCATION_TBX_BEGIN				= 0x0200,
	TBX_ID_CLASS_ALLOCATION_TBX_NEW_BEGIN			= 0x0300,	/* New range for TBX with specific support by hostlib to send messages locally */
	TBX_ID_CLASS_ALLOCATION_TBX_END					= 0x03FF,

	/* TB640 classes */
	TBX_ID_CLASS_ALLOCATION_TB640_BEGIN				= 0x0400,
	TBX_ID_CLASS_ALLOCATION_TB640_END				= 0x05FF,

	/* User Define classes */
	TBX_ID_CLASS_ALLOCATION_USER_BEGIN				= 0x0600,
	TBX_ID_CLASS_ALLOCATION_USER_END				= 0x07FF,

	/* TBX applications classes */
	TBX_ID_CLASS_ALLOCATION_APPS_BEGIN				= 0x0800,
	TBX_ID_CLASS_ALLOCATION_APPS_END				= 0x09FF,

	/* CMC classes */
	TBX_ID_CLASS_ALLOCATION_CMC_BEGIN				= 0x0A00,
	TBX_ID_CLASS_ALLOCATION_CMC_END					= 0x0AFF,

	/* CAF classes */
	TBX_ID_CLASS_ALLOCATION_CAF_BEGIN				= 0x0B00,
	TBX_ID_CLASS_ALLOCATION_CAF_END					= 0x0BFF

} TBX_ID_CLASS_ALLOCATION, *PTBX_ID_CLASS_ALLOCATION;


/* Class of TBX codes */
#define TBX_ID_CLASS_TBX_COMMON				(TBX_ID_CLASS_ALLOCATION_TBX_BEGIN + 0x00)
#define TBX_ID_CLASS_TBX_API				(TBX_ID_CLASS_ALLOCATION_TBX_BEGIN + 0x01)
#define TBX_ID_CLASS_TBX_DB					(TBX_ID_CLASS_ALLOCATION_TBX_BEGIN + 0x02)
#define TBX_ID_CLASS_TBX_UTILLIB			(TBX_ID_CLASS_ALLOCATION_TBX_BEGIN + 0x03)
#define TBX_ID_CLASS_TBX_LOGLIB				(TBX_ID_CLASS_ALLOCATION_TBX_BEGIN + 0x04)
#define TBX_ID_CLASS_TBX_LOGMGR				(TBX_ID_CLASS_ALLOCATION_TBX_BEGIN + 0x05)
#define TBX_ID_CLASS_TBX_FEATLIB			(TBX_ID_CLASS_ALLOCATION_TBX_BEGIN + 0x06)
#define TBX_ID_CLASS_TBX_FEATMGR			(TBX_ID_CLASS_ALLOCATION_TBX_BEGIN + 0x07)
#define TBX_ID_CLASS_TBX_PROCMGR			(TBX_ID_CLASS_ALLOCATION_TBX_BEGIN + 0x08)
#define TBX_ID_CLASS_TBX_CARDAGENT			(TBX_ID_CLASS_ALLOCATION_TBX_BEGIN + 0x09)
#define TBX_ID_CLASS_TBX_CARDAGENTLIB		(TBX_ID_CLASS_ALLOCATION_TBX_BEGIN + 0x0A)
#define TBX_ID_CLASS_TBX_RTP				(TBX_ID_CLASS_ALLOCATION_TBX_BEGIN + 0x0B)
#define TBX_ID_CLASS_TBX_DB2				(TBX_ID_CLASS_ALLOCATION_TBX_BEGIN + 0x0C)
#define TBX_ID_CLASS_TBX_GW					(TBX_ID_CLASS_ALLOCATION_TBX_BEGIN + 0x0D)
#define TBX_ID_CLASS_TBX_PROFILING			(TBX_ID_CLASS_ALLOCATION_TBX_BEGIN + 0x0E)
#define TBX_ID_CLASS_STREAM_SERVER			(TBX_ID_CLASS_ALLOCATION_TBX_BEGIN + 0x0F)
#define TBX_ID_CLASS_TBX_SNMPMGR			(TBX_ID_CLASS_ALLOCATION_TBX_BEGIN + 0x10)
#define TBX_ID_CLASS_TBX_SNMPMIB2_MGR		(TBX_ID_CLASS_ALLOCATION_TBX_BEGIN + 0x11)
#define TBX_ID_CLASS_TBX_SNMPMGRLIB			(TBX_ID_CLASS_ALLOCATION_TBX_BEGIN + 0x12)
#define TBX_ID_CLASS_TBX_AUTOTESTLIB		(TBX_ID_CLASS_ALLOCATION_TBX_BEGIN + 0x13)
#define TBX_ID_CLASS_TBX_AUTOTESTLIB_RESULT	(TBX_ID_CLASS_ALLOCATION_TBX_BEGIN + 0x14)
#define TBX_ID_CLASS_TBX_APPS_MEDIA_SERVER	(TBX_ID_CLASS_ALLOCATION_TBX_BEGIN + 0x15)
#define TBX_ID_CLASS_TBX_HTTP_SERVER		(TBX_ID_CLASS_ALLOCATION_TBX_BEGIN + 0x16)
#define TBX_ID_CLASS_TBX_FIREWALL_LIB		(TBX_ID_CLASS_ALLOCATION_TBX_BEGIN + 0x17)
#define TBX_ID_CLASS_TBX_SPILIB				(TBX_ID_CLASS_ALLOCATION_TBX_BEGIN + 0x18)
#define TBX_ID_CLASS_TBX_PWE				(TBX_ID_CLASS_ALLOCATION_TBX_BEGIN + 0x19)
#define TBX_ID_CLASS_TBX_MPB				(TBX_ID_CLASS_ALLOCATION_TBX_BEGIN + 0x20)
#define TBX_ID_CLASS_TBX_NP1_SWITCH			(TBX_ID_CLASS_ALLOCATION_TBX_BEGIN + 0x21)
#define TBX_ID_CLASS_TBX_RFS				(TBX_ID_CLASS_ALLOCATION_TBX_BEGIN + 0x22)



#define TBX_ID_CLASS_TBX_SERVICES_LIB		(TBX_ID_CLASS_ALLOCATION_TBX_NEW_BEGIN + 0x01)

/*--------------------------------------------------------------------------------------------------------------------------------
 |  Types
 *------------------------------------------------------------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------------------------------------------------------------
 |  Global variables
 *------------------------------------------------------------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------------------------------------------------------------
 |  Macros
 *------------------------------------------------------------------------------------------------------------------------------*/

///*
// * Converts a class Id in the range
// * TBX_ID_CLASS_ALLOCATION_TBX_BEGIN - TBX_ID_CLASS_ALLOCATION_TBX_END
// * to a readable string
//*/
//static __inline PTBX_CHAR TBX_ID_CLASS_TBX_TO_STRING( TBX_UINT32 in_un32MsgId )
//{
//	unsigned long	un32ClassId = (in_un32MsgId & 0x3FFF0000) >> 16;
//
//	switch( un32ClassId )
//	{
//		case TBX_ID_CLASS_TBX_COMMON:				return (PTBX_CHAR)"TBX_COMMON";
//		case TBX_ID_CLASS_TBX_API:					return (PTBX_CHAR)"TBX_API";
//		case TBX_ID_CLASS_TBX_DB:					return (PTBX_CHAR)"TBX_DB";
//		case TBX_ID_CLASS_TBX_UTILLIB:				return (PTBX_CHAR)"TBX_UTILLIB";
//		case TBX_ID_CLASS_TBX_LOGLIB:				return (PTBX_CHAR)"TBX_LOGLIB";
//		case TBX_ID_CLASS_TBX_LOGMGR:				return (PTBX_CHAR)"TBX_LOGMGR";
//		case TBX_ID_CLASS_TBX_FEATLIB:				return (PTBX_CHAR)"TBX_FEATLIB";
//		case TBX_ID_CLASS_TBX_FEATMGR:				return (PTBX_CHAR)"TBX_FEATMGR";
//		case TBX_ID_CLASS_TBX_PROCMGR:				return (PTBX_CHAR)"TBX_PROCMGR";
//		case TBX_ID_CLASS_TBX_CARDAGENT:			return (PTBX_CHAR)"TBX_CARDAGENT";
//		case TBX_ID_CLASS_TBX_CARDAGENTLIB:			return (PTBX_CHAR)"TBX_CARDAGENTLIB";
//		case TBX_ID_CLASS_TBX_RTP:					return (PTBX_CHAR)"TBX_RTP";
//		case TBX_ID_CLASS_TBX_DB2:					return (PTBX_CHAR)"TBX_DB2";
//		case TBX_ID_CLASS_TBX_GW:					return (PTBX_CHAR)"TBX_GW";
//		case TBX_ID_CLASS_TBX_PROFILING:			return (PTBX_CHAR)"TBX_PROFILING";
//		case TBX_ID_CLASS_STREAM_SERVER:			return (PTBX_CHAR)"STREAM_SERVER";
//		case TBX_ID_CLASS_TBX_SNMPMGR:				return (PTBX_CHAR)"TBX_SNMPMGR";
//		case TBX_ID_CLASS_TBX_SNMPMIB2_MGR:			return (PTBX_CHAR)"TBX_SNMPMIB2_MGR";
//		case TBX_ID_CLASS_TBX_SNMPMGRLIB:			return (PTBX_CHAR)"TBX_SNMPMGRLIB";
//		case TBX_ID_CLASS_TBX_AUTOTESTLIB:			return (PTBX_CHAR)"TBX_AUTOTESTLIB";
//		case TBX_ID_CLASS_TBX_AUTOTESTLIB_RESULT:	return (PTBX_CHAR)"TBX_AUTOTESTLIB_RESULT";
//		case TBX_ID_CLASS_TBX_APPS_MEDIA_SERVER:	return (PTBX_CHAR)"TBX_APPS_MEDIA_SERVER";
//		case TBX_ID_CLASS_TBX_HTTP_SERVER:			return (PTBX_CHAR)"TBX_HTTP_SERVER";
//	}
//
//	return (PTBX_CHAR)"";
//}
//
//#define TBX_ID_CLASS_STRING_TO_CLASS_TBX_CASE( __MsgGroup )	\
//	if( strcmp( in_pszClassId, #__MsgGroup ) == 0 )	\
//	{	\
//		return __MsgGroup;	\
//	}
//
///*
// * Converts a string to the corresponding class Id, or 0 if not found
//*/
//static __inline TBX_UINT32 TBX_ID_CLASS_STRING_TO_CLASS_TBX( const TBX_CHAR* in_pszClassId )
//{
//	TBX_ID_CLASS_STRING_TO_CLASS_TBX_CASE( TBX_ID_CLASS_TBX_COMMON );
//	TBX_ID_CLASS_STRING_TO_CLASS_TBX_CASE( TBX_ID_CLASS_TBX_API );
//	TBX_ID_CLASS_STRING_TO_CLASS_TBX_CASE( TBX_ID_CLASS_TBX_DB );
//	TBX_ID_CLASS_STRING_TO_CLASS_TBX_CASE( TBX_ID_CLASS_TBX_UTILLIB );
//	TBX_ID_CLASS_STRING_TO_CLASS_TBX_CASE( TBX_ID_CLASS_TBX_LOGLIB );
//	TBX_ID_CLASS_STRING_TO_CLASS_TBX_CASE( TBX_ID_CLASS_TBX_LOGMGR );
//	TBX_ID_CLASS_STRING_TO_CLASS_TBX_CASE( TBX_ID_CLASS_TBX_FEATLIB );
//	TBX_ID_CLASS_STRING_TO_CLASS_TBX_CASE( TBX_ID_CLASS_TBX_FEATMGR );
//	TBX_ID_CLASS_STRING_TO_CLASS_TBX_CASE( TBX_ID_CLASS_TBX_PROCMGR );
//	TBX_ID_CLASS_STRING_TO_CLASS_TBX_CASE( TBX_ID_CLASS_TBX_CARDAGENT );
//	TBX_ID_CLASS_STRING_TO_CLASS_TBX_CASE( TBX_ID_CLASS_TBX_CARDAGENTLIB );
//	TBX_ID_CLASS_STRING_TO_CLASS_TBX_CASE( TBX_ID_CLASS_TBX_RTP );
//	TBX_ID_CLASS_STRING_TO_CLASS_TBX_CASE( TBX_ID_CLASS_TBX_DB2 );
//	TBX_ID_CLASS_STRING_TO_CLASS_TBX_CASE( TBX_ID_CLASS_TBX_GW );
//	TBX_ID_CLASS_STRING_TO_CLASS_TBX_CASE( TBX_ID_CLASS_TBX_PROFILING );
//	TBX_ID_CLASS_STRING_TO_CLASS_TBX_CASE( TBX_ID_CLASS_STREAM_SERVER );
//	TBX_ID_CLASS_STRING_TO_CLASS_TBX_CASE( TBX_ID_CLASS_TBX_SNMPMGR );
//	TBX_ID_CLASS_STRING_TO_CLASS_TBX_CASE( TBX_ID_CLASS_TBX_SNMPMIB2_MGR );
//	TBX_ID_CLASS_STRING_TO_CLASS_TBX_CASE( TBX_ID_CLASS_TBX_SNMPMGRLIB );
//	TBX_ID_CLASS_STRING_TO_CLASS_TBX_CASE( TBX_ID_CLASS_TBX_AUTOTESTLIB );
//	TBX_ID_CLASS_STRING_TO_CLASS_TBX_CASE( TBX_ID_CLASS_TBX_AUTOTESTLIB_RESULT );
//	TBX_ID_CLASS_STRING_TO_CLASS_TBX_CASE( TBX_ID_CLASS_TBX_APPS_MEDIA_SERVER );
//	TBX_ID_CLASS_STRING_TO_CLASS_TBX_CASE( TBX_ID_CLASS_TBX_HTTP_SERVER );
//	TBX_ID_CLASS_STRING_TO_CLASS_TBX_CASE( TBX_ID_CLASS_TBX_FIREWALL_LIB );
//	TBX_ID_CLASS_STRING_TO_CLASS_TBX_CASE( TBX_ID_CLASS_TBX_SPILIB );
//
//	return 0;
//}


/*--------------------------------------------------------------------------------------------------------------------------------
 |  Function Prototypes
 *------------------------------------------------------------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------------------------------------------------------------
 |  C++ support
 *------------------------------------------------------------------------------------------------------------------------------*/
#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif /* __TBX_ID_H__ */
