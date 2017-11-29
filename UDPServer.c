#include <stdio.h>
#include <winsock2.h>
#include <process.h>

int Port=3333;

typedef struct 
{	LARGE_INTEGER time_send;
	int num;
	int marker;
}  PACKET;

typedef struct 
{  PACKET  packet;
   LARGE_INTEGER time_rcv;
  int filled;
} PACKET_INFO;

#define N_PACKETS 10000 

typedef struct 
{   LARGE_INTEGER  dt_min, dt_max;
	int n_packets, n_pairs;
	int CurN;
	int old_loss;
	int flag;
	PACKET_INFO PacketList[N_PACKETS];
	int J_global;
    CRITICAL_SECTION lock;
} STAT;


struct addr_item
{  struct	sockaddr_in addr;
   STAT stat;
   struct addr_item *next;
};
struct addr_item *AddrList=NULL;
void RecieveThread(void *___p)	
{	int NSec=0;
   while(1)
   {  struct addr_item  **p; 
	   LARGE_INTEGER cur_dt;
	 
	   int J_local; 
	   int n_cl;
	   Sleep(1000);
      
	   NSec++;
	   for(p=&AddrList, n_cl=1;		*p!=NULL;   p=&((*p)->next), n_cl++)
     
	   {   STAT *pStat;
		   EnterCriticalSection(&(*p)->stat.lock);
		    
		   pStat=&(*p)->stat;
          if(pStat->CurN!=pStat->n_packets )
          {  int loss = pStat->n_packets - 1 - pStat->n_pairs; 
		     int J_local=pStat->dt_max.QuadPart-pStat->dt_min.QuadPart;
		     int dn=pStat->n_packets-pStat->CurN;
			 int dloss = loss-pStat->old_loss;
			
			 if (pStat->J_global < J_local)
			 {  pStat->J_global = J_local;
			 }

           pStat->old_loss=loss;
			  pStat->flag=1;
		     pStat->CurN=pStat->n_packets;
            
			 LeaveCriticalSection(&(*p)->stat.lock);
			 printf("%d: %d.%d.%d.%d:%d npackets=%d(+%d) loss=%d(%d) jitter=%d(%d)usec\n", n_cl,(*p)->addr.sin_addr.S_un.S_un_b.s_b1, (*p)->addr.sin_addr.S_un.S_un_b.s_b2, (*p)->addr.sin_addr.S_un.S_un_b.s_b3,(*p)->addr.sin_addr.S_un.S_un_b.s_b4, (*p)->addr.sin_port,
				 pStat->n_packets,dn, loss, dloss, pStat->J_global,J_local);
           
		  }
		  else
		  { 
			 LeaveCriticalSection(&(*p)->stat.lock);
              
		  }		
	   }
   }
}
struct addr_item* GetClientStat(struct	sockaddr_in  client)

{   struct addr_item *new_client, **p;

	for(p=&AddrList;		*p!=NULL;   p=&((*p)->next))
		
	{  if (((*p)->addr.sin_addr.S_un.S_addr==client.sin_addr.S_un.S_addr)
			&&((*p)->addr.sin_port==client.sin_port)
			)
		{    return *p;
		}
	}
	
	new_client=(struct addr_item *)malloc(sizeof(struct addr_item));
	InitializeCriticalSection(&(new_client->stat.lock));   
	new_client->next=NULL;
	new_client->addr=client;
	new_client->stat.n_packets=0;
	new_client->stat.n_pairs=0;
	new_client->stat.dt_min.QuadPart= 0x7FFFFFFFFFFFFFFF;
	new_client->stat.dt_max.QuadPart=-0x8000000000000000;
	new_client->stat.CurN= 0;//
	new_client->stat.old_loss=0;
	new_client->stat.flag = 1;
	new_client->stat.J_global=-1;

	(*p)=new_client;

	return new_client;
}



int main(int argc, char* argv[])
{   int iResult;
	WSADATA wsaData;	
    SOCKET RecvSocket;
    struct sockaddr_in RecvAddr;
    struct	sockaddr_in SenderAddr;
    int SenderAddrSize = sizeof (SenderAddr);
	 LARGE_INTEGER Frequency, t1,t2;
	
    
		
//	Stat.dt_min.QuadPart= 0x7FFFFFFFFFFFFFFF;
//	Stat.dt_max.QuadPart=-0x8000000000000000;

	int n = sscanf(argv[1],"%d", &Port);
   if ( n != 1)
   {
      printf ("port number not defined %d\n");
      return 2;
   }
   
	 //-----------------------------------------------
    // Initialize Winsock
    iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != NO_ERROR) 
	{   wprintf(L"WSAStartup failed with error %d\n", iResult);
        return 1;
    }

	//-----------------------------------------------
    // Create a receiver socket to receive datagrams
  
    RecvSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (RecvSocket == INVALID_SOCKET) 
	{   wprintf(L"socket failed with error %d\n", WSAGetLastError());
        return 1;
    }
    //-----------------------------------------------
    // Bind the socket to any address and the specified port.
    RecvAddr.sin_family = AF_INET;
    RecvAddr.sin_port = htons(Port);
    RecvAddr.sin_addr.s_addr = htonl(INADDR_ANY);

    iResult = bind(RecvSocket, (SOCKADDR *) & RecvAddr, sizeof (RecvAddr));
 
    if (iResult != 0) 
	{   wprintf(L"bind failed with error %d\n", WSAGetLastError());
        return 1;
    }
    
	_beginthread(RecieveThread, 0, NULL);
  
    wprintf(L"Receiving datagrams...\n");
	QueryPerformanceFrequency(&Frequency);
	
	


	for (;;)
	{  
		PACKET packet;
		LARGE_INTEGER time_rcv;
		int n_cell;
		int n_cell_next, n_cell_pre;
		STAT *pStat;
_rcv_packet:
		iResult = recvfrom(RecvSocket, (char*)&packet, sizeof(packet), 0, (SOCKADDR *) & SenderAddr, &SenderAddrSize);
    
		QueryPerformanceCounter(&time_rcv); 

		if (iResult == SOCKET_ERROR) 
		{  wprintf(L"recvfrom failed with error %d\n", WSAGetLastError());
		}
		   
	
		if (iResult != sizeof(PACKET))
		{	goto _rcv_packet;
		}

		if (packet.marker != 0x1234)
		{   goto _rcv_packet;
		}


		time_rcv.QuadPart *=  1000000;
		time_rcv.QuadPart /= Frequency.QuadPart;

		pStat=&(GetClientStat(SenderAddr)->stat);
		
		EnterCriticalSection(&(*pStat).lock);
   
		if (pStat->flag)
		{  pStat->flag=0;
		   pStat->dt_min.QuadPart= 0x7FFFFFFFFFFFFFFF;
		   pStat->dt_max.QuadPart=-0x8000000000000000;
		}
		
		pStat->n_packets++;
		n_cell=packet.num % N_PACKETS;
		pStat->PacketList[n_cell].packet = packet;
		pStat->PacketList[n_cell].filled = 1;
		pStat->PacketList[n_cell].time_rcv = time_rcv;	
		n_cell_next = n_cell +1 ;
		n_cell_pre = n_cell -1;

		if (n_cell_pre == -1)
		{	n_cell_pre += N_PACKETS;
		}

		if (n_cell_next == N_PACKETS)
		{	n_cell_next -= N_PACKETS;
		}

		if (pStat->PacketList[n_cell_next].filled)  
		{   if (pStat->PacketList[n_cell].packet.num+1 == pStat->PacketList[n_cell_next].packet.num )
   
			{  LARGE_INTEGER dt;
		       pStat->n_pairs++;
			   dt.QuadPart = time_rcv.QuadPart - pStat->PacketList[n_cell_next].packet.time_send.QuadPart;
			   if (pStat->dt_min.QuadPart > dt.QuadPart)
			   {	pStat->dt_min.QuadPart = dt.QuadPart;
			   }
			   if (pStat->dt_max.QuadPart < dt.QuadPart)
			   {	pStat->dt_max.QuadPart = dt.QuadPart;
			   }
			}
		}
		if (pStat->PacketList[n_cell_pre].filled) 
		{   if (pStat->PacketList[n_cell_pre].packet.num+1 == pStat->PacketList[n_cell].packet.num )
			{  LARGE_INTEGER dt;
		       pStat->n_pairs++;
			   dt.QuadPart = pStat->PacketList[n_cell_pre].time_rcv.QuadPart - pStat->PacketList[n_cell].packet.time_send.QuadPart;
			   if (pStat->dt_min.QuadPart > dt.QuadPart)
			   {	pStat->dt_min.QuadPart = dt.QuadPart;
			   }
			   if (pStat->dt_max.QuadPart < dt.QuadPart)
			   {	pStat->dt_max.QuadPart = dt.QuadPart;
			   }
			  
			}
		}		
		LeaveCriticalSection(&(*pStat).lock);
	}
    //-----------------------------------------------
    // Close the socket when finished receiving datagrams
	wprintf(L"Finished receiving. Closing socket.\n");
    iResult = closesocket(RecvSocket);
    if (iResult == SOCKET_ERROR) {
        wprintf(L"closesocket failed with error %d\n", WSAGetLastError());
        return 1;
    }
	
	// Initialize Winsock
    iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
    if (iResult != NO_ERROR) 
	{	printf("WSAStartup failed: %d\n", iResult);
		return 1;
    }

	return 0;
}
