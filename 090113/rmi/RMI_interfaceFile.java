/************************************
*           *
*      RMI_interfaceFile.java       *
*                                   *
************************************/
	

// Interfaccia remota

import java.rmi.Remote;
import java.rmi.RemoteException;


public interface RMI_interfaceFile extends Remote 
{
	public int inserisci_sci(String id, String modello, int costo) throws RemoteException;
	public int noleggia_sci(String id, int giorno, int mese, int anno, int durata)	throws RemoteException;
}
