/************************************
*           RPC_Client.c            *
*                                   *
************************************/

#include <stdio.h>
#include <rpc/rpc.h>
#include <stdlib.h>
#include <string.h>

#include "RPC_xFile.h"

#define LENGTH 256

#define NOMEDIRETTORIO_LENGTH 50
#define PREFISSO_LENGTH 10

#define FILENAME_LENGTH 50


int main(int argc, char *argv[])
{

	CLIENT *cl;/*Gestore del trasporto*/
	char *server; /*Nome HOST*/
	

	Input req;
	Result *resStruct;
	int *risInt;

	char inputUtente[256];

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
	printf("\nInserire: \n1 conta_occorrenze_linea \n2 lista_file_prefisso \n ^D per terminare: ");

	while(gets(inputUtente) != NULL)
	{
		if(strcmp(inputUtente, "1")!=0 && strcmp(inputUtente, "2")!=0)
		{
			printf("Inserisci 1 o 2\n");
		}
		
		if(strcmp(inputUtente, "1") == 0)
		{
		    printf("\nScelta: conta_occorrenze_linea\n");

			char linea[LENGTH];

			printf("Inserisci la linea:\n");

			if(gets(linea) == NULL){
				//Lo interpreto come EOF
				break;
			}

		    risInt = conta_occorrenze_linea_1((char**)&linea, cl);

		    // Controllo del risultato
		    if(risInt == NULL) 
		    { 
			    clnt_perror(cl, server); 
			    exit(1); 
		    }

		    //Eventuale errore di logica del programma
		    if(*risInt < 0) 
		    {  
			    printf("Problemi nell'esecuzione: %d\n", *risInt);
		    }    	
		    else 
		    {   
			    printf("Risultato ricevuto da %s: %d\n", server, *risInt);           
		    }

		    printf("\nconta_occorrenze_linea conclusa\n");
		}
		
		if(strcmp(inputUtente, "2") == 0)
		{
		    printf("\nScelta: lista_file_prefisso\n");

			char nomeDirettorio[NOMEDIRETTORIO_LENGTH];
			char prefisso[PREFISSO_LENGTH];

			//Richiesta nome direttorio:
			printf("Inserisci il nome del direttorio:\n");
			if(gets(nomeDirettorio) == NULL){
				//EOF: esco
				break;
			}

			//Richiesta prefisso:
			printf("Inserisci il prefisso:\n");
			if(gets(prefisso) == NULL){
				//EOF: esco
				break;
			}

			//Non so se funziona.
			req.nomeDirettorio = (char*) nomeDirettorio;
			req.prefisso = (char*) prefisso;

		    // Invocazione remota dopo aver caricato la struttura dati
		    resStruct = lista_file_prefisso_1(&req, cl);

		    // Controllo del risultato
		    if(resStruct == NULL) 
		    { 
			    clnt_perror(cl, server); 
			    exit(1); 
		    }

		    //Eventuale errore di logica del programma
		    if(resStruct->errore != 0) 
		    {  
			    printf("Problemi nell'esecuzione: %d\n", resStruct->errore);
		    }
		    //Tutto ok      	
		    else 
		    {   
			    OutputList next = resStruct->Result_u.list;

				while(next != NULL){
					printf("Trovato file: %s\n", next->filename);
					next = next->next;
				}
		    }

		    printf("\nlista_file_prefisso conclusa\n");
		}

		printf("\nInserire: \n1 conta_occorrenze_linea \n2 lista_file_prefisso \n ^D per terminare: ");
	}

	// Libero le risorse distruggendo il gestore di trasporto
	clnt_destroy(cl);
	printf("Fine Client \n");

	return 0;
}
