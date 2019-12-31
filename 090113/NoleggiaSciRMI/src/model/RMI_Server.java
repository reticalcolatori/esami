package model;

import java.rmi.*;
import java.rmi.registry.LocateRegistry;
import java.rmi.server.*;

public class RMI_Server extends UnicastRemoteObject implements RMI_interfaceFile {
	private static final long serialVersionUID = 1L;

	// qui eventuali variabili e strutture dati
	private static final int N = 10;
	static Noleggio tabella[] = null;

	// Costruttore
	public RMI_Server() throws RemoteException {
		super();
		tabella = new Noleggio[N];

		for (int i = 3; i < tabella.length; i++) {
			tabella[i] = new Noleggio("L", -1, -1, -1, -1, "L", -1);
		}

		tabella[0] = new Noleggio("00001", 13, 02, 2013, 10, "uomo", 15);
		tabella[1] = new Noleggio("00002", 24, 04, 2013, 15, "donna", 5);
		tabella[2] = new Noleggio("00003", 20, 03, 2013, 5, "bambino", 10);

		stampa();

	}

	public static void stampa() {
		System.out.println("Identificatore\tData\tGiorni\tModello\tCosto giornaliero\n");

		for (int i = 0; i < N; i++) {
			System.out.println(tabella[i].id + "\t" + tabella[i].giorno + "/" + tabella[i].mese + "/" + tabella[i].anno
					+ "\t" + tabella[i].giorni + "\t" + tabella[i].modello + "\t" + tabella[i].costoGiornaliero);
		}
	}


	// Avvio del Server RMI
	public static void main(String[] args) {
		
		//creo e avvio il registry nella macchina dove risiede il server (devono risiedere sulla stessa macchina)
		try {
			LocateRegistry.createRegistry(1099);
		} catch (RemoteException e1) {
			e1.printStackTrace();
		}
		
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
	
	// Implementazione dei metodi

	@Override
	public int inserisci_sci(String id, String modello, int costo) throws RemoteException {
		
		int res = -1;
		int disp = -1;
		
		for (int i = 0; i < tabella.length; i++) {
			if(tabella[i].id.equals(id))
				return res;
			if(tabella[i].id.equals("L") && disp == -1)
				disp = i;
		}
		
		if(disp == -1)
			return res;
		else {
			tabella[disp].setId(id);
			tabella[disp].setModello(modello);
			tabella[disp].setCostoGiornaliero(costo);
			res = 0;
			stampa();
		}		
	
		return res;
	}

	@Override
	public int noleggia_sci(String id, int giorno, int mese, int anno, int durata) throws RemoteException {
		
		int res = -1;
		
		for (int i = 0; i < tabella.length; i++) {
			if(tabella[i].id.equals(id) && tabella[i].giorni == -1) { //suppongo inserimenti sempre corretti
				tabella[i].setGiorno(giorno);
				tabella[i].setMese(mese);
				tabella[i].setAnno(anno);
				tabella[i].setGiorni(durata);
				res = 0;
				stampa();
				break;
			}
		}
		
		return res;
	}
}
