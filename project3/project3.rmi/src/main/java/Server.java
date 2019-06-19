import utils.Constants;
import utils.DataKeeper;
import utils.results.options.*;

import java.rmi.Naming;
import java.rmi.RemoteException;
import java.rmi.server.UnicastRemoteObject;

import static java.lang.System.err;
import static java.lang.System.out;

public class Server extends UnicastRemoteObject implements DataKeeper {

    private static final long serialVersionUID = 1L;

    private Server() throws RemoteException {
        super();
    }

    @Override
    public ResultOpt1 getAllWithEducation(String education) throws RemoteException {
        return null;
    }

    @Override
    public ResultOpt2 getSkills(String city) throws RemoteException {
        return null;
    }

    @Override
    public ResultOpt3 addExperience(String email, String experience) throws RemoteException {
        return new ResultOpt3(false);
    }

    @Override
    public ResultOpt4 getExperience(String email) throws RemoteException {
        return null;
    }

    @Override
    public ResultOpt5 getAllProfiles() throws RemoteException {
        return null;
    }

    @Override
    public ResultOpt6 getProfile(String email) throws RemoteException {
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