import utils.Constants;
import utils.DataKeeper;
import utils.Person;

import java.rmi.Naming;
import java.rmi.RemoteException;
import java.rmi.server.UnicastRemoteObject;
import java.util.ArrayList;
import java.util.HashMap;

import static java.lang.System.err;
import static java.lang.System.out;

public class Server extends UnicastRemoteObject implements DataKeeper {

    private static final long serialVersionUID = 1L;

    private HashMap<String, Person> database = new HashMap<>();

    private Server() throws RemoteException {
        super();
        initDB();
    }

    private void initDB() {
        Person uno = new Person("uno@mail.com", "Uno", "Dos", "Campinas", "Linguistics");
        Person tres = new Person("tres@mail.com", "Tres", "Cuatro", "Campinas", "CS");
        Person cinco = new Person("cinco@mail.com", "Cinco", "Seis", "Seattle", "CS");

        // TODO add skills and experiences

        insert(uno);
        insert(tres);
        insert(cinco);
    }

    private void insert(Person person) {
        database.put(person.getEmail(), person);
    }

    @Override
    public ArrayList<Person> getAllWithEducation(String education) throws RemoteException {
        return null;
    }

    @Override
    public ArrayList<String> getSkills(String city) throws RemoteException {
        return null;
    }

    @Override
    public boolean addExperience(String email, String experience) throws RemoteException {
        return false;
    }

    @Override
    public ArrayList<String> getExperience(String email) throws RemoteException {
        return null;
    }

    @Override
    public ArrayList<Person> getAllProfiles() throws RemoteException {
        return null;
    }

    @Override
    public Person getProfile(String email) throws RemoteException {
        return null;
    }

    public static void main(String[] args) {
        try {
            Naming.rebind(Constants.ADDRESS, new Server());
            out.println("Server ready");
        } catch (Exception e) {
            err.println("Server exception: " + e.toString());
            e.printStackTrace();
        }
    }
}