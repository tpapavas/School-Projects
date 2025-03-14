import java.rmi.RemoteException;
import java.rmi.server.UnicastRemoteObject;
import java.util.HashMap;
import java.util.Map;

public class Services extends UnicastRemoteObject implements ServicesIntf{
    Map<String,Account> accounts;
    Account usr;
    int objId;
    RMItableIntf rmiTable;

    boolean clientOnline;
    boolean connected;

    public Services() throws RemoteException {
        this.accounts = new HashMap<>();
        this.clientOnline = true;
        this.connected = false;
    }

    @Override
    public Boolean register(String username, String password) throws RemoteException {
        usr = Database.getAccount(username);
        if (usr != null)
            return false;

        usr = Database.addAccount(username, password);
        System.out.println("Account created");
        return true;
    }

    @Override
    public Boolean logIn(String username, String password) throws RemoteException {
        usr = Database.findUser(username,password);

        return usr != null;
    }

    @Override
    public boolean newEmail(String sender, String receiver, String subject, String mainbody) throws RemoteException {
        Email email = new Email(true, sender, receiver, subject, mainbody);
        if(Database.getAccount(email.getReceiver()) != null) {
            Database.updateDatabase("add", email);
            System.out.println("Email sent");
            return true;
        }

        return false;
    }

    @Override
    public String showEmails() throws RemoteException {
        StringBuilder label = new StringBuilder();
        int id = 1;
        usr = Database.getAccount(usr.getUsername());
        int size = usr.getMailbox().size();

        label.append(String.format("%-8s%-16s%-16s\n","Id", "From", "Subject"));
        for (Email e : usr.getMailbox()) {
            label
                    .append(String.format("%-8.8s", e.isNew() ? (id++)+".[new]" : (id++)+"."))
                    .append(String.format("%-16.16s", e.getSender()))
                    .append(String.format("%-16.16s", e.getSubject()))
                    .append(id <= size ? '\n' : "");
        }

        return label.toString();
    }

    @Override
    public String readEmail(int mailNum) throws RemoteException {
        try {
            Email mail = usr.getMailbox().get(mailNum);
            mail.setNew(false);
            return "Subject: " + mail.getSubject() +
                    System.lineSeparator() +
                    "Mainbody: " + mail.getMainbody();
        } catch (IndexOutOfBoundsException e) {
            return null;
        }
    }

    @Override
    public boolean deleteEmail(int mailNum) throws RemoteException {
        try {
            Email mail = usr.getMailbox().get(mailNum);
            Database.updateDatabase("delete", mail);
            return true;
        } catch (IndexOutOfBoundsException e) {
            return false;
        }
    }

    @Override
    public void logOut() throws RemoteException {
        //nothing
    }

    @Override
    public void exit() throws RemoteException {
        //nothing
    }

    @Override
    public void setRMIobjId(RMItableIntf rmiTable, int objId) throws RemoteException {
        this.rmiTable = rmiTable;
        this.objId = objId;
    }

    @Override
    public boolean isOnline() throws RemoteException {
        if (clientOnline) {
            clientOnline = false;
            return true;
        }

        return false;
    }

    @Override
    public void stillOnline() throws RemoteException {
        clientOnline = true;
    }

    @Override
    public boolean clientConnected() throws RemoteException {
        return connected;
    }

    @Override
    public void setConnected(boolean connected) throws RemoteException {
        this.connected = connected;
    }
}
