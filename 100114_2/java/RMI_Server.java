/************************************
*           RMI_Server.java         *
************************************/

// Implementazione del Server RMI

/*----- import ---- */

import java.io.*;
import java.rmi.*;
import java.rmi.server.*;

public class RMI_Server extends UnicastRemoteObject implements RMI_interfaceFile {
	private static final long serialVersionUID = 1L;

	public static final int TABLEDIM = 10;
	private static final Riga[] tabella = new Riga[TABLEDIM];

	// Costruttore
	public RMI_Server() throws RemoteException {
		super();
	}


	// Implementazione dei metodi

	@Override
	public int inserimento_evento(Riga inserimento) throws RemoteException {
		int result = -1;
		
		boolean trovato = false;
		boolean doppione = false;
		int index = -1;

		for (int i = 0; i < tabella.length; i++) {
			if(tabella[i].getDescrizione().equals(inserimento.getDescrizione())){
				doppione = true;
				break;
			}
			if(!trovato && tabella[i].getDescrizione().equals("L")){
				trovato = true;
				index = i;
			}
		}

		if(!doppione){
			if(trovato){
				tabella[i] = inserimento;
				result = 0;
			}
		}

		return result;
	}

	@Override
	public int acquista_biglietti(String descrizione, int numeroBiglietti) throws RemoteException {
		int result = -1;
		boolean trovato = false;

		for (int i = 0; i < tabella.length; i++) {
			if(tabella[i].getDescrizione().equals(inserimento.getDescrizione())){
				if(tabella[i].getDisponibilita() >= numeroBiglietti){
					tabella[i].setDisponibilita(tabella[i].getDisponibilita()-numeroBiglietti);
					result = 0;
				}
			}
		}

		return result;
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

		for (int i = 0; i < TABLEDIM; i++) {
			tabella[i] = new Riga();
		}

		tabella[0].setDescrizione("String");
		tabella[0].setTipo("Concerto");
		tabella[0].setLuogo("Verona");
		tabella[0].setData("11/11/11");
		tabella[0].setPrezzo(100);
		tabella[0].setDisponibilita(50);

		tabella[1].setDescrizione("BFC-JUV");
		tabella[1].setTipo("Calcio");
		tabella[1].setLuogo("Verona");
		tabella[1].setData("11/11/12");
		tabella[1].setPrezzo(50);
		tabella[1].setDisponibilita(150);

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
