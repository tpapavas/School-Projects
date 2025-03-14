import java.io.FileOutputStream;
import java.io.IOException;
import java.io.ObjectOutputStream;
import java.net.MalformedURLException;
import java.rmi.Naming;
import java.rmi.RemoteException;
import java.rmi.registry.LocateRegistry;
import java.rmi.registry.Registry;
import java.util.Arrays;
import java.util.Properties;

public class Server {
    private static final int NUMBER_OF_THREADS = 3;

    static Services[] services = new Services[NUMBER_OF_THREADS];
    static RMItable rmiTable;
    static Registry registry;

    public static void main(String[] Args) throws IOException {
//        //////For starting position of "users" file//////
        ObjectOutputStream outStream = new ObjectOutputStream(new FileOutputStream("users"));
        Email[] m1 = {
                new Email(true, "Stergios", "Thodoris", "test 1", "This is 1st test"),
                new Email(true, "Stergios", "Thodoris", "test 2", "This is 2nd test"),
                new Email(true, "Stergios", "Thodoris", "test 3", "This is 3rd test")
        };
        Email[] m2 = {
                new Email(true, "Thodoris", "Stergios", "test 1", "This is 1st test"),
                new Email(true, "Thodoris", "Stergios", "test 2", "This is 2nd test"),
                new Email(true, "Thodoris", "Stergios", "test 3", "This is 3rd test")
        };
        Account Thodoris = new Account("Thodoris","1234", Arrays.asList(m1.clone()));
        Account Stergios = new Account("Stergios","5678", Arrays.asList(m2.clone()));
        outStream.writeObject(Thodoris);
        outStream.writeObject(Stergios);

        //initiate registry
        int serverPort = Integer.parseInt(Args[0]);
        registry = LocateRegistry.createRegistry(serverPort);
        startRMItable();
        Database.loadDatabase();

        for (int i = 0; i < NUMBER_OF_THREADS; i++)
            new MyThread().start();
    }

    /**
     * Binds a Services object with a reference name of the registry
     * @param objIndex the object's index (e.g EmailServer1, EmailServer2 etc).
     */
    private static void createObj(int objIndex) throws RemoteException {
        services[objIndex] = new Services();

        registry.rebind("EmailServer" + objIndex, services[objIndex]);
        System.out.println("Created Services obj #" + objIndex);
    }

    private static void startRMItable() {
        try {
            rmiTable = new RMItable(NUMBER_OF_THREADS);
            bindRMItable();
        } catch (RemoteException | MalformedURLException e) {
            e.printStackTrace();
        }
    }

    private static void bindRMItable() throws MalformedURLException, RemoteException {
        Naming.rebind("RMItable", rmiTable);
    }

    /**
     * Checks whether the client is still online by receiving and sending ping messages
     */
    public static class MyThread extends Thread {
        @Override
        public void run() {
            try {
                int objId = rmiTable.addObj();
                createObj(objId);
                while (true) {
                    while (true) {
                        sleep(5000);
                        if (services[objId].clientConnected()) {
                            System.out.println("Client connected on #" + objId);
                            break;
                        }
                    }
                    while (true) {
                        sleep(5000);
                        if (!services[objId].isOnline()) {
                            System.out.println("Client disconnected from #" + objId);
                            rmiTable.freeObj(objId);
                            createObj(objId);
                            break;
                        }
                    }
                }
            } catch (RemoteException | InterruptedException e) {
                e.printStackTrace();
            }
        }
    }
}
