/************************************
*            *
*           RPC_Server.c            *
*                                   *
************************************/

#include <stdio.h>
#include <rpc/rpc.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <dirent.h>
#include <fcntl.h>
	
#include "RPC_xFile.h"

#define TABLEDIM 10
#define LISTADIM 6
#define ID_LENGTH 10
#define TIPO_LENGTH 10
#define CITTA_LENGTH 20
#define FILE_LENGTH 50

typedef struct
{
  	char id[ID_LENGTH];
  	char tipo[TIPO_LENGTH];
  	char partenza[CITTA_LENGTH];
	char arrivo[CITTA_LENGTH];
  	int orarioAttesa_ora;
	int orarioAttesa_minuti;
  	int ritardo;
  	char audio[FILE_LENGTH];
} Riga;

static Riga tabella[TABLEDIM];

static int inizializzato=0;

void inizializza()
{
	int i;

	if(inizializzato==1)
		return;

	for(i=0;i<TABLEDIM;i++)
	{
		strcpy(tabella[i].id,"L");
		strcpy(tabella[i].tipo,"L");
		strcpy(tabella[i].partenza,"L");
		strcpy(tabella[i].arrivo,"L");		
		tabella[i].orarioAttesa_ora = -1;
		tabella[i].orarioAttesa_minuti = -1;
		tabella[i].ritardo = -1;
		strcpy(tabella[i].audio,"L");
	}

	strcpy(tabella[0].id,"SATA1234");
	strcpy(tabella[0].tipo,"Partenza");
	strcpy(tabella[0].partenza,"Bologna");
	strcpy(tabella[0].arrivo,"Bari");

	tabella[0].orarioAttesa_ora = 12;
	tabella[0].orarioAttesa_minuti = 15;
	tabella[0].ritardo = 0;

	strcpy(tabella[0].audio,"SATA1234.mp4");

	strcpy(tabella[1].id,"MATA3333");
	strcpy(tabella[1].tipo,"Arrivo");
	strcpy(tabella[1].partenza,"Milano");
	strcpy(tabella[1].arrivo,"Bologna");

	tabella[1].orarioAttesa_ora = 16;
	tabella[1].orarioAttesa_minuti = 30;
	tabella[1].ritardo = 21;

	strcpy(tabella[1].audio,"MATA3333.mp4");

	strcpy(tabella[4].id,"CATA1111");
	strcpy(tabella[4].tipo,"Partenza");
	strcpy(tabella[4].partenza,"Bologna");
	strcpy(tabella[4].arrivo,"Napoli");

	tabella[4].orarioAttesa_ora = 13;
	tabella[4].orarioAttesa_minuti = 00;
	tabella[4].ritardo = 190;

	strcpy(tabella[4].audio,"CATA1111.mp4");

	inizializzato = 1;
}

OutputList * visualizza_lista_1_svc(char **tipo, struct svc_req *rp){
	static OutputList res;
	xdr_free((xdrproc_t) xdr_OutputList, (caddr_t) &res);
	
	inizializza();

	int listLength = 0;
	int i = 0;

	//Inizializzazione lista
	
	OutputList head = NULL;
	OutputList next = NULL;

	for(; i < TABLEDIM && listLength < LISTADIM; i++){
		if(strcmp(*tipo, tabella[i].tipo) == 0){
			//Posso aggiornare la lista.
			if(head == NULL){
				res = (OutputList) malloc(sizeof(OutputStruct));
				//Per l'amor di dio metti sto NULL
				res->next = NULL;
				head = res;
				next = head->next;
			}
			else if(next == NULL){
				head->next = (OutputList) malloc(sizeof(OutputStruct));
				//Per l'amor di dio metti sto NULL
				head->next->next = NULL;
				head = head->next;
				next = head->next;
			}

			head->id = (char*) malloc(sizeof(char)*(strlen(tabella[i].id)+1));
			head->partenza = (char*) malloc(sizeof(char)*(strlen(tabella[i].partenza)+1));
			head->arrivo = (char*) malloc(sizeof(char)*(strlen(tabella[i].arrivo)+1));

			strcpy(head->id, tabella[i].id);
			strcpy(head->partenza, tabella[i].partenza);
			strcpy(head->arrivo, tabella[i].arrivo);

			head->orarioAttesa_ora = tabella[i].orarioAttesa_ora;
			head->orarioAttesa_minuti = tabella[i].orarioAttesa_minuti;
			head->ritardo = tabella[i].ritardo;

			listLength++;
		}
	}

	return &res;
}


int * modifica_ritardo_viaggo_1_svc(InputStruct *input, struct svc_req *rp){
	static int res;
	res = -1;

	inizializza();

	//Checks input
	if(input->ritardo >= 0 ){
		int i = 0;

		for(; i < TABLEDIM; i++){
			if(strcmp(input->id, tabella[i].id) == 0){
				tabella[i].ritardo = input->ritardo;
				res = 0;
				return &res;
			}
		}
	}

	return &res;
}
