package utils;

import java.io.Serializable;
import java.util.ArrayList;

public class Person implements Serializable {

    private String email;
    private String name;
    private String surname;
    private String city;
    private String education;
    private ArrayList<String> skills;
    private ArrayList<String> experiences;

    // TODO getters and setters, or not?
}