import utils.Constants;
import utils.DataKeeper;
import utils.Person;
import utils.results.DataResult;
import utils.results.options.*;

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

        uno.addSkill("Acoustic Engineering", "English");
        tres.addSkill("Read", "Write", "Code");
        cinco.addSkill("Spanish", "English", "Reap", "Sow");

        uno.addExperience("Work", "Research");
        tres.addExperience("Work", "Study");
        cinco.addExperience("Study", "Work", "Study more");

        insert(uno);
        insert(tres);
        insert(cinco);
    }

    private void insert(Person person) {
        database.put(person.getEmail(), person);
    }


    @Override
    public DataResult getAllWithEducation(String education) throws RemoteException {
        ArrayList<Person> result = new ArrayList<>();
        for (Person person : database.values()) {
            if (person.getEducation().equals(education)) {
                result.add(person);
            }
        }
        return new ResultOpt1(result);
    }

    @Override
    public DataResult getSkills(String city) throws RemoteException {
        ArrayList<String> result = new ArrayList<>();
        for (Person person : database.values()) {
            if (person.getCity().equals(city)) {
                result.addAll(person.getSkills());
            }
        }
        return new ResultOpt2(result); // TODO remove duplicates if necessary
    }

    @Override
    public DataResult addExperience(String email, String experience) throws RemoteException {
        Person person = database.get(email);
        if (person != null) {
            person.addExperience(experience);
            return new ResultOpt3(true); // TODO check that the experience has been added
        }
        return new ResultOpt3(false);
    }

    @Override
    public DataResult getExperience(String email) throws RemoteException {
        Person person = database.get(email);
        return new ResultOpt4(person != null ? person.getExperiences() : new ArrayList<>());
    }

    @Override
    public DataResult getAllProfiles() throws RemoteException {
        return new ResultOpt5(new ArrayList<>(database.values()));
    }

    @Override
    public DataResult getProfile(String email) throws RemoteException {
        return new ResultOpt6(database.get(email));
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