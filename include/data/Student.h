#include <string>
#include "Uzytkownik.h"


class Student : public Uzytkownik {
protected:
    int nrAlbumu;
    std::string imie;
    std::string nazwisko;

public:
    Student(int nrAlbumu, const std::string& imie, const std::string& nazwisko, const std::string& login)
        : Uzytkownik(login) {}
};