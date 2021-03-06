package utils;

import java.io.Serializable;

public interface DataResult<T> extends Serializable {

    T getData();

    String printable();
}
