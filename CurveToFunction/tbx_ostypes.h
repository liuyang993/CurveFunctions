#ifndef __TBX_OSTYPES_H__
#define __TBX_OSTYPES_H__

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */


#define TBX_VOID			void
#define PTBX_VOID			void*

/* Define TBX_CHAR data type */
typedef char				TBX_CHAR, *PTBX_CHAR;


/* Define TBX_BOOL data type */
typedef	unsigned __int32	TBX_BOOL, *PTBX_BOOL;


/* Define TBX_INT data type */
typedef signed int			TBX_INT, *PTBX_INT;


/* Define TBX_UINT32 data type */
typedef unsigned __int32	TBX_UINT32, *PTBX_UINT32;

/* Define TBX_UINT8 data type */
typedef unsigned __int8		TBX_UINT8, *PTBX_UINT8;

/* Define TBX_UINT64 data type */
typedef unsigned __int64	TBX_UINT64, *PTBX_UINT64;

/* Define TBX_INT32 data type */
typedef signed __int32		TBX_INT32, *PTBX_INT32;


typedef unsigned long       DWORD;

//typedef struct _FILETIME {
//    DWORD dwLowDateTime;
//    DWORD dwHighDateTime;
//} FILETIME, *PFILETIME, *LPFILETIME;
//#define _FILETIME_


/* Define TBX_UINT16 data type */
typedef unsigned __int16	TBX_UINT16, *PTBX_UINT16;


#define TBX_API_MAX_EXTRA_GW_PORTS			16




#define TBX_VOID			void
#define PTBX_VOID			void*

#define IN
#define OUT
#define IN_OUT

typedef TBX_UINT32 *		TBX_HANDLE;

typedef TBX_HANDLE	TBX_LIB_HANDLE, *PTBX_LIB_HANDLE;


/* Define TBX_TRUE constant */
#define TBX_TRUE		(1 == 1)


/* Define TBX_FALSE	constant */
#define TBX_FALSE		(0 == 1)


/* Define TBX_BOOLEAN data type */
typedef	unsigned char		TBX_BOOLEAN, *PTBX_BOOLEAN;

typedef unsigned __int32	TBX_ADDR, *PTBX_ADDR;


/*--------------------------------------------------------------------------------------------------------------------------------
 |  C++ support
 *------------------------------------------------------------------------------------------------------------------------------*/
#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif /* __TBX_OSTYPES_H__ */

