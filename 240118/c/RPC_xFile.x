/************************************
*            RPC_xFile.x            *
*                                   *
************************************/

const NOMEDIRETTORIO_LENGTH = 50;
const PREFISSO_LENGTH = 10;

const FILENAME_LENGTH = 50;

struct Input
{
	string nomeDirettorio<NOMEDIRETTORIO_LENGTH>;
	string prefisso<PREFISSO_LENGTH>;
};

typedef struct OutputStruct *OutputList;
struct OutputStruct { string filename<FILENAME_LENGTH>; OutputList next; };

union Result switch (int errore){ 
	case 0: OutputList list;
	default: void;
};

program ESAMEPROG
{
	version ESAMEVERS
	{
		int CONTA_OCCORRENZE_LINEA(string) = 1;
		Result LISTA_FILE_PREFISSO(Input) = 2;
	} = 1;
} = 0x20042069;
