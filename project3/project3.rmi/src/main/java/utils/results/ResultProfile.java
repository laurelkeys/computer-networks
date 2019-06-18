package utils.results;

import utils.DataResult;
import utils.Person;

public class ResultProfile implements DataResult<Person> {

    private Person data;

    public ResultProfile(Person profile) {
        data = profile;
    }

    @Override
    public String printable() {
        return data.toString(); // FIXME
    }
}