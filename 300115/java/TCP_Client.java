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
		System.out.println("E: Elimina le vocali in un file remoto");
		System.out.println("D: Download di tutti i file con due lowercase nel nome");
		System.out.print("\n^D(Unix)/^Z(Win)+invio per uscire, oppure immetti input: ");

		try {

			while ((richiesta = stdIn.readLine()) != null) {

				if (richiesta.equalsIgnoreCase("E")) {

					// Richiedo il nome del file

					System.out.println("Inserisci il nome del file:");

					String nomeFile = null;

					if((nomeFile = stdIn.readLine()) == null){
						//EOF esco
						System.out.println("Hai digitato EOF");
						socket.shutdownOutput();
						socket.shutdownInput();
						socket.close();
						return;
					}

					try {
						// Elaborazione richiesta e invio
						outSock.writeUTF(richiesta);
						System.out.println("Inviata richiesta: " + richiesta);

						//Invio nome file
						outSock.writeUTF(nomeFile);
						System.out.println("Inviato nome file: "+nomeFile);

					} catch (Exception e) {
						System.out.println("Problemi nell'invio della richiesta " + richiesta);
						e.printStackTrace();

						System.out.println("E: Elimina le vocali in un file remoto");
						System.out.println("D: Download di tutti i file con due lowercase nel nome");
						System.out.print("\n^D(Unix)/^Z(Win)+invio per uscire, oppure immetti input: ");
						continue;
						// il client continua l'esecuzione riprendendo dall'inizio del ciclo
					}

					// Ricezione della risposta

					int risposta;

					try {
						risposta = inSock.readInt();
						System.out.println("Risposta: " + risposta);

					} catch (SocketTimeoutException ste) {
						System.out.println("Timeout scattato: ");
						ste.printStackTrace();

						System.out.println("E: Elimina le vocali in un file remoto");
						System.out.println("D: Download di tutti i file con due lowercase nel nome");
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

						System.out.println("E: Elimina le vocali in un file remoto");
						System.out.println("D: Download di tutti i file con due lowercase nel nome");
						System.out.print("\n^D(Unix)/^Z(Win)+invio per uscire, oppure immetti input: ");
						continue;
						// il client continua l'esecuzione riprendendo dall'inizio del ciclo
					}
				} else if (richiesta.equalsIgnoreCase("D")) {
					//Invio il nome del direttorio

					System.out.println("Inserisci il nome del direttorio:");
					String nomeDir = null;

					if((nomeDir = stdIn.readLine()) == null){
						//EOF esco
						System.out.println("Hai digitato EOF");
						socket.shutdownOutput();
						socket.shutdownInput();
						socket.close();
						return;
					}

					try {
						// Elaborazione richiesta e invio
						outSock.writeUTF(richiesta);
						System.out.println("Inviata richiesta: " + richiesta);

						//Invio nome file
						outSock.writeUTF(nomeDir);
						System.out.println("Inviato nome dir: "+nomeDir);

					} catch (Exception e) {
						System.out.println("Problemi nell'invio della richiesta " + richiesta);
						e.printStackTrace();

						System.out.println("E: Elimina le vocali in un file remoto");
						System.out.println("D: Download di tutti i file con due lowercase nel nome");
						System.out.print("\n^D(Unix)/^Z(Win)+invio per uscire, oppure immetti input: ");
						continue;
						// il client continua l'esecuzione riprendendo dall'inizio del ciclo
					}

					// Ricezione della risposta

					try {
						int nFiles = inSock.readInt();
						System.out.println("nFiles: " + nFiles);


						for (int i = 0; i < nFiles; i++) {
							String nomeFile = inSock.readUTF();
							File myFile = new File(nomeFile);

							long fileLength = inSock.readLong();

							if(!myFile.exists()){
								myFile.createNewFile();

								if(myFile.canWrite()){
									OutputStream oStream = new FileOutputStream(myFile);

									for (long j = 0; j < fileLength; j++) {
										oStream.write(inSock.readByte());
									}

									oStream.close();
								}else{
									System.out.println("Errore download file "+nomeFile+", impossibile scrivere");	
									System.out.println("Scarto il file...");
								}
							}else{
								System.out.println("Errore download file "+nomeFile+", già esistente");
								System.out.println("Scarto il file...");
							}
						}
					} catch (SocketTimeoutException ste) {
						System.out.println("Timeout scattato: ");
						ste.printStackTrace();

						System.out.println("E: Elimina le vocali in un file remoto");
						System.out.println("D: Download di tutti i file con due lowercase nel nome");
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

						System.out.println("E: Elimina le vocali in un file remoto");
						System.out.println("D: Download di tutti i file con due lowercase nel nome");
						System.out.print("\n^D(Unix)/^Z(Win)+invio per uscire, oppure immetti input: ");
						continue;
						// il client continua l'esecuzione riprendendo dall'inizio del ciclo
					}

				} else
					System.out.println("Errore nella scelta...");

				// Tutto ok, pronto per nuova richiesta
				System.out.println("E: Elimina le vocali in un file remoto");
				System.out.println("D: Download di tutti i file con due lowercase nel nome");
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