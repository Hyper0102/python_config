#ifndef UNICODE
#define UNICODE
#endif
#define WIN32_LEAN_AND_MEAN

#include <winsock2.h>
#include <Ws2tcpip.h>
#include <stdio.h>
#include <string>
#include "TaskPool.h"
using namespace std;
#pragma comment(lib, "Ws2_32.lib")
#define CR "\r" 
#define LF "\n" 
char *send_data;
int upload_size = 0;

BOOL OpenURL(string url, string &answer)

{ 
   int iResult;

   string path=string(url.c_str()+strlen("http://"));
   string::size_type pos=path.find_first_of("/");
   
   string page, host, addr;
   unsigned short port=80;

   if (pos==string::npos)
   {  page="/";
      host=path;
   }
   else
   {  page=path.substr(pos, path.size()-pos);
      host=path.substr(0,pos);
   }

   string::size_type pos2=host.find_first_of(":");
   if (pos2==string::npos)
   {  addr=host;
   }
   else
   {  addr=host.substr(0,pos2);
      int val=80;
      if (sscanf(host.c_str()+pos2+1,"%d",&val)==1)
      {  port=val; 
      }
   }

   SOCKET ConnectSocket = INVALID_SOCKET;
   struct sockaddr_in clientService; 

   
#if 0
   {string  sendbuf = string("")
                     +"POST "+page+" HTTP/1.1"+CR+LF
                     +"Host: "+host+CR+LF
                     +"User-Agent: Undefined"+CR+LF
                     +"Accept: */*"+CR+LF
                     +"Accept-Encoding: gzip, deflate"+CR+LF
                     +"Connection: keep-alive"+CR+LF
                     +"Content-Length: 0"+CR+LF
                     +CR+LF;}
  #endif  
    string  sendbuf = string("")
                     +"POST "+page+" HTTP/1.1"+CR+LF
                     +"Host: "+host+CR+LF
                     +"User-Agent: Undefined"+CR+LF
                     +"Accept: */*"+CR+LF
                     +"Accept-Encoding: gzip, deflate"+CR+LF
                     +"Connection: keep-alive"+CR+LF
                     +"Content-Length: " + to_string(upload_size)+CR+LF
                     +CR+LF;
    

     char recvbuf[65536] = "";

   int recvbuflen = sizeof(recvbuf);
   //----------------------

   //----------------------
   // Create a SOCKET for connecting to server
   ConnectSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
   if (ConnectSocket == INVALID_SOCKET) 
   {  return false;
   }

   //----------------------
   // The sockaddr_in structure specifies the address family,
   // IP address, and port of the server to be connected to.
   clientService.sin_family = AF_INET;
   clientService.sin_addr.s_addr = inet_addr( addr.c_str() );
   clientService.sin_port = htons( port );

   //----------------------
   // Connect to server.
   iResult = connect( ConnectSocket, (SOCKADDR*) &clientService, sizeof(clientService) );
   if (iResult == SOCKET_ERROR) 
   {  closesocket(ConnectSocket);
      return false;
   }

   //----------------------
   // Send an initial buffer
   iResult = send( ConnectSocket, sendbuf.c_str(), (int)sendbuf.size(), 0 );
   if (iResult == SOCKET_ERROR) 
   {  closesocket(ConnectSocket);
      return false;
   }

   if(upload_size > 0)
   {
      iResult = send( ConnectSocket, send_data, upload_size, 0 );
      if (iResult == SOCKET_ERROR) 
      {  closesocket(ConnectSocket);
         return false;
      }
   }

   // shutdown the connection since no more data will be sent
   iResult = shutdown(ConnectSocket, SD_SEND);
   if (iResult == SOCKET_ERROR) 
   {  closesocket(ConnectSocket);
      return false;
   }

   // Receive until the peer closes the connection
   do 
   {  iResult = recv(ConnectSocket, recvbuf, recvbuflen, 0);
      if ( iResult > 0 )
      {  // wprintf(L"Bytes received: %d\n", iResult);
         answer+=string(recvbuf, iResult);
      }
      else 
      if ( iResult == 0 )
      {  // wprintf(L"Connection closed\n");
      }
      else
      {  // wprintf(L"recv failed with error: %d\n", WSAGetLastError());
      }

   } while( iResult > 0 );

   // close the socket
   iResult = closesocket(ConnectSocket);
   if (iResult == SOCKET_ERROR) 
   {  return false;
   }
 
   return true;
}


typedef struct
{  string url;
   string answer;
   unsigned long long all_size;
   int upload_size;
}  TASK_PARAM;


void func(void *_p)
{  TASK_PARAM *p=(TASK_PARAM *)_p;
   if(OpenURL(p->url, p->answer))
   {  p->all_size+=p->answer.size();
   }
}

TASK_PARAM Param[10];
TASKPOOL_FUNC<func,10> TaskPool;

int main(int argc, char *argv[]) 
{  int iResult;
   // Initialize Winsock
   WSADATA wsaData;
   iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
   if (iResult != NO_ERROR) 
   {  return false;
   }

   string answer;
	
   LARGE_INTEGER Frequency, start_time, end_time;
	QueryPerformanceFrequency(&Frequency);

   if(argc > 2)
   {
      sscanf(argv[2],"%d",&upload_size);
      if(upload_size > 0)
      {
         send_data =(char*)malloc(sizeof(upload_size));
      }
   }
    

   for(int ntask=0; ntask<10; ntask++)
   {  Param[ntask].url=argv[1];
      Param[ntask].all_size=0;
      Param[ntask].upload_size = upload_size;
   }

	QueryPerformanceCounter(&start_time);  

   for(int i=0; i<100; i++)
   {  int ntask;
      TaskPool.WaitForAnyReady(&ntask,INFINITE);
      Param[ntask].answer.clear();
      TaskPool.Start(ntask, &Param[ntask]);
   }
   TaskPool.WaitForAllComplete(INFINITE);

   
	QueryPerformanceCounter(&end_time);  
   unsigned long long  all_size=0;
   for(int ntask=0; ntask<10; ntask++)
   {  all_size+=Param[ntask].all_size;
   }

   double dt=end_time.QuadPart-start_time.QuadPart;
   dt/=Frequency.QuadPart;
      
   double BitRate;
   BitRate=all_size*8/ dt;
   printf("size=%llu,  dt=%f sec, BitRate=%f mbit/sec\n", all_size, dt, BitRate*0.000001);

   WSACleanup();
}





