#ifndef __TBX_OS_H__
#define __TBX_OS_H__

/*--------------------------------------------------------------------------------------------------------------------------------
 |  C++ support
 *------------------------------------------------------------------------------------------------------------------------------*/
#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */


#include "tbx_ostypes.h"
//#include <winnt.h>
//#include "minwindef.h"
//#include <timeapi.h>
#include "tbx_result.h"
#include <windows.h>

typedef void *HANDLE;

typedef struct _TBX_WIN32_THREAD
{
	HANDLE			hThread;

	int				iRunning;
	HANDLE		hDoneEvent;

	unsigned __int32 (*pFct) (void*);
	void*			pCtx;

} TBX_WIN32_THREAD, *PTBX_WIN32_THREAD;

#define TBX_THREAD						TBX_WIN32_THREAD*
#define TBX_THREAD_INVALID				0


///

typedef RTL_CRITICAL_SECTION CRITICAL_SECTION;

 /********************************************
 * Event
 ********************************************/

#define TBX_EVENT_NO_WAIT				0
#define TBX_EVENT_WAIT_FOREVER			INFINITE
#define TBX_EVENT_INVALID				0
#define TBX_EVENT						HANDLE

#define TBX_EVENT_CREATE( _pEvent, _InitState ) \
	(NULL != (*(_pEvent) = CreateEvent(  \
    NULL,   		/* no security attributes */ \
    TBX_TRUE,		/* Manual reset */ \
    (_InitState), 	/* Initial state (TRUE -> signaled, FALSE -> nonsignaled) */ \
    NULL)))   		/* unnamed event */ \
	? TBX_RESULT_OK : TBX_RESULT_FAIL

#define TBX_EVENT_DESTROY( _pEvent ) \
	CloseHandle( *(_pEvent) )

#define TBX_EVENT_WAIT( _Event, _Tmo ) \
	((WAIT_OBJECT_0 == WaitForSingleObjectEx( (_Event), (_Tmo), TBX_FALSE )) ? TBX_RESULT_OK : TBX_RESULT_TIMEOUT)

#define TBX_EVENT_WAIT_ALERTABLE( _Event, _Tmo ) \
	((WAIT_OBJECT_0 == WaitForSingleObjectEx( (_Event), (TBX_UINT32)(_Tmo), TBX_TRUE )) ? TBX_RESULT_OK : TBX_RESULT_TIMEOUT)

#define TBX_EVENT_SET( _Event ) \
	(SetEvent( (_Event) ) ? TBX_RESULT_OK : TBX_RESULT_FAIL)

#define TBX_EVENT_RESET( _Event ) \
	(ResetEvent( (_Event) ) ? TBX_RESULT_OK : TBX_RESULT_FAIL)




/********************************************
 * Critical Section
 ********************************************/
typedef struct _WIN32_CRITICAL_SECTION
{
	CRITICAL_SECTION	CriticalSection;

} WIN32_CRITICAL_SECTION, *PWIN32_CRITICAL_SECTION;



#define TBX_CRITICAL_SECTION			WIN32_CRITICAL_SECTION*
#define TBX_MUTEX					TBX_CRITICAL_SECTION

/********************************************
 * Mutex
 ********************************************/


static __inline
TBX_RESULT TBX_CRITICAL_SECTION_CREATE(
  IN	TBX_CRITICAL_SECTION* 	pCriticalSection )
{
	*pCriticalSection = (WIN32_CRITICAL_SECTION*)calloc( sizeof(WIN32_CRITICAL_SECTION), 1 );

	if( *pCriticalSection )
	{
		InitializeCriticalSection( &(*pCriticalSection)->CriticalSection );
	}

	return (*pCriticalSection) ? TBX_RESULT_OK : TBX_RESULT_NOT_ENOUGH_MEMORY;
}

static __inline
TBX_RESULT TBX_CRITICAL_SECTION_DESTROY(
  IN	TBX_CRITICAL_SECTION* 	pCriticalSection )
{
	DeleteCriticalSection( &(*pCriticalSection)->CriticalSection );
	free( *pCriticalSection );

	return TBX_RESULT_OK;
}


/* extern TBX_UINT32 g_un32CritSectContextSwitch; -> Can be used to estimate nb of context switches due to critical sections */
#define TBX_CRITICAL_SECTION_ENTER( _CritSect )	\
	/*if( (_CritSect)->CriticalSection.OwningThread && (DWORD)(_CritSect)->CriticalSection.OwningThread != GetCurrentThreadId() )*/	\
			/*g_un32CritSectContextSwitch++;*/	\
	EnterCriticalSection( &_CritSect->CriticalSection )

	static __inline TBX_VOID TBX_CRITICAL_SECTION_LEAVE_WIN32_DEBUG
	(
		IN		TBX_CRITICAL_SECTION		in_CriticalSection,
		IN		const char*					in_pszFile,
		IN		int							in_iLine
	)
	{
		(void)in_pszFile;
		(void)in_iLine;
		if( in_CriticalSection->CriticalSection.OwningThread == 0 )
		{
			//assert( in_CriticalSection->CriticalSection.OwningThread == 0 );
			TBX_CRITICAL_SECTION_ENTER( in_CriticalSection );
		}
		LeaveCriticalSection( &in_CriticalSection->CriticalSection );
	}

	#define TBX_CRITICAL_SECTION_LEAVE( _CritSect )	TBX_CRITICAL_SECTION_LEAVE_WIN32_DEBUG( _CritSect, __FILE__, __LINE__ )


#define TBX_MUTEX_CREATE( _pMutex ) TBX_CRITICAL_SECTION_CREATE( _pMutex )

#define TBX_MUTEX_DESTROY( _pMutex ) TBX_CRITICAL_SECTION_DESTROY(_pMutex )

#define TBX_MUTEX_GET( _Mutex ) TBX_CRITICAL_SECTION_ENTER( _Mutex )

#define TBX_MUTEX_GIV( _Mutex ) TBX_CRITICAL_SECTION_LEAVE( _Mutex )




#define CLOCKS_PER_SEC  1000
#define TBX_TICKS_PER_SEC				((TBX_TICK) CLOCKS_PER_SEC )
#define TBX_MSEC_PER_TICKS				(1000/TBX_TICKS_PER_SEC)




/********************************************
 * Time & Clock - Time & Clock
 ********************************************/
#define TBX_YIELD_CPU
#define TBX_SLEEP_MS( _ms )				Sleep( (_ms) )
#define TBX_TICKS_PER_SEC				((TBX_TICK) CLOCKS_PER_SEC )
#define TBX_GET_TICK()					((TBX_TICK) GetTickCount() )

extern TBX_VOID GetTickCount64MsDev98( PTBX_UINT64 io_pun64Tick );
static __inline TBX_UINT64 TBX_GET_TICK_64()
{
	static TBX_UINT64 un64Tick = 0;
	GetTickCount64MsDev98( &un64Tick );
	return un64Tick;
}

#define TBX_GET_TICK_PRECISE()			((TBX_TICK) timeGetTime() )
#define TBX_MSEC_PER_TICKS				(1000/TBX_TICKS_PER_SEC)
#define TBX_TICK						TBX_UINT32
#define TBX_LOCALTIME( _pTimet, _pStructTm ) \
{ \
	time_t	fixedTime = (*_pTimet & 0x7FFFFFFF); \
	*(_pStructTm) = *(localtime( &fixedTime )); \
}
#define TBX_GMTIME( _pTimet, _pStructTm ) \
	*(_pStructTm) = *(gmtime( _pTimet ))

#define TBX_GET_TIMESTAMP_MS			(TBX_GET_TICK() * TBX_MSEC_PER_TICKS)

/*

extern TBX_RESULT TBX_THREAD_CREATE_ACTUAL
(
  OUT	TBX_THREAD* 			pThread,
  IN	TBX_RESULT (*pFct) (void*),
  IN	PTBX_VOID 				pCtx,
  IN	char*					in_pszFile,
  IN	int						in_iLine
);

static __inline
TBX_RESULT TBX_THREAD_CREATE_ACTUAL_HPRIO(
  OUT	TBX_THREAD* 	phThread,
  IN	TBX_RESULT (*pFct) (void*),
  IN	PTBX_VOID 		pCtx )
{
	TBX_RESULT	Result;

	Result = TBX_THREAD_CREATE_ACTUAL( phThread, pFct, pCtx, __FILE__, __LINE__ );
	if( TBX_RESULT_SUCCESS( Result ) )
	{
		SetThreadPriority( (*phThread)->hThread, THREAD_PRIORITY_HIGHEST );
	}

	return Result;
}

static __inline
TBX_RESULT TBX_THREAD_CREATE_ACTUAL_VHPRIO(
  OUT	TBX_THREAD* 	phThread,
  IN	TBX_RESULT (*pFct) (void*),
  IN	PTBX_VOID 		pCtx )
{
	TBX_RESULT	Result;

	Result = TBX_THREAD_CREATE_ACTUAL( phThread, pFct, pCtx, __FILE__, __LINE__ );
	if( TBX_RESULT_SUCCESS( Result ) )
	{
		SetThreadPriority( (*phThread)->hThread, THREAD_PRIORITY_TIME_CRITICAL );
	}

	return Result;
}

static __inline
TBX_RESULT TBX_THREAD_CREATE_ACTUAL_LPRIO(
  OUT	TBX_THREAD* 	phThread,
  IN	TBX_RESULT (*pFct) (void*),
  IN	PTBX_VOID 		pCtx )
{
	TBX_RESULT	Result;

	Result = TBX_THREAD_CREATE_ACTUAL( phThread, pFct, pCtx, __FILE__, __LINE__ );
	if( TBX_RESULT_SUCCESS( Result ) )
	{
		SetThreadPriority( (*phThread)->hThread, THREAD_PRIORITY_BELOW_NORMAL );
	}

	return Result;
}

static __inline
TBX_RESULT TBX_THREAD_DESTROY_ACTUAL( IN TBX_THREAD* in_phThread )
{
	HANDLE hThread = (*in_phThread)->hThread;

	if( (*in_phThread)->hDoneEvent )
	{
		TBX_EVENT_WAIT( (*in_phThread)->hDoneEvent, TBX_EVENT_WAIT_FOREVER );
		TBX_EVENT_DESTROY( &(*in_phThread)->hDoneEvent );
	}

	free( (*in_phThread) );
	(*in_phThread) = NULL;

	return CloseHandle( hThread ) ? TBX_RESULT_OK : TBX_RESULT_FAIL;
}



#define TBX_THREAD_CREATE( a, b, c )			TBX_THREAD_CREATE_ACTUAL( a, b, c, __FILE__, __LINE__ )
#define TBX_THREAD_CREATE_VHPRIO( a, b, c )		TBX_THREAD_CREATE_ACTUAL_VHPRIO( a, b, c )
#define TBX_THREAD_CREATE_HPRIO( a, b, c )		TBX_THREAD_CREATE_ACTUAL_HPRIO( a, b, c )
#define TBX_THREAD_CREATE_LPRIO( a, b, c )		TBX_THREAD_CREATE_ACTUAL_LPRIO( a, b, c )
#define TBX_THREAD_DESTROY						TBX_THREAD_DESTROY_ACTUAL

*/

/*--------------------------------------------------------------------------------------------------------------------------------
 |  C++ support
 *------------------------------------------------------------------------------------------------------------------------------*/
#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif /* __TBX_OS_H__ */

