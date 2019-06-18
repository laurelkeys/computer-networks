package utils;

import java.rmi.Remote;
import java.rmi.RemoteException;
import java.util.ArrayList;

public interface DataKeeper extends Remote {

    String helloTo(String name) throws RemoteException;

    ArrayList<Person> getAllWithEducation(String education) throws RemoteException;

    ArrayList<String> getSkills(String city) throws RemoteException;

    boolean addExperience(String email, String experience) throws RemoteException;

    ArrayList<String> getExperience(String email) throws RemoteException;

    ArrayList<Person> getAllProfiles() throws RemoteException;

    Person getProfile(String email) throws RemoteException;
}