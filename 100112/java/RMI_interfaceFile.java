/************************************
*      RMI_interfaceFile.java       *
************************************/
	

// Interfaccia remota

import java.rmi.Remote;
import java.rmi.RemoteException;
import java.util.*;


public interface RMI_interfaceFile extends Remote 
{
	public List<String> lista_file_caratteri(String nomeDirettorio, char charA, char charB) throws RemoteException;
	public int conta_occorrenze_interpunzione(String nomeFile)	throws RemoteException;
}
