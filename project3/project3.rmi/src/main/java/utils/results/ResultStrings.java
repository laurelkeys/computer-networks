package utils.results;

import java.util.ArrayList;
import java.util.Arrays;

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
        return Arrays.toString(data.toArray()); // FIXME on inherit classes
    }
}