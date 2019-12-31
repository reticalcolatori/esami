/************************************
*           RPC_Server.c            *
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

OutputArray * lista_file_1_svc(Input *inData, struct svc_req *rp){
	static OutputArray res;
	xdr_free((xdrproc_t) xdr_OutputArray, (caddr_t) &res);
	res.errore = 0;
	
	
	// for (int i = 0; i < res.nFiles; i++)
	// {
	// 	free(res.files[i].filename);
	// }

	res.nFiles = 0;
	
	
	//Apro la cartella.
    DIR *myDir;
    struct dirent *currItem;

	myDir = opendir(inData->nomeDirettorio);

    if(myDir == NULL){
		res.errore = errno;
        return &res;
    }

	while((currItem = readdir(myDir)) != NULL && res.nFiles < NUM_FILES){
        if(currItem->d_type == DT_REG){
			//Tipo di file regolare.

			//Devo verificare la presenza di consonanti.
			
			//Ciclo tutta la stringa
			int i = 0, j = 0;
			
			char fakeString[3];
			fakeString[0] = '\0';
			fakeString[1] = '\0';
			fakeString[2] = '\0';

			char *currentChar = fakeString+1;
			char *prevChar = fakeString;

			char filenameChar;

			while((filenameChar=currItem->d_name[i++]) != '\0'){

				fakeString[j++] = filenameChar;

				if(j == 2){
					if(!(*currentChar == 'a' || *currentChar == 'A' || *currentChar == 'e' || *currentChar == 'E' || *currentChar == 'i' || *currentChar == 'I' || *currentChar == 'o' || *currentChar == 'O' || *currentChar == 'u' || *currentChar == 'U')){
						if(!(*prevChar == 'a' || *prevChar == 'A' || *prevChar == 'e' || *prevChar == 'E' || *prevChar == 'i' || *prevChar == 'I' || *prevChar == 'o' || *prevChar == 'O' || *prevChar == 'u' || *prevChar == 'U')){
							//Ho trovato un file valido: posso inserirlo nella lista.
							//Prima devo fare un malloc: ci penserà la free a fare le cose da GC.
							//res.files[res.nFiles].filename = (char*)malloc(sizeof(char)*(FILENAME_LENGTH));
							strcpy(res.files[res.nFiles++].filename, currItem->d_name);
							break;//Importante
						}
					}

					//Shifting
					fakeString[0] = fakeString[1];
					j = 1;
				}
			}
		}
	}

	printf("errno: %d, nfiles: %d\n", res.errore, res.nFiles);

	// for(int i = 0; i < NUM_FILES - res.nFiles; i++)
	// {
	// 	res.files[res.nFiles+i].filename = NULL;
	// }

	closedir(myDir);

	return &res;
}


int * numerazione_righe_1_svc(char **nomeFile, struct svc_req *rp){
	static int res;
	res = 0;

	char myLine[DIM_BUFFER];
	//Reset della linea
	memset(myLine, '\0', sizeof(char)*DIM_BUFFER);

	int numeratore = 1;
	
	int nread = 0;
	int nwrite = 0;
	char readChar = '\0';

	int fdRead = open(*nomeFile, O_RDONLY);

	char nomeFileTmp[DIM_BUFFER];
	strcpy(nomeFileTmp, *nomeFile);
	strcat(nomeFileTmp, ".tmp");

	int fdWrite = open(nomeFileTmp, O_WRONLY | O_CREAT | O_TRUNC, 0644);

	if(fdRead < 0){
		res = fdRead;
		return &res;
	}

	if(fdWrite < 0){
		close(fdRead);
		res = fdWrite;
		return &res;
	}

	//Prima stampo il numeratore e uno spazio
	sprintf(myLine, "%d ", numeratore);

	nwrite = write(fdWrite, myLine, sizeof(char) * strlen(myLine));//Non metto il terminatore.

	if(nwrite < 0){
		close(fdRead);
		close(fdWrite);
		res = nwrite;
		return &res;
	}

	//Per ogni riga devo aggiungere un numeratore.
	while((nread=read(fdRead, &readChar, sizeof(char))) > 0){
		if(readChar == '\n'){
			numeratore++;
			res++;

			//Reset della linea
			memset(myLine, '\0', sizeof(char)*DIM_BUFFER);
			//Prima stampo il numeratore e uno spazio
			sprintf(myLine, "\n%d ", numeratore);

			if((nwrite = write(fdWrite, myLine, sizeof(char) * strlen(myLine))) < 0){//Non metto il terminatore.
				close(fdRead);
				close(fdWrite);
				res = nwrite;
				return &res;
			}

		}else{
			if((nwrite = write(fdWrite, &readChar, sizeof(char))) < 0){
				close(fdRead);
				close(fdWrite);
				res = nwrite;
				return &res;
			}
		}
	}

	if(nread < 0){
		res = nread;
		return &res;
	}

	close(fdRead);
	close(fdWrite);


	//Ora devo copiare il file temp nel file originale solo se ci sono state modifiche
	if(res > 0){
		remove(*nomeFile);
		
		//Copia byte a byte.
		fdRead = open(nomeFileTmp, O_RDONLY);

		if(fdRead < 0){
			res = fdRead;
			return &res;
		}

		fdWrite = open(*nomeFile, O_CREAT | O_TRUNC | O_WRONLY, 0644);

		if(fdWrite < 0){
			close(fdRead);
			res = fdWrite;
			return &res;
		}

		while((nread=read(fdRead, &readChar, sizeof(char)))>0){
			if((nwrite=write(fdWrite, &readChar, sizeof(char))) < 0){
				close(fdRead);
				close(fdWrite);
				res = nwrite;
				return &res;
				}
		}

		if(nread < 0){
			res = nread;
			return &res;
		}

		close(fdRead);
		close(fdWrite);

		remove(nomeFileTmp);
	}

	return &res;
}