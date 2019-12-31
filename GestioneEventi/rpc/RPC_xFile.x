/* Innocenti, Mattia, 0000825046 */
struct Inserimento {
	char descrizione[25];
	char tipo[10];
    int gg;
    int mm;
    int aaaa;
    char luogo[20];
    int disponibilita;
    int prezzo;
};

struct Acquisto {
	char descrizione[25];
	int nBiglietti;
};


program FILEPROG
{
	version FILEVERS
	{
		int INSERIMENTO_EVENTO(Inserimento) = 1;
		int ACQUISTA_BIGLIETTI(Acquisto) = 2;
	} = 1;
} = 0x20000013;