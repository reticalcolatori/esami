package model;

import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.IOException;
import java.net.Socket;
import java.net.SocketTimeoutException;

class TCP_ServerThread extends Thread
{
	private Socket clientSocket = null;
	private static final int N = 10;

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

		//------------------------------------------------------------------------
		//QUESTO WHILE PER GESTIRE TUTTO IN UNA UNICA CONNESSIONE FINO A EOF DEL CLIENTE
		
			try 
			{
				while((richiesta = inSock.readUTF()) != null){

					System.out.println("Riceuvta la richiesta " + richiesta);

					//Struttura richesta Elimina occorrenza
					//Stringa parola
					//Stringa nome file testo

					if(richiesta.equalsIgnoreCase("V"))
					{
						 
						//VISUALIZZA EVENTI DI UN DET. TIPO IN UN DET. LUOGO
						//Richiedo ulteriori info.
						String tipoEvento = null;
						String luogo = null;

						try
						{
							tipoEvento = inSock.readUTF();
							luogo = inSock.readUTF();

							System.out.println("RICEVUTO: tipo evento = " + tipoEvento + ", luogo = " + luogo);
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

						String tmpRes[] = new String[N];
						int pos = 0;
						
						for (int i = 0; i < N; i++) {
							if(TCP_Server.eventi[i].getTipo().equals(tipoEvento) && TCP_Server.eventi[i].getLuogo().equals(luogo)) {
								tmpRes[pos] = TCP_Server.eventi[i].getDescrizione();
								pos++;
							}
						}
					
						try
						{
							outSock.writeInt(pos);
							for (int i = 0; i < pos; i++) {
								outSock.writeUTF(tmpRes[i]);
							}
							
							//clientSocket.shutdownOutput();
							//System.out.println("Terminata connessione con " + clientSocket);
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

					//----------------------DOWNLOAD DI FILE SOPRA UNA CERTA SOGLIA-------------------------
					else if(richiesta.equalsIgnoreCase("T"))
					{
						 
						
					}
			}
			// qui catturo le eccezioni non catturate all'interno del while
	// in seguito alle quali il server termina l'esecuzione
			
		}
		catch (IOException e) 
		{
			e.printStackTrace();
			System.out.println("Rilevato IOException, TCP_ServerThread: termino...");
			System.exit(3);
		}
		catch (Exception e) 
		{
			e.printStackTrace();
			System.out.println("Errore irreversibile, TCP_ServerThread: termino...");
			System.exit(3);
		}
	
    } // run

} // TCP_ServerThread


