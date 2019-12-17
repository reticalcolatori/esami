package model;

import java.rmi.*;
import java.io.*;

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

		// Connessione al servizio RMI remoto
		try {
			String completeName = "//" + registryHost + ":" + registryPort + "/" + serviceName;
			RMI_interfaceFile serverRMI = (RMI_interfaceFile) Naming.lookup(completeName);
			System.out.println("ClientRMI: Servizio \"" + serviceName + "\" connesso");

			System.out.println("\nRichieste di servizio fino a fine file");

			String service;
			System.out.print("\nServizio ? \n1) inserisci_sci\n2) noleggia_sci\n(ctrl+d) per terminare: \n");

			while ((service = stdIn.readLine()) != null) {
				// Richiamo i metodi remoti e controllo che tutto vada bene
				if (service.equals("1")) {
					// LOGICA APPLICATIVA
					
					String id, modello;
					int costoGG;
					
					System.out.println("Inserisci id:");
					id = stdIn.readLine();
					
					System.out.println("Inserisci modello:");
					modello = stdIn.readLine();
					
					System.out.println("Inserisci costoGG:");
					costoGG = Integer.parseInt(stdIn.readLine());

					// Eseguo il metodo richiesto
					try {
						int esito = serverRMI.inserisci_sci(id, modello, costoGG);

						if (esito == 0) {
							System.out.println("Inserimento andato a buon fine");
						} else if (esito < 0) {
							System.out.println("Errore metodo 1");
						}
					} catch (RemoteException re) {
						System.out.println("Errore Remoto: " + re.toString());
					}
				} // Fine 1

				else if (service.equals("2")) {
					// LOGICA APPLICATIVA
					
					String id;
					int giorno, mese, anno, durata;
					
					System.out.println("Inserisci id:");
					id = stdIn.readLine();
					
					System.out.println("Inserisci giorno:");
					giorno = Integer.parseInt(stdIn.readLine());
					
					System.out.println("Inserisci mese:");
					mese = Integer.parseInt(stdIn.readLine());
					
					System.out.println("Inserisci anno:");
					anno = Integer.parseInt(stdIn.readLine());
					
					System.out.println("Inserisci durata:");
					durata = Integer.parseInt(stdIn.readLine());
					
					try {
						int esito = serverRMI.noleggia_sci(id, giorno, mese, anno, durata);
						
						if (esito == 0) {
							System.out.println("Inserimento andato a buon fine");
						} else if (esito < 0) {
							System.out.println("Errore metodo 2");
						}
					} catch (RemoteException re) {
						System.out.println("Errore Remoto: " + re.toString());
					}
				}

				else {
					System.out.println("Servizio non disponibile");
				}

				System.out.print("\nServizio ? \n1) Metodo1\n2) Metodo2\n(ctrl+d) per terminare: \n");
			}

		} catch (Exception e) {
			System.err.println("ClientRMI: " + e.getMessage());
			e.printStackTrace();
			System.exit(1);
		}
	}
}
