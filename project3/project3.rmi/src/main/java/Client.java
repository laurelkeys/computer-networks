import utils.Constants;
import utils.Interface;
import utils.Person;

import java.io.IOException;
import java.net.MalformedURLException;
import java.rmi.Naming;
import java.rmi.NotBoundException;
import java.rmi.RemoteException;
import java.util.ArrayList;

public class Client {

    private static Interface server;

    private static void printMenu() {
        System.out.printf("Escolha uma das seguintes opções:\n");
        System.out.printf("(1) listar todas as pessoas formadas em um determinado curso;\n");
        System.out.printf("(2) listar as habilidades dos perfis que moram em uma determinada cidade;\n");
        System.out.printf("(3) acrescentar uma nova experiência em um perfil;\n");
        System.out.printf("(4) dado o email do perfil, retornar sua experiência;\n");
        System.out.printf("(5) listar todas as informações de todos os perfis;\n");
        System.out.printf("(6) dado o email de um perfil, retornar suas informações;\n");
        System.out.printf("(7) sair.\n\n");
    }

    private static void opt1() throws RemoteException {
        System.out.print("Digite o curso> ");
        String education = System.console().readLine();
        ArrayList<Person> results = server.getAllWithEducation(education);
    }

    private static void opt2() throws RemoteException {
        System.out.print("Digite a cidade> ");
        String city = System.console().readLine();
        ArrayList<String> results = server.getSkills(city);
    }

    private static void opt3() throws RemoteException {
        System.out.print("Digite o email> ");
        String email = System.console().readLine();
        System.out.print("Digite a habilidade> ");
        String experience = System.console().readLine();
        boolean added = server.addExperience(email, experience);
        if (added) {
            System.out.println("Experiencia adicionada com sucesso");
        } else {
            System.out.println("Algum erro aconteceu");
        }
    }

    private static void opt4() throws RemoteException {
        System.out.print("Digite o email> ");
        String email = System.console().readLine();
        ArrayList<String> results = server.getExperience(email);
    }

    private static void opt5() throws RemoteException {
        ArrayList<Person> results = server.getAllProfiles();
    }

    private static void opt6() throws RemoteException {
        System.out.print("Digite o email> ");
        String email = System.console().readLine();
        Person results = server.getProfile(email);
    }

    public static void main(String[] args) throws MalformedURLException, RemoteException, NotBoundException {
        if (System.getSecurityManager() == null) System.setSecurityManager(new SecurityManager());

        server = (Interface) Naming.lookup(Constants.ADDRESS);

        int option;
        boolean quit = false;
        while (!quit) {
            printMenu();

            try {
                option = System.in.read();
            } catch (IOException e) {
                System.out.println("Input inválido");
                continue;
            }

            option = (option >= 1 && option <= 6) ? option : 7;
            switch (option) {
                case 1:
                    opt1();
                    break;
                case 2:
                    opt2();
                    break;
                case 3:
                    opt3();
                    break;
                case 4:
                    opt4();
                    break;
                case 5:
                    opt5();
                    break;
                case 6:
                    opt6();
                    break;
                default:
                    quit = true;
                    break;
            }
        }

        // String txt = JOptionPane.showInputDialog("What is your name?");

        // String response = look_up.helloTo(txt);
        // JOptionPane.showMessageDialog(null, response);
    }
}