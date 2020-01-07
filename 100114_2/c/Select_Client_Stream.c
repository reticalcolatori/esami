 /* ---- VERSIONE 2 -------------------------------------------- */
 /* 	Una UNICA connessione 
  *	-- LA CREAZIONE DELLA SOCKET FUORI DEL CICLO -- */

 #include <stdio.h>

 #include <stdlib.h>

 #include <string.h>

 #include <unistd.h>

 #include <fcntl.h>

 #include <sys/types.h>

 #include <sys/socket.h>

 #include <netinet/in.h>

 #include <netdb.h>

 #define DIM_BUFF 100
 #define STRING_LENGTH 30

typedef struct
{
  	char descrizione [STRING_LENGTH];
  	char tipo [STRING_LENGTH];
	char data [STRING_LENGTH];  
  	char luogo [STRING_LENGTH];
	int disponibilita;
	int prezzo;
} Riga;

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

 int main(int argc, char * argv[]) {
     int sd, nread, nwrite, port;
     struct hostent * host;
     struct sockaddr_in serverAddress;

    char tipo[STRING_LENGTH];
    char luogo[STRING_LENGTH];


    int i;


     /* CONTROLLO ARGOMENTI ---------------------------------- */
     if (argc != 3) {
         printf("Error:%s serverAddress serverPort\n", argv[0]);
         exit(1);
     }

     printf("Client avviato\n");

     /* PREPARAZIONE INDIRIZZO SERVER ----------------------------- */
     memset((char * ) & serverAddress, 0, sizeof(struct sockaddr_in));
     serverAddress.sin_family = AF_INET;

     host = gethostbyname(argv[1]);
     if (host == NULL) {
         printf("%s not found in /etc/hosts\n", argv[1]);
         exit(2);
     }

     //Controllo secondo argomento --> numero di porta intero
     nread = 0;
     while (argv[2][nread] != '\0') {
         if (argv[2][nread] < '0' || argv[2][nread] > '9') {
             printf("Secondo argomento non intero\n");
             exit(2);
         }
         nread++;
     }
     port = atoi(argv[2]);
     if (port < 1024 || port > 65535) {
         printf("Porta non corretta, range numerico sbagliato\n");
         exit(3);
     }

     serverAddress.sin_addr.s_addr = ((struct in_addr * )(host->h_addr_list[0]))->s_addr;
     serverAddress.sin_port = htons(atoi(argv[2]));

     /* CREAZIONE E CONNESSIONE SOCKET (BIND IMPLICITA) ----------------- */
     sd = socket(AF_INET, SOCK_STREAM, 0);
     if (sd < 0) {
         perror("apertura socket ");
         exit(3);
     }
     printf("Creata la socket sd=%d\n", sd);

     if (connect(sd, (struct sockaddr * ) & serverAddress, sizeof(struct sockaddr)) < 0) {
         perror("Errore in connect");
         exit(4);
     }
     printf("Connect ok\n");

     /* CORPO DEL CLIENT: */

     printf("Inserisci tipo per procedere, EOF per terminare\n");
     
     while (gets(tipo) != NULL) {

        //Checks sul tipo
        if(strcmp(tipo, "Concerto") != 0 && strcmp(tipo, "Calcio") != 0 && strcmp(tipo, "Formula1") != 0){
            printf("Tipo non valido\n");
            printf("Inserisci tipo per procedere, EOF per terminare\n");
        }

        printf("Inserisci il luogo:\n");

        if(gets(luogo) == NULL){
            //EOF:
            printf("Ricevuto EOF Esco\n");
            break;
        }

        //Invio i dati.
        if((nwrite=write(sd, tipo, sizeof(char)*(strlen(tipo)+1))) < 0){
            //Scrittura fallita
            perror("Write tipo");
            shutdown(sd, SHUT_WR);
            shutdown(sd, SHUT_RD);
            close(sd);
        }

        //Invio i dati.
        if((nwrite=write(sd, luogo, sizeof(char)*(strlen(luogo)+1))) < 0){
            //Scrittura fallita
            perror("Write luogo");
            shutdown(sd, SHUT_WR);
            shutdown(sd, SHUT_RD);
            close(sd);
        }

        //Ricezione risposta.

        int resultLengthNetwork;

        nread = read(sd, &resultLengthNetwork, sizeof(int));

        if(nread == 0){
            printf("Ricevuto EOF Esco\n");
            shutdown(sd, SHUT_RD);
            break;
        }

        int resultLength = ntohl(resultLengthNetwork);
        Riga riga;

        for(i=0; i < resultLength; i++){
            nread = read(sd, &riga, sizeof(Riga));

            if(nread == 0){
                printf("Ricevuto EOF Esco\n");
                shutdown(sd, SHUT_RD);
                shutdown(sd, SHUT_WR);
                close(sd);
                return 0;
            }

            if(nread < 0){
                perror("read riga");
                exit(1);
            }

            stampa(riga);
        }


        printf("Inserisci tipo per procedere, EOF per terminare\n");

     } //while

    printf("\nClient Stream: termino...\n");

    // Chiusura socket
    shutdown(sd, SHUT_WR);
    shutdown(sd, SHUT_RD);
    close(sd);

    return 0;

 }