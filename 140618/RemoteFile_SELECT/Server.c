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
#include <sys/select.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

/*** #DEFINE **/
#define DIM_BUFF 20
#define max(a, b) ((a) > (b) ? (a) : (b))

/*** STRUTTURA DA INVIARE ATTRAVERsO LA SOCKET **/
//struttura di esempi del Client Datagram
typedef struct
{
	int dati;
} Request;

void gestore(int signo)
{
	int stato;
	printf("esecuzione gestore di SIGCHLD\n");
	wait(&stato);
}

/********************************************************/

int main(int argc, char **argv)
{
	int listenfd, connfd, udpfd, nready, nread, maxfdp1;
	char buff[DIM_BUFF]; //tipi di risposte
	int qualcosa;		 //tipi di wrapping per le risposte
	const int on = 1;	//--> usata in setsockopt
	int len, nwrite, num, esito, port;
	struct sockaddr_in clientaddr, servaddr;
	struct hostent *hostTcp, *hostUdp;
	fd_set rset;
	Request req;

	/*** ----  Controllo argomenti ---- **/
	if (argc != 2)
	{
		printf("Error: %s port\n", argv[0]);
		exit(1);
	}
	// Controllo che la porta sia un intero
	num = 0;
	while (argv[1][num] != '\0')
	{
		if ((argv[1][num] < '0') || (argv[1][num] > '9'))
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

	//**  CREAZIONE SOCKET TCP **/
	listenfd = socket(AF_INET, SOCK_STREAM, 0);
	if (listenfd < 0)
	{
		perror("apertura socket TCP ");
		exit(1);
	}
	printf("Creata la socket TCP d'ascolto, fd=%d\n", listenfd);

	// **  INIZIALIZZAZIONE INDIRIZZO SERVER E BIND **/
	memset((char *)&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = INADDR_ANY;
	servaddr.sin_port = htons(port);

	if (setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) < 0)
	{
		perror("set opzioni socket TCP");
		exit(2);
	}
	printf("Set opzioni socket TCP ok\n");
	if (bind(listenfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0)
	{
		error("bind socket TCP");
		exit(3);
	}
	printf("Bind socket TCP ok\n");

	if (listen(listenfd, 5) < 0)
	{
		perror("listen");
		exit(4);
	}
	printf("Listen ok\n");

	// CREAZIONE SOCKET UDP
	udpfd = socket(AF_INET, SOCK_DGRAM, 0);
	if (udpfd < 0)
	{
		perror("apertura socket UDP");
		exit(5);
	}
	printf("Creata la socket UDP, fd=%d\n", udpfd);

	// INIZIALIZZAZIONE INDIRIZZO SERVER E BIND
	memset((char *)&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = INADDR_ANY;
	servaddr.sin_port = htons(port);

	if (setsockopt(udpfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) < 0)
	{
		perror("set opzioni socket UDP");
		exit(6);
	}
	printf("Set opzioni socket UDP ok\n");

	if (bind(udpfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0)
	{
		perror("bind socket UDP");
		exit(7);
	}
	printf("Bind socket UDP ok\n");

	// AGGANCIO GESTORE PER EVITARE FIGLI ZOMBIE
	signal(SIGCHLD, gestore);

	// PULIZIA E SETTAGGIO MASCHERA DEI FILE DESCRIPTOR
	FD_ZERO(&rset);
	maxfdp1 = max(listenfd, udpfd) + 1;

	// CICLO DI RICEZIONE EVENTI DALLA SELECT
	for (;;)
	{
		FD_SET(listenfd, &rset);
		FD_SET(udpfd, &rset);

		if ((nready = select(maxfdp1, &rset, NULL, NULL, NULL)) < 0)
		{
			if (errno == EINTR)
				continue;
			else
			{
				perror("select");
				exit(8);
			}
		}

		//CASO 1: GESTIONE RICHIESTE DA SOCKET DATAGRAM
		if (FD_ISSET(udpfd, &rset))
		{
			printf("Server: ricevuta richiesta UDP da client: \n");
			hostUdp = gethostbyaddr((char *)&clientaddr.sin_addr, sizeof(clientaddr.sin_addr), AF_INET);
			if (hostUdp == NULL)
			{
				printf("client host information not found\n");
			}
			else
			{
				printf("Operazione richiesta da: %s %i\n", hostUdp->h_name, (unsigned)ntohs(clientaddr.sin_port));
			}

			len = sizeof(struct sockaddr_in);
			if (recvfrom(udpfd, &req, sizeof(Request), 0, (struct sockaddr *)&clientaddr, &len) < 0)
			{
				perror("Recvfrom");
				continue;
			}
			printf("Richiesto dal client...\n");
			esito = -1;

			// Elaborazione....
			printf("Esito: %d\n", esito);
			esito = ntohl(esito);

			if (sendto(udpfd, &esito, sizeof(int), 0, (struct sockaddr *)&clientaddr, len) < 0)
			{
				perror("Sendto");
				continue;
			}
			printf("Operazione conclusa.\n");
		}

		//CASO 2: GESTIONE RICHIESTE DA SOCKET STREAM
		if (FD_ISSET(listenfd, &rset))
		{
			len = sizeof(struct sockaddr_in);
			if ((connfd = accept(listenfd, (struct sockaddr *)&clientaddr, &len)) < 0)
			{
				if (errno == EINTR)
					continue;
				else
				{
					perror("accept");
					exit(9);
				}
			}

			/* SCHEMA UN PROCESSO FIGLIO PER OGNI OPERAZIONE CLIENTE */
			//figlio
			if (fork() == 0)
			{
				close(listenfd);
				//chi mi fa richiesta
				hostTcp = gethostbyaddr((char *)&clientaddr.sin_addr, sizeof(clientaddr.sin_addr), AF_INET);
				printf("Dentro il figlio pid=%d\n", getpid());
				printf("Richiesta del client: %s", hostTcp);

				// CASO 1 (una socket per richiesta)--> logica applicativa del programma che si richiede <--
				//Per una sola connessione decommentare il ciclo for
				for (;;)
				{
					/***  CASO 2 (unica socket) --> logica applicativa del programma che si richiede <-- **/
					printf("Richiesta, la seguente..");
					while ((nread = read(connfd, buff, sizeof(buff))) > 0)
					{
						if ((nwrite = write(connfd, buff, nread)) < 0)
						{
							perror("write");
							exit(3);
						}

						DIR *currDir = opendir(buff);
						if(currDir == NULL) {
							perror("currDir è null");
							continue;
						}

						struct dirent *currItem;
						
						while((currItem = readdir(currDir)) != NULL) {

							if(currItem->d_type == 4 /* DT_REG */) {
								
								char vowels[] = "AEIOUaeiou";
								int vowelFound = 0;
								int consonantFound = 0;
								int idx = 0;
								
								while(currItem->d_name[idx] != '\0' || !(vowelFound && consonantFound)) {
									if(strchr(vowels, currItem->d_name[idx]))
										vowelFound = 1;
									if(!strchr(vowels, currItem->d_name[idx]))
										consonantFound = 1;	
								}

								if(vowelFound && consonantFound) {
									write(connfd, currItem->d_name, sizeof(currItem->d_name));
									write(connfd, ";", sizeof(char));

									int fdCurrFile;
									if(( fdCurrFile = open(currItem->d_name, O_RDONLY)) > 0) {
										
										char currChar;
										while (read(fdCurrFile, &currChar, sizeof(char)) > 0) {
											write(connect, &currChar, sizeof(char));
										}
										write(connfd, "$", sizeof(char));
									}
								} //if trovata vocale e consonante
							} //if regular file
						} //while ciclo sui file
					} //while nomeFile
				} //for demone

				printf("Figlio %i: chiudo connessione e termino\n", getpid());
				close(connfd);
				exit(0);
			} //fine figlio

			close(connfd);
		} //if listen

	} /* ciclo for della select */

	/* NEVER ARRIVES HERE */
	exit(0);
} //main
