/************************************
*      RMI_interfaceFile.java       *
************************************/
	

// Interfaccia remota

import java.rmi.Remote;
import java.rmi.RemoteException;


public interface RMI_interfaceFile extends Remote 
{	
	int inserimento_evento(Riga inserimento) throws RemoteException;
	int acquista_biglietti(String descrizione, int numeroBiglietti)	throws RemoteException;
}
