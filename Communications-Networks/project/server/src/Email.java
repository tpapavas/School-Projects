import java.io.Serializable;
import java.util.Objects;

public class Email implements Serializable {
    boolean isNew;
    String sender;
    String receiver;
    String subject;
    String mainbody;

    public Email(boolean isNew, String sender, String receiver, String subject, String mainbody) {
        this.isNew = isNew;
        this.sender = sender;
        this.receiver = receiver;
        this.subject = subject;
        this.mainbody = mainbody;
    }

    public String getReceiver() {
        return receiver;
    }

    public String getSender() {
        return sender;
    }

    public String getSubject() {
        return subject;
    }

    public String getMainbody() {
        return mainbody;
    }

    public boolean isNew() {
        return isNew;
    }

    public void setNew(boolean aNew) {
        isNew = aNew;
    }

    @Override
    public boolean equals(Object o) {
        if (this == o) return true;
        if (o == null || getClass() != o.getClass()) return false;
        Email email = (Email) o;
        return isNew == email.isNew && Objects.equals(sender, email.sender) && Objects.equals(receiver, email.receiver) && Objects.equals(subject, email.subject) && Objects.equals(mainbody, email.mainbody);
    }

    @Override
    public int hashCode() {
        return Objects.hash(isNew, sender, receiver, subject, mainbody);
    }
}
