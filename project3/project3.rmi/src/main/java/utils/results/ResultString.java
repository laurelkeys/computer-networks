package utils.results;

import utils.DataResult;

public class ResultString implements DataResult<String> {

    private String data;

    public ResultString(String string) {
        data = string;
    }

    @Override
    public String getData() {
        return data;
    }

    @Override
    public String printable() {
        return data;
    }
}
