#pragma once
#include <string>

class Kurs {
protected:
    std::string name;
    std::string nazwiskoProwadzacego;
    std::string nazwaWydzialu;
    std::vector<Tresc> tresci;

public:
    Kurs(std::string name, std::string nazwiskoProwadzacego, std::string nazwaWydzialu);

    std::string getName() const { return name; }
    std::string getProwadzacy() const { return nazwiskoProwadzacego; }
    std:string getDepartment() const { return nazwaWydzialu; }
    std::vector<Tresc> getTresc() const { return tresc; }
    void dodajZadanie(Zadanie zadanie);
    void dodajZasob(Zasob zasob);

    virtual ~Kurs() = default;
};