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
#define STRING_LENGTH 30
#define TABLEDIM 5

/*** STRUTTURA DA INVIARE ATTRAVERO LA SOCKET ***/
typedef struct {
    int prezzoMassimo;
}
RequestDatagram;

typedef struct
{
  	char descrizione [STRING_LENGTH];
  	char tipo [STRING_LENGTH];
	char data [STRING_LENGTH];  
  	char luogo [STRING_LENGTH];
	int disponibilita;
	int prezzo;
} Riga;

typedef struct {
    int listaLength;
    Riga lista[TABLEDIM];
}
ResponseDatagram;

void stampa(Riga riga)
{
	int i,j;
	
	printf("\ndescrizione\ttipo\tdata\tluogo\tdisponibilità\tprezzo");
	printf("\n");

		printf("\n%s\t%s\t%s\t%s\t%d\t%d", riga.descrizione, 
						riga.tipo, 
						riga.data,
						riga.luogo,
						riga.disponibilita,
						riga.prezzo);
	
	printf("\n");
}

//MAIN
int main(int argc, char ** argv) {
    struct hostent * host;
    struct sockaddr_in clientaddr, servaddr;
    int sd, len, port, num;
    int nbyte_recv, nbyte_send;

    char input[STRING_LENGTH];

    //Variabili che cambiano
    RequestDatagram req;
    ResponseDatagram res;

    int i;

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
        printf("Inserisci prezzo massimo, (ctrl+d per terminare): \n");
        
        if(gets(input) == NULL){
            //EOF
            printf("Inserito EOF\n");
            break;
        }

        num = 0;
        while (input[num] != '\0') {
            if ((input[num] < '0') || (input[num] > '9')) {
                
                num = -1;
                break;
            }
            num++;
        }

        if(num == -1){
            printf("prezzo non intero\n");
            continue;
        }

        req.prezzoMassimo = atoi(input);

        if(req.prezzoMassimo < 0){
            printf("Prezzo non valido\n");
            continue;
        }

        req.prezzoMassimo = htonl(req.prezzoMassimo);

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
        nbyte_recv = recvfrom(sd, &res, sizeof(ResponseDatagram), 0, (struct sockaddr * ) & servaddr, & len);
        if (nbyte_recv < 0) {
            perror("Errore recvfrom");
            //Se questo invio fallisce il client torna all'inizio del ciclo
            continue;
        }

        res.listaLength = ntohl(res.listaLength);

        for(i=0; i < res.listaLength; i++){
            stampa(res.lista[i]);
        }

    } //fine ciclo WHILE

    printf("\nClient: termino...\n");
    close(sd);
    exit(0);

} //main