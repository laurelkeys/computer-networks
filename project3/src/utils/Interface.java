package utils;

import java.rmi.Remote;
import java.rmi.RemoteException;
import java.util.ArrayList;

import utils.Person;

public interface Interface extends Remote {
    public String helloTo(String name) throws RemoteException;

    public ArrayList<Person> getAllWithEducation(String education) throws RemoteException;
    public ArrayList<String> getSkills(String city) throws RemoteException;
    public String addExperience(String email) throws RemoteException;
    public ArrayList<String> getExperience(String email) throws RemoteException;
    public ArrayList<Person> getAllProfiles() throws RemoteException;
    public Person getProfile(String email) throws RemoteException;
}