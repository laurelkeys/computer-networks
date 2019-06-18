package utils.results;

import utils.DataResult;

public class ResultBoolean implements DataResult<Boolean> {

    private Boolean data;

    public ResultBoolean(Boolean bool) {
        data = bool;
    }

    @Override
    public String printable() {
        return data.toString(); // FIXME
    }
}