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

 int main(int argc, char * argv[]) {
     int sd, nread, nwrite, port;
     struct hostent * host;
     struct sockaddr_in serverAddress;

    char nomeDirettorio[DIM_BUFF];
    char suffisso[DIM_BUFF];

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
     /* ciclo di accettazione di richieste di file ------- */

     printf("Richieste fino a EOF\n");
     
     while (1) {

         //Devo richiedere all'utente: nome direttorio e suffisso

		//Richiedo nome dir e suffisso:
		printf("Inserisci nome dir: \n");

		if(gets(nomeDirettorio) == NULL){
			//EOF
			printf("Ricevuto EOF: esco\n");
			return 0;
		}

		printf("Inserisci il suffisso: \n");

		if(gets(suffisso) == NULL){
			//EOF
			printf("Ricevuto EOF: esco\n");
			return 0;
		}

        //Invio al server

        if(write(sd, nomeDirettorio, sizeof(char)*(strlen(nomeDirettorio)+1)) < 0){
            perror("write");
            break;
        }
        
        if(write(sd, suffisso, sizeof(char)*(strlen(suffisso)+1)) < 0){
            perror("write");
            break;
        }

        printf("Richiesta inviata... \n");


        //Ricezione numero files
        int numeroFilesNetwork;
        int numeroFiles;


        if (read(sd, &numeroFilesNetwork, sizeof(int))<0)
        {
            perror("read");      
            break;
        }

        numeroFiles = ntohl(numeroFilesNetwork);


        if(numeroFiles < 0){
            printf("Il server ha riscontrato un errore: %d\n", numeroFiles);
            break;
        }

        for(int i = 0; i < numeroFiles; i++){
            //Richiesta nome file e lunghezza

            char nomeFile[DIM_BUFF];
            int lengthNomeFile = 0;

			while((nread = read(sd, nomeFile+lengthNomeFile, sizeof(char))) > 0 && lengthNomeFile < DIM_BUFF){
				if(nomeFile[lengthNomeFile] == '\0'){
					break;
				}
				lengthNomeFile++;
			}

            //Richiesta lunghezza
            int lunghezzaNetwork;
            int lunghezza;


            if (read(sd, &lunghezzaNetwork, sizeof(int))<0)
            {
                perror("read");      
                break;
            }

            lunghezza = ntohl(lunghezzaNetwork);
            
            if(lunghezza < 0){
                //Errore server
                printf("Il server ha riscontrato un errore (%d) nella lettura del file %s\n", lunghezza, nomeFile);
                continue;
            }

            //Apro il file e lo salvo.
            char nextCh;
            int fdWrite = open(nomeFile, O_CREAT | O_TRUNC | O_WRONLY, 0644);

            if(fdWrite < 0){
                perror("errore apertura file scrittura");
                //Comunque devo continuare a leggere a vuoto
                for(int i = 0; i < lunghezza; i++) read(sd, &nextCh, sizeof(char));
            }

            for(int i = 0; i < lunghezza; i++){
                nread = read(sd, &nextCh, sizeof(char));
                if(nread > 0){
                    if(write(fdWrite, &nextCh, sizeof(char)) < 0){
                        perror("write file download");
                        exit(1);
                    }
                }else if(nread == 0) {
                    //EOF:
                    printf("Il server ha lanciato EOF\n");

                    exit(1);
                }else{
                    perror("read");      
                    break;
                }
            }

            close(fdWrite);
        }
     } //while

     printf("\nClient Stream: termino...\n");

     // Chiusura socket
     close(sd);

     return 0;
 }