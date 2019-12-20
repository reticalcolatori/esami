/************************************
*           *
*           RMI_Server.java         *
*                                   *
************************************/

// Implementazione del Server RMI

/*----- import ---- */

import java.io.*;
import java.rmi.*;
import java.rmi.server.*;

public class RMI_Server extends UnicastRemoteObject implements RMI_interfaceFile {
	private static final long serialVersionUID = 1L;
	private static final int TAB_LENGTH = 5;
	private static Noleggio[] tabella;

	// Costruttore
	public RMI_Server() throws RemoteException {
		super();
	}

	// Eventuali metodi legati alla struttura dati

	public int inserisci_sci(String id, String modello, int costo) throws RemoteException {

		// Check sul costo
		if (costo <= 0) {
			return -3;
		}

		// Ricerca nel database di un doppione
		// Inoltre ricerco un buco vuoto.
		int postoVuoto = -1;
		for (int i = 0; i < TAB_LENGTH; i++) {
			if (tabella[i].getId().equals(id)) {
				// throw new RemoteException("ID già usato");
				return -1;
			}

			if (postoVuoto == -1) {
				if (tabella[i].isLibero()) {
					postoVuoto = i;
				}
			}
		}

		// Check se c'è ancora posto.
		if (postoVuoto == -1) {
			return -2;
		}

		// Inserisco nel primo spazio disponibile
		tabella[postoVuoto].setId(id);
		tabella[postoVuoto].setModello(modello);
		tabella[postoVuoto].setCostoGiornaliero(costo);

		return 0;

	}

	public int noleggia_sci(String id, int giorno, int mese, int anno, int durata) throws RemoteException {
		int posto = -1;

		// Ricerca nella struttura
		for (int i = 0; i < TAB_LENGTH; i++) {
			if (tabella[i].getId().equals(id))
				posto = i;
		}

		// Verifica presenza
		if (posto == -1) {
			return -1;
		}

		// Check noleggio
		if (!tabella[posto].isNoleggioOk(giorno, mese, anno, durata)) {
			return -2;
		}

		// Aggiornamento dei dati
		tabella[posto].setGiorno(giorno);
		tabella[posto].setMese(mese);
		tabella[posto].setAnno(anno);
		tabella[posto].setGiorni(durata);

		return 0;
	}

	// Avvio del Server RMI
	public static void main(String[] args) {
		int registryPort = 1099;
		String registryHost = "localhost";
		String serviceName = "ServerRMI";

		// Controllo dei parametri della riga di comando
		if (args.length != 0 && args.length != 1) {
		System.out.println("Sintassi: ServerRMI [REGISTRYPORT]");
		System.exit(1);
		}
		if (args.length == 1) {
			try {
				registryPort = Integer.parseInt(args[0]);
			} catch (Exception e) {
				System.out.println("Sintassi: ServerRMI [REGISTRYPORT], REGISTRYPORT intero");
				System.exit(2);
			}
		}

		// Qui inizializzo eventuali strutture dati

		tabella = new Noleggio[TAB_LENGTH];

		for (int i = 0; i < TAB_LENGTH; i++)
			tabella[i] = new Noleggio();

		tabella[0] = new Noleggio("00001", 13, 02, 2013, 10, "uomo", 15);
		tabella[1] = new Noleggio("00002", 24, 04, 2013, 15, "donna", 5);
		tabella[2] = new Noleggio("00003", 20, 03, 2013, 5, "bambino", 10);
		tabella[3] = new Noleggio();
		tabella[4] = new Noleggio();

		// Impostazione del SecurityManager
		if (System.getSecurityManager() == null){
			System.setProperty("java.security.policy", "rmi.policy");
			System.setSecurityManager(new RMISecurityManager());
		}

		// Registrazione del servizio
		String completeName = "//" + registryHost + ":" + registryPort + "/" + serviceName;

		try {
			RMI_Server serverRMI = new RMI_Server();
			Naming.rebind(completeName, serverRMI);
			System.out.println("Server RMI: Servizio \"" + serviceName + "\" registrato");
		} catch (Exception e) {
			System.err.println("Server RMI \"" + serviceName + "\": " + e.getMessage());
			e.printStackTrace();
			System.exit(1);
		}
	}
}
