import java.io.IOException;
import java.net.MalformedURLException;
import java.rmi.Naming;
import java.rmi.NotBoundException;
import java.rmi.RemoteException;
import java.util.Objects;
import java.util.Scanner;

public class Client {
    static ServicesIntf serverServices;
    static RMItableIntf rmiTable;
    static String usrnm;
    static String pswd;

    static Scanner input;
    static int mailNumber;
    static Boolean logOut = false;
    static Boolean online = true;

    static String newMailReceiver;
    static String newMailSubject;
    static String newMailMainBody;

    public static void main(String[] args) throws IOException, ClassNotFoundException, InterruptedException {
        String serverIP = args[0];
        int serverPort = Integer.parseInt(args[1]);

        String emailServerRMItable = "rmi://" + serverIP + ":" + serverPort + "/RMItable";
        String emailServerURL = "rmi://" + serverIP + ":"+ serverPort + "/EmailServer";

        try {
            rmiTable = (RMItableIntf) Naming.lookup(emailServerRMItable);
        } catch (NotBoundException | MalformedURLException | RemoteException e) {
            e.printStackTrace();
        }

        int objId = rmiTable.bindObj();  //find a free remote obj
        if(objId == -1) {
            System.out.println("Too many users online. Try later.");
            return;
        }

        //Obtain a reference for object "EmailServer" in registry//
        try {
            serverServices = (ServicesIntf) Naming.lookup(emailServerURL + objId);
            serverServices.setRMIobjId(rmiTable, objId);
            serverServices.setConnected(true);
        } catch (NotBoundException | MalformedURLException | RemoteException e) {
            e.printStackTrace();
        }

        new PingThread().start();

        input = new Scanner(System.in);
        System.out.println("Welcome to our mail service");
        while (true) {
            showStartMenu();
            String userInput = input.nextLine();
            if (userInput.equalsIgnoreCase("LogIn")) {
                readCredentials();
                Boolean loggedIn = serverServices.logIn(usrnm, pswd);
                System.out.println("---------------------------");
                if (loggedIn) {
                    System.out.println("LogIn successful");
                    while (!logOut) {
                        showLogInMenu();
                        userInput = input.nextLine();
                        if (userInput.equalsIgnoreCase("newEmail")) {
                            readNewMail();
                            boolean sent = serverServices.newEmail(usrnm, newMailReceiver, newMailSubject, newMailMainBody);
                            System.out.println(sent ? "Sent" : "Email wasn't sent. There is no such user.");
                        } else if (userInput.equalsIgnoreCase("showEmails")) {
                            String mails = serverServices.showEmails();
                            System.out.println(mails);
                        } else if (userInput.equalsIgnoreCase("readEmail")) {
                            readMailNumber();
                            System.out.println("---------------------------");
                            String mailMainbody = serverServices.readEmail(mailNumber);
                            System.out.println(Objects.requireNonNullElse(mailMainbody, "Wrong email id"));
                        } else if (userInput.equalsIgnoreCase("deleteEmail")) {
                            readMailNumber();
                            System.out.println(serverServices.deleteEmail(mailNumber) ? "Deleted" : "Wrong id");
                        } else if (userInput.equalsIgnoreCase("logOut")) {
                            serverServices.logOut();
                            logOut = true;
                        } else {
                            System.out.println("Please give a valid command");
                        }
                    }
                    logOut = false;
                } else {
                    System.out.println("Invalid user or password");
                }
            } else if (userInput.equalsIgnoreCase("SignIn")) {
                readCredentials();
                Boolean gotRegistered = serverServices.register(usrnm, pswd);
                System.out.println(gotRegistered ? "You signed in successfully" : "Username already exists");
            } else if (userInput.equalsIgnoreCase("Exit")) {
                serverServices.exit();
                online = false;
                break;
            } else {
                System.out.println("Please give a valid command");
            }
        }

        try {
            Naming.unbind(emailServerURL + objId);
        } catch (NotBoundException e) {
            e.printStackTrace();
        }
    }

    private static void showStartMenu() {
        //clrscr();
        System.out.println("===========================");
        System.out.println("-LogIn");
        System.out.println("-SignIn");
        System.out.println("-Exit");
    }

    private static void showLogInMenu() {
        System.out.println("===========================");
        System.out.println("-newEmail");
        System.out.println("-showEmails");
        System.out.println("-readEmail");
        System.out.println("-deleteEmail");
        System.out.println("-logOut");
    }

    private static void readCredentials() {
        //clrscr();
        System.out.print("Username: ");
        usrnm = input.nextLine();
        System.out.print("Password: ");
        pswd = input.nextLine();
    }

    private static void readNewMail() {
        String receiverName, subject, mainbody;

        System.out.print("Receiver: ");
        newMailReceiver = input.nextLine();
        System.out.print("Subject: ");
        newMailSubject = input.nextLine();
        System.out.print("Main Body: ");
        newMailMainBody = input.nextLine();
    }

    private static void readMailNumber() {
        System.out.println("Give the mail id");
        mailNumber = Integer.parseInt(input.nextLine()) - 1;
    }

    public static class PingThread extends Thread {
        @Override
        public void run() {
            try {
                while (online) {
                    sleep(2000);
                    serverServices.stillOnline();
                }
            } catch (InterruptedException | RemoteException e) {
                e.printStackTrace();
            }
        }
    }
}
