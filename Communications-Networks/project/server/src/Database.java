import java.io.*;
import java.util.HashMap;

public class Database {
    static final String usersFile = "users";
    static HashMap<String,Account> accounts = new HashMap<>();

    static void loadDatabase() {
        ObjectInputStream inStream = null;
        try {
            inStream = new ObjectInputStream(new FileInputStream(usersFile));

            Account usr = (Account) inStream.readObject();
            while (usr != null) {
                accounts.put(usr.getUsername(),usr);
                usr = (Account) inStream.readObject();
            }

            inStream.close();
        } catch (Exception e) {
            try {
                assert inStream != null;
                inStream.close();
            } catch (IOException ioException) {
                ioException.printStackTrace();
            }
        }
    }

    /**
     * Adds or removes a mail from an account
     * @param func Defines addition/removal
     * @param email the email that's about to be added/removed
     */
    static synchronized void updateDatabase(String func, Email email) {
        Account usr = accounts.get(email.getReceiver());
        if (func.equals("add"))
            usr.getMailbox().add(0,email);
        else if (func.equals("delete"))
            usr.getMailbox().remove(email);
        updateFile(usr);
    }

    static Account findUser(String username, String password) {
        for (Account usr : accounts.values()) {
            if (usr.getUsername().equals(username) && usr.getPassword().equals(password))
                return usr;
        }

        return null;
    }

    static synchronized Account addAccount(String username, String password) {
        Account newAccount = new Account(username,password,null);
        updateFile(newAccount);

        return newAccount;
    }

    static Account getAccount(String name) {
        return accounts.get(name);
    }

    /**
     * Updates the account that have been changed and
     * copies all accounts (including the updated one)
     * to the file
     * @param account a modified account
     */
    private static synchronized void updateFile(Account account) {
        ObjectOutputStream outStream = null;
        try {
            outStream = new ObjectOutputStream(new FileOutputStream(usersFile));
        } catch (IOException e) {
            e.printStackTrace();
        }

        accounts.put(account.getUsername(), account);
        try {
            assert outStream != null;
            for (Account user : accounts.values()) {
                outStream.writeObject(user);
            }
            outStream.close();
        } catch (IOException e) {
            e.printStackTrace();
            try {
                outStream.close();
            } catch (IOException ioException) {
                ioException.printStackTrace();
            }
        }
    }
}
