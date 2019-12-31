//Innocenti Mattia 0000825046 NumeroCompito

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#define LENGTH 25
#define LENGTH_FILE_NAME 20
#define N_ROW 10


typedef struct
{    
    char tipo[LENGTH];
    char carbone;
} Request;

typedef struct
{
	char id[LENGTH];
} ReturnStreamSocket;

int main(int argc, char *argv[])
{
    int sd, nread, port;
    char c, ok, nome_file[LENGTH_FILE_NAME];
    struct hostent *host;
    struct sockaddr_in serverAddress;  

    Request req;
    ReturnStreamSocket returnId[N_ROW];

    /* CONTROLLO ARGOMENTI ---------------------------------- */
    if(argc!=3)
    {
        printf("Error:%s serverAddress serverPort\n", argv[0]);
        exit(1);
    }
  
    printf("Client avviato\n");


    /* PREPARAZIONE INDIRIZZO SERVER ----------------------------- */
    memset((char *)&serverAddress, 0, sizeof(struct sockaddr_in));
    serverAddress.sin_family = AF_INET;

    host = gethostbyname(argv[1]);
    if (host == NULL)
    {
        printf("%s not found in /etc/hosts\n", argv[1]);
        exit(2);
    }

    //Controllo secondo argomento --> numero di porta intero
    nread = 0;
    while(argv[2][nread] != '\0')
    {
        if(argv[2][nread] < '0' || argv[2][nread] >'9')
        {
            printf("Secondo argomento non intero\n");
            exit(2);
        }
        nread++;
    }
    port = atoi(argv[2]);
    if (port < 1024 || port > 65535)
    {
        printf("Porta non corretta, range numerico sbagliato\n");
        exit(3);
    }

  
    serverAddress.sin_addr.s_addr=((struct in_addr*) (host->h_addr))->s_addr;
    serverAddress.sin_port = htons(atoi(argv[2]));


    /* CREAZIONE E CONNESSIONE SOCKET (BIND IMPLICITA) ----------------- */
    sd = socket(AF_INET, SOCK_STREAM, 0);
    if (sd < 0){
        perror("apertura socket ");
        exit(3);
    }
    printf("Creata la socket sd = %d\n", sd);

    if (connect(sd,(struct sockaddr *) &serverAddress, sizeof(struct sockaddr))<0) {
        perror("Errore in connect");
        exit(4);
    }
    printf("Connect ok\n");


    /* CORPO DEL CLIENT: */
    /* ciclo di accettazione di richieste di file ------- */

    printf("Richieste di file fino alla fine del file di input\n");
    printf("Qualsiasi tasto per procedere, EOF per terminare\n");
    printf("Inserisci il modello degli sci che vorresti noleggiare:\n");
    
    char consumer[256];
    char modelloRichiesta[25];

    //leggo il tipo e appena entro verifico che sia o celiaco o normale

    while (gets(modelloRichiesta)) {


/* SE OCCORRONO PIÙ PARAMETRI E CREARE UNA STRUTTURA PER CHIEDERLA AL SERVER DECOMMENTA QUESTO BLOCCO!
        //controllo sul tipo della calza
        if(strcmp(req.tipo, "Celiaco") != 0 && strcmp(req.tipo, "Normale") != 0){
            printf("Tipo non consentito. Inserisci il tipo di calza (Normale/Celiaco):\n");
            continue;
        }

        do {
            printf("Inserisci indicatore carbone (S/N): \n");
            req.carbone = getchar();

            gets(consumer);
        } while (req.carbone != 'N' && req.carbone != 'S');


*/
        if (write(sd, &modelloRichiesta, sizeof(modelloRichiesta))<0) {
            perror("write");
            break;
        }
        printf("Richiesta inviata... \n");

        if (read(sd, &returnId, sizeof(returnId))<0) {
            perror("read");      
            break;
        }

        for(int i = 0; i < N_ROW && (strcmp(returnId[i].id, "L") != 0); i++){
            printf("ID = %s\n", returnId[i].id);
        }

        printf("Inserisci il modello di sci che intendi noleggiare:\n");

    }//while

    printf("\nClient Stream: termino...\n");
  
    // Chiusura socket
    close(sd);

    exit(0);
 
}