#include "Prowadzacy.h"
#include "Kurs.h"
#include <iostream>
#include <stdexcept>
#include <memory>

// Przekazujemy "prowadzacy" jako rol�
Prowadzacy::Prowadzacy(int id, int nrAlbumu,
                       const std::string& imie,
                       const std::string& haslo)
    : Uzytkownik(id, nrAlbumu, imie, haslo, "prowadzacy")
{}

// Destruktor
Prowadzacy::~Prowadzacy() = default;

