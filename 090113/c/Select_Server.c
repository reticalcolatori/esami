/******** #INCLUDE ********/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>
#include <fcntl.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/select.h>
#include <netinet/in.h>
#include <netdb.h>

/*** #DEFINE **/
#define DIM_BUFF 100
#define max(a,b) ((a) > (b) ? (a) : (b))

#define LENGTH 25
#define N 5

/*** STRUTTURA DA INVIARE ATTRAVERO LA SOCKET DATAGRAM **/
typedef struct
{
	char id[LENGTH];
}
Request;

/********************************************************/
// Eventuale struttura dati del server
typedef struct
{
	char id[LENGTH];
	int giorno;
	int mese;
	int anno;
	char modello[LENGTH];
	int costoGiornaliero;
}
Noleggio;

/********************************************************/

void gestore(int signo)
{
	int stato;
	printf("esecuzione gestore di SIGCHLD\n");
	wait(&stato);
}

/********************************************************/

int main(int argc, char **argv)
{
	int listenfd, connfd, udpfd, nready,nread, maxfdp1; 
	char buff[DIM_BUFF]; 
	
    const int on = 1; //--> usata in setsockopt
    int len, nwrite, num, esito, port;
    struct sockaddr_in clientaddr, servaddr;
	struct hostent *hostTcp, *hostUdp;
    
	fd_set rset;
    Request req;

	Noleggio tabella[N];
      
    /*** ----  Controllo argomenti ---- **/
    if(argc!=2)
    {
    	printf("Error: %s port\n", argv[0]);
     	exit(1);
    }
    // Controllo che la porta sia un intero
    num=0;
    while( argv[1][num]!= '\0' )
    {
    	if( (argv[1][num] < '0') || (argv[1][num] > '9') )
      	{
      		printf("Argomento non intero\n");
      		exit(2);	
      	}
      	num++;
	}
      
    // Controllo che la porta sia nel range
    port = atoi(argv[1]);
    if (port < 1024 || port > 65535)
     {
      	printf("Error: porta non valida!\n");
      	printf("1024 <= port <= 65535\n");
      	exit(2);
    }
    printf("Server avviato\n");
      
    // Qui eventuale inizializzazione della struttura dati del server
	
	for(int i=0; i<N; i++)
	{
	    strcpy(tabella[i].id, "L");
	    strcpy(tabella[i].modello, "L");
	    tabella[i].giorno = -1;
		tabella[i].mese = -1;
		tabella[i].anno = -1;
		tabella[i].costoGiornaliero = -1;	    
	}

	strcpy(tabella[0].id, "0001");
	strcpy(tabella[0].modello, "Novistar");
	tabella[0].costoGiornaliero = 10;
	
	strcpy(tabella[1].id, "0002");
	strcpy(tabella[1].modello, "Rooster");
	tabella[1].costoGiornaliero = 15;

	strcpy(tabella[2].id, "0003");
	strcpy(tabella[2].modello, "Rooster");
	tabella[2].costoGiornaliero = 20;

	//**  CREAZIONE SOCKET TCP **/
	listenfd=socket(AF_INET, SOCK_STREAM, 0);
	if (listenfd <0)
	{
		perror("apertura socket TCP ");
		exit(1);
	}
	printf("Creata la socket TCP d'ascolto, fd=%d\n", listenfd);
	
      // **  INIZIALIZZAZIONE INDIRIZZO SERVER E BIND **/
	memset ((char *)&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = INADDR_ANY;
	servaddr.sin_port = htons(port);

	if (setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on))<0)
	{
		perror("set opzioni socket TCP");
		exit(2);    
	}
	printf("Set opzioni socket TCP ok\n");
	if (bind(listenfd,(struct sockaddr *) &servaddr, sizeof(servaddr))<0)
	{
		perror("bind socket TCP");
		exit(3);
	}


	printf("Bind socket TCP ok\n");
	
	if (listen(listenfd, 5)<0)
	{
		perror("listen");
		exit(4);
	}
	printf("Listen ok\n");
	
      // CREAZIONE SOCKET UDP
	udpfd=socket(AF_INET, SOCK_DGRAM, 0);
	if(udpfd <0)
	{
		perror("apertura socket UDP");
		exit(5);
	}
	printf("Creata la socket UDP, fd=%d\n", udpfd);

      // INIZIALIZZAZIONE INDIRIZZO SERVER E BIND
	memset ((char *)&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = INADDR_ANY;
	servaddr.sin_port = htons(port);

	if(setsockopt(udpfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on))<0)
	{
		perror("set opzioni socket UDP");
		exit(6);
	}
	printf("Set opzioni socket UDP ok\n");

	if(bind(udpfd,(struct sockaddr *) &servaddr, sizeof(servaddr))<0)
	{
		perror("bind socket UDP");
		exit(7);
	}
	printf("Bind socket UDP ok\n");
	
      // AGGANCIO GESTORE PER EVITARE FIGLI ZOMBIE
	signal(SIGCHLD, gestore);

      // PULIZIA E SETTAGGIO MASCHERA DEI FILE DESCRIPTOR
	FD_ZERO(&rset);
	maxfdp1=max(listenfd, udpfd)+1;
	
	
      // CICLO DI RICEZIONE EVENTI DALLA SELECT
	for(;;)
	{
		FD_SET(listenfd, &rset);
		FD_SET(udpfd, &rset);
		
		if ((nready=select(maxfdp1, &rset, NULL, NULL, NULL))<0)
		{
			if (errno==EINTR) continue;
			else
			{
				perror("select");
				exit(8);
			}
		}

	  //CASO 1: GESTIONE RICHIESTE DA SOCKET DATAGRAM
		if(FD_ISSET(udpfd,&rset))
		{
			printf("Server: ricevuta richiesta UDP da client: \n");
			hostUdp = gethostbyaddr((char *) &clientaddr.sin_addr,sizeof(clientaddr.sin_addr), AF_INET);
			if (hostUdp == NULL)
			{
				printf("client host information not found\n");
			}
			else
			{
				printf("Operazione richiesta da: %s %i\n", hostUdp->h_name, (unsigned)ntohs(clientaddr.sin_port));
			}

			len=sizeof(struct sockaddr_in);
			if(recvfrom(udpfd,&req,sizeof(Request),0,(struct sockaddr *)&clientaddr, &len)<0)
			{
				perror("Recvfrom");
				continue;
			}
			printf("Richiesto dal client...\n");
			esito = -1;
			
	      	// Ricevo ID e restituisco
			// il costo giornaliero.

			for(int i=0; i < N; i++){
				if(strcmp(tabella[i].id, req.id) == 0){
					esito = tabella[i].costoGiornaliero;
					break;
				}
			}

			printf("Esito: %d\n",esito);
			esito = ntohl(esito);
			
			if(sendto(udpfd,&esito,sizeof(int),0,(struct sockaddr *)&clientaddr, len)<0)
			{
				perror("Sendto");
				continue;
			}
			printf("Operazione conclusa.\n");
		}
		
      	//CASO 2: GESTIONE RICHIESTE DA SOCKET STREAM
		if(FD_ISSET(listenfd,&rset))
		{
			len=sizeof(struct sockaddr_in);
			if((connfd=accept(listenfd,(struct sockaddr *)&clientaddr,&len))<0)
			{
				if(errno==EINTR)
					continue;
				else
				{
					perror("accept");
					exit(9);
				}
			}

			if(fork()==0)
			{
				close(listenfd);
				//chi mi fa richiesta
				hostTcp = gethostbyaddr((char *) &clientaddr.sin_addr,sizeof(clientaddr.sin_addr), AF_INET);
				printf("Dentro il figlio pid=%d\n", getpid());
				printf("Richiesta del client: %s\n", hostTcp->h_addr_list[0]);
				
				for(;;)
				{

					char modello[LENGTH];
					int i = 0;

					//Ciclo finchè arrivo al terminatore.
					while((nread=read(connfd,modello+i,sizeof(char)))>0)
					{
						if(modello[i++] == '\0'){
							break;
						}
					}

					if(nread == 0){
						printf("%d) EOF received\n", getpid());
						//EOF: esco
						exit(0);
					}

					if(nread<0)
					{
						perror("read stream request");
						exit(3);
					}

					//Ho il modello ricerco id validi.
					char *ids[N];
					int idsLength = 0;

					for(i=0;i<N;i++){
						if(strcmp(tabella[i].modello, modello) == 0){
							ids[idsLength++] = tabella[i].id;
						}
					}

					//Devo inviare i risultati
					//Invio prima length

					printf("Ids Length: %d\n", idsLength);

					int tmpInt = htonl(idsLength);

					nwrite = write(connfd, &tmpInt, sizeof(int));

					if(nwrite<0){
						perror("Write length error");
						exit(4);
					}

					//Ora invio il resto dei dati
					for(i=0; i <idsLength;i++){

						printf("Id %d: %s\n", i, ids[i]);

						//Invio anche il terminatore.
						nwrite = write(connfd, ids[i], sizeof(char) * (strlen(ids[i])+1));

						if(nwrite<0){
							perror("Write length error");
							exit(4);
						}	
					}


				} //for
				
				printf("Figlio %i: chiudo connessione e termino\n", getpid());
				close(connfd);
				exit(0);
	  }//fine figlio
	  
	  close(connfd);
      }//if listen

      } /* ciclo for della select */

      exit(0);
}//main
