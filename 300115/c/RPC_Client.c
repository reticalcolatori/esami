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
	
	char input[LENGTH];

	Input inputStruct1;
	OutputArray *outputStruct1;

	char *nomeFile2;
	int *res2;

	
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
	printf("\nInserire: \n1 LISTA_FILE \n2 NUMERAZIONE_RIGHE \n ^D per terminare: ");

	while(gets(input))
	{
		if(strcmp(input, "1")!=0 && strcmp(input, "2")!=0)
		{
			printf("Argomento ERRATO!\n");
		}
		
		if(strcmp(input, "1") == 0)
		{
		    printf("\nScelta: LISTA_FILE\n");

			//Devo passare un direttorio.

			printf("Inserisci un direttorio: \n");

			if(gets(input) == NULL){
				//EOF esco
				printf("Esco EOF.\n");
				return 0;
			}

			inputStruct1.nomeDirettorio = (char*)malloc(sizeof(char)*(strlen(input)+1));
			strcpy(inputStruct1.nomeDirettorio, input);
		    
		    // Invocazione remota dopo aver caricato la struttura dati
		    outputStruct1 = lista_file_1(&inputStruct1, cl);

			free(inputStruct1.nomeDirettorio);

		    // Controllo del risultato
		    if(outputStruct1 == NULL) 
		    { 
			    clnt_perror(cl, server); 
			    exit(1); 
		    }
		    //Eventuale errore di logica del programma
		    if(outputStruct1->errore != 0) 
		    {  
			    printf("Problemi nell'esecuzione: %d\n", outputStruct1->errore);
		    }
		    //Tutto ok      	
		    else 
		    {   
			    printf("Risultato ricevuto da %s. Numero files: %d\n", server, outputStruct1->nFiles);           

				for(int i = 0; i < outputStruct1->nFiles; i++){
					printf("File %d: %s\n", i+1, outputStruct1->files[i].filename);
				}
		    }

		    printf("\nLISTA_FILE conclusa\n");
		}
		
		if(strcmp(input, "2") == 0)
		{
		    printf("\nScelta: NUMERAZIONE_RIGHE\n");

			//Richiesta nome file.

			printf("Inserisci un nome file: \n");

			if(gets(input) == NULL){
				//EOF esco
				printf("Esco EOF.\n");
				return 0;
			}

			nomeFile2 = (char*)malloc(sizeof(char)*(strlen(input)+1));
			strcpy(nomeFile2, input);

		    // Invocazione remota dopo aver caricato la struttura dati
		    res2 = numerazione_righe_1(&nomeFile2, cl);

			free(nomeFile2);

		    // Controllo del risultato
		    if(res2 == NULL) 
		    { 
			    clnt_perror(cl, server); 
			    exit(1); 
		    }
		    //Eventuale errore di logica del programma
		    if(*res2 < 0) 
		    {  
			    printf("Problemi nell'esecuzione: %d\n", *res2);
		    }
		    //Tutto ok      	
		    else 
		    {   
			    printf("Risultato ricevuto da %s: %i\n", server, *res2);             
		    }
			
		    printf("\nNUMERAZIONE_RIGHE conclusa\n");
		}

		printf("\nInserire: \n1 LISTA_FILE \n2 NUMERAZIONE_RIGHE \n ^D per terminare: ");
	}

	// Libero le risorse distruggendo il gestore di trasporto
	clnt_destroy(cl);
	printf("Fine Client \n");

	return 0;
}
