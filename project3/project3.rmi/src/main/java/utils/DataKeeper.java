package utils;

import java.rmi.Remote;
import java.rmi.RemoteException;

public interface DataKeeper extends Remote {

    DataResult getAllWithEducation(String education) throws RemoteException;

    DataResult getSkills(String city) throws RemoteException;

    DataResult addExperience(String email, String experience) throws RemoteException;

    DataResult getExperience(String email) throws RemoteException;

    DataResult getAllProfiles() throws RemoteException;

    DataResult getProfile(String email) throws RemoteException;
}