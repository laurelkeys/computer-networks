package utils;

import java.rmi.Remote;
import java.rmi.RemoteException;

public interface DataKeeper extends Remote {

    <T> DataResult<T> getAllWithEducation(String education) throws RemoteException;

    <T> DataResult<T> getSkills(String city) throws RemoteException;

    <T> DataResult<T> addExperience(String email, String experience) throws RemoteException;

    <T> DataResult<T> getExperience(String email) throws RemoteException;

    <T> DataResult<T> getAllProfiles() throws RemoteException;

    <T> DataResult<T> getProfile(String email) throws RemoteException;
}