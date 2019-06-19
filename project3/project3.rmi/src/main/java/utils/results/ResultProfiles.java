package utils.results;

import utils.DataResult;
import utils.Person;

import java.util.ArrayList;
import java.util.Arrays;

public class ResultProfiles implements DataResult<ArrayList<Person>> {

    private ArrayList<Person> data;

    public ResultProfiles(ArrayList<Person> profiles) {
        data = profiles;
    }

    @Override
    public ArrayList<Person> getData() {
        return data;
    }

    @Override
    public String printable() {
        return Arrays.toString(data.toArray()); // FIXME on inherit classes
    }
}
