/************************************
*     TCP_ServerThread.java		    *
************************************/
import java.io.*;
import java.net.*;

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

			//WHILE QUI SE FAI UNA CONNESSIONE PER SESSIONE!
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
					clientSocket.shutdownOutput();
					clientSocket.shutdownInput();
					clientSocket.close();
					return;
				}

				/*
				Per ogni richiesta di visualizzazione di tutti gli eventi di un certo tipo in un determinato
				luogo, il figlio riceve il tipo e il luogo, scorre la struttura cercando tutti gli eventi di un certo tipo e
				luogo e restituisce l’elenco degli eventi trovate al client.
				*/
				if(richiesta.equalsIgnoreCase("A"))
				{																
					//Ricevo tipo e luogo
					String tipo = null;
					String luogo = null;

					try 
					{
						tipo = inSock.readUTF();
						luogo = inSock.readUTF();

						System.out.println("Tipo: "+tipo);
						System.out.println("Luogo: "+luogo);				
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
						clientSocket.shutdownOutput();
						clientSocket.shutdownInput();
						clientSocket.close();
					}

					//Ricerca di tutte le righe favorevoli

					Riga[] tmpTable = new Riga[TCP_Server.TABLEDIM];
					int tmpTableLength = 0;

					for (int i = 0; i < TCP_Server.TABLEDIM; i++) {
						if(tabella[i].getTipo().equals(tipo) && tabella[i].getLuogo().equals(luogo)){
							//Trovata corrispondenza.
							tmpTable[tmpTableLength++] = new Riga(tabella[i].getDescrizione(),tabella[i].getTipo(), tabella[i].getData(), tabella[i].getLuogo(), tabella[i].getPrezzo(), tabella[i].getDisponibilita());
						}
					}

					
					// Elaborazione e invio della risposta
					try
					{
						//invio dimensione tabella
						outSock.writeInt(tmpTableLength);

						for(int i = 0; i < tmpTableLength; i++){
							outSock.writeUTF(tmpTable[i].toString());
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
				/*
				Per ogni richiesta visualizzazione di tutti gli eventi disponibili e con prezzo inferiore o
				uguale a un prezzo dato , il figlio riceve il prezzo massimo per biglietto, e restituisce una lista
				con tutti gli eventi disponibili con prezzo inferiore a quello richiesto.
				*/
				else if(richiesta.equalsIgnoreCase("B"))
				{
					//Ricevo prezzo biglietto massimo
					int prezzoMax = 0;

					try 
					{
						prezzoMax = inSock.readInt();
						System.out.println("Prezzo MAX: "+prezzoMax);				
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
						clientSocket.shutdownOutput();
						clientSocket.shutdownInput();
						clientSocket.close();
					}

					//Ricerca di tutte le righe favorevoli

					Riga[] tmpTable = new Riga[TCP_Server.TABLEDIM];
					int tmpTableLength = 0;

					for (int i = 0; i < TCP_Server.TABLEDIM; i++) {
						if(tabella[i].getPrezzo() <= prezzoMax){
							//Trovata corrispondenza.
							tmpTable[tmpTableLength++] = new Riga(tabella[i].getDescrizione(),tabella[i].getTipo(), tabella[i].getData(), tabella[i].getLuogo(), tabella[i].getPrezzo(), tabella[i].getDisponibilita());
						}
					}
					
					// Elaborazione e invio della risposta
					try
					{
						//invio dimensione tabella
						outSock.writeInt(tmpTableLength);

						for(int i = 0; i < tmpTableLength; i++){
							outSock.writeUTF(tmpTable[i].toString());
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


