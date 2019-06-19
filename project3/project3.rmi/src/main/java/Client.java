import utils.Constants;
import utils.DataKeeper;
import utils.results.DataResult;

import java.net.MalformedURLException;
import java.rmi.Naming;
import java.rmi.NotBoundException;
import java.rmi.RemoteException;

import static java.lang.System.*;

public class Client {

    private static DataKeeper server;

    private static void printMenu() {
        out.print("\nEscolha uma das seguintes opções:\n");
        out.print("(1) listar todas as pessoas formadas em um determinado curso;\n");
        out.print("(2) listar as habilidades dos perfis que moram em uma determinada cidade;\n");
        out.print("(3) acrescentar uma nova experiência em um perfil;\n");
        out.print("(4) dado o email do perfil, retornar sua experiência;\n");
        out.print("(5) listar todas as informações de todos os perfis;\n");
        out.print("(6) dado o email de um perfil, retornar suas informações;\n");
        out.print("(7) sair.\n\n");
    }

    private static void printResult(DataResult result) {
        out.println("--resultado-->");
        out.println(result.printable());
        out.println("<------------");
    }

    private static void opt1() throws RemoteException {
        out.print("Digite o curso> ");
        String education = console().readLine();
        printResult(server.getAllWithEducation(education));
    }

    private static void opt2() throws RemoteException {
        out.print("Digite a cidade> ");
        String city = console().readLine();
        printResult(server.getSkills(city));
    }

    private static void opt3() throws RemoteException {
        out.print("Digite o email> ");
        String email = console().readLine();
        out.print("Digite a habilidade> ");
        String experience = console().readLine();
        printResult(server.addExperience(email, experience));
    }

    private static void opt4() throws RemoteException {
        out.print("Digite o email> ");
        String email = console().readLine();
        printResult(server.getExperience(email));
    }

    private static void opt5() throws RemoteException {
        printResult(server.getAllProfiles());
    }

    private static void opt6() throws RemoteException {
        out.print("Digite o email> ");
        String email = console().readLine();
        printResult(server.getProfile(email));
    }

    public static void main(String[] args) throws MalformedURLException, RemoteException, NotBoundException {
        server = (DataKeeper) Naming.lookup(Constants.ADDRESS);

        int option;
        boolean quit = false;
        while (!quit) {
            printMenu();

            try {
                option = Integer.parseInt(console().readLine());
            } catch (Exception e) {
                err.println("Input inválido");
                continue;
            }

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
    }
}