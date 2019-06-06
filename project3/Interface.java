import java.rmi.Remote;
import java.rmi.RemoteException;
import Person;

public interface RMIInterface extends Remote {
    public String helloTo(String name) throws RemoteException;

    public Array<Person> getAllWithEducation(String education) throws RemoteException;
    public Array<String> getSkills(String city) throws RemoteException;
    public String addExperience(String email) throws RemoteException;
    public Array<String> getExperience(String email) throws RemoteException;
    public Array<Person> getAllProfiles() throws RemoteException;
    public Person getProfile(String email) throws RemoteException;
}