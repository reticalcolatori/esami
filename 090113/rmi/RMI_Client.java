
/************************************
*          *
*           RMI_Client.java         *
*                                   *
************************************/

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
		if (args.length < 1) {
			System.out.println("Sintassi: ClientFile NomeHost [registryPort]");
			System.exit(1);
		}

		registryHost = args[0];

		if (args.length == 2) {
			try {
				registryPort = Integer.parseInt(args[1]);
			} catch (Exception e) {
				System.out.println("Sintassi: ClientFile NomeHost [registryPort], registryPort intero");
				System.exit(2);
			}
		}

		// Connessione al servizio RMI remoto
		try {

			//Impostazioni delle policy di sicurezza
			// Impostazione del SecurityManager
			if (System.getSecurityManager() == null){
				System.setProperty("java.security.policy", "rmi.policy");
				System.setSecurityManager(new RMISecurityManager());
			}
			
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

					// Richiesta id:
					System.out.println("ID:");
					String id = stdIn.readLine();

					if (id == null) {
						// EOF
						break;
					}

					// Richiesta modello:
					System.out.println("Modello:");
					String modello = stdIn.readLine();

					if (modello == null) {
						// EOF
						break;
					}

					// Richiesta costo giornaliero:
					System.out.println("Costo giornaliero:");
					String costoGiornalieroString = stdIn.readLine();

					if (costoGiornalieroString == null) {
						// EOF
						break;
					}

					int costoGiornaliero = -1;

					try {
						costoGiornaliero = Integer.parseInt(costoGiornalieroString);
					} catch (NumberFormatException e) {
						System.out.println("Inserisci un intero");
						System.out.print("\nServizio ? \n1) inserisci_sci\n2) noleggia_sci\n(ctrl+d) per terminare: \n");
						continue;
					}

					// Checks sul costo
					if (costoGiornaliero <= 0) {
						System.out.println("Inserisci un costo positivo.");
						System.out.print("\nServizio ? \n1) inserisci_sci\n2) noleggia_sci\n(ctrl+d) per terminare: \n");
						continue;
					}

					// Eseguo il metodo richiesto
					try {
						int esito = serverRMI.inserisci_sci(id, modello, costoGiornaliero);

						if (esito >= 0) {
							System.out.println("Inserimento andato a buon fine: " + esito);
						} else if (esito < 0) {
							System.out.println("Errore metodo inserisci_sci: " + esito);
						}
					} catch (RemoteException re) {
						System.out.println("Errore Remoto inserisci_sci: " + re.toString());
					}
				} // Fine 1
				else if (service.equals("2")) {
					// LOGICA APPLICATIVA

					// Richiesta id:
					System.out.println("ID:");
					String id = stdIn.readLine();

					if (id == null) {
						// EOF
						break;
					}

					// Richiesta giorno:
					System.out.println("Giorno:");
					String giornoString = stdIn.readLine();

					if (giornoString == null) {
						// EOF
						break;
					}

					int giorno = -1;

					try {
						giorno = Integer.parseInt(giornoString);
					} catch (NumberFormatException e) {
						System.out.println("Inserisci un intero");
						System.out.print("\nServizio ? \n1) inserisci_sci\n2) noleggia_sci\n(ctrl+d) per terminare: \n");
						continue;
					}

					// Checks sul giorno
					if (giorno <= 0) {
						System.out.println("Inserisci un giorno strettamente positivo.");
						System.out.print("\nServizio ? \n1) inserisci_sci\n2) noleggia_sci\n(ctrl+d) per terminare: \n");
						continue;
					}

					if (giorno > 30) {
						System.out.println("Inserisci un giorno <= 30.");
						System.out.print("\nServizio ? \n1) inserisci_sci\n2) noleggia_sci\n(ctrl+d) per terminare: \n");
						continue;
					}

					// Richiesta mese:
					System.out.println("Mese:");
					String meseString = stdIn.readLine();

					if (meseString == null) {
						// EOF
						break;
					}

					int mese = -1;

					try {
						mese = Integer.parseInt(meseString);
					} catch (NumberFormatException e) {
						System.out.println("Inserisci un intero");
						System.out.print("\nServizio ? \n1) inserisci_sci\n2) noleggia_sci\n(ctrl+d) per terminare: \n");
						continue;
					}

					// Checks sul mese
					if (mese <= 0) {
						System.out.println("Inserisci un mese strettamente positivo.");
						System.out.print("\nServizio ? \n1) inserisci_sci\n2) noleggia_sci\n(ctrl+d) per terminare: \n");
						continue;
					}

					if (mese > 12) {
						System.out.println("Inserisci un mese <= 12.");
						System.out.print("\nServizio ? \n1) inserisci_sci\n2) noleggia_sci\n(ctrl+d) per terminare: \n");
						continue;
					}

					// Richiesta anno:
					System.out.println("Anno:");
					String annoString = stdIn.readLine();

					if (annoString == null) {
						// EOF
						break;
					}

					int anno = -1;

					try {
						anno = Integer.parseInt(annoString);
					} catch (NumberFormatException e) {
						System.out.println("Inserisci un intero");
						System.out.print("\nServizio ? \n1) inserisci_sci\n2) noleggia_sci\n(ctrl+d) per terminare: \n");
						continue;
					}

					// Checks sul anno
					if (anno <= 0) {
						System.out.println("Inserisci un anno strettamente positivo.");
						System.out.print("\nServizio ? \n1) inserisci_sci\n2) noleggia_sci\n(ctrl+d) per terminare: \n");
						continue;
					}

					// Richiesta durata:
					System.out.println("Durata:");
					String durataString = stdIn.readLine();

					if (durataString == null) {
						// EOF
						break;
					}

					int durata = -1;

					try {
						durata = Integer.parseInt(durataString);
					} catch (NumberFormatException e) {
						System.out.println("Inserisci un intero");
						System.out.print("\nServizio ? \n1) inserisci_sci\n2) noleggia_sci\n(ctrl+d) per terminare: \n");
						continue;
					}

					// Checks sul durata
					if (durata <= 0) {
						System.out.println("Inserisci una durata strettamente positivo.");
						System.out.print("\nServizio ? \n1) inserisci_sci\n2) noleggia_sci\n(ctrl+d) per terminare: \n");
						continue;
					}

					// Eseguo il metodo richiesto
					try {
						int esito = serverRMI.noleggia_sci(id, giorno, mese, anno, durata);

						if (esito == 0) {
							System.out.println("Inserimento andato a buon fine: "+esito);
						} else if (esito < 0) {
							System.out.println("Errore metodo noleggia_sci: "+esito);
						}
					} catch (RemoteException re) {
						System.out.println("Errore Remoto: " + re.toString());
					}
				} else {
					System.out.println("Servizio non disponibile");
				}

				System.out.print("\nServizio ? \n1) inserisci_sci\n2) noleggia_sci\n(ctrl+d) per terminare: \n");
			}

		} catch (Exception e) {
			System.err.println("ClientRMI: " + e.getMessage());
			e.printStackTrace();
			System.exit(1);
		}
	}
}
