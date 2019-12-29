/************************************
*     TCP_Server.java		        *
************************************/
import java.io.*;
import java.net.*;


public class TCP_Server
{
    // porta nel range consentito 1024-65535!
    // dichiarata come statica perche caratterizza il server
	private static final int PORT = 54321;

	public static final int N = 5;

	public static Riga[] tabella = new Riga[TCP_Server.N];

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

		for (int i = 0; i < N; i++) {
			tabella[i] = new Riga();
		}

		tabella[0] = new Riga("SATA1234", "Partenza", "Bologna", "Bari", 12, 15, 0, "SATA1234.mp4");
		tabella[1] = new Riga("MATA3333", "Arrivo", "Milano", "Bologna", 16, 30, 21, "MATA3333.mp4");
		tabella[3] = new Riga("CATA1111", "Partenza", "Bologna", "Napoli", 13, 00, 190, "CATA1111.mp4");

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
					new TCP_ServerThread(clientSocket, tabella).start();
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
	private Riga[] tabella;

    //COSTRUTTORE - va opportunamente creato
	public TCP_ServerThread(Socket clientSocket, Riga[] tabella) 
	{
		this.clientSocket = clientSocket;
		this.tabella = tabella;
	}

	public void run() 
	{
		System.out.println("Info: Attivazione server thread. (Thread=" + getName() + ")");
		DataInputStream inSock;
		DataOutputStream outSock;
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
				System.err.println("Problemi nella creazione degli stream di input/output "+ "su socket: ");
				ioe.printStackTrace();
				return;
			}
			catch (Exception e) 
			{
				System.err.println("Problemi nella creazione degli stream di input/output "+ "su socket: ");
				e.printStackTrace();
				return;
			}

	    	// Ricezione della richiesta
			String richiesta;

			while(true){
				try 
				{
					richiesta = inSock.readUTF();
					
					// if(richiesta == null)
					// {
					// 	System.out.println("EOF");
					// 	//Devo ritornare ho ricevuto EOF
					// 	clientSocket.shutdownInput();
					// 	clientSocket.shutdownOutput();
					// 	clientSocket.close();
					// 	return;
					// }
	
					System.out.println("Richiesta: " + richiesta);
				}
				catch(EOFException ex){
					System.out.println("EOF");
					//Devo ritornare ho ricevuto EOF
					clientSocket.shutdownInput();
					clientSocket.shutdownOutput();
					clientSocket.close();
					return;
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
					// servo nuove richieste
					return;
				}
	
				//Download file audio di  annuncio  per  viaggi  in  arrivo  nella prossima ora
	
				if(richiesta.equalsIgnoreCase("D"))
				{											
					
					//Ricevo l'ora.
	
					int ora = -1;
	
					try{
						ora = inSock.readInt();
					}catch(SocketTimeoutException ste)
					{
						System.out.println("Timeout scattato: ");
						ste.printStackTrace();
						clientSocket.shutdownOutput();
						clientSocket.shutdownInput();
						clientSocket.close();
						return;          
					}        
					catch (Exception e) 
					{
						System.out.println("Problemi nella ricezione della richiesta: ");
						e.printStackTrace();
						return;
					}
					
	
					//Bene ora posso tirar fuori gli annucci audio
	
					Riga[] tmpTabella = new Riga[TCP_Server.N];
					int tmpLength = 0;
	
	
					for (int i = 0; i < tabella.length; i++) {
						if(tabella[i].getOrarioAttesa_ora() == ora){
							tmpTabella[tmpLength++] = tabella[i];
						}
					}
	
	
					try
					{
						//Invio la rispsta.
	
						//Invio numero file.
						//Per ogni file invio nome, lunghezza e contenuto.
						//Per ogni file ho anche un check di esistenza e lettura
	
						outSock.writeInt(tmpLength);
	
						for (int i = 0; i < tmpLength; i++) {
							
							//Nome file.
							outSock.writeUTF(tmpTabella[i].getAudio());
	
							File file = new File(tmpTabella[i].getAudio());
	
							int fileLength = (int)file.length();
	
							//Invio conferma che posso inviare il file.
							if(file.exists()){
								if(file.canRead()){
									outSock.writeUTF("OK");
									//Invio lunghezza
									outSock.writeInt(fileLength);
	
									//Invio contenuto.
									BufferedReader reader = new BufferedReader(new FileReader(file));
	
									int ch = -1;
	
									while((ch=reader.read()) != -1){
										outSock.writeByte(ch);
									}
	
									reader.close();
								}else{
									outSock.writeUTF("Impossibile leggere il file audio.");
								}
							}else{
								outSock.writeUTF("Impossibile trovare il file audio.");
							}
						}
	
					}
					catch(SocketTimeoutException ste)
					{
						System.out.println("Timeout scattato: ");
						ste.printStackTrace();
						clientSocket.shutdownOutput();
						clientSocket.shutdownInput();
						clientSocket.close();
						return;          
					}              
					catch (Exception e) 
					{
						System.err.println("\nProblemi nell'invio della risposta: "+ e.getMessage());
						e.printStackTrace();
						clientSocket.shutdownOutput();
						clientSocket.shutdownInput();
						clientSocket.close();
						return;
					}
				}
				else if(richiesta.equalsIgnoreCase("I"))
				{
					// inserimento  di  un  nuovo  viaggio

					/*
						 il  figlio  riceve  l’identificatore  del viaggio,
						 il tipo, l’ora attesa e il nome del file audio,
						 riceve il file audio e restituisce al client l’esito dell’operazione,
						 positivo se l’inserimento ha successo, oppure un negativo,
						 ad esempio se un viaggio  con  lo  stesso  identificatore  è  già  stato  inserito,
						 se  la  struttura  dati  è  piena  o  se falliscono alcuni controlli sui dati di input.
					*/


					//ID
					String id = null;
					String tipo = null;
					String partenza = null;
					String arrivo = null;
					int orarioAttesa_ora = -1;
					int orarioAttesa_minuti = -1;
					String audio = null;

					try{
						id = inSock.readUTF();

						if(id == null){
							System.out.println("EOF");
							//Devo ritornare ho ricevuto EOF
							clientSocket.shutdownInput();
							clientSocket.shutdownOutput();
							clientSocket.close();
							return;
						}

						tipo = inSock.readUTF();

						if(tipo == null){
							System.out.println("EOF");
							//Devo ritornare ho ricevuto EOF
							clientSocket.shutdownInput();
							clientSocket.shutdownOutput();
							clientSocket.close();
							return;
						}

						partenza = inSock.readUTF();

						if(partenza == null){
							System.out.println("EOF");
							//Devo ritornare ho ricevuto EOF
							clientSocket.shutdownInput();
							clientSocket.shutdownOutput();
							clientSocket.close();
							return;
						}

						arrivo = inSock.readUTF();

						if(arrivo == null){
							System.out.println("EOF");
							//Devo ritornare ho ricevuto EOF
							clientSocket.shutdownInput();
							clientSocket.shutdownOutput();
							clientSocket.close();
							return;
						}

						orarioAttesa_ora = inSock.readInt();
						orarioAttesa_minuti = inSock.readInt();

						audio = inSock.readUTF();

						if(audio == null){
							System.out.println("EOF");
							//Devo ritornare ho ricevuto EOF
							clientSocket.shutdownInput();
							clientSocket.shutdownOutput();
							clientSocket.close();
							return;
						}

					}catch(SocketTimeoutException ste)
					{
						System.out.println("Timeout scattato: ");
						ste.printStackTrace();
						clientSocket.shutdownOutput();
						clientSocket.shutdownInput();
						clientSocket.close();
						return;          
					}        
					catch (Exception e) 
					{
						System.out.println("Problemi nella ricezione della richiesta: ");
						e.printStackTrace();
						clientSocket.shutdownOutput();
						clientSocket.shutdownInput();
						clientSocket.close();
						// servo nuove richieste
						return;
					}

					String risposta = null;

					File fileAudio = new File(audio);
					boolean ok = false;
					
					//Checks su dati input
					//Orario ok, Tipo valido etc
					if(orarioAttesa_ora < 0 || orarioAttesa_ora > 12){
						//Ora non valida
						risposta = "Ora non valida";
					}else{
						if(orarioAttesa_minuti < 0 || orarioAttesa_minuti > 60){
							//Minuti non validi
							risposta = "Minuti non validi";
						}else{
							if("Arrivo".equals(tipo) || "Partenza".equals(tipo)){
								if(!fileAudio.exists()){
									fileAudio.createNewFile();
									
									if(fileAudio.canWrite()){
										//Provo ad inserire una nuova riga
								
										boolean trovato = false;
										boolean idDoppione = false;
	
										int indexLibero = -1;
	
										for (int i = 0; i < tabella.length; i++) {
											if(tabella[i].getId().equals("L") && !trovato){
												trovato = true;
												indexLibero = i;
											}
											//Check doppione
											if(tabella[i].getId().equals(id)){
												idDoppione = true;
												break;
											}
										}
	
										if(!trovato){
											//Non ho trovato spazio
											risposta = "Spazio esaurito";
										}else{
											if(idDoppione){
												//Ho trovato un doppione.
												risposta = "ID già presente";
											}else{
												Riga nuovaRiga = new Riga(id, tipo, partenza, arrivo, orarioAttesa_ora, orarioAttesa_minuti, 0, audio);
												tabella[indexLibero] = nuovaRiga;
	
												//Tutto ok
												risposta = "OK";
												ok = true;
											}
										}
									}else{
										risposta = "Non posso scrivere il file audio";
									}								
								}else{
									risposta = "Il file audio esiste già";
								}
							}else{
								risposta = "Tipo non valido";
							}
						}
					}

					try{
						outSock.writeUTF(risposta);
					}catch(SocketTimeoutException ste)
					{
						System.out.println("Timeout scattato: ");
						ste.printStackTrace();
						clientSocket.shutdownOutput();
						clientSocket.shutdownInput();
						clientSocket.close();
						return;          
					}              
					catch (Exception e) 
					{
						System.err.println("\nProblemi nell'invio della risposta: "+ e.getMessage());
						e.printStackTrace();
						clientSocket.shutdownOutput();
						clientSocket.shutdownInput();
						clientSocket.close();
						return;
					}

					if(ok){
						//Devo ricevere il file audio.
						try{
							//Ricevo lunghezza
							int length = inSock.readInt();

							BufferedWriter writer = new BufferedWriter(new FileWriter(fileAudio));
							
							for (int i = 0; i < length; i++) {
								writer.write(inSock.readByte());
							}

							writer.close();
						}
						catch(EOFException ex){
							System.out.println("EOF Ricevuto");
							clientSocket.shutdownInput();
							clientSocket.shutdownOutput();
							clientSocket.close();

						}
						catch(SocketTimeoutException ste)
						{
							System.out.println("Timeout scattato: ");
							ste.printStackTrace();
							clientSocket.shutdownOutput();
							clientSocket.shutdownInput();
							clientSocket.close();
							return;          
						}        
						catch (Exception e) 
						{
							System.out.println("Problemi nella ricezione della richiesta: ");
							e.printStackTrace();
							clientSocket.shutdownOutput();
							clientSocket.shutdownInput();
							clientSocket.close();
							// servo nuove richieste
							return;
						}
					}
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