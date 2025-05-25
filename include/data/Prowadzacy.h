#include <string>
#include "Uzytkownik.h"
#include "Tytul.h"

class Prowadzacy : public Uzytkownik {
protected:
    std::string imie;
    std::string nazwisko;
    Tytul title;

public:
    Prowadzacy(const std::string& imie, const std::string& nazwisko, Tytul tytul, std::string login)
        : Uzytkownik(login) {}

    int getNrAlbumu() const { return nrAlbumu; }
    std::string getImie() const { return nazwisko; }
    std::string getNazwisko() const { return nazwisko; }
    std::Tytuł getTytul() const { return title; }
};