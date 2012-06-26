#include "stdafx.h"
#include <http.h>

typedef struct _HttpListenerOverlapped HTTP_LISTENER_OVERLAPPED;
typedef struct _HttpListenerOverlapped* PHTTP_LISTENER_OVERLAPPED;
typedef struct _HttpListener HttpListener;
typedef HttpListener *PHTTP_LISTENER;


typedef void (*HttpIoCompletionRoutine)(PHTTP_LISTENER_OVERLAPPED);
typedef DWORD (*HttpListenerOnRequest)(PHTTP_LISTENER, PHTTP_REQUEST);

#define HTTP_LISTENER_MAX_PENDING_RECEIVES 10

enum HTTP_LISTENER_STATE
{		
	HTTP_LISTENER_STATE_STARTED = 1,
	HTTP_LISTENER_STATE_DISPOSING = 2,
	HTTP_LISTENER_STATE_STOPPED = 3,
	HTTP_LISTENER_STATE_FAULTED = 4,
	HTTP_LISTENER_STATE_REQUEST = 5,
	HTTP_LISTENER_STATE_RESPONSE = 6,

};

typedef struct _HttpListener
{
	 _TCHAR** 				urls;
	int						urlsCount;
	HANDLE					hRequestQueue;
	HTTP_URL_GROUP_ID		urlGroupId;
	HTTP_SERVER_SESSION_ID	sessionId;
	PTP_IO					pthreadPoolIO;	
	DWORD					errorCode;
	ULONG					ulActiveRequests;
	ULONG					ulPendingReceives;
	ULONG					state; //HTTP_LISTENER_STATE	
	HttpListenerOnRequest	OnRequestCompletionRoutine;
	PCRITICAL_SECTION		m_criticalSection;
} HttpListener;


typedef struct _HttpListenerOverlapped: public OVERLAPPED
{
	HttpIoCompletionRoutine completionRoutine;
	PHTTP_LISTENER listener;	
		
	PHTTP_REQUEST	   pRequest;    
    HTTP_REQUEST_ID    requestId;
    DWORD              bytesRead;    
	ULONG			   errorCode;
    ULONG              requestSize;
	ULONG			   isCompleted;
	ULONG			   operationState;
	DWORD			   responseErrorCode;				
} HTTP_LISTENER_OVERLAPPED;

DWORD
CreateHttpListener(
	PHTTP_LISTENER* listener
);

DWORD
StartHttpListener(
	PHTTP_LISTENER listener, 
	int argc, 
	_TCHAR* urls[]
);

void DisposeHttpListener(
	PHTTP_LISTENER listener
);

DWORD
SendHttpResponse(
    IN PHTTP_LISTENER listener,
    IN PHTTP_REQUEST  pRequest,
    IN USHORT         StatusCode,
    IN PSTR           pReason,
    IN PSTR           pEntity
);