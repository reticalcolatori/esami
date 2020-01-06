/************************************
*            RPC_xFile.x            *
************************************/

const STRING_LENGTH = 30;

struct Inserimento{
	char descrizione[STRING_LENGTH];
	char tipo[STRING_LENGTH];
	char data[STRING_LENGTH];
	char luogo[STRING_LENGTH];
	int disponibilita;
	int prezzo;
};

struct Acquisto{
	char descrizione[STRING_LENGTH];
	int numeroBiglietti;
};

program ESAMEPROG
{
	version ESAMEVERS
	{
		int INSERIMENTO_EVENTO(Inserimento) = 1;
		int ACQUISTA_BIGLIETTI(Acquisto) = 2;
	} = 1;
} = 0x20042069;
