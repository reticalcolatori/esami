/************************************
*           RMI_Server.java         *
************************************/

// Implementazione del Server RMI

/*----- import ---- */

import java.io.*;
import java.rmi.*;
import java.rmi.server.*;
import java.util.*;

public class RMI_Server extends UnicastRemoteObject implements RMI_interfaceFile {
	private static final long serialVersionUID = 1L;

	// qui eventuali variabili e strutture dati
	/*
	 * private static final int N = 10; static Noleggio tabella[] = null;
	 */

	// Costruttore
	public RMI_Server() throws RemoteException {
		super();
	}

	// Implementazione dei metodi
	public List<String> lista_file_caratteri(String nomeDirettorio, char charA, char charB) throws RemoteException{
		List<String> result = new ArrayList<>();
		File myDir = new File(nomeDirettorio);

		if(myDir.isDirectory()){
			//Prendo lista dei nomi e verifico che almeno uno dei due caratteri sia presente.
			File[] files = myDir.listFiles();

			for (int i = 0; i < files.length; i++) {
				if(files[i].isFile()){
					if(files[i].getName().indexOf(charA) != -1 || files[i].getName().indexOf(charB) != -1){
						result.add(files[i].getName());
					}
				}
			}
		}else{
			throw new RemoteException("Il nome direttorio non fa riferimento ad alcun direttorio");
		}

		return result;
	}

	public int conta_occorrenze_interpunzione(String nomeFile)	throws RemoteException{
		File myFile = new File(nomeFile);
		int result = -1;
		int occorrenze = 0;
		String checkString = ",:.";

		if(myFile.isFile()){
			if(myFile.canRead()){
				try{
					Reader reader = new FileReader(myFile);
					int nextChar = -1;
	
					while((nextChar=reader.read()) != -1){
						if(checkString.indexOf(nextChar) != -1) occorrenze++;
					}
					
					reader.close();
				}catch(IOException ex){
					throw new RemoteException("Errore durante la lettura del file "+ nomeFile);
				}

				result = occorrenze;
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
