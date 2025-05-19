#include <string>
#include "Uzytkownik.h"

class Prowadzacy : public Uzytkownik {
public:
    Prowadzacy(int id, int nrAlbumu, const std::string& imie, const std::string& haslo)
        : Uzytkownik(id, nrAlbumu, imie, haslo, "prowadzacy") {}

    void prowadzKurs() {
        // Metoda specyficzna dla prowadzącego
    }
};