/************************************
*           RPC_Server.c            *
*                                   *
************************************/

#define _DEFAULT_SOURCE

#include <stdio.h>
#include <rpc/rpc.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <dirent.h>
#include <fcntl.h>
#include "RPC_xFile.h"

#define DIM_BUFFER 256

//extern int errno;
#include <errno.h>

int * conta_occorrenze_linea_1_svc(char **linea, struct svc_req *rp){
	static int res = 0;
	res = 0;

	//conto tutte le occorrenze della linea nei file del direttorio corrente.

	//Apro la cartella.
    DIR *myDir;
	struct dirent *currItem;
	myDir = opendir(".");

	if(myDir == NULL){
        //Launch rpc exception?
        res = -1;
        return (&res);
    }

	while((currItem = readdir(myDir)) != NULL){
        
        if(currItem->d_type == DT_REG){

			//Tipo di file regolare.
			//File di testo se ha estensione txt
			if(strstr(currItem->d_name, "txt") != NULL){
				//Apro e leggo
				int fd = open(currItem->d_name, O_RDONLY);

				if(fd < 0){
					res = fd;
					return (&res);
				}

				char line[DIM_BUFFER];
				int nread;
				char x;
				int i;
				
				//Leggo una linea
				while((nread = read(fd, line+i, sizeof(char))) > 0){
					if(line[i] == '\n'){
						line[i] = '\0';
						break;
					}

					i++;
				}

				//Confronto
				if(strcmp(line, *linea) == 0) res++;

			}
		}
	}

	closedir(myDir);

	return &res;
}

Result * lista_file_prefisso_1_svc(Input *req, struct svc_req *rp){
	static Result res;
	xdr_free((xdrproc_t) xdr_Result, (caddr_t) &res);

	    //Apro la cartella.
    DIR *myDir;
    struct dirent *currItem;

    myDir = opendir(req->nomeDirettorio);

    if(myDir == NULL){
		res.errore = errno;
        return &res;
    }

	//Inizializzo la struttura
	res.Result_u.list = (OutputList) malloc(sizeof(OutputStruct));
	OutputList head = res.Result_u.list;
	OutputList next = res.Result_u.list;

	while((currItem = readdir(myDir)) != NULL){
        if(currItem->d_type == DT_REG){
			//Verifico il prefisso
			if(strstr(currItem->d_name, req->prefisso) != NULL){

				//Posso aggiornare la struttura.
				if(next == NULL){
					head->next = (OutputList) malloc(sizeof(OutputStruct));
					head = head->next;
					next = head->next;
				}else{
					//Avviene solo all'inizio. Serve a saltare un malloc già fatto prima.
					next = head->next;
				}

				head->filename = (char*) malloc(sizeof(char)*(strlen(currItem->d_name)+1));	//Va fatta malloc perchè si tratta di un puntatore.
				strcpy(head->filename, currItem->d_name);
			}
		}
	}

	closedir(myDir);
	res.errore = 0;
	return &res;

}
