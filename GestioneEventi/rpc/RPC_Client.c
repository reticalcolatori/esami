#include <stdio.h>
#include <rpc/rpc.h>
#include <stdlib.h>
#include <string.h>

#include "RPC_xFile.h"

#define LENGTH 256

int main(int argc, char *argv[])
{
	CLIENT *cl;/*Gestore del trasporto*/
	char *server; /*Nome HOST*/
	
	int *ris;
	char input[LENGTH];

    //Paramentri da richiedere all'utente da passare come parametro per le RPC
	char *descrizione;
    char *tipo;
    char *luogo;
    char *data;
    int gg, mm, aaaa, disponibilita, prezzo;
    descrizione = (char *) malloc(25*sizeof(char));
    tipo = (char *) malloc(10*sizeof(char));
    luogo = (char *) malloc(20*sizeof(char));
    data = (char *) malloc(11*sizeof(char));


    int nBigliettiAcquistare;


   // Operandi op;
	int numero;
	// Qui eventuali altre variabili di strutture utilizzate
	// Input in;
	// Aggiunta aggiunta;
	// Lista* lista;
	
	/********** CONTROLLI ***************/	
	if (argc != 2)
	{
		fprintf(stderr, "Usage: %s host\n", argv[0]);
		exit(1);
	}

	server = argv[1];

	// Possibilità di cambiare il protocollo di trasporto
	cl = clnt_create(server, FILEPROG, FILEVERS, "udp");
	if (cl == NULL)
	{
		clnt_pcreateerror(server);
		exit(1);
	}

	// Interazione utente
	printf("\nInserire: \n1 Aggiungi evento \n2 Compra biglietti evento \n ^D per terminare: ");

	while(gets(input))
	{
		if(strcmp(input, "1")!=0 && strcmp(input, "2")!=0)
		{
			printf("Servizio non disponibile!\n");
		}
		
		if(strcmp(input, "1") == 0)
		{ //conta le occorrenze di una linea
		    printf("\nScelta: aggiungi evento\n");

            printf("Inserisci la descrizione dell'evento: \n");
			gets(descrizione);

            do {
                printf("Inserisci il tipo dell'evento: \n");
                gets(tipo);
            } while((strcmp(tipo, "Concerto") != 0) && (strcmp(tipo, "Calcio") != 0) && (strcmp(tipo, "Formula1") != 0));

            printf("Inserisci la data dell'evento: \n");
			gets(data);

            char *nextToken = strtok(data, "/");
            
            gg = atoi(nextToken);
            nextToken = strtok(NULL, "/");

            mm = atoi(nextToken);
            nextToken = strtok(NULL, "/");

            aaaa = atoi(nextToken);

            printf("Stampa verifica split stringa data %d/%d/%d\n", gg, mm, aaaa);

            printf("Inserisci il luogo dell'evento: \n");
			gets(luogo);

            char c;

            do {
                printf("Inserisci la disponibilità dell'evento: \n");

                while (scanf("%i", &disponibilita) != 1){
                    do
                    {
                        printf("%c ", c);
                    } while (c!= '\n');
                }

            } while(disponibilita < 0);

            do {
                printf("Inserisci il prezzo del biglietto dell'evento: \n");

                while (scanf("%i", &prezzo) != 1){
                    do
                    {
                        printf("%c ", c);
                    }while (c!= '\n');
                }
            } while(prezzo < 0);

            Inserimento ins;
            strcpy(ins.descrizione, descrizione);
            strcpy(ins.tipo, tipo);
            ins.gg = gg;
            ins.mm = mm;
            ins.aaaa = aaaa;

            strcpy(ins.luogo, luogo);

            ins.disponibilita = disponibilita;
            ins.prezzo = prezzo;



		    // .....interagisco con l'utente per caricare la struttura dati da utilizzare
		    /*
		    		RICORDA COME SI FA LA MALLOC 

				   printf("Inserisci Primo Parametro: ");
				   gets(input);
				   op.primo_paramentro =(char*)malloc(strlen(input) + 1);
				   strcpy(op.primo_paramentro, input);
	

				RICORDA COME SI FA PER I NUMERI: 
				
				   printf("Inserisci Secondo parametro: ");
				   while (scanf("%i", &numero) != 1)
				   {
				      do
				      {
					      printf("%c ", c);
				      }while (c!= '\n');
				      printf("Inserisci Secondo Parametro: ");
				    }
				    op.secondo_paramentro = numero;
				    gets(input);
				
				OPPURE
				printf("Inserisci NUMERO: ");
				gets(input);
				numero = atoi(input);


		    */
		    // Invocazione remota dopo aver caricato la struttura dati
		    ris = inserimento_evento_1(&ins, cl);

		    // Controllo del risultato
		    if(ris == NULL) 
		    { 
			    clnt_perror(cl, server); 
			    exit(1); 
		    }
		    //Eventuale errore di logica del programma
		    if(*ris == -1) {  
			    printf("Problemi nell'inserimento del nuovo evento\n");
		    }
		    //Tutto ok      	
		    else {   
			    printf("Evento %s inserito con successo\n", descrizione);           
		    }
		    printf("\nFunzione1 conclusa\n");
		}
		
		if(strcmp(input, "2") == 0)
		{ //conta i file che iniziano con un prefisso da chiedere all'utente
        
		    printf("\nScelta: acquisto biglietti\n");

		    // .....interagisco con l'utente per caricare la struttura dati da utilizzare
            Acquisto acquisto;
        
            printf("Inserisci la descrione dell'evento per cui intendi acquistare i biglietti: \n");
			gets(descrizione);

            char c;

            do {
                printf("Inserisci il numero di biglietti da acquistare: \n");

                while (scanf("%i", &nBigliettiAcquistare) != 1){
                    do
                    {
                        printf("%c ", c);
                    } while (c!= '\n');
                }

            } while(nBigliettiAcquistare < 0);

            strcpy(acquisto.descrizione, descrizione);
            acquisto.nBiglietti = nBigliettiAcquistare;


		    // Invocazione remota dopo aver caricato la struttura dati
		    ris = acquista_biglietti_1(&acquisto, cl);

		    // Controllo del risultato
		    if(ris == NULL) 
		    { 
			    clnt_perror(cl, server); 
			    exit(1); 
		    }
		    //Eventuale errore di logica del programma
		    if(*ris == -1) {  
			    printf("Problemi nell'acquisto dei biglietti\n");
		    }
		    //Tutto ok      	
		    else {   
			    printf("acquistato %d biglietti per evento %s\n", nBigliettiAcquistare, descrizione);           
		    }
		    printf("\nFunzione2 conclusa\n");
            
		}
        gets();

	    printf("\nInserire: \n1 Aggiungi evento \n2 Compra biglietti evento \n ^D per terminare: ");
	}


/*
descrizione = (char *) malloc(25*sizeof(char));
    tipo = (char *) malloc(10*sizeof(char));
    luogo = (char *) malloc(20*sizeof(char));
    data = (char *) malloc(11*sizeof(char));
*/
    free(descrizione);
    free(tipo);
    free(data);
    free(luogo);

	// Libero le risorse distruggendo il gestore di trasporto
	clnt_destroy(cl);
	printf("Fine Client \n");
}