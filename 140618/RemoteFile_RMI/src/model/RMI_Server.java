package model;

import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileReader;
import java.io.FileWriter;
import java.io.IOException;
import java.nio.file.Files;
import java.nio.file.StandardCopyOption;
import java.rmi.*;
import java.rmi.registry.LocateRegistry;
import java.rmi.server.*;
import java.util.ArrayList;

public class RMI_Server extends UnicastRemoteObject implements RMI_interfaceFile {
	
	private static final long serialVersionUID = 1L;

	// qui eventuali variabili e strutture dati
	

	// Costruttore
	public RMI_Server() throws RemoteException {
		super();
	}

	/*
	public static void stampa() {
		System.out.println("Identificatore\tData\tGiorni\tModello\tCosto giornaliero\n");

		for (int i = 0; i < N; i++) {
			System.out.println(tabella[i].id + "\t" + tabella[i].giorno + "/" + tabella[i].mese + "/" + tabella[i].anno
					+ "\t" + tabella[i].giorni + "\t" + tabella[i].modello + "\t" + tabella[i].costoGiornaliero);
		}
	}
	*/

	// Avvio del Server RMI
	public static void main(String[] args) {
		
		try {
			LocateRegistry.createRegistry(1099);
		} catch (RemoteException e1) {
			e1.printStackTrace();
		}
		
		int registryPort = 1099;
		String registryHost = "localhost";
		String serviceName = "ServerRMI";

		// Controllo dei parametri della riga di comando
		if (args.length != 0 && args.length != 1) {
			System.out.println("Sintassi: ServerRMI [REGISTRYPORT]");
			System.exit(1);
		}
		if (args.length == 1) {
			try {
				registryPort = Integer.parseInt(args[0]);
			} catch (Exception e) {
				System.out.println("Sintassi: ServerRMI [REGISTRYPORT], REGISTRYPORT intero");
				System.exit(2);
			}
		}

		// Registrazione del servizio
		String completeName = "//" + registryHost + ":" + registryPort + "/" + serviceName;

		try {
			RMI_Server serverRMI = new RMI_Server();
			Naming.rebind(completeName, serverRMI);
			System.out.println("Server RMI: Servizio \"" + serviceName + "\" registrato");
		} catch (Exception e) {
			System.err.println("Server RMI \"" + serviceName + "\": " + e.getMessage());
			e.printStackTrace();
			System.exit(1);
		}
	}
	
	// Implementazione dei metodi

	@Override
	public String[] lista_file(String nomeDir) throws RemoteException {
		
		File currDir = new File(nomeDir);
		
		if(!currDir.isDirectory())
			throw new RemoteException("Non è una directory");
		
		String fileList[] = currDir.list();
		System.out.println("Lunghezza filelist: " + fileList.length);
		
		if(fileList == null)
			throw new RemoteException("Non è presente alcun file nella directory corrente");
		
		//ArrayList non funziona quando casti a String[] in risultato
		//String res[] = new String[fileList.length];
		ArrayList<String> res = new ArrayList<String>();
		int resIdx = 0;
		
		for (String fileName : fileList) {
			
			File currFileName = new File(fileName);
			if(currFileName.isFile()) {
			
				System.out.println("Trovato file --> " + fileName);
				for (int i = 0; i < fileName.length() - 1; i++) {
					
					char currChar = fileName.charAt(i);
					char nextChar = fileName.charAt(i+1);
					
					if("aeiouAEIOU".indexOf(currChar) > -1 && "aeiouAEIOU".indexOf(nextChar) > -1) {
						System.out.println("Trovato due vocali in " + fileName);
						//res[resIdx++] = fileName;
						res.add(fileName);
						break;
					}
				} //for
			} else if(currFileName.isDirectory()){ //controllo inutile ma vbbuoh
				System.out.println("Trovata subdir --> " + fileName);
				String fileListSubDir[] = currFileName.list();
				
				if(fileListSubDir == null) {
					throw new RemoteException("SubDir ha restituito null!");
				}
				
				for (String fileNameSubDir : fileListSubDir) {
					
					File currFileNameSubDIr = new File(fileNameSubDir);
					if(!currFileNameSubDIr.isDirectory()) {
						
						System.out.println("Analizzo --> " + currFileNameSubDIr);
						for (int i = 0; i < fileNameSubDir.length() - 1; i++) {
							
							char currChar = fileNameSubDir.charAt(i);
							char nextChar = fileNameSubDir.charAt(i+1);
							
							if("aeiouAEIOU".indexOf(currChar) > -1 && "aeiouAEIOU".indexOf(nextChar) > -1) {
								System.out.println("Trovato due vocali in " + fileNameSubDir);
								//res[resIdx++] = fileName;
								res.add(fileNameSubDir);
								break;
							}
						}//for
					}//if
				}//for
			}//else
		}
		
		//return res;
		return res.toArray(new String[0]);		
	}				


	@Override
	public int numerazione_righe(String nomeFile) throws RemoteException {
		
		int res = -1;
		File currFile = new File(nomeFile);
		
		if(!currFile.exists() || !currFile.isFile() || !currFile.canRead() || !currFile.canWrite())
			return res;
		
		/*
		 * LOGICA APPLICATIVA:
		 * CICLO: CREO FILE TMP, SCRIVO SU FILE TMP IL NUMERO DELLA RIGA,
		 * LEGGO DAL FILE UNA RIGA ALLA VOLTA E LA SCRIVO SU TMP, RIPETO...
		 * INFINE SOVRASCRIVO IL FILE ORIGINALE CON QUELLO TEMPORANEO
		 */
		
		FileReader fr;
		BufferedReader br;
		FileWriter fw;
		BufferedWriter bw;
		File tmp;
		
		try {
			
			//posso usare metodo statico?
			tmp = new File("tmp");
			
			fw = new FileWriter(tmp);
			bw = new BufferedWriter(fw);
			
			fr = new FileReader(currFile);
			br = new BufferedReader(fr);
			
		} catch (FileNotFoundException e) {
			throw new RemoteException("Impossibile aprire il file");
		} catch (IOException e1) {
			throw new RemoteException("Impossibile creare file tmp");
		}
		
		res = 0;
		String currLine = null;
		
		try {
			while((currLine = br.readLine()) != null) {
				
				res++;
				bw.write(String.valueOf(res));
				bw.write(" ");
				bw.write(currLine);
				System.out.println(currLine);
				bw.write("\n");
				
			}
			
			FileReader rcheck = new FileReader(tmp);
			BufferedReader check = new BufferedReader(rcheck);
			while((currLine = check.readLine()) != null) 
				System.out.println(currLine);
			
			fr.close();
			fw.close();
			
		} catch (IOException e) {
			throw new RemoteException("Impossibile leggere il file");
		}
		
		
		//devo "cancellare" il contenuto del file originario prima di sovrascriverlo
		try {
			fw = new FileWriter(currFile, false);
			fw.write("");
			fw.close();
		} catch (IOException e1) {
			throw new RemoteException("Impossibile cancellare contenuto file originario");
		}
		
		/*
		try {
			Files.move(tmp.toPath(), currFile.toPath(), StandardCopyOption.REPLACE_EXISTING);
		} catch (IOException e) {
			throw new RemoteException("Impossibile sostituire file");
		}
		*/
		
		//sovrascrivo file originario con quello tmp, posso usare la MOVE?
		try {
			
			fw = new FileWriter(currFile);
			bw = new BufferedWriter(fw);
			
			fr = new FileReader(tmp);
			br = new BufferedReader(fr);
			
			while((currLine = br.readLine()) != null) {
				bw.append(currLine);
				System.out.println(currLine);
				bw.append("\n");
			}
			
			fr.close();
			fw.close();
			
		} catch (IOException e) {
			throw new RemoteException("Impossibile aprire in scrittura il file");
		}
		
		
		return res;
		
	}
}