/************************************
*            RPC_xFile.x            *
************************************/

const ID_LENGTH = 10;
const CITTA_LENGTH = 20;


struct InputStruct{
	string id<ID_LENGTH>;
	int ritardo;
};

typedef struct OutputStruct *OutputList;
struct OutputStruct{
	string id<ID_LENGTH>;
	string partenza<CITTA_LENGTH>;
	string arrivo<CITTA_LENGTH>;
	int orarioAttesa_ora;
	int orarioAttesa_minuti;
	int ritardo;
	OutputList next;
};


program ESAMEPROG
{
	version ESAMEVERS
	{
		OutputList VISUALIZZA_LISTA(string) = 1;
		int MODIFICA_RITARDO_VIAGGO(InputStruct) = 2;
	} = 1;
} = 0x20000069;
