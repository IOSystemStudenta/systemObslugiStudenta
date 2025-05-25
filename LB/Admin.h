#pragma once

#include "Uzytkownik.h"
#include <string>

class Admin : public Uzytkownik {
public:
    // Deklaracja konstruktora
    Admin(int id, int nrAlbumu,
          const std::string& imie,
          const std::string& haslo);

    // Wirtualny destruktor
    virtual ~Admin();

};
