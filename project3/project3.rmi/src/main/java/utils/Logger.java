package utils;

import java.io.FileWriter;
import java.io.IOException;

import static java.lang.System.err;
import static java.lang.System.out;

public final class Logger {

    public enum ClientServer {
        Client,
        Server
    }

    public enum Option {
        Opt1,
        Opt2,
        Opt3,
        Opt4,
        Opt5,
        Opt6
    }

    public enum Timestamp {
        BeforeInputSend, // T1
        BeforeQuery, // T2
        AfterQuery, // T3
        AfterResultReceived // T4
    }

    public static void log(ClientServer me, Option option, Timestamp timestamp) {
        log(me, option, timestamp, "");
    }

    public static void log(ClientServer me, Option option, Timestamp timestamp, String metadata) {
        String optionNumber = Integer.toString(option.ordinal() + 1);
        String timestampNumber = Integer.toString(timestamp.ordinal() + 1);
        String currentTime = Long.toString(System.currentTimeMillis());
        String log = String.join(",", me.name(), currentTime, optionNumber, timestampNumber);
        if (!metadata.isEmpty()) log += "," + metadata;

        try {
            FileWriter writer = new FileWriter("timestamp" + optionNumber + ".csv", true);
            writer.write(log + "\n");
            writer.close();
        } catch (IOException e) {
            err.println("Logger exception: " + e.toString());
        }
        if (me.equals(ClientServer.Server)) out.println(log);
    }

    public static void emptyFiles() {
        for (int opt = 1; opt <= 6; ++opt) {
            try {
                FileWriter writer = new FileWriter("timestamp" + opt + ".csv");
                writer.write("");
                writer.close();
            } catch (IOException e) {
                err.println("Logger exception: " + e.toString());
            }
        }
    }
}
