#pragma once
#include "Uzytkownik.h"

class Admin : public Uzytkownik {
    public:
        Admin(int id, int nrAlbumu, const std::string& imie, const std::string& haslo)
            : Uzytkownik(id, nrAlbumu, imie, haslo, "admin") {}

};