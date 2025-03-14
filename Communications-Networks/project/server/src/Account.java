import java.io.Serializable;
import java.util.ArrayList;
import java.util.List;
import java.util.Objects;

public class Account implements Serializable {
    String username;
    String password;
    List<Email> mailbox;

    public Account(String username, String password, List<Email> mailbox) {
        this.username = username;
        this.password = password;
        if (mailbox != null)
            this.mailbox = new ArrayList<>(mailbox);
        else
            this.mailbox = new ArrayList<>();
    }

    public String getUsername() {
        return username;
    }

    public String getPassword() {
        return password;
    }

    public List<Email> getMailbox() {
        return mailbox;
    }

    @Override
    public boolean equals(Object o) {
        if (this == o) return true;
        if (o == null || getClass() != o.getClass()) return false;
        Account account = (Account) o;
        return Objects.equals(username, account.username) && Objects.equals(password, account.password) && Objects.equals(mailbox, account.mailbox);
    }

    @Override
    public int hashCode() {
        return Objects.hash(username, password, mailbox);
    }
}
