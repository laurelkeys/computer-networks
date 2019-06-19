package utils.results;

import java.util.ArrayList;

public class ResultStrings implements DataResult<ArrayList<String>> {

    private ArrayList<String> data;

    public ResultStrings(ArrayList<String> strings) {
        data = strings;
    }

    @Override
    public ArrayList<String> getData() {
        return data;
    }

    @Override
    public String printable() {
        return String.join("\n", data);
    }
}