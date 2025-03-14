import java.rmi.Remote;
import java.rmi.RemoteException;

public interface RMItableIntf extends Remote {
    int addObj() throws RemoteException;
    int bindObj() throws RemoteException;
    int getNextObj() throws RemoteException;
    void freeObj(int a) throws RemoteException;
}
