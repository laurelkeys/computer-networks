package utils;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

import static java.lang.System.err;
import static utils.AddressName.address;
import static utils.AddressName.localhost;

public final class Parser {

    public static Map<String, List<String>> parseArgs(String[] args) {
        Map<String, List<String>> params = new HashMap<>();
        List<String> options = null;
        for (final String arg : args) {
            if (arg.charAt(0) == '-') {
                if (arg.length() < 2) {
                    err.println("Error at argument " + arg);
                    return params;
                }
                options = new ArrayList<>();
                params.put(arg.substring(1), options);
            } else if (options != null) {
                options.add(arg);
            } else {
                err.println("Illegal parameter usage");
                return params;
            }
        }
        return params;
    }

    public static String addressName(Map<String, List<String>> params) {
        if (params.containsKey("a")) {
            if (params.containsKey("p")) {
                return address(params.get("a").get(0), params.get("p").get(0));
            }

            return address(params.get("a").get(0));
        }

        if (params.containsKey("p")) {
            return localhost(params.get("p").get(0));
        }

        return AddressName.LOCALHOST;
    }
}
