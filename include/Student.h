#include <string>
#include "Uzytkownik.h"


class Student : public Uzytkownik {
public:
    Student(int id, int nrAlbumu, const std::string& imie, const std::string& haslo)
        : Uzytkownik(id, nrAlbumu, imie, haslo, "student") {}

    void wyslijZadanie() {
        // Metoda specyficzna dla studenta
    }
};