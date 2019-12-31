package model;

import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.IOException;
import java.net.Socket;
import java.net.SocketTimeoutException;

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

						if(tipo.isEmpty()){
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

					//----------------------DOWNLOAD DI FILE SOPRA UNA CERTA SOGLIA
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

							System.out.println("RICEVUTO: direttorio = " + direttorio + ", soglia = " + soglia);
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

										System.out.println("Mando al client la risposta e il numero dei file che superano la soglia " + risposta + " " + nFiles);

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

						
					}else{
						try
						{
							outSock.writeUTF("Comando non riconosciuto");
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
						}
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


