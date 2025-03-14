import java.io.IOException;
import java.rmi.Remote;
import java.rmi.RemoteException;

public interface ServicesIntf extends Remote {
    Boolean register(String a, String b) throws RemoteException;
    Boolean logIn(String a, String b) throws RemoteException, IOException, ClassNotFoundException;
    boolean newEmail(String s, String r, String sub, String body) throws RemoteException;
    String showEmails() throws RemoteException;
    String readEmail(int m) throws RemoteException;
    boolean deleteEmail(int m) throws RemoteException;
    void logOut() throws RemoteException;
    void exit() throws RemoteException;

    void setRMIobjId(RMItableIntf r, int o) throws RemoteException;

    boolean isOnline() throws RemoteException;
    void stillOnline() throws RemoteException;
    boolean clientConnected() throws RemoteException;
    void setConnected(boolean c) throws RemoteException;
}