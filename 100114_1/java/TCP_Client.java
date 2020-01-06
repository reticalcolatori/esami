/************************************
*     TCP_Client.java	            *
************************************/

/* CLIENTE CON CONNESSIONE*/

import java.net.*;
import java.io.*;

public class TCP_Client {

	/**
	 * @param args - Usage: java TCP_Client <address> <port>
	 */

	public static void main(String[] args) throws IOException {
		/*
		 * Come argomenti si devono passare un nome di host valido e una porta
		 * 
		 */
		InetAddress addr = null;
		int port = -1;

		// Controllo argomenti
		try {
			if (args.length == 2) {
				addr = InetAddress.getByName(args[0]);
				port = Integer.parseInt(args[1]);
			} else {
				System.out.println("Usage: java TCP_Client serverAddr serverPort");
				System.exit(1);
			}
		} catch (NumberFormatException e) {
			System.out.println("Numero di porta errato: " + args[1]);
			e.printStackTrace();
			System.out.println("Usage: java TCP_Client serverAddr serverPort");
			System.exit(1);
		} catch (Exception e) {
			System.out.println("Problemi, i seguenti: ");
			e.printStackTrace();
			System.out.println("Usage: java TCP_Client serverAddr serverPort");
			System.exit(1);
		}

		/** ----- Inizializzazione strutture dati Cliente ----- **/
		Socket socket = null;
		DataInputStream inSock = null;
		DataOutputStream outSock = null;
		String richiesta = null;

		// Creazione socket
		// Creazione stream di input/output su socket
		try {
			socket = new Socket(addr, port);
			// Setto il timeout per non bloccare indefinitivamente il client
			socket.setSoTimeout(30000);
			System.out.println("Creata la socket: " + socket);
			inSock = new DataInputStream(socket.getInputStream());
			outSock = new DataOutputStream(socket.getOutputStream());
		} catch (IOException e) {
			System.out.println("Problemi nella creazione degli stream su socket: ");
			e.printStackTrace();
			System.exit(1);
		} catch (Exception e) {
			System.out.println("Problemi nella creazione della socket: ");
			e.printStackTrace();
			System.exit(2);
		}

		// Eventuale stream per l'interazione con l'utente da tastiera

		BufferedReader stdIn = new BufferedReader(new InputStreamReader(System.in));
		System.out.print("\n^D(Unix)/^Z(Win)+invio per uscire, oppure immetti input: ");
		try {

			while ((richiesta = stdIn.readLine()) != null) {

				if (richiesta.equalsIgnoreCase("A")) {

					//Prima ricavo tipo e luogo

					System.out.println("Inserisci tipo");
					String tipo = stdIn.readLine();

					if(tipo == null){
						System.out.println("EOF Ricevuto");
						socket.shutdownInput();
						socket.shutdownOutput();
						socket.close();
						return;
					}

					System.out.println("Inserisci luogo");
					String luogo = stdIn.readLine();

					if(luogo == null){
						System.out.println("EOF Ricevuto");
						socket.shutdownInput();
						socket.shutdownOutput();
						socket.close();
						return;
					}


					try {
						// Elaborazione richiesta e invio
						outSock.writeUTF(richiesta);
						outSock.writeUTF(tipo);
						outSock.writeUTF(luogo);
						System.out.println("Inviata richiesta: " + richiesta);
					} catch (Exception e) {
						System.out.println("Problemi nell'invio della richiesta " + richiesta);
						e.printStackTrace();
						
						System.out.print("\n^D(Unix)/^Z(Win)+invio per uscire, oppure immetti input: ");
						continue;
						// il client continua l'esecuzione riprendendo dall'inizio del ciclo
					}

					// Ricezione della risposta

					try {
						
						int lengthTable = inSock.readInt();

						for (int i = 0; i < lengthTable; i++) {
							System.out.println(inSock.readUTF());
						}

					} catch (SocketTimeoutException ste) {
						System.out.println("Timeout scattato: ");
						ste.printStackTrace();

						// socket.close();

						System.out.print("\n^D(Unix)/^Z(Win)+invio per uscire, oppure immetti input: ");
						continue;
						// il client continua l'esecuzione riprendendo dall'inizio del ciclo
					}

					catch (EOFException e) {
						System.out.println("Raggiunta la fine delle ricezioni, chiudo...");

						socket.shutdownInput();
						socket.shutdownOutput();
		      			socket.close();

						System.out.println("TCP_Client: termino...");
						return;
					} catch (Exception e) {
						System.out.println("Problemi nella ricezione della risposta, i seguenti: ");
						e.printStackTrace();

						// socket.close();

						System.out.print("\n^D(Unix)/^Z(Win)+invio per uscire, oppure immetti input: ");
						continue;
						// il client continua l'esecuzione riprendendo dall'inizio del ciclo
					}
				} else if (richiesta.equalsIgnoreCase("U")) {
					//Prima ricavo prezzo max

					System.out.println("Inserisci prezzo max");
					String prezzoMaxString = stdIn.readLine();

					if(prezzoMaxString == null){
						System.out.println("EOF Ricevuto");
						socket.shutdownInput();
						socket.shutdownOutput();
						socket.close();
						return;
					}

					int prezzoMax = -1;

					try{
						prezzoMax = Integer.parseInt(prezzoMaxString);
					}catch(NumberFormatException ex){
						System.out.println("Inserito prezzo non valido");
						continue;
					}

					try {
						// Elaborazione richiesta e invio
						outSock.writeUTF(richiesta);
						outSock.writeInt(prezzoMax);
						System.out.println("Inviata richiesta: " + richiesta);
					} catch (Exception e) {
						System.out.println("Problemi nell'invio della richiesta " + richiesta);
						e.printStackTrace();
						
						System.out.print("\n^D(Unix)/^Z(Win)+invio per uscire, oppure immetti input: ");
						continue;
					}

					// Ricezione della risposta

					try {
						
						int lengthTable = inSock.readInt();

						for (int i = 0; i < lengthTable; i++) {
							System.out.println(inSock.readUTF());
						}

					} catch (SocketTimeoutException ste) {
						System.out.println("Timeout scattato: ");
						ste.printStackTrace();

						// socket.close();

						System.out.print("\n^D(Unix)/^Z(Win)+invio per uscire, oppure immetti input: ");
						continue;
						// il client continua l'esecuzione riprendendo dall'inizio del ciclo
					}

					catch (EOFException e) {
						System.out.println("Raggiunta la fine delle ricezioni, chiudo...");

						socket.shutdownInput();
						socket.shutdownOutput();
						socket.close();

						System.out.println("TCP_Client: termino...");
						return;
					} catch (Exception e) {
						System.out.println("Problemi nella ricezione della risposta, i seguenti: ");
						e.printStackTrace();

						// socket.close();

						System.out.print("\n^D(Unix)/^Z(Win)+invio per uscire, oppure immetti input: ");
						continue;
						// il client continua l'esecuzione riprendendo dall'inizio del ciclo
					}
				} else
					System.out.println("Errore nella scelta...");

				// Tutto ok, pronto per nuova richiesta
				System.out.print("\n^D(Unix)/^Z(Win)+invio per uscire, oppure immetti input: ");

			} // while

			System.out.println("TCP_Client: termino...");
		}

		// qui catturo le eccezioni non catturate all'interno del while
		// quali per esempio la caduta della connessione con il server
		// in seguito alle quali il client termina l'esecuzione
		catch (Exception e) {
			System.err.println("Errore irreversibile, il seguente: ");
			e.printStackTrace();
			System.err.println("Chiudo!");
			System.exit(3);
		}
	} // main
}