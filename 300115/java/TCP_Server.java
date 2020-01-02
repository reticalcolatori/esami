/************************************
*     TCP_Server.java		    *
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
			String richiesta;

			while(true){ 
				try 
				{
					richiesta = inSock.readUTF();
					System.out.println("Richiesta: " + richiesta);
				}
				catch(EOFException ex){
					System.out.println("EOF Ricevuto");
					clientSocket.shutdownInput();
					clientSocket.shutdownOutput();
					clientSocket.close();
					return;
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
					// servo nuove richieste
					return;
				}
	
				//eliminare tutte le occorrenze di caratteri vocali all’interno di un file di testo remoto
				if(richiesta.equalsIgnoreCase("E"))
				{								
					String nomeFile = null;								
					
					//Richiesta nome file.
					try 
					{
						nomeFile = inSock.readUTF();
						System.out.println("NomeFile: " + nomeFile);
					}
					catch(EOFException ex){
						System.out.println("EOF Ricevuto");
						clientSocket.shutdownInput();
						clientSocket.shutdownOutput();
						clientSocket.close();
						return;
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
						System.out.println("Problemi nella ricezione della nome file: ");
						e.printStackTrace();
						clientSocket.shutdownOutput();
						clientSocket.shutdownInput();
						clientSocket.close();
						return;
					}

					//Elaborazione della risposta.
					int risposta = -1;
					int occorrenzeEliminate = 0;

					File myFile = new File(nomeFile);
					File myTmpFile = new File(nomeFile+".tmp");

					if(myFile.exists()){
						if(myFile.canRead() && myFile.canWrite()){
							if(!myTmpFile.exists()){
								myTmpFile.createNewFile();

								try{
									FileReader reader = new FileReader(myFile);
									FileWriter writer = new FileWriter(myTmpFile);
	
									int nextCh = -1;
	
									while((nextCh = reader.read()) != -1){
										if("aeiouAEIOU".indexOf(nextCh) == -1){
											writer.write(nextCh);
										}else{
											occorrenzeEliminate++;
										}
									}
	
									writer.close();
									reader.close();
								}catch(IOException ex){
									System.out.println("Errore durante eliminazione delle vocali:");
									ex.printStackTrace();	
								}

								//Ora devo sostituire il file originale.
								if(myFile.delete()){
									try{
										FileReader reader = new FileReader(myTmpFile);
										FileWriter writer = new FileWriter(myFile);
		
										int nextCh = -1;
		
										while((nextCh = reader.read()) != -1){
											writer.write(nextCh);
										}
		
										writer.close();
										reader.close();
									}catch(IOException ex){
										System.out.println("Errore durante la copia del tmp");
										ex.printStackTrace();	
									}

									if(myTmpFile.delete()){
										risposta = occorrenzeEliminate;
									}else{
										System.out.println("Impossibile eliminare il file tmp");
									}
								}else{
									System.out.println("Impossibile eliminare il file originale");
								}
							}
							
						}
					}

					try
					{
						outSock.writeInt(risposta);
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
				else if(richiesta.equalsIgnoreCase("D"))
				{
					//Prendo il nome del direttorio
					String nomeDir = null;								
					
					try 
					{
						nomeDir = inSock.readUTF();
						System.out.println("Nome Dir: " + nomeDir);
					}
					catch(EOFException ex){
						System.out.println("EOF Ricevuto");
						clientSocket.shutdownInput();
						clientSocket.shutdownOutput();
						clientSocket.close();
						return;
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
						System.out.println("Problemi nella ricezione del nomeDir: ");
						e.printStackTrace();
						clientSocket.shutdownOutput();
						clientSocket.shutdownInput();
						clientSocket.close();
						return;
					}

					int nFiles = 0;
					File myDir = new File(nomeDir);
					File[] filesToTransfer = new File[0];

					if(myDir.isDirectory()){
						File[] files = myDir.listFiles();
						filesToTransfer = new File[files.length];

						for(int i = 0; i < files.length; i++){
							File myFile = files[i];
							String myFileName = myFile.getName();
							int occorrenzeMinuscole = 0;

							//Invio solo i file che posso leggere
							if(myFile.canRead()){
								for(int j = 0; j < myFileName.length() && occorrenzeMinuscole < 2; j++){
									if("qwertyuiopasdfghjklzxcvbnm".indexOf(myFileName.charAt(j)) != -1) occorrenzeMinuscole++;
								}
	
								if(occorrenzeMinuscole >= 2){
									filesToTransfer[nFiles++] = myFile;
								}
							}							
						}
					}

					//Posso inviare i file al client

					try
					{
						outSock.writeInt(nFiles);

						for (int i = 0; i < nFiles; i++) {
							File myFile = filesToTransfer[i];

							//Invio nome file
							outSock.writeUTF(myFile.getName());

							//Invio lunghezza
							outSock.writeLong(myFile.length());

							//Invio contenuto
							
							try{
								InputStream iStream = new FileInputStream(myFile);

								int nextData = -1;
	
								while((nextData=iStream.read()) != -1){
									outSock.writeByte(nextData);
								}
	
								iStream.close();
							}catch(IOException ex){
								System.out.println("Errore durante la lettura del file "+myFile.getName()+":");
								ex.printStackTrace();
								clientSocket.shutdownOutput();
								clientSocket.shutdownInput();
								clientSocket.close();
								return;
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


