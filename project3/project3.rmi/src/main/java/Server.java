import utils.AddressName;
import utils.DataKeeper;
import utils.Logger;
import utils.Logger.ClientServer;
import utils.Person;
import utils.results.DataResult;
import utils.results.options.*;

import java.rmi.Naming;
import java.rmi.RemoteException;
import java.rmi.server.UnicastRemoteObject;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

import static java.lang.System.err;
import static java.lang.System.out;
import static utils.AddressName.address;
import static utils.AddressName.localhost;
import static utils.Logger.Option.*;
import static utils.Logger.Timestamp.AfterQuery;
import static utils.Logger.Timestamp.BeforeQuery;

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

    public static void main(String[] args) {
        final Map<String, List<String>> params = new HashMap<>();
        parseArgs(args, params);
        String name = addressName(params);
        out.println(name);

        try {
            Naming.rebind(AddressName.LOCALHOST, new Server());
            out.println("Server ready");
        } catch (Exception e) {
            err.println("Server exception: " + e.toString());
            e.printStackTrace();
        }
    }

    private void insert(Person person) {
        database.put(person.getEmail(), person);
    }

    @Override
    public DataResult getAllWithEducation(String education) throws RemoteException {
        Logger.log(ClientServer.Server, Opt1, BeforeQuery);
        ArrayList<Person> result = new ArrayList<>();
        for (Person person : database.values()) {
            if (person.getEducation().equals(education)) {
                result.add(person);
            }
        }
        Logger.log(ClientServer.Server, Opt1, AfterQuery);
        return new ResultOpt1(result);
    }

    @Override
    public DataResult getSkills(String city) throws RemoteException {
        Logger.log(ClientServer.Server, Opt2, BeforeQuery);
        ArrayList<String> result = new ArrayList<>();
        for (Person person : database.values()) {
            if (person.getCity().equals(city)) {
                result.addAll(person.getSkills());
            }
        }
        Logger.log(ClientServer.Server, Opt2, AfterQuery);
        return new ResultOpt2(result); // TODO remove duplicates if necessary
    }

    @Override
    public DataResult addExperience(String email, String experience) throws RemoteException {
        Logger.log(ClientServer.Server, Opt3, BeforeQuery);
        Person person = database.get(email);
        if (person != null) {
            person.addExperience(experience);
            return new ResultOpt3(true); // TODO check that the experience has been added
        }
        Logger.log(ClientServer.Server, Opt3, AfterQuery);
        return new ResultOpt3(false);
    }

    @Override
    public DataResult getExperience(String email) throws RemoteException {
        Logger.log(ClientServer.Server, Opt4, BeforeQuery);
        Person person = database.get(email);
        Logger.log(ClientServer.Server, Opt4, AfterQuery);
        return new ResultOpt4(person != null ? person.getExperiences() : new ArrayList<>());
    }

    @Override
    public DataResult getAllProfiles() throws RemoteException {
        Logger.log(ClientServer.Server, Opt5, BeforeQuery);
        ArrayList<Person> profiles = new ArrayList<>(database.values());
        Logger.log(ClientServer.Server, Opt5, AfterQuery);
        return new ResultOpt5(profiles);
    }

    @Override
    public DataResult getProfile(String email) throws RemoteException {
        Logger.log(ClientServer.Server, Opt6, BeforeQuery);
        Person profile = database.get(email);
        Logger.log(ClientServer.Server, Opt6, AfterQuery);
        return new ResultOpt6(profile);
    }

    private static String addressName(Map<String, List<String>> params) {
        if (params.containsKey("a")) {
            if (params.containsKey("p")) {
                return address(params.get("a").get(0), params.get("p").get(0));
            }

            return address(params.get("a").get(0));
        }

        if (params.containsKey("p")) {
            return localhost(params.get("p").get(0));
        }

        return AddressName.LOCALHOST;
    }

    private static void parseArgs(String[] args, Map<String, List<String>> params) {
        List<String> options = null;
        for (final String arg : args) {
            if (arg.charAt(0) == '-') {
                if (arg.length() < 2) {
                    err.println("Error at argument " + arg);
                    return;
                }
                options = new ArrayList<>();
                params.put(arg.substring(1), options);
            } else if (options != null) {
                options.add(arg);
            } else {
                err.println("Illegal parameter usage");
                return;
            }
        }
    }
}