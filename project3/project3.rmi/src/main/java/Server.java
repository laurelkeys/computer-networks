import utils.Constants;
import utils.DataKeeper;
import utils.Person;

import java.rmi.Naming;
import java.rmi.RemoteException;
import java.rmi.server.UnicastRemoteObject;
import java.util.ArrayList;

import static java.lang.System.err;
import static java.lang.System.out;

public class Server extends UnicastRemoteObject implements DataKeeper {

    private static final long serialVersionUID = 1L;

    private Server() throws RemoteException {
        super();
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