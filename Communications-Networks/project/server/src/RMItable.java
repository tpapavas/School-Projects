import java.rmi.RemoteException;
import java.rmi.server.UnicastRemoteObject;
import java.util.LinkedList;
import java.util.NoSuchElementException;

public class RMItable extends UnicastRemoteObject implements RMItableIntf {
    final int maxSize;
    int size;

    LinkedList<Integer> freeObjs;

    public RMItable(int maxSize) throws RemoteException {
        this.maxSize = maxSize;
        this.size = 0;
        this.freeObjs = new LinkedList<>();
    }

    @Override
    public int addObj() throws RemoteException{
        freeObjs.add(size);
        return size++;
    }

    @Override
    public int bindObj() throws RemoteException {
        try {
            return freeObjs.remove();
        } catch (NoSuchElementException e) {
            return -1;
        }
    }

    @Override
    public int getNextObj() throws RemoteException {
        return freeObjs.getFirst();
    }

    @Override
    public void freeObj(int obj) {
        freeObjs.addLast(obj);
    }
}
