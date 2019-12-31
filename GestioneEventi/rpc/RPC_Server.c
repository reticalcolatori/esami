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
#define LISTADIM 3

/*
typedef struct
{
	char parolaChiave[LENGTH];
}Parola;

typedef struct
{
  	char titolo [LENGTH];
  	char nomeFile [LENGTH];
  	char partita [LENGTH];
  	int inizio;
  	int fine;
  	Parola parole [LISTADIM];
} Moviola;
*/

static Inserimento tabella[TABLEDIM];
static int inizializzato=0;

// Eventuali funzioni per la visualizzazione e inizializzazione di una struttura dati

void stampa()
{
	int i,j;
	
	printf("\nDescrizione\tTipo\tData\tLuogo\tDisponibilità\tPrezzo");
	/*for(j=0;j<LISTADIM;j++)
	{
		printf("Parola%d\t", j+1);
	}
	printf("\n");*/

	for(i=0;i<TABLEDIM;i++)
	{
		printf("\n%s\t%s\t%d/%d/%d\t%s\t%d\t%d", 
                        tabella[i].descrizione, 
						tabella[i].tipo, 
						tabella[i].gg,
                        tabella[i].mm,
                        tabella[i].aaaa,
						tabella[i].luogo, 
						tabella[i].disponibilita,
                        tabella[i].prezzo);
        /*
		for(j=0;j<LISTADIM;j++)
		{
			printf("%s\t", tabella[i].parole[j].parolaChiave);
		}*/
	}
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
		tabella[i].gg = -1;
        tabella[i].mm = -1;
        tabella[i].aaaa = -1;

		strcpy(tabella[i].luogo,"L");
        tabella[i].disponibilita = -1;
        tabella[i].prezzo = -1;

/*
		for(j=0;j<LISTADIM;j++)
		{
			strcpy(tabella[i].parole[j].parolaChiave,"L");
		}*/
	}

	strcpy(tabella[1].descrizione,"String");
    strcpy(tabella[1].tipo,"Concerto");
    tabella[1].gg = 11;
    tabella[1].mm = 01;
    tabella[1].aaaa = 2014;

    strcpy(tabella[1].luogo,"Verona");
    tabella[1].disponibilita = 40;
    tabella[1].prezzo = 40;


    strcpy(tabella[2].descrizione,"Junentus-Inger");
    strcpy(tabella[2].tipo,"Calcio");
    tabella[2].gg = 03;
    tabella[2].mm = 05;
    tabella[2].aaaa = 2014;

    strcpy(tabella[2].luogo,"Torino");
    tabella[2].disponibilita = 21;
    tabella[2].prezzo = 150;

	inizializzato = 1;

	stampa();
}


int * inserimento_evento_1_svc(Inserimento *ins, struct svc_req *sr){

    inizializza();

    static int ris;
    ris = -1;

    int pos = -1;

    for(int i = 0; i < TABLEDIM; i++){
        if(strcmp(tabella[i].descrizione, ins->descrizione) == 0){
            //ritorna già valore negativo non puoi inserire
            ris = -1;
            return (&ris);
        } else {
            if(strcmp(tabella[i].descrizione, "L") == 0){
                if(pos == -1)
                    pos = i;
                break;
            }
        }
    }

    if(pos != -1){
        strcpy(tabella[pos].descrizione, ins->descrizione);
        strcpy(tabella[pos].tipo, ins->tipo);
        tabella[pos].gg = ins->gg;
        tabella[pos].mm = ins->mm;
        tabella[pos].aaaa = ins->aaaa;

        strcpy(tabella[pos].luogo, ins->luogo);

        tabella[pos].disponibilita = ins->disponibilita;
        tabella[pos].prezzo = ins->prezzo;
        ris = 0;
        stampa();
    }


    return &ris;
}


int * acquista_biglietti_1_svc(Acquisto *acquisto, struct svc_req *sr){

    inizializza();

    static int ris;
    ris = -1;

    for(int i = 0; i < TABLEDIM; i++){
        if(strcmp(tabella[i].descrizione, acquisto->descrizione) == 0){
            //verifico se c'è disponibiita
            int nBiglietti = acquisto->nBiglietti;

            if(tabella[i].disponibilita - nBiglietti >= 0){
                tabella[i].disponibilita -= nBiglietti;
                ris = 0;
                stampa();
            }
            return (&ris);
        } 
    }

    return &ris;
}

/*
int * (char **lineaRicerca, struct svc_req *sr){

    printf("Ricevuta richiesta per contare occorrenze della linea %s\n", *lineaRicerca);

	static int ris;
	ris = 0;

	// inizializza() qui richiamo l'inizializzazione della struttura dati se presente
	
	// Calcolo ris secondo la logica della funzione
	
	// stampa();

    DIR *currDir;

    char pwdPath[256];
    size_t size;

    getcwd(pwdPath, size);
    printf("Path del direttorio corrente = %s\n", pwdPath);

    currDir = opendir(pwdPath);
    if(currDir == NULL){
        printf("Unable to open dir %s\n", pwdPath);
        ris = -1;
        return (&ris);
    }


    struct dirent *currItem;

    int fdCurrFile = -1;

    while((currItem = readdir(currDir)) != NULL){
        printf("%s\n", currItem->d_name);

        if((strcmp(currItem->d_name, ".") != 0) && (strcmp(currItem->d_name, "..") != 0) && (currItem->d_type == 8)){ //== 8 costante per i file
            
            printf("Apro il file %s e verifico il numero di occorrenze di %s...\n", currItem->d_name, *lineaRicerca);

            if((fdCurrFile = open(currItem->d_name, O_RDONLY)) < 0) {
                ris = -1;
                return (&ris);
            }

            char currCh;
            char currLine[256];
            int pos = 0;

            while((read(fdCurrFile, &currCh, sizeof(char))) > 0){
                if(currCh == '\n'){ //allora confronto la linea attuale con quella da ricerca se uguali incremento il ris

                    currLine[pos] = '\0';

                    printf("Confornto la linea corrente -> %s\nCon la linea di rif -> %s\n", currLine, *lineaRicerca);

                    if(strcmp(currLine, *lineaRicerca) == 0){
                        ris++;
                    } else {
                        //non faccio niente
                    }

                    pos = 0;

                } else {
                    currLine[pos] = currCh;
                    pos++;
                }
            }

        }
    }

    /*

    DIR *currDir;
    struct dirent *currItem;

    // Verifico se esiste e ne possiedo i diritti
    currDir = opendir(nomeFile);

    if(currDir == NULL){
        printf("SERVER FIGLIO (%d): errore apertura direttorio %s\n", getpid(), nomeFile);
        // il file che mi hai passato o non esiste o non hai i diritti te lo comunico e rileggo
        strcpy(errorMsg, "Errore nella lettura del nome del file.\n");  
        if (write(fdConnect, errorMsg, strlen(errorMsg)) < 0) {
            perror("Errore scrittura su socket connessa");
            closedir(currDir);
            close(fdConnect);
        }
        write(fdConnect, &zero, sizeof(char));
        continue;
    }

    
    
	
	return (&ris);
    
}
*/
/*
Output * lista_file_prefisso_1_svc(Input *input, struct svc_req *sr)
{
	static Output ris;

    for(int i = 0; i < 6; i++)
        strcpy(ris.files[i].name, "L");
    ris.res = -1;

    DIR *dirInput;

    dirInput = opendir(input->nomeDir);
    if(dirInput == NULL) {
        printf("Unable to open the dir %s\n", input->nomeDir);
        return (&ris);
    }

    struct dirent *currItem;

    int pos = 0;


    //ritorna solo i primi possibili 6 che matchano con questa radice

    while((currItem = readdir(dirInput)) != NULL){
        if((strncmp(currItem->d_name, input->prefix, strlen(input->prefix)) == 0)){
            if(pos == 6)
                break;

            strcpy(ris.files[pos].name, currItem->d_name);
            pos++;

            ris.res = 0;
        }
    }
	
	// inizializza() qui richiamo l'inizializzazione della struttura dati se presente
	
	// Calcolo ris secondo la logica della funzione
	
	// stampa();
	
	return (&ris);
}
*/
