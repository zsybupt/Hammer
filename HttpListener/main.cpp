// HttpTest.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "HttpListener.h"
#pragma comment(lib, "httpapi.lib")


//
// Prototypes.
//


DWORD HandleRequest(PHTTP_LISTENER, PHTTP_REQUEST);

void DisplayWin32Error(DWORD NTStatusMessage);

int _tmain(int argc, _TCHAR* argv[])
{
	char input[200];
    if (argc < 2)
    {
        wprintf(L"%ws: <Url1> [Url2] ... \n", argv[0]);
        return -1;
    }

	PHTTP_LISTENER listener;
	DWORD result = CreateHttpListener(&listener);
	if(result != NO_ERROR)
	{
		DisplayWin32Error(result);
		return result;	
	}

	// Setup the callback to handle the request.
	listener->OnRequestCompletionRoutine = HandleRequest;

	result = StartHttpListener(listener, argc, argv);
	if(result != NO_ERROR)
	{
		DisplayWin32Error(result);
		return result;	
	}
	else
	{
		wprintf(L"Listener Started ...\n\t");

		while(true)
		{
			result = wscanf(L"%1s", input, 1);
			if(strcmp(input, "q") == 0 )
			{
				DisposeHttpListener(listener);
				result = 0;
				break;
			}
		}
	}
    return result;
}


void DisplayWin32Error(DWORD NTStatusMessage)
{
   LPVOID lpMessageBuffer;
   HMODULE Hand = LoadLibrary(L"NTDLL.DLL");
   
   FormatMessage(
       FORMAT_MESSAGE_ALLOCATE_BUFFER | 
       FORMAT_MESSAGE_FROM_SYSTEM | 
       FORMAT_MESSAGE_FROM_HMODULE,
       Hand, 
       NTStatusMessage,  
       MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
       (LPTSTR) &lpMessageBuffer,  
       0,  
       NULL );

   
   wprintf(L"%s",lpMessageBuffer);
   
   // Free the buffer allocated by the system.
   LocalFree( lpMessageBuffer ); 
   FreeLibrary(Hand);
}



// 
// Test data 
// 
void gen_random(char *s, const int len) {
    static const char alphanum[] =
        "0123456789"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz";

    for (int i = 0; i < len; ++i) {
        s[i] = alphanum[rand() % (sizeof(alphanum) - 1)];
    }

    s[len] = 0;
}

char* global_responseBuffer;

DWORD 
HandleRequest(
	PHTTP_LISTENER pListener,
	PHTTP_REQUEST pRequest
)
{
	const int messageSize = 500;
	if(global_responseBuffer == NULL)
	{
		global_responseBuffer = new char[messageSize +1];
		gen_random(global_responseBuffer, messageSize);
	}

	//We only handle get for now...
	DWORD result = SendHttpResponse(
							pListener,
							pRequest, 
							200,
							"OK",
							global_responseBuffer
							);

	if(result != NO_ERROR)
	{
		DisplayWin32Error(result);
	}

	return result;
}