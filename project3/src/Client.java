import java.net.MalformedURLException;
import java.rmi.Naming;
import java.rmi.NotBoundException;
import java.rmi.RemoteException;

import javax.swing.JOptionPane;

import utils.Interface;
import utils.Constants;

public class Client {
	private static Interface look_up;

	public static void main(String[] args) 
		throws MalformedURLException, RemoteException, NotBoundException {
		
		look_up = (Interface) Naming.lookup(Constants.ADDRESS);
		String txt = JOptionPane.showInputDialog("What is your name?");
			
		String response = look_up.helloTo(txt);
		JOptionPane.showMessageDialog(null, response);
	}
}