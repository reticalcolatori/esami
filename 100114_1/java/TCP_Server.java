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

	public static final int TABLEDIM = 10;
	private static final Riga[] tabella = new Riga[TABLEDIM];

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

		for (int i = 0; i < TABLEDIM; i++) {
			tabella[i] = new Riga();
		}

		tabella[0].setDescrizione("String");
		tabella[0].setTipo("Concerto");
		tabella[0].setLuogo("Verona");
		tabella[0].setData("11/11/11");
		tabella[0].setPrezzo(100);
		tabella[0].setDisponibilita(50);

		tabella[1].setDescrizione("BFC-JUV");
		tabella[1].setTipo("Calcio");
		tabella[1].setLuogo("Verona");
		tabella[1].setData("11/11/12");
		tabella[1].setPrezzo(50);
		tabella[1].setDisponibilita(150);
		

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
