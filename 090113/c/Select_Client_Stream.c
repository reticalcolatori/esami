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
#define LENGTH 25

int main(int argc, char *argv[])
{
    int sd, nread, nwrite, port;
    struct hostent *host;
    struct sockaddr_in serverAddress;  

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
    nread =0;
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
    if (port <1024 || port >65535)
    {
        printf("Porta non corretta, range numerico sbagliato\n");
        exit(3);
    }

  
    serverAddress.sin_addr.s_addr=((struct in_addr*) (host->h_addr_list[0]))->s_addr;
    serverAddress.sin_port = htons(atoi(argv[2]));


    /* CREAZIONE E CONNESSIONE SOCKET (BIND IMPLICITA) ----------------- */
    sd=socket(AF_INET, SOCK_STREAM, 0);
    if (sd <0)
      {perror("apertura socket "); exit(3);}
    printf("Creata la socket sd=%d\n", sd);

    if (connect(sd,(struct sockaddr *) &serverAddress, sizeof(struct sockaddr))<0)
      {perror("Errore in connect"); exit(4);}
    printf("Connect ok\n");


    /* CORPO DEL CLIENT: */

    printf("Inserisci modello sci ");
    printf("EOF per terminare:\n");

    char modello[LENGTH];

    while (gets(modello) != 0)
    {
      
      //Invio al server il modello
      nwrite = write(sd, modello, sizeof(char) * (strlen(modello)+1));
      if(nwrite < 0){
        perror("impossibile inviare il modello al server.");
        printf("Inserisci modello sci ");
        printf("EOF per terminare:\n");
        continue;
      }

      //Ricevo i dati
      //Prima ricevo il numero di sci trovati

      int sciTrovati = -1;

      nread = read(sd, &sciTrovati, sizeof(int));

      if(nread < 0){
        perror("Impossibile leggere risposta.");
        printf("Inserisci modello sci ");
        printf("EOF per terminare:\n");
        continue;
      }

      sciTrovati = ntohl(sciTrovati);

      for(int i = 0; i < sciTrovati;i++){

        char id[LENGTH];
        int k = 0;

        while((nread=read(sd,id+k,sizeof(char))) > 0){
          if(id[k++] == '\0'){
							break;
					}
        }

        if(nread < 0){
          perror("Impossibile leggere gli sci");
          break;
        }

        printf("Sci: %s\n", id);
      }


      printf("Inserisci modello sci ");
      printf("EOF per terminare:\n");
    }//while

    printf("\nClient Stream: termino...\n");

    shutdown(sd, SHUT_WR);
    shutdown(sd, SHUT_RD);
  
    // Chiusura socket
    close(sd);

    exit(0);
 
}