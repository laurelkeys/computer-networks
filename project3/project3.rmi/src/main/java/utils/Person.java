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

    public Person(String email, String name, String surname, String city, String education,
                  ArrayList<String> skills, ArrayList<String> experiences) {
        this.email = email;
        this.name = name;
        this.surname = surname;
        this.city = city;
        this.education = education;
        this.skills = skills;
        this.experiences = experiences;
    }

    public Person(String email, String name, String surname, String city, String education) {
        this(email, name, surname, city, education, new ArrayList<>(), new ArrayList<>());
    }

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

    public String getEmail() {
        return email;
    }

    public String getName() {
        return name;
    }

    public String getSurname() {
        return surname;
    }

    public String getCity() {
        return city;
    }

    public String getEducation() {
        return education;
    }

    public ArrayList<String> getSkills() {
        return skills;
    }

    public ArrayList<String> getExperiences() {
        return experiences;
    }

    public void addSkill(String skill) {
        skills.add(skill);
    }

    public void addExperience(String experience) {
        experiences.add(experience);
    }
}