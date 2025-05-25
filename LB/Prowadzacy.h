#pragma once

#include "Uzytkownik.h"
#include <string>

class Prowadzacy : public Uzytkownik {
public:
    // Deklaracja konstruktora
    Prowadzacy(int id, int nrAlbumu,
               const std::string& imie,
               const std::string& haslo);

    // Wirtualny destruktor
    virtual ~Prowadzacy();
};
