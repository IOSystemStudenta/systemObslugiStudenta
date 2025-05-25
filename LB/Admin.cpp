#include "Admin.h"
#include "Wydzial.h"
#include <iostream>
#include <stdexcept>
#include <memory>

// Przekazujemy "admin" jako rolê
Admin::Admin(int id, int nrAlbumu,
             const std::string& imie,
             const std::string& haslo)
    : Uzytkownik(id, nrAlbumu, imie, haslo, "admin")
{}

// Destruktor
Admin::~Admin() = default;
