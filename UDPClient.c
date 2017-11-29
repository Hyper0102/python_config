#define WIN32_LEAN_AND_MEAN
#include <winsock2.h>
#include <stdio.h>
#include <conio.h>
#include "addr.h"
#define USEC (10L)
#define MSEC (1000*USEC)

int mul(int a, int b);

int main(int argc, char* argv[])
{	
	ADDR ext_addr;
	WSADATA wsaData;
   int iResult;
   SOCKET SendSocket = INVALID_SOCKET;
   struct sockaddr_in clientService; 
	struct sockaddr_in SendAddr;

   char *sendbuf = "this is a test";
	struct 
	{  LARGE_INTEGER time;
	   int num;
	   int marker;
	}  packet;

	char *a="he34";
	int sss=(int)a;
   int port;
	char*p=a;
	int res;
	int ip[4];
	unsigned long addr;
	printf(p);
	res=sscanf(argv[1], "%d.%d.%d.%d:%d", &ip[3], &ip[2], &ip[1], &ip[0],&port);
	if (res!=5)
	{  return 2;
	}
	addr=(ip[3]<<24) | (ip[2]<<16) | (ip[1]<<8) | ip[0];
	
	set_addr(addr,port,&ext_addr);
    
  
    //----------------------
    // Initialize Winsock
    iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
    if (iResult != NO_ERROR) {
      printf("WSAStartup failed: %d\n", iResult);
      return 1;
    }

    //----------------------
    // Create a SOCKET for connecting to server
    SendSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (SendSocket == INVALID_SOCKET) {
        printf("Error at socket(): %ld\n", WSAGetLastError() );
        WSACleanup();
        return 1;
    }

 //---------------------------------------------
    // Set up the SendAddr structure with the IP address of
    // the receiver (in this example case "192.168.1.1")
    // and the specified port number.
    SendAddr.sin_family = AF_INET;
    SendAddr.sin_port = htons(port);
   // SendAddr.sin_addr.s_addr = inet_addr("192.168.1.1");
	SendAddr.sin_addr.s_addr = htonl( ext_addr.ip) ;
    MessageBox(NULL,TEXT("test"),TEXT(""), MB_OK);
    wprintf(L"Sending a datagram to the receiver...\n");
	{  HANDLE hTimer=CreateWaitableTimer(NULL,FALSE,NULL);
		LARGE_INTEGER  dT;
	
	   LARGE_INTEGER Frequency;
		
		int i;
		LARGE_INTEGER start_time;
		LARGE_INTEGER cur_time;

       
	   cur_time.QuadPart=0;
		packet.marker=0x1234;
		QueryPerformanceFrequency(&Frequency);
      
		for (i = 0;  ; i++)
		{	dT.QuadPart=-1000*USEC;
      
			SetWaitableTimer(hTimer,&dT,0, NULL,NULL,0);
			WaitForSingleObject(hTimer, INFINITE);
//	        Sleep(10);
			packet.num = i;
			packet.time = cur_time;
         	
         QueryPerformanceCounter(&cur_time);
    		iResult = sendto(SendSocket, (char *) &packet, sizeof(packet), 0, (SOCKADDR *) & SendAddr, sizeof (SendAddr));
         if (1)
         {	
				cur_time.QuadPart *= 1000000;
				cur_time.QuadPart /= Frequency.QuadPart;
			}
			else
			{   start_time=cur_time;
			}
			
			if (iResult == SOCKET_ERROR) 
			{   wprintf(L"sendto failed with error: %d\n", WSAGetLastError());
				closesocket(SendSocket);
				WSACleanup();
				return 1;
			}
		}
	}

	printf("OK\n");
    _getch();
	return ;
}

