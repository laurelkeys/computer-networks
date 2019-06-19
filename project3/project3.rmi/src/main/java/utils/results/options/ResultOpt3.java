package utils.results.options;

import utils.results.ResultBoolean;

public class ResultOpt3 extends ResultBoolean {

    public ResultOpt3(Boolean bool) {
        super(bool);
    }

    @Override
    public String printable() {
        return getData() ? "Experiência adicionada com sucesso" : "Algum erro aconteceu";
    }
}
