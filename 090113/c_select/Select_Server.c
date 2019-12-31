//Innocenti Mattia 0000825046 NumeroCompito

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>
#include <fcntl.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/select.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

/*** #DEFINE **/
#define DIM_BUFF 100
#define max(a,b) ((a) > (b) ? (a) : (b))

/*** STRUTTURA DA INVIARE ATTRAVERO LA SOCKET **/
//struttura di esempi del Client Datagram

//in questo esempio non occorre definire una struttura apposita in quanto la richiesta che proviene dal client datagram è un tipo calza
// GIÀ DEFINITA


/********************************************************/
// Eventuale struttura dati del server

#define LENGTH 25
#define LENGTH_MSG 256
#define N_ROW 10
typedef struct
{
	char id[LENGTH];
	int gg;
	int mm;
	int aaaa;
	int giorniNoleggio;
	char modello[LENGTH];
    int costoGiornaliero;
} Sci;

typedef struct
{
	char id[LENGTH];
} ReturnStreamSocket; //lista degli identificatori di quel modello (passato dal client)
//ancora noleggiabili

// Funzione per la stampa della struttura dati server
void stampa(Sci tabella[])
{
	int i;
	printf("ID\tDATA\tGIORNI NOLEGGIO\tMODELLO\tCOSTO GG\n");
	for( i=0; i < N_ROW; i++)
	{
		printf("%s\t",tabella[i].id);
		printf("%d/%d/%d\t",tabella[i].gg, tabella[i].mm, tabella[i].aaaa);
		printf("%d\t",tabella[i].giorniNoleggio);
		printf("%s\t",tabella[i].modello);
		printf("%d\n",tabella[i].costoGiornaliero);
	}
}


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
	char buff[LENGTH]; //tipi di risposte
    const int on = 1; //--> usata in setsockopt
    int len, nwrite, num, esito, port;
    struct sockaddr_in clientaddr, servaddr;
	struct hostent *hostTcp, *hostUdp;
    fd_set rset;
    
	//Calza req;
    ReturnStreamSocket returnId[N_ROW];

    Sci tabella[N_ROW];
      
      /*** ----  Controllo argomenti ---- **/
      //server porta
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
	int i;
	for(i=0; i<N_ROW; i++)
	{
	    strcpy(tabella[i].id, "L");
	    tabella[i].gg = -1;
		tabella[i].mm = -1;
		tabella[i].aaaa = -1;
		tabella[i].giorniNoleggio = -1;
	    strcpy(tabella[i].modello, "L");
		tabella[i].costoGiornaliero = -1;

		//INIZIALIZZAZIONE ANCHE DELLA STRUTTURA CHE RITORNO COME RISPOSTA ALLA RICHIESTA DEL CLIENT STREAM
		strcpy(returnId[i].id , "L");
	}

	strcpy(tabella[1].id, "X12AB");
	tabella[1].gg = 12;
	tabella[1].mm = 12;
	tabella[1].aaaa = 2012;
	tabella[1].giorniNoleggio = 14;
	strcpy(tabella[1].modello, "Volki");
	tabella[1].costoGiornaliero = 7;

	strcpy(tabella[2].id, "ACD14");
	tabella[2].gg = -1;
	tabella[2].mm = -1;
	tabella[2].aaaa = -1;
	tabella[2].giorniNoleggio = -1;
	strcpy(tabella[2].modello, "Fisher");
	tabella[2].costoGiornaliero = 5;

	strcpy(tabella[3].id, "Y23CC");
	tabella[3].gg = 23;
	tabella[3].mm = 12;
	tabella[3].aaaa = 2012;
	tabella[3].giorniNoleggio = 7;
	strcpy(tabella[3].modello, "Volki");
	tabella[3].costoGiornaliero = 7;

	strcpy(tabella[5].id, "ACDRE");
	tabella[5].gg = -1;
	tabella[5].mm = -1;
	tabella[5].aaaa = -1;
	tabella[5].giorniNoleggio = -1;
	strcpy(tabella[5].modello, "Fisher");
	tabella[5].costoGiornaliero = 51;

	stampa(tabella);
    
	
	
	//**  CREAZIONE SOCKET TCP **/
	listenfd=socket(AF_INET, SOCK_STREAM, 0);
	if (listenfd < 0)
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
	maxfdp1 = max(listenfd, udpfd)+1;
	
	
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

		//DOVE SALVO IL PARAMETRO DI RICEZIONE
		char idRichiesta[LENGTH];

		if(FD_ISSET(udpfd,&rset))
		{
			printf("Server: ricevuta richiesta UDP da client: \n");
			hostUdp = gethostbyaddr((char *) &clientaddr.sin_addr,sizeof(clientaddr.sin_addr), AF_INET);
			if (hostUdp == NULL) {
				printf("client host information not found\n");
			}
			else {
				printf("Operazione richiesta da: %s %i\n", hostUdp->h_name, (unsigned)ntohs(clientaddr.sin_port));
			}

			len=sizeof(struct sockaddr_in);
			if(recvfrom(udpfd,&idRichiesta,sizeof(idRichiesta),0,(struct sockaddr *)&clientaddr, &len) < 0)	{
				perror("Recvfrom");
				continue;
			}

			printf("Richiesto dal client...\n");
			esito = -1; //quella che uso per tornare il valore al client (se trovo altro id = al mio allora ritorno direttamente esito)
			
			// LOGICA APPLICATIVA --> devo settare la variabile intera da ritornare 0 in caso di successo -1 fallimento
			int posLibera = -1; //ci sono posti a disposizione
			int duplicato = -1;

			for(i = 0; i < N_ROW; i++){
				if(strcmp(tabella[i].id, idRichiesta) == 0){
					if((tabella[i].gg != -1) && (tabella[i].mm != -1) && (tabella[i].aaaa != -1) && (tabella[i].giorniNoleggio != -1)){
						esito = tabella[i].costoGiornaliero * tabella[i].giorniNoleggio;
					}
				}
			}

			printf("Esito: %d\n",esito);
	    	//RICORDA DI NON FARE SUI DATI QUESTA CONVERSIONE IN FORMATO DI RETE
			//esito = ntohl(esito); 
			
			if(sendto(udpfd,&esito,sizeof(int),0,(struct sockaddr *)&clientaddr, len)<0) {
				perror("Sendto");
				continue;
			}

	    	stampa(tabella);
			printf("Operazione conclusa.\n");
		}
		

		
    	//CASO 2: GESTIONE RICHIESTE DA SOCKET STREAM
		if(FD_ISSET(listenfd,&rset)) {
			len=sizeof(struct sockaddr_in);
			if((connfd=accept(listenfd,(struct sockaddr *)&clientaddr,&len))<0) {
				if(errno==EINTR)
					continue;
				else {
					perror("accept");
					exit(9);
				}
			}
			
			//SCHEMA UN PROCESSO FIGLIO PER OGNI OPERAZIONE CLIENTE 
			//figlio
			if(fork()==0) {
				close(listenfd);
				//chi mi fa richiesta
				hostTcp = gethostbyaddr((char *) &clientaddr.sin_addr,sizeof(clientaddr.sin_addr), AF_INET);
				printf("Dentro il figlio pid=%d\n", getpid());
				printf("Richiesta del client: %s", hostTcp);
				
    			// CASO 1 (una socket per richiesta)--> logica applicativa del programma che si richiede <-- 
	    		//Per una sola connessione decommentare il ciclo for
				for(;;) {
					//leggo tipo di calza e carbone dalla socket CONNESSA (connfd)
					//printf("Richiesta, la seguente..");
					while((nread=read(connfd,buff,sizeof(buff)))>0)
					{

						/* Se dovessi leggere ulteriori parametri decommenta!

						char carbone;
						//in buffer ho il tipo di calza ora leggo carbone S/N
						if((nread = read(connfd, &carbone, sizeof(char))) < 0){
							perror("read indicatore carbone");
							exit(3);
						}

						*/

						int pos = 0;
						for(i = 0; i < N_ROW; i++){
							if((strcmp(tabella[i].modello, buff) == 0) && (tabella[i].gg == -1) && (tabella[i].mm == -1) && (tabella[i].aaaa == -1) && (tabella[i].giorniNoleggio == -1)){
								strcpy(returnId[pos].id, tabella[i].id);
								pos++;
							}
						}

						if((nwrite=write(connfd,returnId,sizeof(returnId)))<0)
						{
							perror("write");
							exit(3);
						}

						//risetto a tutto L il mio vettore di id che torno
						for(i = 0; i < N_ROW; i++)
							strcpy(returnId[i].id, "L");
					}
				}
	      // CASO 2 (unica socket) --> logica applicativa del programma che si richiede <-
	  /*
				printf("Richiesta, la seguente..");
				while((nread=read(connfd,buff,sizeof(buff)))>0)
				{
					if((nwrite=write(connfd,buff,nread))<0)
					{
						perror("write");
						exit(3);
					}
				}
	*/
	    
	  //} //for
				
				printf("Figlio %i: chiudo connessione e termino\n", getpid());
				close(connfd);
				exit(0);
			}//fine figlio
	  
	    	close(connfd);
    	}//if listen
		

    } /* ciclo for della select */

    exit(0);

}//main