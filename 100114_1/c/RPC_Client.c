/************************************
*           RPC_Client.c            *
************************************/

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
	
	Inserimento inserimento;
	Acquisto acquisto;

	int *ris;
	char input[LENGTH];

	
	/********** CONTROLLI ***************/	
	if (argc != 2)
	{
		fprintf(stderr, "Usage: %s host\n", argv[0]);
		exit(1);
	}

	server = argv[1];

	// Possibilità di cambiare il protocollo di trasporto
	cl = clnt_create(server, ESAMEPROG, ESAMEVERS, "udp");
	if (cl == NULL)
	{
		clnt_pcreateerror(server);
		exit(1);
	}

	// Interazione utente
	printf("\nInserire: \n1 INSERIMENTO_EVENTO \n2 ACQUISTA_BIGLIETTI \n ^D per terminare: ");

	while(gets(input) != NULL)
	{
		if(strcmp(input, "1")!=0 && strcmp(input, "2")!=0)
		{
			printf("Argomento ERRATO!\n");
		}
		
		if(strcmp(input, "1") == 0)
		{
		    printf("\nScelta: INSERIMENTO_EVENTO\n");

			//Richiesta di input all'utente


			printf("Inserisci la descrizione:\n");

			if(gets(inserimento.descrizione) == NULL){
				//EOF
				printf("Ricevuto EOF\n");
				return 0;
			}

			printf("Inserisci il tipo (Concerto, Calcio, Formula1):\n");

			if(gets(inserimento.tipo) == NULL){
				//EOF
				printf("Ricevuto EOF\n");
				return 0;
			}

			if(strcmp(inserimento.tipo, "Concerto") != 0 && strcmp(inserimento.tipo, "Calcio") != 0 && strcmp(inserimento.tipo, "Formula1") != 0){
				printf("Tipo non corretto\n");
				continue;
			}

			printf("Inserisci la data:\n");

			if(gets(inserimento.data) == NULL){
				//EOF
				printf("Ricevuto EOF\n");
				return 0;
			}

			printf("Inserisci il luogo:\n");

			if(gets(inserimento.luogo) == NULL){
				//EOF
				printf("Ricevuto EOF\n");
				return 0;
			}

			printf("Inserisci la disponibilità (>0):\n");

			if(gets(input) == NULL){
				//EOF
				printf("Ricevuto EOF\n");
				return 0;
			}

			//Check
			int num = 0;
			int ok = 1;
			while (input[num] != '\0') {
				if ((input[num] < '0') || (input[num] > '9')) {
					ok = 0;
					break;
				}
				num++;
			}

			if(!ok){
				printf("disponibilià non intero\n");
				break;
			}

			inserimento.disponibilita = atoi(input);

			if(inserimento.disponibilita <= 0){
				printf("disponibilià insufficiente\n");
				break;
			}


			printf("Inserisci il prezzo (>=0):\n");

			if(gets(input) == NULL){
				//EOF
				printf("Ricevuto EOF\n");
				return 0;
			}

			//Check
			num = 0;
			ok = 1;
			while (input[num] != '\0') {
				if ((input[num] < '0') || (input[num] > '9')) {
					ok = 0;
					break;
				}
				num++;
			}

			if(!ok){
				printf("prezzo non intero\n");
				break;
			}

			inserimento.prezzo = atoi(input);

			if(inserimento.prezzo < 0){
				printf("prezzo insufficiente\n");
				break;
			}

		    // Invocazione remota dopo aver caricato la struttura dati
		    ris = inserimento_evento_1(&inserimento, cl);

		    // Controllo del risultato
		    if(ris == NULL) 
		    { 
			    clnt_perror(cl, server); 
			    exit(1); 
		    }
		    //Eventuale errore di logica del programma
		    if(*ris < 0) //Oppure != 0
		    {  
			    printf("Problemi nell'esecuzione\n");
		    }
		    //Tutto ok      	
		    else 
		    {   
			    printf("Risultato ricevuto da %s: %i\n", server, *ris);           
		    }
		    printf("\nINSERIMENTO_EVENTO conclusa\n");
		}
		
		if(strcmp(input, "2") == 0)
		{
		    printf("\nScelta: ACQUISTA_BIGLIETTI\n");


			printf("Inserisci la descrizione:\n");

			if(gets(acquisto.descrizione) == NULL){
				//EOF
				printf("Ricevuto EOF\n");
				return 0;
			}

		    printf("Inserisci il numero di biglietti (>0):\n");

			if(gets(input) == NULL){
				//EOF
				printf("Ricevuto EOF\n");
				return 0;
			}

			//Check
			int num = 0;
			int ok = 1;
			while (input[num] != '\0') {
				if ((input[num] < '0') || (input[num] > '9')) {
					ok = 0;
					break;
				}
				num++;
			}

			if(!ok){
				printf("numeroBiglietti non intero\n");
				break;
			}

			acquisto.numeroBiglietti = atoi(input);

			if(acquisto.numeroBiglietti <= 0){
				printf("disponibilià insufficiente\n");
				break;
			}

		    // Invocazione remota dopo aver caricato la struttura dati
		    ris = acquista_biglietti_1(&acquisto, cl);

		    // Controllo del risultato
		    if(ris == NULL) 
		    { 
			    clnt_perror(cl, server); 
			    exit(1); 
		    }
		    //Eventuale errore di logica del programma
		    if(*ris < 0) //Oppure != 0
		    {  
			    printf("Problemi nell'esecuzione\n");
		    }
		    //Tutto ok      	
		    else 
		    {   
			    printf("Risultato ricevuto da %s: %i\n", server, *ris);             
		    }
		    printf("\nACQUISTA_BIGLIETTI conclusa\n");
		}

		printf("\nInserire: \n1 INSERIMENTO_EVENTO \n2 ACQUISTA_BIGLIETTI \n ^D per terminare: ");
	}

	// Libero le risorse distruggendo il gestore di trasporto
	clnt_destroy(cl);
	printf("Fine Client \n");
	
	return 0;
}
