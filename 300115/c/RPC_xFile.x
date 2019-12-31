/************************************
*            RPC_xFile.x            *
************************************/

const NOMEDIRETTORIO_LENGTH = 50;
const PREFISSO_LENGTH = 10;

const FILENAME_LENGTH = 25;

const NUM_FILES = 10;


/*

typedef struct OutputStruct *OutputList;
struct OutputStruct { string filename<FILENAME_LENGTH>; OutputList next; };

union Result switch (int errore){ 
	case 0: OutputList list;
	default: void;
};

*/

struct Input
{
	string nomeDirettorio<NOMEDIRETTORIO_LENGTH>;
};

struct MyFile {
	/* Con string crasha........... Boh */
	char filename[FILENAME_LENGTH];
};

struct OutputArray {
	int errore;
	int nFiles;
	MyFile files[NUM_FILES];
};

program ESAMEPROG
{
	version ESAMEVERS
	{
		OutputArray LISTA_FILE(Input) = 1;
		int NUMERAZIONE_RIGHE(string) = 2;
	} = 1;
} = 0x20042069;
