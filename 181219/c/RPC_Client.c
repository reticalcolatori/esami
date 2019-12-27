/************************************
*           RPC_Client.c            *
*                                   *
************************************/

#include <stdio.h>
#include <rpc/rpc.h>
#include <stdlib.h>
#include <string.h>

#include "RPC_xFile.h"

#define INPUT_LENGTH 256

#define ID_LENGTH 10
#define TIPO_LENGTH 10
#define CITTA_LENGTH 20
#define FILE_LENGTH 50

int main(int argc, char *argv[])
{

	CLIENT *cl;/*Gestore del trasporto*/
	char *server; /*Nome HOST*/
	
	OutputList *ris1;
	int *ris2;

	char input[INPUT_LENGTH];

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
	printf("\nInserire: \n1 VISUALIZZA_LISTA \n2 MODIFICA_RITARDO_VIAGGO \n ^D per terminare: ");

	while(gets(input))
	{
		if(strcmp(input, "1")!=0 && strcmp(input, "2")!=0)
		{
			printf("Argomento ERRATO!\n");
		}
		
		if(strcmp(input, "1") == 0)
		{
		    printf("\nScelta: VISUALIZZA_LISTA\n");

		    printf("Inserisci il tipo:\n");


			char tipo[TIPO_LENGTH];
			char *tipoPointer = tipo;

			if(gets(tipo) == NULL){
				//EOF
				break;
			}

			ris1 = visualizza_lista_1(&tipoPointer, cl);

		    // Controllo del risultato
		    if(ris1 == NULL) 
		    { 
			    clnt_perror(cl, server); 
			    exit(1); 
		    }
		    //Eventuale errore di logica del programma
		    if(*ris1 == NULL) 
		    {  
			    printf("Lista vuota\n");
		    }
		    //Tutto ok      	
		    else 
		    {   
			    OutputList next = *ris1;

				while(next != NULL){
					printf("%s %s from %s to %s, %d:%d %d delay\n", next->id, tipo, next->partenza, next->arrivo, next->orarioAttesa_ora, next->orarioAttesa_minuti, next->ritardo);
					next = next->next;
				}           
		    }
		    printf("\nFunzione1 conclusa\n");
		}
		
		if(strcmp(input, "2") == 0)
		{
		    printf("\nScelta: MODIFICA_RITARDO_VIAGGO\n");

		    printf("Inserisci id:\n");


			char id[TIPO_LENGTH];
			char *idPointer = id;

			if(gets(id) == NULL){
				//EOF
				break;
			}

			printf("Inserisci il ritardo:\n");

			char ritardoString[INPUT_LENGTH];
			int ritardo = 0;
			int num = 0;
			int checkInt = 1;

			if(gets(ritardoString) == NULL){
				//EOF
				break;
			}


			while( ritardoString[num]!= '\0' )
			{
				if( (ritardoString[num] < '0') || (ritardoString[num] > '9') )
				{
					//Non è un intero, devo richiedere
					checkInt = 0;
					break;
				}
				num++;
			}

      		if(checkInt){
				ritardo = atoi(ritardoString);

				if(ritardo >= 0){
					InputStruct inputOp;
					inputOp.id = idPointer;
					inputOp.ritardo = ritardo;

					// Invocazione remota dopo aver caricato la struttura dati
					ris2 = modifica_ritardo_viaggo_1(&inputOp, cl);

					// Controllo del risultato
					if(ris2 == NULL) 
					{ 
						clnt_perror(cl, server); 
						exit(1); 
					}
					//Eventuale errore di logica del programma
					if(*ris2 == -1) 
					{  
						printf("Problemi nell'esecuzione\n");
					}
					//Tutto ok      	
					else 
					{   
						printf("Risultato ricevuto da %s: %i\n", server, *ris2);             
					}
				}
				else{
					printf("Devi inserire un intero POSITIVO valido come ritardo\n");	
				}
			}else{
				printf("Devi inserire un intero POSITIVO valido come ritardo\n");
			}

		    

		    printf("\nFunzione2 conclusa\n");
		}

		printf("\nInserire: \n1 VISUALIZZA_LISTA \n2 MODIFICA_RITARDO_VIAGGO \n ^D per terminare: ");
	}

	// Libero le risorse distruggendo il gestore di trasporto
	clnt_destroy(cl);
	printf("Fine Client \n");
}
