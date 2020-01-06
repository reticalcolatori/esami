/************************************
*           RPC_Server.c            *
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


// Eventuali strutture di supporto

#define TABLEDIM 10

typedef struct
{
  	char descrizione [STRING_LENGTH];
  	char tipo [STRING_LENGTH];
	char data [STRING_LENGTH];  
  	char luogo [STRING_LENGTH];
	int disponibilita;
	int prezzo;
} Riga;

static Riga tabella[TABLEDIM];
static int inizializzato=0;

// Eventuali funzioni per la visualizzazione e inizializzazione di una struttura dati

void stampa()
{
	int i,j;
	
	printf("\ndescrizione\ttipo\tdata\tluogo\tdisponibilità\tprezzo");
	printf("\n");

	for(i=0;i<TABLEDIM;i++)
	{
		printf("\n%s\t%s\t%s\t%s\t%d\t%d", tabella[i].descrizione, 
						tabella[i].tipo, 
						tabella[i].data,
						tabella[i].luogo,
						tabella[i].disponibilita,
						tabella[i].prezzo);
	}

	printf("\n");
}

void inizializza()
{
	int i,j;

	if(inizializzato==1)
		return;

	for(i=0;i<TABLEDIM;i++)
	{
		strcpy(tabella[i].descrizione,"L");
		strcpy(tabella[i].tipo,"L");
		strcpy(tabella[i].data,"L");
		strcpy(tabella[i].luogo,"L");
		tabella[i].disponibilita = -1;
		tabella[i].prezzo = -1;
	}

	strcpy(tabella[0].descrizione,"String");
	strcpy(tabella[0].tipo,"Concerto");
	strcpy(tabella[0].data,"11/02/2014");
	strcpy(tabella[0].luogo,"Verona");
	tabella[0].prezzo = 40;
	tabella[0].disponibilita = 40;

	strcpy(tabella[1].descrizione,"Giuventius-Bolo");
	strcpy(tabella[1].tipo,"Calcio");
	strcpy(tabella[1].data,"15/05/2014");
	strcpy(tabella[1].luogo,"Tornio");
	tabella[1].prezzo = 150;
	tabella[1].disponibilita = 21;

	inizializzato = 1;

	stampa();
}



int * inserimento_evento_1_svc(Inserimento *op, struct svc_req *rp)
{
	static int ris;
	ris = -1;

	inizializza(); //qui richiamo l'inizializzazione della struttura dati se presente
	
	// Calcolo ris secondo la logica della funzione

	//Devo fare un po di checks
	if(op->prezzo >= 0){
		if(op->disponibilita >= 0){
			if(strcmp(op->tipo, "Concerto") == 0 || strcmp(op->tipo, "Calcio") == 0 || strcmp(op->tipo, "Formula1") == 0){
				//Ricerca di doppioni e spazio vuoto.
				int doppione = -1;
				int trovato = -1;


				for(int i = 0; i < TABLEDIM; i++){
					if(trovato == -1 && strcmp(tabella[i].descrizione, "L") == 0){
						trovato = i;
					}

					if(strcmp(tabella[i].descrizione, op->descrizione) == 0){
						doppione = i;
						break;
					}
				}

				if(doppione == -1){
					if(trovato != -1){
						//copia
						strcpy(tabella[trovato].descrizione, op->descrizione);
						strcpy(tabella[trovato].tipo, op->tipo);
						strcpy(tabella[trovato].luogo, op->luogo);
						strcpy(tabella[trovato].data, op->data);
						tabella[trovato].prezzo = op->prezzo;
						tabella[trovato].disponibilita = op->disponibilita;
						ris = 0;
					}
				}
			}
		}
	}
	
	stampa();
	
	return (&ris);
}

int * acquista_biglietti_1_svc(Acquisto *op, struct svc_req *rp)
{
	static int ris;
	ris = -1;
	
	inizializza(); //qui richiamo l'inizializzazione della struttura dati se presente
	
	// Calcolo ris secondo la logica della funzione

	//Ricerca descrizione
	int trovato = -1;

	for(int i = 0; i < TABLEDIM; i++){
		if(strcmp(tabella[i].descrizione, op->descrizione) == 0){
			trovato = i;
			break;
		}
	}

	if(trovato != -1){
		//Checks
		if(tabella[trovato].disponibilita >= op->numeroBiglietti){
			tabella[trovato].disponibilita -= op->numeroBiglietti;
			ris = 0;
		}
	}
		
	stampa();
	
	return (&ris);
}

