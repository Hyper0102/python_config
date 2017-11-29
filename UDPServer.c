// ConsoleUDPServer.cpp : Defines the entry point for the console application.
//

#include <stdio.h>
//#include <windows.h>
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
  int filled;//условие, что в этой ячейке существует хотя бы 1 принятый пакет (он может быть старый)
} PACKET_INFO;


#define N_PACKETS 10000 

typedef struct 
{   LARGE_INTEGER  dt_min, dt_max;
	int n_packets, n_pairs;//меняются в main
   //n_packets- сколько пришло новых пакетов
	int CurN;//сколько пакетов уже распечатывали
	int old_loss;
	int flag;
	PACKET_INFO PacketList[N_PACKETS];
	int J_global;
    CRITICAL_SECTION lock;//указатель на lock
} STAT;

//STAT Stat; 

//int flag=1;



//двигает индекс last вслед за first


struct addr_item
{  struct	sockaddr_in addr;
   STAT stat;
   struct addr_item *next;
};	//сам адрес+ указатель на следующий
	
struct addr_item *AddrList=NULL;//список активных адресов .Пустой список(если не нул то есть содержимое)
void RecieveThread(void *___p)//подали ук-ль на чтонибудь неизв типа 
//void RecieveThread(int ___p)- подали аргументом int
//void RecieveThread(int* ___p)- подали аргументом ук-ль на int
	
{	int NSec=0;//Nsec - номер секунды (сеанса)- сколько раз делали sleep
 /*  int pp=(int)___p;//можем преобразовать адрес в integer - из 4 байтов преобразовываем в 8(зависит от системы)
   if (pp==1)*/
    
//curN - кол-во пакетов с пред раза
   while(1)
   {  struct addr_item  **p; //создали только ук-ль на ук-ль, но не создали ничего больше
	   LARGE_INTEGER cur_dt;
	 
	   int J_local; 
	   int n_cl;//счет
	   Sleep(1000);//1 Сек = 1000 милисекунд
      //ждет 1 секунду и заново запускает цикл
	   NSec++;
	   for(p=&AddrList, n_cl=1;		*p!=NULL;   p=&((*p)->next), n_cl++)//пробегаем по всем клиентам
      //в С++ проверяли бы на конец контейнера вместо *p!=NULL
	   {   STAT *pStat;//указатель на объект структуры STAT
		   
/////+++закрытие замка
		   EnterCriticalSection(&(*p)->stat.lock);
         //берем поле объекта addr_item, его подоле lock- и у этого всего адрес = (&((*p)->stat.lock));
		    
		   pStat=&(*p)->stat;//взяли статистику
         //pStat - указатель на структуру Stat
	      

          if(pStat->CurN!=pStat->n_packets ) //пришли ли новые пакеты
          {  int loss = pStat->n_packets - 1 - pStat->n_pairs; //на n пакетов ожидаем n-1 пар

		     int J_local=pStat->dt_max.QuadPart-pStat->dt_min.QuadPart;
		     int dn=pStat->n_packets-pStat->CurN;
			 int dloss = loss-pStat->old_loss;
			
			 if (pStat->J_global < J_local)
			 {  pStat->J_global = J_local;
			 }//интегральная статистика 

             pStat->old_loss=loss;
			 pStat->flag=1;//обновить данные
          //сбрасываем статистику иначе максимум будет расти

		     pStat->CurN=pStat->n_packets;
            
/////----откр замок обратно
			 LeaveCriticalSection(&(*p)->stat.lock);
			// LeaveCriticalSection(&pStat->lock);//не можем работать с копиями замков

			 printf("%d: %d.%d.%d.%d:%d npackets=%d(+%d) loss=%d(%d) jitter=%d(%d)usec\n", n_cl,(*p)->addr.sin_addr.S_un.S_un_b.s_b1, (*p)->addr.sin_addr.S_un.S_un_b.s_b2, (*p)->addr.sin_addr.S_un.S_un_b.s_b3,(*p)->addr.sin_addr.S_un.S_un_b.s_b4, (*p)->addr.sin_port,
				 pStat->n_packets,dn, loss, dloss, pStat->J_global,J_local);
             //J_local- выводим статистику в пределах последней 1 секунды
             //J_global - абсолютный максимум джиттер - интегральная сумма
		  }
///откр замок обратно если не удалось в тот раз
		  else
		  { 
			 LeaveCriticalSection(&(*p)->stat.lock);
              
/////----откр замок обратно
		  }

		  
         // printf("%d: npackets=%d(+%d) Loss=%d(+%d) Jitter=%d(%d)usec\n",NSec, pStat->n_packets, pStat->n_packets-CurN,  loss,loss-old_loss, J_global,  J_local);
			
	   }
   }

/*	
	
	int J_global=-1;
    while(1)
	{   LARGE_INTEGER cur_dt;
		STAT st;
		int J_local; 
		Sleep(1000);
		NSec++;

		EnterCriticalSection(&lock);//вешаем замок, пока будем копировать
		st=Stat;//забрали статистику, копируем из глобальной к локальную
		LeaveCriticalSection(&lock);
		
	
		flag=1;//просим тред обнулить данные
		cur_dt.QuadPart = pStat->dt_max.QuadPart - pStat->dt_min.QuadPart;

		
        J_local=pStat->dt_max.QuadPart-pStat->dt_min.QuadPart;
		
		if (J_global < J_local)
		{  J_global = J_local;
		}

		if(CurN!=pStat->n_packets)
		{   int loss = pStat->n_packets - 1 - pStat->n_pairs; //на n пакетов ожидаем n-1 пар
//			printf("%d: npackets=%d(+%d) nPair=%d Loss=%d dt_min=%lld dt_max=%lld dt=%lld\n",NSec, n_packet, n_packet-CurN, n_pairs, Loss, cur_dt_min.QuadPart, cur_dt_max.QuadPart, cur_dt_max.QuadPart-cur_dt_min.QuadPart);
			printf("%d: npackets=%d(+%d) Loss=%d(+%d) Jitter=%d(%d)usec\n",NSec, pStat->n_packets, pStat->n_packets-CurN,  loss,loss-old_loss, J_global,  J_local);
			CurN=pStat->n_packets;
			old_loss=loss;
		}
	}
	*/
}
struct addr_item* GetClientStat(struct	sockaddr_in  client)//функция получить статистику для нового клиента
//ф-ция возвращает ссылку на экз структуры addr_item
//struct addr_item (*GetClientStat)(struct	sockaddr_in  client) -ук-ль на фцию
{   struct addr_item *new_client, **p;
//выделили память только для адресов а не для объектов! нужен malloc
//в декларации -указатель на указатель (в действии было бы- содержимое указателя, может быть чем угодно)
    // *new_client- ук-ль на экз структуры addr_item , **p - укль на ук-ль
	for(p=&AddrList;		*p!=NULL;   p=&((*p)->next))//присваиваем p начало списка
		//p -указатель от указателя, до первого клиента *p= null
      //если *p==NULL - значит  не нашлось статистики для клиента с данным адресом 
	{  if (((*p)->addr.sin_addr.S_un.S_addr==client.sin_addr.S_un.S_addr)
			&&((*p)->addr.sin_port==client.sin_port)
			)
		{    return *p;//возвращаем указатель на запись статистики клиента
                	//функция возвращает адрес, а не копию , потому что хотим писать туда
		}
	}
	//не нашли клиента среди уже подключенных
	new_client=(struct addr_item *)malloc(sizeof(struct addr_item));
   //Выд дин память - heap
   //malloc-выделяет из кучи память размера данных
   //new_client - ук-ль на выделенную память для нового клиента (размер памяти будет =(sizeof( struct addr_item))
  
	InitializeCriticalSection(&(new_client->stat.lock));
    // struct addr_item *new_client, - *new_client - явл ук-лем на экз данной структуры
	//(struct addr-преобр из void в указатель на struct
	//создает замок

	new_client->next=NULL;
	new_client->addr=client;//заполнили поля
   //new client= экз структуры addr_item,его поле addr= также экз-р стр sockaddr_in
	
	new_client->stat.n_packets=0;
	new_client->stat.n_pairs=0;
	new_client->stat.dt_min.QuadPart= 0x7FFFFFFFFFFFFFFF;//макс зн-е
	new_client->stat.dt_max.QuadPart=-0x8000000000000000;
	new_client->stat.CurN= 0;//
	new_client->stat.old_loss=0;
	new_client->stat.flag = 1;//обновить данные
	new_client->stat.J_global=-1;

	(*p)=new_client;//модифицировать конец Addrlist;

	return new_client;//создали копию, и return неявная копия newclient не уничтожится при выходе из функции!(т к malloc, если б в стеке- то уничтожилась бы после выхода из ф-ции)
	//ошибкой является ставить & на уничтожающийся объект 
   //неправильно : struct addr_item  new_cl;-живет в стеке, после retrun все что в стеке уничтожается
   //return &new_cl;-здесь будет ссылка на мусор
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


	//создали указатель для пакета, для дешифрации получ пакета
	//проверим соответствует ли маркер
	int n = sscanf(argv[1],"%d", &Port);
   if ( n != 1)
   {
      printf ("port number not defined %d\n");
      return 2;
      //программа закончилась
   }

   
	 //-----------------------------------------------
    // Initialize Winsock
    //стартует сокетную библиотеку
    iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != NO_ERROR) 
	{   wprintf(L"WSAStartup failed with error %d\n", iResult);
        return 1;
    }

	//-----------------------------------------------
    // Create a receiver socket to receive datagrams
    //создается сокет для будущего соед-я = выд память ( еще нет соед-я)
    //здесь сокет еще пустой
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
    //засунули адрес в сокет
    //RecvSocket = handle, нам недоступны внутренности ,доступен только сам handle
    //RecvSocket -для нас это SOCKET (int для других систем)
    //(SOCKADDR *) & RecvAddr - преобразовали только адрес, объект не трогали
    if (iResult != 0) 
	{   wprintf(L"bind failed with error %d\n", WSAGetLastError());
        return 1;
    }
    
	_beginthread(RecieveThread, 0, NULL);
   //_beginthread(RecieveThread, 0, NULL);//будет бессмысленно повторять на одно и то же- NULL аргумент для экз-ра ReceiveThread кот запустится здесь
   //_beginthread(RecieveThread, 0, (void *)2);//2=значение адреса, не знаем куда он указывает
  // int w=2;
    // _beginthread(RecieveThread, 0, &w);//передали ук-ль на переменную w содержимое кот =2
   //создали второй тред-windows C++ ф-ция
   //0- размер стека (по ум)
   //тред для распечатки
   //NULL- ф-ция стартует с входным параметром

	//-----------------------------------------------
    // Call the recvfrom function to receive datagrams
    // on the bound socket.
    wprintf(L"Receiving datagrams...\n");
	QueryPerformanceFrequency(&Frequency);
	
	


	for (;;)
   //=while(1)
	{  
		PACKET packet;//ячейка для получения нового пакета
		LARGE_INTEGER time_rcv;
		int n_cell;//номер ячейки
		int n_cell_next, n_cell_pre;
		STAT *pStat;//Ук-ль на статистику
_rcv_packet: //Метка для goto
		iResult = recvfrom(RecvSocket, (char*)&packet, sizeof(packet), 0, (SOCKADDR *) & SenderAddr, &SenderAddrSize);
      //возвращает кол-во байт полученного одного пакета
      //запустили сокет на прием
      //получаем пакеты
      //арг-ты: номер сокета,место куда надо писать - адрес пакета преобр-й в тип char, сколько места есть для принятия данных, 0, 
      //откуда - кто прислал= обр -й адрес, размер места куда можно запис обр адрес)
      //
      //на выходе- содержимое пакета положат в адрес &packet, обратный адрес  & SenderAddr- кто прислал
		QueryPerformanceCounter(&time_rcv);//время получ пакета  

		//проверка на то, удалось ли принять пакет
		if (iResult == SOCKET_ERROR) 
		{  wprintf(L"recvfrom failed with error %d\n", WSAGetLastError());
		}
		   
		//проверяем размер пакета
		if (iResult != sizeof(PACKET))
		{	goto _rcv_packet;
		}

		//проверяем на маркер
		if (packet.marker != 0x1234)
		{   goto _rcv_packet;//игнорируем пакет
		}


		time_rcv.QuadPart *=  1000000;
		time_rcv.QuadPart /= Frequency.QuadPart;

		pStat=&(GetClientStat(SenderAddr)->stat);//вызвали функцию,с параметром SenderAddr, (неявно копируется SenderAddr изSenderAddr В client)
		//получили указатель на клиента, выбрали поле stat, взяли адрес от него. Получили указатель на статистику клиента
		EnterCriticalSection(&(*pStat).lock);
      //EnterCriticalSection(&pStat->lock);
		      
		//здесь обнуляем dt
		//
		if (pStat->flag)
		{  pStat->flag=0;
		   pStat->dt_min.QuadPart= 0x7FFFFFFFFFFFFFFF;
		   pStat->dt_max.QuadPart=-0x8000000000000000;
		}
		
		pStat->n_packets++;
      //стало на 1 пакет больше
		//10000 ячеек, кладем по остатку от деления
		n_cell=packet.num % N_PACKETS;
      //смотрим в содержимое, какой номер этого пакета
      //способ контролировать посл-сть  пакетов
      //1023 % 10000 = 23
      //
		pStat->PacketList[n_cell].packet = packet;//кладем полученный пакет в PacketList
		pStat->PacketList[n_cell].filled = 1;
		pStat->PacketList[n_cell].time_rcv = time_rcv;
	    
		
		n_cell_next = n_cell +1 ;
		n_cell_pre = n_cell -1;

		if (n_cell_pre == -1)
		{	n_cell_pre += N_PACKETS;//станет 9999
		}

		if (n_cell_next == N_PACKETS)
		{	n_cell_next -= N_PACKETS;
		}

		if (pStat->PacketList[n_cell_next].filled)  //там был следующий пакет - свежий или старый ?
		{   if (pStat->PacketList[n_cell].packet.num+1 == pStat->PacketList[n_cell_next].packet.num )//10023 +1 ! = 24
      //если усл вып= пара есть
			{  LARGE_INTEGER dt;
		       pStat->n_pairs++;
			   dt.QuadPart = time_rcv.QuadPart - pStat->PacketList[n_cell_next].packet.time_send.QuadPart;//в след пакете -лежит время отпр текущего пакета
			   if (pStat->dt_min.QuadPart > dt.QuadPart)//dt_min - минимальное время пакета в пути
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


		//Stat=st;
		
		LeaveCriticalSection(&(*pStat).lock);
      //убрали замок
		

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
