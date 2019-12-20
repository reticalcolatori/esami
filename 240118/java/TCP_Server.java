/************************************
*           *
*     TCP_Server.java		    *
*           		            *
************************************/
import java.io.*;
import java.net.*;


public class TCP_Server
{
    // porta nel range consentito 1024-65535!
    // dichiarata come statica perche caratterizza il server
	private static final int PORT = 54321;

	public static void main (String[] args) throws IOException
	{
      	// Porta sulla quale ascolta il server
		int port = -1;

      	// Controllo degli argomenti
		try 
		{
			if (args.length == 1) 
			{
				port = Integer.parseInt(args[0]);
	    		// controllo che la porta sia nel range consentito 1024-65535
				if (port < 1024 || port > 65535) 
				{
					System.out.println("Usage: java TCP_Server [serverPort>1024]");
					System.exit(1);
				}
			} 
			else if (args.length == 0) 
			{
				port = PORT;
			} 
			else 
			{
				System.out.println("Usage: java TCP_Server or java TCP_Server port");
				System.exit(1);
			}
		} 
		catch (Exception e) 
		{
			System.out.println("Problemi, i seguenti: ");
			e.printStackTrace();
			System.out.println("Usage: java TCP_Server or java TCP_Server port");
			System.exit(1);
		}
		/******** INIZIALIZZAZIONE TABELLA ************/

		/*********************************************/

		ServerSocket serverSocket = null;
		Socket clientSocket = null;
		try 
		{
			serverSocket = new ServerSocket(port);
			serverSocket.setReuseAddress(true);
			System.out.println("TCP_Server: avviato ");
			System.out.println("Server: creata la server socket: " + serverSocket);
		}
		catch (Exception e) 
		{
			System.err.println("Server: problemi nella creazione della server socket: "+ e.getMessage());
			e.printStackTrace();
			System.exit(1);
		}

		try 
		{
			while (true) 
			{
				System.out.println("Server: in attesa di richieste...\n");

				try 
				{
	    			// bloccante finchè non avviene una connessione
					clientSocket = serverSocket.accept();
	    			// timeout per evitare che un thread si blocchi indefinitivamente
					clientSocket.setSoTimeout(60000);

					System.out.println("Server: connessione accettata: " + clientSocket);
				}
				catch (Exception e) 
				{
					System.err.println("Server: problemi nella accettazione della connessione: "+ e.getMessage());
					e.printStackTrace();
					continue;
	    			// il server continua a fornire il servizio ricominciando dall'inizio del ciclo
				}

	  			// delego il servizio ad un nuovo thread
				try 
				{
					new TCP_ServerThread(clientSocket).start();
				}
				catch (Exception e) 
				{
					System.err.println("Server: problemi nel server thread: "+ e.getMessage());
					e.printStackTrace();
					continue;
	   				// il server continua a fornire il servizio ricominciando dall'inizio del ciclo
				}

			} 
		}
	     // qui catturo le eccezioni non catturate all'interno del while
	     // in seguito alle quali il server termina l'esecuzione
		catch (Exception e) 
		{
			e.printStackTrace();
			System.out.println("TCP_Server: termino...");
			System.exit(2);
		}

	}
} // TCP_Server


// Thread lanciato per ogni richiesta accettata
class TCP_ServerThread extends Thread
{
	private Socket clientSocket = null;

    //COSTRUTTORE - va opportunamente creato
	public TCP_ServerThread(Socket clientSocket) 
	{
		this.clientSocket = clientSocket;
	}

	public void run() 
	{
		System.out.println("Info: Attivazione server thread. (Thread=" + getName() + ")");

		DataInputStream inSock;
		DataOutputStream outSock;

		String richiesta = null;
		String risposta = null;

		try 
		{
			try 
			{
				// creazione stream di input e out da socket
				inSock = new DataInputStream(clientSocket.getInputStream());
				outSock = new DataOutputStream(clientSocket.getOutputStream());
			}
			catch (IOException ioe) 
			{
				System.out.println("Problemi nella creazione degli stream di input/output "+ "su socket: ");
				ioe.printStackTrace();
				// il server continua l'esecuzione riprendendo dall'inizio del ciclo
				return;
			}
			catch (Exception e) 
			{
				System.out.println("Problemi nella creazione degli stream di input/output "+ "su socket: ");
				e.printStackTrace();
				// il server continua l'esecuzione riprendendo dall'inizio del ciclo
				return;
			}

	    	// Ricezione della richiesta
			richiesta = null;

			try 
			{
				richiesta = inSock.readUTF();
			}
			catch(SocketTimeoutException ste)
			{
				System.out.println("Timeout scattato: ");
				ste.printStackTrace();
				clientSocket.close();
				return;          
			}        
			catch (Exception e) 
			{
				System.out.println("Problemi nella ricezione della richiesta: ");
				e.printStackTrace();
				return;
			}

			//Struttura richesta Elimina occorrenza
			//Stringa parola
			//Stringa nome file testo

			if(richiesta.equalsIgnoreCase("E"))
			{
				//Richiedo ulteriori info.
				String parola = null;
				String nomeFile = null;

				try
				{
					parola = inSock.readUTF();
					nomeFile = inSock.readUTF();
				}
				catch(SocketTimeoutException ste)
				{
					System.out.println("Timeout scattato: ");
					ste.printStackTrace();
					clientSocket.close();
					return;
				}
				catch (Exception e)
				{
					System.out.println("Problemi nella ricezione della richiesta: ");
					e.printStackTrace();
					return;
				}

				//Checks

				if(parola.isEmpty()){
					risposta = "Parola non valida";
				}else{
					if(nomeFile.isEmpty()){
						risposta = "Nome File non valido";
					}else{

						//Apertura del file
						File myFile = new File(nomeFile);

						if(myFile.exists()){
							if(myFile.canRead() && myFile.canWrite()){
								File myFileTemp = new File(Thread.currentThread().getId()+".tmp");
								myFileTemp.createNewFile();

								BufferedReader readFileStream = new BufferedReader(new FileReader(myFile));
								BufferedWriter outputFileStream = new BufferedWriter(new FileWriter(myFileTemp));

								String linea = null;
								int occorrenze = 0;

								while((linea=readFileStream.readLine()) != null){
									String[] split = linea.split(" ");

									StringBuilder stringBuilder = new StringBuilder();

									for (String parolaX:split) {
										if(!parolaX.equalsIgnoreCase(parola)){
											stringBuilder.append(parolaX).append(' ');
										}else{
											occorrenze++;
										}
									}

									outputFileStream.write(stringBuilder.toString().trim());
									outputFileStream.newLine();
								}

								readFileStream.close();
								outputFileStream.close();

								//Copio contenuto dei file temporanei in quello definitivo
								myFile.delete();
								myFile.createNewFile();

								readFileStream = new BufferedReader(new FileReader(myFileTemp));
								outputFileStream = new BufferedWriter(new FileWriter(myFile));

								while((linea=readFileStream.readLine()) != null){
									outputFileStream.write(linea);
									outputFileStream.newLine();
								}

								readFileStream.close();
								outputFileStream.close();

								myFileTemp.delete();

								risposta = "Ho eliminato "+occorrenze+" parole";

							}else{
								risposta = "Non posso leggere/scrivere il file";
							}
						}else{
							risposta = "File non trovato";
						}
					}
				}

				try
				{
					outSock.writeUTF(risposta);
					clientSocket.shutdownOutput();
					System.out.println("Terminata connessione con " + clientSocket);
				}
				catch(SocketTimeoutException ste)
				{
					System.out.println("Timeout scattato: ");
					ste.printStackTrace();
					clientSocket.close();
					System.out.print("\n^D(Unix)/^Z(Win)+invio per uscire, solo invio per continuare: ");
					// il client continua l'esecuzione riprendendo dall'inizio del ciclo
					return;          
				}              
				catch (Exception e) 
				{
					System.err.println("\nProblemi nell'invio della risposta: "+ e.getMessage());
					e.printStackTrace();
					clientSocket.close();
					System.out.println("Terminata connessione con " + clientSocket);
					return;
					// il server continua a fornire il servizio ricominciando
					// dall'inizio del ciclo esterno
				}
			}
			else if(richiesta.equalsIgnoreCase("T"))
			{
				//Richiedo ulteriori info.
				String direttorio = null;
				int soglia = 0;

				//Invio prima il numero di file da inviare.
				int nFiles = 0;


				try
				{
					direttorio = inSock.readUTF();
					soglia = inSock.readInt();
				}
				catch(SocketTimeoutException ste)
				{
					System.out.println("Timeout scattato: ");
					ste.printStackTrace();
					clientSocket.close();
					return;
				}
				catch (Exception e)
				{
					System.out.println("Problemi nella ricezione della richiesta: ");
					e.printStackTrace();
					return;
				}

				//Checks
				if(direttorio.isEmpty()){
					risposta = "Direttorio non valido";
				}else{
					if(soglia < 0){
						risposta = "Soglia >=0";
					}else{

						//Verifica disponiblità direttorio
						File myDir = new File(direttorio);

						if(myDir.isDirectory()){
							File[] ls = myDir.listFiles();

							for (File element:ls) {
								if (element.isFile()){
									if(element.canRead()){
										if (element.length() >= soglia){
											//posso inviarlo allo zio.
											nFiles++;
										}
									}
								}
							}

							risposta = "ok";

							try
							{
								outSock.writeUTF(risposta);
								outSock.writeInt(nFiles);

								for (File element:ls) {
									if (element.isFile()){
										if(element.canRead()){
											if (element.length() >= soglia){
												//posso inviarlo allo zio.

												//Invio il file all'utente: nomeFile: dimensione : contenuto
												outSock.writeUTF(element.getName());
												outSock.writeLong(element.length());

												BufferedInputStream streamFile = new BufferedInputStream(new FileInputStream(element));

												int x = -1;

												//Restituisce -1 se EOF
												while((x=streamFile.read()) != -1){
													outSock.write(x);
												}

												streamFile.close();
											}
										}
									}
								}
							}
							catch(SocketTimeoutException ste)
							{
								System.out.println("Timeout scattato: ");
								ste.printStackTrace();
								clientSocket.close();
								System.out.print("\n^D(Unix)/^Z(Win)+invio per uscire, solo invio per continuare: ");
								// il client continua l'esecuzione riprendendo dall'inizio del ciclo
								return;
							}
							catch (Exception e)
							{
								System.err.println("\nProblemi nell'invio della risposta: "+ e.getMessage());
								e.printStackTrace();
								clientSocket.close();
								System.out.println("Terminata connessione con " + clientSocket);
								return;
							}

						}else{
							risposta = "Direttorio non è direttorio";
						}
					}
				}

				try
				{
					outSock.writeUTF(risposta);
					clientSocket.shutdownOutput();
					System.out.println("Terminata connessione con " + clientSocket);
				}
				catch(SocketTimeoutException ste)
				{
					System.out.println("Timeout scattato: ");
					ste.printStackTrace();
					clientSocket.close();
					System.out.print("\n^D(Unix)/^Z(Win)+invio per uscire, solo invio per continuare: ");
					// il client continua l'esecuzione riprendendo dall'inizio del ciclo
					return;
				}
				catch (Exception e)
				{
					System.err.println("\nProblemi nell'invio della risposta: "+ e.getMessage());
					e.printStackTrace();
					clientSocket.close();
					System.out.println("Terminata connessione con " + clientSocket);
					return;
				}
			}else{
				try
				{
					outSock.writeUTF("Comando non riconosciuto");
					clientSocket.shutdownOutput();
					System.out.println("Terminata connessione con " + clientSocket);
				}
				catch(SocketTimeoutException ste)
				{
					System.out.println("Timeout scattato: ");
					ste.printStackTrace();
					clientSocket.close();
					System.out.print("\n^D(Unix)/^Z(Win)+invio per uscire, solo invio per continuare: ");
					// il client continua l'esecuzione riprendendo dall'inizio del ciclo
					return;
				}
				catch (Exception e)
				{
					System.err.println("\nProblemi nell'invio della risposta: "+ e.getMessage());
					e.printStackTrace();
					clientSocket.close();
					System.out.println("Terminata connessione con " + clientSocket);
					return;
				}
			}
		}
	// qui catturo le eccezioni non catturate all'interno del while
	// in seguito alle quali il server termina l'esecuzione
		catch (Exception e) 
		{
			e.printStackTrace();
			System.out.println("Errore irreversibile, TCP_ServerThread: termino...");
			System.exit(3);
		}
    } // run

} // TCP_ServerThread


