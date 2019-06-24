import utils.DataKeeper;
import utils.Logger;
import utils.Logger.ClientServer;
import utils.DataResult;

import java.net.MalformedURLException;
import java.rmi.Naming;
import java.rmi.NotBoundException;
import java.rmi.RemoteException;

import static java.lang.System.*;
import static utils.Logger.Option.*;
import static utils.Logger.Timestamp.AfterResultReceived;
import static utils.Logger.Timestamp.BeforeInputSend;
import static utils.Parser.addressName;
import static utils.Parser.parseArgs;

import java.io.*;   

public class Client {

    private static DataKeeper server;
    private static BufferedReader reader;

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

    public static void main(String[] args) throws MalformedURLException, RemoteException, NotBoundException, IOException {
        server = (DataKeeper) Naming.lookup(addressName(parseArgs(args))); // AddressName.LOCALHOST by default

        Logger.emptyFiles();
        reader = new BufferedReader(new InputStreamReader(System.in));   

        int option;
        boolean quit = false;
        while (!quit) {
            printMenu();

            try {
				//con = console().readLine();
                //System.out.println("console:"+con);
                //str = reader.readLine();
                option = Integer.parseInt(reader.readLine());
				//System.out.println("input"+option);
            } catch (Exception e) {
				//System.out.println("input"+option);
				//System.out.println("console:"+con);
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

        atExit();
    }

    private static void atExit() {
        // TODO
    }

    private static void printResult(DataResult result) {
        out.println("--resultado-->");
        out.println(result.printable());
        out.println("<------------");
    }

    private static void opt1() throws RemoteException, IOException {
        out.print("Digite o curso> ");
        String education = reader.readLine();
        out.print("Curso selecionado:"+education);

        Logger.log(ClientServer.Client, Opt1, BeforeInputSend);
        DataResult result = server.getAllWithEducation(education);
        Logger.log(ClientServer.Client, Opt1, AfterResultReceived);
        printResult(result);
    }

    private static void opt2() throws RemoteException, IOException {
        out.print("Digite a cidade> ");
        String city = reader.readLine();

        Logger.log(ClientServer.Client, Opt2, BeforeInputSend);
        DataResult result = server.getSkills(city);
        Logger.log(ClientServer.Client, Opt2, AfterResultReceived);
        printResult(result);
    }

    private static void opt3() throws RemoteException, IOException {
        out.print("Digite o email> ");
        String email = reader.readLine();
        out.print("Digite a experiência> ");
        String experience = reader.readLine();

        Logger.log(ClientServer.Client, Opt3, BeforeInputSend);
        DataResult result = server.addExperience(email, experience);
        Logger.log(ClientServer.Client, Opt3, AfterResultReceived);
        printResult(result);
    }

    private static void opt4() throws RemoteException, IOException {
        out.print("Digite o email> ");
        String email = reader.readLine();
        printResult(server.getExperience(email));

        Logger.log(ClientServer.Client, Opt4, BeforeInputSend);
        DataResult result = server.getExperience(email);
        Logger.log(ClientServer.Client, Opt4, AfterResultReceived);
        printResult(result);
    }

    private static void opt5() throws RemoteException, IOException {
        Logger.log(ClientServer.Client, Opt5, BeforeInputSend);
        DataResult result = server.getAllProfiles();
        Logger.log(ClientServer.Client, Opt5, AfterResultReceived);
        printResult(result);
    }

    private static void opt6() throws RemoteException, IOException {
        out.print("Digite o email> ");
        String email = reader.readLine();

        Logger.log(ClientServer.Client, Opt6, BeforeInputSend);
        DataResult result = server.getProfile(email);
        Logger.log(ClientServer.Client, Opt6, AfterResultReceived);
        printResult(result);
    }
}
