/************************************
 *     Select_Client_Datagram.c      *
 ************************************/

/******** #INCLUDE ********/
#include <stdio.h>

#include <stdlib.h>

#include <unistd.h>

#include <sys/types.h>

#include <sys/socket.h>

#include <netinet/in.h>

#include <netdb.h>

#include <string.h>

#include <dirent.h>

#include <fcntl.h>

/******** #DEFINE *******/
#define DIM_BUFF 100

/*** STRUTTURA DA INVIARE ATTRAVERO LA SOCKET DATAGRAM **/
typedef struct {
    char nomeFile[DIM_BUFF];
	char carattereSostitutivo;
}
RequestDatagram;

//MAIN
int main(int argc, char ** argv) {
    struct hostent * host;
    struct sockaddr_in clientaddr, servaddr;
    int sd, len, port, num, ris;
    int nbyte_recv, nbyte_send;

    //Variabili che cambiano
    int condition, ris;
    RequestDatagram req;

	char input[DIM_BUFF];

    /* CONTROLLO ARGOMENTI ---------------------------------- */
    if (argc != 3) {
        printf("Error:%s serverAddress serverPort\n", argv[0]);
        exit(1);
    }

    /* INIZIALIZZAZIONE INDIRIZZO CLIENT E SERVER --------------------- */
    /* ----- Inizializzazione clientaddr -----------------*/
    memset((char * ) & clientaddr, 0, sizeof(struct sockaddr_in));
    clientaddr.sin_family = AF_INET;

    clientaddr.sin_addr.s_addr = INADDR_ANY;
    /*
    * host=gethostbyname("localhost");
    clientaddr.sin_addr.s_addr=((struct in_addr *)(host->h_addr))->s_addr;
    if( clientaddr.sin_addr.s_addr == INADDR_NONE )
    {
		perror("Bad address");
		exit(2);
    }*/

    clientaddr.sin_port = 0;

    /* ----- Inizializzazione servaddr -----------------*/
    memset((char * ) & servaddr, 0, sizeof(struct sockaddr_in));
    servaddr.sin_family = AF_INET;
    host = gethostbyname(argv[1]);
    if (host == NULL) {
        printf("%s not found in /etc/hosts\n", argv[1]);
        exit(2);
    }

    // Controllo che la porta sia un intero
    num = 0;
    while (argv[2][num] != '\0') {
        if ((argv[2][num] < '0') || (argv[2][num] > '9')) {
            printf("Secondo argomento non intero\n");
            printf("Error:%s serverAddress serverPort\n", argv[0]);
            exit(2);
        }
        num++;
    }

    // Controllo che la porta sia nel range
    port = atoi(argv[2]);
    if (port < 1024 || port > 65535) {
        printf("Error: porta non valida!\n");
        printf("1024 <= port <= 65535\n");
        exit(2);
    } else {
        servaddr.sin_addr.s_addr = ((struct in_addr * )(host->h_addr_list[0]))->s_addr;
        servaddr.sin_port = htons(atoi(argv[2]));
    }
    printf("Client Datagram avviato\n");

    /******* CREAZIONE DELLA SOCKET *******/
    //Creazione:
    sd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sd < 0) {
        perror("apertura socket");
        exit(3);
    }
    printf("Creata la socket sd=%d\n", sd);

    // Bind socket a una porta scelta dal sistema
    if (bind(sd, (struct sockaddr * ) & clientaddr, sizeof(clientaddr)) < 0) {
        perror("bind socket ");
        exit(1);
    }
    printf("Client: bind socket ok, alla porta %i\n", clientaddr.sin_port);

    /******* CORPO DEL CLIENT *******/
    //Ciclo di accettazione di richieste da utente
    printf("\n************************************\n");

    while (1) {
        
		//Richiedo nome file e suffisso:
		printf("Inserisci nome file: \n");

		if(gets(req.nomeFile) == NULL){
			//EOF
			printf("Ricevuto EOF: esco\n");
			return 0;
		}

		printf("Inserisci il carattere di rimpiazzo: \n");

		if(gets(input) == NULL){
			//EOF
			printf("Ricevuto EOF: esco\n");
			return 0;
		}

		req.carattereSostitutivo = input[0];

        /*RICHIESTA DI OPERAZIONE*/
        len = sizeof(servaddr);

        nbyte_send = sendto(sd, & req, sizeof(RequestDatagram), 0, (struct sockaddr * ) & servaddr, len);
        if (nbyte_send < 0) {
            perror("Errore sendto");
            //Se questo invio fallisce il client torna all'inizio del ciclo
            continue;
        }

        /*RICEZIONE DEL RISULTATO*/
        printf("\n************************************\n");
        printf("Attesa della risposta ...\n");

        len = sizeof(servaddr);
        nbyte_recv = recvfrom(sd, & ris, sizeof(ris), 0, (struct sockaddr * ) & servaddr, & len);
        if (nbyte_recv < 0) {
            perror("Errore recvfrom");
            //Se questo invio fallisce il client torna all'inizio del ciclo
            continue;
        }

        //Manca lato server
        if (((int) ntohl(ris)) < 0) {
            printf("File non trovato lato server. \n");
        } else //lo trova stampo il risultato
        {
            ris = (int) ntohl(ris);
            printf("Esito: %i\n", ris);
            if (ris < 0)
                printf("Operazione non eseguita\n");
            if (ris >= 0)
                printf("Operazione eseguita con successo: %d\n", ris);
        }

    } //fine ciclo WHILE

    printf("\nClient: termino...\n");
    close(sd);
    exit(0);

} //main