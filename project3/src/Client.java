import java.net.MalformedURLException;
import java.rmi.Naming;
import java.rmi.NotBoundException;
import java.rmi.RemoteException;

import javax.swing.JOptionPane;

import utils.Interface;
import utils.Constants;

public class Client {
	private static Interface look_up;

	private void printMenu(){
		System.out.printf("Escolha uma das seguintes opções:\n");
		System.out.printf("(1) listar todas as pessoas formadas em um determinado curso;\n");
		System.out.printf("(2) listar as habilidades dos perfis que moram em uma determinada cidade;\n");
		System.out.printf("(3) acrescentar uma nova experiência em um perfil;\n");
		System.out.printf("(4) dado o email do perfil, retornar sua experiência;\n");
		System.out.printf("(5) listar todas as informações de todos os perfis;\n");
		System.out.printf("(6) dado o email de um perfil, retornar suas informações;\n");
		System.out.printf("(7) sair.\n\n");
	}

	public static void main(String[] args) 
		throws MalformedURLException, RemoteException, NotBoundException {
		
		
		look_up = (Interface) Naming.lookup(Constants.ADDRESS);
		String txt = JOptionPane.showInputDialog("What is your name?");
			
		String response = look_up.helloTo(txt);
		JOptionPane.showMessageDialog(null, response);
	}
}