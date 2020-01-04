/************************************
*           RMI_Client.java         *
************************************/

import java.rmi.*;
import java.io.*;
import java.util.*;

public class RMI_Client {
	/* --- AVVIO DEL CLIENT RMI ---- */
	public static void main(String[] args) {
		int registryPort = 1099;
		String registryHost = null;
		String serviceName = "ServerRMI";
		BufferedReader stdIn = new BufferedReader(new InputStreamReader(System.in));

		// Controllo degli argomenti
		if (args.length != 1 && args.length != 2) {
			System.out.println("Sintassi: ClientFile NomeHost [registryPort]");
			System.exit(1);
		} else {
			registryHost = args[0];
			if (args.length == 2) {
				try {
					registryPort = Integer.parseInt(args[1]);
				} catch (Exception e) {
					System.out.println("Sintassi: ClientFile NomeHost [registryPort], registryPort intero");
					System.exit(2);
				}
			}
		}

		// Impostazione del SecurityManager
		if (System.getSecurityManager() == null){
			System.setProperty("java.security.policy", "rmi.policy");
			System.setSecurityManager(new RMISecurityManager());
		}

		// Connessione al servizio RMI remoto
		try {
			String completeName = "//" + registryHost + ":" + registryPort + "/" + serviceName;
			RMI_interfaceFile serverRMI = (RMI_interfaceFile) Naming.lookup(completeName);
			System.out.println("ClientRMI: Servizio \"" + serviceName + "\" connesso");

			System.out.println("\nRichieste di servizio fino a fine file");

			String service;
			System.out.print("\nServizio ? \n1) lista_file_caratteri\n2) conta_occorrenze_interpunzione\n(ctrl+d) per terminare: \n");

			while ((service = stdIn.readLine()) != null) {
				// Richiamo i metodi remoti e controllo che tutto vada bene
				if (service.equals("1")) {
					// LOGICA APPLICATIVA

					System.out.println("Inserisci il nome del direttorio:");

					String nomeDir = stdIn.readLine();

					if(nomeDir == null){
						//EOF: esco
						System.out.println("Ricevuto EOF");
						return;
					}

					System.out.println("Inserisci il carattere A:");

					String charAString = stdIn.readLine();

					if(charAString == null){
						//EOF: esco
						System.out.println("Ricevuto EOF");
						return;
					}

					System.out.println("Inserisci il carattere B:");

					String charBString = stdIn.readLine();

					if(charBString == null){
						//EOF: esco
						System.out.println("Ricevuto EOF");
						return;
					}

					char charA = charAString.charAt(0);
					char charB = charBString.charAt(0);


					// Eseguo il metodo richiesto
					try {
						List<String> esito;
						esito = serverRMI.lista_file_caratteri(nomeDir, charA, charB);

						if (esito.size() == 0) {
							System.out.println("Nessun file trovato");
						} else  {
							System.out.println("Ricevuti "+esito.size()+" files.");

							System.out.println("-----------------");

							for (String nomeFile : esito) {
								System.out.println(nomeFile);
							}

							System.out.println("-----------------");
						}
					} catch (RemoteException re) {
						System.out.println("Errore Remoto: " + re.toString());
					}
				} // Fine 1

				else if (service.equals("2")) {
					// LOGICA APPLICATIVA

					System.out.println("Inserisci il nome del file:");

					String nomeFile = stdIn.readLine();

					if(nomeFile == null){
						//EOF: esco
						System.out.println("Ricevuto EOF");
						return;
					}


					// Eseguo il metodo richiesto
					try {
						int esito = -1;

						esito = serverRMI.conta_occorrenze_interpunzione(nomeFile);

						if (esito >= 0) {
							System.out.println("Numero occorrenze: "+esito);
						} else if (esito < 0) {
							System.out.println("Errore metodo conta_occorrenze_interpunzione");
						}
					} catch (RemoteException re) {
						System.out.println("Errore Remoto: " + re.toString());
					}
				}

				else {
					System.out.println("Servizio non disponibile");
				}

				System.out.print("\nServizio ? \n1) lista_file_caratteri\n2) conta_occorrenze_interpunzione\n(ctrl+d) per terminare: \n");
			}

		} catch (Exception e) {
			System.err.println("ClientRMI: " + e.getMessage());
			e.printStackTrace();
			System.exit(1);
		}
	}
}
