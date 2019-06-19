package utils.results;

import utils.Person;

import java.util.ArrayList;

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
        ArrayList<String> printable = new ArrayList<>();
        for (Person profile : data) {
            printable.add(profile.toString());
        }
        return String.join("\n\n", printable);
    }
}
