#pragma once
#include <string>

class Wydzial {
protected:
    std::string nazwa;
    std::vector<Kurs> kursy;
    std::vector<Prowadzacy> prowadzacy;

public:
    Wydzial(std::string nazwa);

    std::string getNazwa() const { return nazwa; }
    std::vector<Kurs> getKurs() const { return kurs; }
    std::vector<Prowadzacy> getProwadzacy() const { return prowadzacy; }
    void dodajKurs(Kurs kurs);
    void dodajProwadzacego(Prowadzacy prowadzacy);

    virtual ~Wydzial() = default;
};