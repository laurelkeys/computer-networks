import java.rmi.Naming;
import java.rmi.RemoteException;
import java.rmi.server.UnicastRemoteObject;

import RMIInterface;
import Util;

public class Server extends UnicastRemoteObject implements RMIInterface{
    private static final long serialVersionUID = 1L;

    protected Server() throws RemoteException {
        super();
    }

    @Override
    public String helloTo(String name) throws RemoteException{
        System.err.println(name + " is trying to contact!");
        return "Server says hello to " + name;
    }

    public static void main(String[] args){
        try {
            Naming.rebind(Util.ADDRESS, new Server());            
            System.err.println("Server ready");
        } catch (Exception e) {
            System.err.println("Server exception: " + e.toString());
            e.printStackTrace();
        }
    }
}