package utils;

import java.io.Serializable;
import java.util.ArrayList;

public class Person implements Serializable {

    private static final long serialVersionUID = 1L;

    private String email;
    private String name;
    private String surname;
    private String city;
    private String education;
    private ArrayList<String> skills;
    private ArrayList<String> experiences;

    @Override
    public String toString() {
        return "Email: " + email
                + "\nNome: " + name
                + "\nSobrenome: " + surname
                + "\nResidência: " + city
                + "\nFormação Acadêmica: " + education
                + "\nHabilidades: " + String.join(", ", skills)
                + "\nExperiência: " + String.join(", ", experiences);
    }

    // TODO getters and setters, or not?
}