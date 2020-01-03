/************************************
 *     Select_Server.c               *
 ************************************/

//Luca Bartolomei 0000825005


//usato per le primitive opendir etc
#define _DEFAULT_SOURCE

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

#include <sys/stat.h>

#include <sys/socket.h>

#include <sys/wait.h>

#include <sys/time.h>

#include <sys/types.h>

#include <netinet/in.h>

#include <netdb.h>

/*** #DEFINE **/
#define DIM_BUFF 100

#define max(a, b)((a) > (b) ? (a) : (b))

/*** STRUTTURA DA INVIARE ATTRAVERO LA SOCKET DATAGRAM **/
typedef struct {
    char nomeFile[DIM_BUFF];
	char carattereSostitutivo;
}
RequestDatagram;

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

	char buff[DIM_BUFF]; //tipi di risposte
    int qualcosa; //tipi di wrapping per le risposte

    RequestDatagram req;

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
            if (recvfrom(udpfd, & req, sizeof(RequestDatagram), 0, (struct sockaddr * ) & clientaddr, & len) < 0) {
                perror("Recvfrom");
                continue;
            }

			//Preferisco gestire il carico in modo parallelo per evitare che il server sia troppo impegnato 
			//Nella gestione di file lunghi: dato che dovrò fare un file temporaneo e sostituizione.

			int childPid = fork();
			esito = -1;

			if(childPid == 0){
				//Figlio

				//Devo aprire il file e sostituire tutte le occorrenze 

				int udpFdRead = open(req.nomeFile, O_RDONLY);

				if(udpFdRead >= 0){
					//Devo aprire anche un file temporaneo

					char nomeFileTmp[DIM_BUFF+5];

					strcpy(nomeFileTmp, req.nomeFile);
					strcat(nomeFileTmp, ".tmp");

					int udpFdWrite = open(nomeFileTmp, O_CREAT | O_TRUNC | O_WRONLY, 0644);

					if(udpFdWrite >= 0){
						//Esecuzione
						//int nread, nwrite;
						char nextCh;
						esito = 0;

						while((nread = read(udpFdRead, &nextCh, sizeof(char))) > 0){
							if(nextCh == ' ') {
								nextCh = req.carattereSostitutivo;
								esito++;
							}
							
							if((nwrite = write(udpFdWrite, &nextCh, sizeof(char))) < 0){
								perror("Write UDP TMP");
								esito = -1;
								break;
							}
						}

						if(nread < 0){
							perror("Read UDP");
							esito = -1;
						}

						close(udpFdRead);
						close(udpFdWrite);

						//Sposto il file temporaneo in quello definitivo
						if(remove(req.nomeFile)){
							int udpFdWrite = open(req.nomeFile, O_CREAT | O_TRUNC | O_WRONLY, 0644);

							if(udpFdWrite >= 0){
								int udpFdRead = open(nomeFileTmp, O_RDONLY);

								if(udpFdRead >= 0){
									while((nread=read(udpFdRead, &nextCh, sizeof(char))) > 0){
										if((nwrite = write(udpFdWrite, &nextCh, sizeof(char))) < 0){
											perror("Write UDP ORIGINAL");
											esito = -1;
											break;
										}
									}
								}else{
									perror("Open UDP TMP Read");
									esito = -1;
								}
							}else{
								perror("Open UDP Write");
								esito = -1;
							}
						}else{
							printf("Impossibile rimuovere il file originale\n");
							esito = -1;
						}
					}else{
						perror("Open UDP TMP");
					}
				}else{
					perror("Open UDP");
				}

				printf("Esito: %d\n", esito);
				esito = ntohl(esito);

				if (sendto(udpfd, & esito, sizeof(int), 0, (struct sockaddr * ) & clientaddr, len) < 0) {
					perror("Sendto");
					continue;
				}

				printf("Operazione DATAGRAM conclusa: %d\n", getpid());

				//Exit
				exit(esito >= 0 ? 0 : 1);
			}else if(childPid > 0){
				//Padre
				printf("Gestione DATAGRAM in corso: %d", childPid);
			}else{
				//Errore
				//Errore nella fork:
				printf("Errore nella fork DATAGRAM: %d\n", childPid);

				printf("Esito: %d\n", esito);
				esito = ntohl(esito);

				if (sendto(udpfd, & esito, sizeof(int), 0, (struct sockaddr * ) & clientaddr, len) < 0) {
					perror("Sendto");
					continue;
				}

				printf("Operazione DATAGRAM conclusa. Errore fork\n");
			}           
            
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
					char nomeDirettorio[DIM_BUFF];
					int lengthNomeDirettorio = 0;
					//int nread, nwrite;

					//Richiedo nome direttorio: \n per terminare
					while((nread = read(connfd, nomeDirettorio+lengthNomeDirettorio, sizeof(char))) > 0 && lengthNomeDirettorio < DIM_BUFF){
						if(nomeDirettorio[lengthNomeDirettorio] == '\n'){
							//Taglio
							nomeDirettorio[lengthNomeDirettorio] = '\0';
							break;
						}

						lengthNomeDirettorio++;
					}

					if(nread == 0){
						//EOF esco.
						printf("%d) Ricevuto EOF: esco", getpid());
						shutdown(connfd, SHUT_RD);
						shutdown(connfd, SHUT_WR);
						close(connfd);
						exit(0);
					}

					if(nread < 0){
						perror("Read TCP Request: Nome Direttorio");
						exit(1);
					}

					//Richiesta suffisso
					char suffisso[DIM_BUFF];
					int lengthSuffisso = 0;

					//Richiedo suffisso: \n per terminare
					while((nread = read(connfd, suffisso+lengthSuffisso, sizeof(char))) > 0 && lengthSuffisso < DIM_BUFF){
						if(suffisso[lengthSuffisso] == '\n'){
							//Taglio
							suffisso[lengthSuffisso] = '\0';
							break;
						}

						lengthSuffisso++;
					}

					if(nread == 0){
						//EOF esco.
						printf("%d) Ricevuto EOF: esco", getpid());
						shutdown(connfd, SHUT_RD);
						shutdown(connfd, SHUT_WR);
						close(connfd);
						exit(0);
					}

					if(nread < 0){
						perror("Read TCP Request: Suffisso");
						exit(1);
					}

					printf("Richiesta, la seguente..");
					printf("Nome direttorio: %s\nSuffisso: %s\n", nomeDirettorio, suffisso);

					//Risposta: 
					//Numero files: negativo se errore
					//Per ogni file: nome file, lunghezza e contenuto

					//Devo cercare tutti i file avente suffisso x...
					int numeroFiles = 0;

					
					//Apro la cartella.
					DIR *myDir;
					struct dirent *currItem;

					myDir = opendir(nomeDirettorio);

					if(myDir == NULL){
						//Impossibile aprire cartella
						printf("%d)Impossibile aprire la cartella: %s", getpid(), nomeDirettorio);
						numeroFiles = -1;
					}else{
						//Cartella aperta

						while((currItem = readdir(myDir)) != NULL){
							if(currItem->d_type == DT_REG){
								//Se file regolare verifico il suffisso.
								char *token = strtok(currItem->d_name, ".");

								if(token != NULL){
									char *nextToken = token;
								
									//Ciclo fino all'ultimo pezzo
									while((nextToken=strtok(NULL, ".")) != NULL){
										token = nextToken;
									}
								}

								if(strcmp(token, suffisso) == 0){
									//Ho una corrispondenza:
									numeroFiles++;
								}
							}
						}

						closedir(myDir);

						//Invio il numero di files e apro di nuovo la cartella, sta volta per trasferire i file.

						int numeroFilesNetwork = htonl(numeroFiles);

						if((nwrite=write(connfd, &numeroFilesNetwork, sizeof(int))) < 0)
						{
							perror("Impossibile scrivere numeroFiles");
							exit(1);
						}

					
						myDir = opendir(nomeDirettorio);
						
						if(myDir == NULL){
							perror("Impossibile riaprire cartella");
							exit(1);
						}else{
							//Cartella aperta

							while((currItem = readdir(myDir)) != NULL){
								if(currItem->d_type == DT_REG){
									//Copio il nome in un buffer temporaneo per dopo.
									char nomeTmp[DIM_BUFF+5];
									strcpy(nomeTmp, currItem->d_name);
									strcat(nomeTmp, ".");
									strcat(nomeTmp, suffisso);

									//Se file regolare verifico il suffisso.
									char *token = strtok(currItem->d_name, ".");

									if(token != NULL){
										char *nextToken = token;
									
										//Ciclo fino all'ultimo pezzo
										while((nextToken=strtok(NULL, ".")) != NULL){
											token = nextToken;
										}
									}

									if(strcmp(token, suffisso) == 0){
										//Ho una corrispondenza:
										//Invio lunghezza:

										struct stat bufstat;

										if(stat(nomeTmp, &bufstat)){
											int size = (int)bufstat.st_size;//Ci vorrebbe un htonl a 64 bit

											int tcpFdRead = open(nomeTmp, O_RDONLY);

											if(tcpFdRead < 0)
											{
												//Non riesco ad aprire il file, invio una lunghezza negativa
												//Invio lunghezza negativa
												int erroreNetwork = htonl(-1);

												if((nwrite=write(connfd, &erroreNetwork, sizeof(int))) < 0)
												{
													perror("Impossibile scrivere length");
													exit(1);
												}

											}else{

												int sizeNetwork = htonl(size);

												if((nwrite=write(connfd, &sizeNetwork, sizeof(int))) < 0)
												{
													perror("Impossibile scrivere length");
													exit(1);
												}


												//Lettura
												char nextChar;
												while((nread=read(tcpFdRead, &nextChar, sizeof(char))) > 0){
													if((nwrite=write(connfd, &nextChar, sizeof(char))) < 0){
														perror("Impossibile scrivere file");
														exit(1);
													}
												}

												close(tcpFdRead);
											}
										}else{
											//Impossibile ricavare lunghezza file.
											//Invio lunghezza negativa
											int erroreNetwork = htonl(-1);

											if((nwrite=write(connfd, &erroreNetwork, sizeof(int))) < 0)
											{
												perror("Impossibile scrivere length");
												exit(1);
											}
										}
									}
								}
							}

							closedir(myDir);
						}


					}
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