package utils;


/** Name in the form: '//host:port/name' */
public final class AddressName {

    public static final String LOCALHOST = "//localhost/MyServer"; // defaults to port 1099

    public static String localhost(String port) {
        return "//localhost:" + port + "/MyServer";
    }

    public static String address(String address, String port) {
        return "//" + address + ":" + port + "/MyServer";
    }

    public static String address(String address) {
        return "//" + address + "/MyServer"; // defaults to port 1099
    }
}