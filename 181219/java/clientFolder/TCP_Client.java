/************************************
*     TCP_Client.java	            *
************************************/

/* CLIENTE CON CONNESSIONE*/

import java.net.*;
import java.io.*;

public class TCP_Client 
{

  /**
  * @param args - Usage: java TCP_Client <address> <port>
  */

 	public static void main(String[] args) throws IOException 
  	{
	     /*
	     * Come argomenti si devono passare un nome di host valido e una porta
	     *  
	     */
	     InetAddress addr = null;
	     int port = -1;

	      // Controllo argomenti 
	     try
	     {
	     	if(args.length == 2)
	     	{
	     		addr = InetAddress.getByName(args[0]);
	     		port = Integer.parseInt(args[1]);
	     	} 
	     	else
	     	{
	     		System.out.println("Usage: java TCP_Client serverAddr serverPort");
	     		System.exit(1);
	     	}
	     } 
	     catch(NumberFormatException e)
	     {
	     	System.out.println("Numero di porta errato: " + args[1]);
	     	e.printStackTrace();
	     	System.out.println("Usage: java TCP_Client serverAddr serverPort");
	     	System.exit(1);
	     }
	     catch(Exception e)
	     {
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
	     try
	     {
	     	socket = new Socket(addr, port);
		    // Setto il timeout per non bloccare indefinitivamente il client
	     	socket.setSoTimeout(30000);
	     	System.out.println("Creata la socket: " + socket);
	     	inSock = new DataInputStream(socket.getInputStream());
	     	outSock = new DataOutputStream(socket.getOutputStream());
	     }
	     catch(IOException e)
	     {
	     	System.out.println("Problemi nella creazione degli stream su socket: ");
	     	e.printStackTrace();
	     	System.exit(1);
	     }
	     catch(Exception e)
	     {
	     	System.out.println("Problemi nella creazione della socket: ");
	     	e.printStackTrace();
	     	System.exit(2);
	     }

	      // Eventuale stream per l'interazione con l'utente da tastiera

	     BufferedReader stdIn = new BufferedReader(new InputStreamReader(System.in));
	     System.out.print("\n^D(Unix)/^Z(Win)+invio per uscire, oppure immetti input: ");
	     try
	     {

	     	while((richiesta = stdIn.readLine()) != null)
	     	{
		  		
		      	if(richiesta.equalsIgnoreCase("D"))
		      	{
					//Richiedo l'ora

					System.out.println("Inserisci ora:");
					String oraLine = stdIn.readLine();
					int ora = -1;
					try{
						ora = Integer.parseInt(oraLine);
					}catch(NumberFormatException ex){
						System.out.println("Ora non valida riprova");
						System.out.print("\n^D(Unix)/^Z(Win)+invio per uscire, oppure immetti input: ");
						continue;
					}


		      		try
		      		{
				    	// Elaborazione richiesta e invio
		      			outSock.writeUTF(richiesta);
						System.out.println("Inviata richiesta: " + richiesta);
						
						//Invio ora.
						outSock.writeInt(ora);

		      		}
		      		catch(Exception e)
		      		{
		      			System.out.println("Problemi nell'invio della richiesta " + richiesta );
		      			e.printStackTrace();
		      			System.out.print("\n^D(Unix)/^Z(Win)+invio per uscire, oppure immetti input: ");		            
		      			continue;
				      // il client continua l'esecuzione riprendendo dall'inizio del ciclo
		      		}

					

			    	// Ricezione della risposta

		      		
		      		try
		      		{
						int nFiles = inSock.readInt();
						System.out.println("Numero files:"+nFiles);
						
						for (int i = 0; i < nFiles; i++) {
							String nomeFile = inSock.readUTF();
							String risposta = inSock.readUTF();
							
							//Check risposta affermativa
							if("OK".equals(risposta)){
								int fileLength = inSock.readInt();
								File file = new File(nomeFile);

								BufferedWriter writer = new BufferedWriter(new FileWriter(file));

								for (int j = 0; j < fileLength; j++) {
									writer.write(inSock.readByte());
								}

								writer.close();

							}else{
								System.out.println("Errore nella ricezione di un file: "+risposta);
							}
						}
		      		}
		      		catch(SocketTimeoutException ste)
		      		{
		      			System.out.println("Timeout scattato: ");
		      			ste.printStackTrace();
		      			socket.close();
		      			System.out.print("\n^D(Unix)/^Z(Win)+invio per uscire, oppure immetti input: ");		            
		      			continue;
				      	// il client continua l'esecuzione riprendendo dall'inizio del ciclo         
		      		}
		      		catch (EOFException e) 
		      		{
		      			System.out.println("Raggiunta la fine delle ricezioni, chiudo...");
		      			socket.close();
		      			System.out.println("TCP_Client: termino...");
		      			System.exit(0);
		      		}
		      		catch(Exception e)
		      		{
		      			System.out.println("Problemi nella ricezione della risposta, i seguenti: ");
		      			e.printStackTrace();
		      			socket.close();
		      			System.out.print("\n^D(Unix)/^Z(Win)+invio per uscire, oppure immetti input: ");		            
		      			continue;
				      // il client continua l'esecuzione riprendendo dall'inizio del ciclo
		      		}
				}
				else if(richiesta.equalsIgnoreCase("I"))
		      	{
					//Richiedo all'utente le info per l'inserimento.

					System.out.println("Inserisci ID:");
					String id = stdIn.readLine();

					if(id == null){
						//EOF
						//Esco.
						socket.shutdownOutput();
						socket.shutdownInput();
						socket.close();
					}
					
					System.out.println("Inserisci Tipo:");
					String tipo = stdIn.readLine();

					if(tipo == null){
						//EOF
						//Esco.
						socket.shutdownOutput();
						socket.shutdownInput();
						socket.close();
					}

					System.out.println("Inserisci Partenza:");
					String partenza = stdIn.readLine();

					if(partenza == null){
						//EOF
						//Esco.
						socket.shutdownOutput();
						socket.shutdownInput();
						socket.close();
					}

					System.out.println("Inserisci Arrivo:");
					String arrivo = stdIn.readLine();

					if(arrivo == null){
						//EOF
						//Esco.
						socket.shutdownOutput();
						socket.shutdownInput();
						socket.close();
					}

					System.out.println("Inserisci ora:");
					String oraLine = stdIn.readLine();
					int ora = -1;
					try{
						ora = Integer.parseInt(oraLine);
					}catch(NumberFormatException ex){
						System.out.println("Ora non valida riprova");
						System.out.print("\n^D(Unix)/^Z(Win)+invio per uscire, oppure immetti input: ");
						continue;
					}

					System.out.println("Inserisci minuti:");
					String minutiLine = stdIn.readLine();
					int minuti = -1;
					try{
						minuti = Integer.parseInt(minutiLine);
					}catch(NumberFormatException ex){
						System.out.println("Ora non valida riprova");
						System.out.print("\n^D(Unix)/^Z(Win)+invio per uscire, oppure immetti input: ");
						continue;
					}

					System.out.println("Inserisci Audio:");
					String audio = stdIn.readLine();

					if(audio == null){
						//EOF
						//Esco.
						socket.shutdownOutput();
						socket.shutdownInput();
						socket.close();
					}

					//Verifico che esista il file audio.
					File fileAudio = new File(audio);

					if(!fileAudio.exists() || !fileAudio.canRead()){
						System.out.println("Non posso leggere il file audio inserito. Riprova");
						System.out.print("\n^D(Unix)/^Z(Win)+invio per uscire, oppure immetti input: ");		            
						continue;
					}


					try
		      		{
				      // Elaborazione richiesta e invio
		      			outSock.writeUTF(richiesta);
						System.out.println("Inviata richiesta: " + richiesta);
						  
						//Invio dati
						outSock.writeUTF(id);
						outSock.writeUTF(tipo);
						outSock.writeUTF(partenza);
						outSock.writeUTF(arrivo);
						outSock.writeInt(ora);
						outSock.writeInt(minuti);
						outSock.writeUTF(audio);


		      		}
		      		catch(Exception e)
		      		{
		      			System.out.println("Problemi nell'invio della richiesta " + richiesta );
		      			e.printStackTrace();
		      			System.out.print("\n^D(Unix)/^Z(Win)+invio per uscire, oppure immetti input: ");		            
		      			continue;
				      	// il client continua l'esecuzione riprendendo dall'inizio del ciclo
		      		}

					

			    	// Ricezione della risposta

		      		String risposta = null;
		      		try
		      		{
		      			risposta = inSock.readUTF();
		      			System.out.println("Risposta: " + risposta);
		      		}
		      		catch(SocketTimeoutException ste)
		      		{
		      			System.out.println("Timeout scattato: ");
		      			ste.printStackTrace();
		      			socket.close();
		      			System.out.print("\n^D(Unix)/^Z(Win)+invio per uscire, oppure immetti input: ");		            
		      			continue;
				      	// il client continua l'esecuzione riprendendo dall'inizio del ciclo         
		      		}
		      		catch (EOFException e) 
		      		{
		      			System.out.println("Raggiunta la fine delle ricezioni, chiudo...");
		      			socket.close();
		      			System.out.println("TCP_Client: termino...");
		      			System.exit(0);
		      		}
		      		catch(Exception e)
		      		{
		      			System.out.println("Problemi nella ricezione della risposta, i seguenti: ");
		      			e.printStackTrace();
		      			socket.close();
		      			System.out.print("\n^D(Unix)/^Z(Win)+invio per uscire, oppure immetti input: ");		            
		      			continue;
				        // il client continua l'esecuzione riprendendo dall'inizio del ciclo
					}
					  
					//Check risposta
					if("OK".equals(risposta)){
						//Posso inviare il file audio
						try{
							//invio lunghezza
							outSock.writeInt((int)fileAudio.length());
							
							BufferedReader reader = new BufferedReader(new FileReader(fileAudio));
							int ch = -1;
							while((ch=reader.read()) != -1){
								outSock.writeByte(ch);
							}
							reader.close();
						}catch(Exception e)
						{
							System.out.println("Problemi nell'invio della richiesta " + richiesta );
							e.printStackTrace();
							System.out.print("\n^D(Unix)/^Z(Win)+invio per uscire, oppure immetti input: ");		            
							continue;
							// il client continua l'esecuzione riprendendo dall'inizio del ciclo
						}
					}

				} 
				else System.out.println("Errore nella scelta..."); 

				// Tutto ok, pronto per nuova richiesta
		      	System.out.print("\n^D(Unix)/^Z(Win)+invio per uscire, oppure immetti input: ");	

			}// while

			System.out.println("TCP_Client: termino...");
		}

	    // qui catturo le eccezioni non catturate all'interno del while
	    // quali per esempio la caduta della connessione con il server
	    // in seguito alle quali il client termina l'esecuzione
		catch(Exception e)
		{
			System.err.println("Errore irreversibile, il seguente: ");
			e.printStackTrace();
			System.err.println("Chiudo!");
			System.exit(3); 
		}
	} // main
}