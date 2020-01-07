/************************************
 *     Select_Server.c               *
 ************************************/

/******** #INCLUDE ********/
#include <stdio.h>

#include <stdlib.h>

#include <string.h>

#include <unistd.h>

#include <signal.h>

#include <errno.h>

#include <fcntl.h>

#include <dirent.h>

#include <sys/select.h>

#include <sys/socket.h>

#include <sys/wait.h>

#include <sys/time.h>

#include <sys/types.h>

#include <netinet/in.h>

#include <netdb.h>

/*** #DEFINE **/
#define DIM_BUFF 100
#define max(a, b)((a) > (b) ? (a) : (b))

#define STRING_LENGTH 30
#define TABLEDIM 5

/*** STRUTTURA DA INVIARE ATTRAVERO LA SOCKET DATAGRAM **/

typedef struct {
    int prezzoMassimo;
}
RequestDatagram;

/********************************************************/

typedef struct
{
  	char descrizione [STRING_LENGTH];
  	char tipo [STRING_LENGTH];
	char data [STRING_LENGTH];  
  	char luogo [STRING_LENGTH];
	int disponibilita;
	int prezzo;
} Riga;

static Riga tabella[TABLEDIM];

typedef struct {
    int listaLength;
    Riga lista[TABLEDIM];
}
ResponseDatagram;

void stampa()
{
	int i,j;
	
	printf("\ndescrizione\ttipo\tdata\tluogo\tdisponibilità\tprezzo");
	printf("\n");

	for(i=0;i<TABLEDIM;i++)
	{
		printf("\n%s\t%s\t%s\t%s\t%d\t%d", tabella[i].descrizione, 
						tabella[i].tipo, 
						tabella[i].data,
						tabella[i].luogo,
						tabella[i].disponibilita,
						tabella[i].prezzo);
	}

	printf("\n");
}


/********************************************************/

void gestore(int signo) {
    int stato;
    printf("esecuzione gestore di SIGCHLD\n");
    wait( & stato);
}

/********************************************************/

int main(int argc, char ** argv) {
    int listenfd, connfd, udpfd, nready, nread, maxfdp1;
    const int on = 1; //--> usata in setsockopt
    int len, nwrite, num, esito, port;
    struct sockaddr_in clientaddr, servaddr;
    struct hostent * hostTcp, * hostUdp;
    fd_set rset;

    int i;

	char buff[DIM_BUFF]; //tipi di risposte

    RequestDatagram reqData;
    ResponseDatagram resData;

    /*** ----  Controllo argomenti ---- **/
    if (argc != 2) {
        printf("Error: %s port\n", argv[0]);
        exit(1);
    }
    // Controllo che la porta sia un intero
    num = 0;
    while (argv[1][num] != '\0') {
        if ((argv[1][num] < '0') || (argv[1][num] > '9')) {
            printf("Argomento non intero\n");
            exit(2);
        }
        num++;
    }

    // Controllo che la porta sia nel range
    port = atoi(argv[1]);
    if (port < 1024 || port > 65535) {
        printf("Error: porta non valida!\n");
        printf("1024 <= port <= 65535\n");
        exit(2);
    }
    printf("Server avviato\n");

    // Qui eventuale inizializzazione della struttura dati del server
    
	for(i=0;i<TABLEDIM;i++)
	{
		strcpy(tabella[i].descrizione,"L");
		strcpy(tabella[i].tipo,"L");
		strcpy(tabella[i].data,"L");
		strcpy(tabella[i].luogo,"L");
		tabella[i].disponibilita = -1;
		tabella[i].prezzo = -1;
	}

	strcpy(tabella[0].descrizione,"String");
	strcpy(tabella[0].tipo,"Concerto");
	strcpy(tabella[0].data,"11/02/2014");
	strcpy(tabella[0].luogo,"Verona");
	tabella[0].prezzo = 40;
	tabella[0].disponibilita = 40;

	strcpy(tabella[1].descrizione,"Giuventius-Bolo");
	strcpy(tabella[1].tipo,"Calcio");
	strcpy(tabella[1].data,"15/05/2014");
	strcpy(tabella[1].luogo,"Tornio");
	tabella[1].prezzo = 150;
	tabella[1].disponibilita = 21;
	
	stampa();
    

    //**  CREAZIONE SOCKET TCP **/
    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    if (listenfd < 0) {
        perror("apertura socket TCP ");
        exit(1);
    }
    printf("Creata la socket TCP d'ascolto, fd=%d\n", listenfd);

    // **  INIZIALIZZAZIONE INDIRIZZO SERVER E BIND **/
    memset((char * ) & servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = INADDR_ANY;
    servaddr.sin_port = htons(port);

    if (setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) < 0) {
        perror("set opzioni socket TCP");
        exit(2);
    }
    printf("Set opzioni socket TCP ok\n");
    if (bind(listenfd, (struct sockaddr * ) & servaddr, sizeof(servaddr)) < 0) {
        perror("bind socket TCP");
        exit(3);
    }
    printf("Bind socket TCP ok\n");

    if (listen(listenfd, 5) < 0) {
        perror("listen");
        exit(4);
    }
    printf("Listen ok\n");

    // CREAZIONE SOCKET UDP
    udpfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (udpfd < 0) {
        perror("apertura socket UDP");
        exit(5);
    }
    printf("Creata la socket UDP, fd=%d\n", udpfd);

    // INIZIALIZZAZIONE INDIRIZZO SERVER E BIND
    memset((char * ) & servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = INADDR_ANY;
    servaddr.sin_port = htons(port);

    if (setsockopt(udpfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) < 0) {
        perror("set opzioni socket UDP");
        exit(6);
    }
    printf("Set opzioni socket UDP ok\n");

    if (bind(udpfd, (struct sockaddr * ) & servaddr, sizeof(servaddr)) < 0) {
        perror("bind socket UDP");
        exit(7);
    }
    printf("Bind socket UDP ok\n");

    // AGGANCIO GESTORE PER EVITARE FIGLI ZOMBIE
    signal(SIGCHLD, gestore);

    // PULIZIA E SETTAGGIO MASCHERA DEI FILE DESCRIPTOR
    FD_ZERO( & rset);
    maxfdp1 = max(listenfd, udpfd) + 1;

    // CICLO DI RICEZIONE EVENTI DALLA SELECT
    for (;;) {
        FD_SET(listenfd, & rset);
        FD_SET(udpfd, & rset);

        if ((nready = select(maxfdp1, & rset, NULL, NULL, NULL)) < 0) {
            if (errno == EINTR) continue;
            else {
                perror("select");
                exit(8);
            }
        }

        //CASO 1: GESTIONE RICHIESTE DA SOCKET DATAGRAM
        if (FD_ISSET(udpfd, & rset)) {
            printf("Server: ricevuta richiesta UDP da client: \n");
            hostUdp = gethostbyaddr((char * ) & clientaddr.sin_addr, sizeof(clientaddr.sin_addr), AF_INET);
            if (hostUdp == NULL) {
                printf("client host information not found\n");
            } else {
                printf("Operazione richiesta da: %s %i\n", hostUdp->h_name, (unsigned) ntohs(clientaddr.sin_port));
            }

            len = sizeof(struct sockaddr_in);
            if (recvfrom(udpfd, &reqData, sizeof(RequestDatagram), 0, (struct sockaddr * ) & clientaddr, & len) < 0) {
                perror("Recvfrom");
                continue;
            }

            //Gestisco in modo seriale: la richiesta non è troppo esosa a termini computazionali, se la tabella rimane piccola.
            reqData.prezzoMassimo = ntohl(reqData.prezzoMassimo);


			printf("Richiesto dal client...\n");
			printf("Prezzo massimo: %d\n", reqData.prezzoMassimo);

			//Devo cercare tutti gli eventi con posti disponibili avente prezzo minore di przMax
            resData.listaLength = 0;

            for(i = 0; i < TABLEDIM; i++){
                if(tabella[i].prezzo <= reqData.prezzoMassimo && tabella[i].disponibilita > 0){

                    strcpy(resData.lista[resData.listaLength].descrizione, tabella[i].descrizione);
                    strcpy(resData.lista[resData.listaLength].luogo, tabella[i].luogo);
                    strcpy(resData.lista[resData.listaLength].data, tabella[i].data);
                    strcpy(resData.lista[resData.listaLength].tipo, tabella[i].tipo);

                    resData.lista[resData.listaLength].prezzo = htonl(tabella[i].prezzo);
                    resData.lista[resData.listaLength].disponibilita = htonl(tabella[i].disponibilita);

                    resData.listaLength++;
                }
            }

            printf("Trovati %d eventi.\n", resData.listaLength);

            if (sendto(udpfd, &resData, sizeof(ResponseDatagram), 0, (struct sockaddr * ) & clientaddr, len) < 0) {
                perror("Sendto");
                continue;
            }

            printf("Operazione conclusa.\n");
        }

        //CASO 2: GESTIONE RICHIESTE DA SOCKET STREAM
        if (FD_ISSET(listenfd, & rset)) {
            len = sizeof(struct sockaddr_in);
            if ((connfd = accept(listenfd, (struct sockaddr * ) & clientaddr, & len)) < 0) {
                if (errno == EINTR)
                    continue;
                else {
                    perror("accept");
                    exit(9);
                }
            }

            /* SCHEMA UN PROCESSO FIGLIO PER OGNI OPERAZIONE CLIENTE */
            //figlio

			int childPid = fork();

            if (childPid == 0) {
                close(listenfd);
                //chi mi fa richiesta
                hostTcp = gethostbyaddr((char * ) & clientaddr.sin_addr, sizeof(clientaddr.sin_addr), AF_INET);
                printf("Dentro il figlio pid=%d\n", getpid());
                printf("Richiesta del client: %s\n", hostTcp->h_addr_list[0]);


                for(;;)
                {
                    //Ricavo tipo e luogo
                    char tipo[STRING_LENGTH];
                    int tipoLength = 0;
                    char luogo[STRING_LENGTH];
                    int luogoLength = 0;

                    while((nread = read(connfd, tipo+tipoLength, sizeof(char))) > 0){
                        if(tipo[tipoLength] == '\0'){
                            break;
                        }
                        tipoLength++;
                    }

                    if(nread == 0){
                        //EOF:
                        printf("%d) Ricevuto EOF: esco\n", getpid());
						shutdown(connfd, SHUT_RD);
						shutdown(connfd, SHUT_WR);
						close(connfd);
						exit(0);
                    }


                    if(nread < 0){
						perror("Read TCP Request: Tipo");
						exit(1);
					}

                    while((nread = read(connfd, luogo+luogoLength, sizeof(char))) > 0){
                        if(luogo[luogoLength] == '\0'){
                            break;
                        }
                        luogoLength++;
                    }

                    if(nread == 0){
                        //EOF:
                        printf("%d) Ricevuto EOF: esco\n", getpid());
						shutdown(connfd, SHUT_RD);
						shutdown(connfd, SHUT_WR);
						close(connfd);
						exit(0);
                    }


                    if(nread < 0){
						perror("Read TCP Request: Luogo");
						exit(1);
					}

                    printf("%d)Richiesta, la seguente..\n", getpid());
                    printf("Tipo: %s,\nLuogo: %s\n", tipo, luogo);


                    //Elaborazione della richiesta.
                    Riga result[TABLEDIM];
                    int resultLength = 0;

                    for(i = 0; i < TABLEDIM; i++){
                        if(strcmp(tabella[i].luogo, luogo) == 0 && strcmp(tabella[i].tipo, tipo) == 0){
                            strcpy(result[resultLength].descrizione, tabella[i].descrizione);
                            strcpy(result[resultLength].luogo, tabella[i].luogo);
                            strcpy(result[resultLength].data, tabella[i].data);
                            strcpy(result[resultLength].tipo, tabella[i].tipo);

                            result[resultLength].prezzo = htonl(tabella[i].prezzo);
                            result[resultLength].disponibilita = htonl(tabella[i].disponibilita);

                            resultLength++;
                        }
                    }

                    printf("Trovati %d eventi.\n", resultLength);
                    //Invio della risposta.
                    int resultLengthNetwork = htonl(resultLength);

                    //Invio della dimensione degli eventi.
                    if((nwrite=write(connfd, &resultLengthNetwork, sizeof(int))) < 0){
                        perror("Impossibile scrivere resultLength");
                        shutdown(connfd, SHUT_WR);
                        shutdown(connfd, SHUT_RD);
						close(connfd);
						exit(1);
                    }

                    //Invio delle righe
                    for(i = 0; i < resultLength; i++){
                        if((nwrite=write(connfd, result+i, sizeof(Riga))) < 0){
                            perror("Impossibile scrivere riga");
                            shutdown(connfd, SHUT_WR);
                            shutdown(connfd, SHUT_RD);
						    close(connfd);
                            exit(1);
                        }
                    }

                    printf("%d) Attendo nuova richiesta fino EOF\n", getpid());

                } //for

                printf("Figlio %i: chiudo connessione e termino\n", getpid());
                close(connfd);
                exit(0);
            } //fine figlio
			else if(childPid > 0){
				//Cose del padre.
			}else{
				//Errore nella fork:
				printf("Errore nella fork STREAM: %d\n", childPid);
			}

			close(connfd);
            
        } //if listen

    } /* ciclo for della select */

    /* NEVER ARRIVES HERE */
    exit(0);
} //main