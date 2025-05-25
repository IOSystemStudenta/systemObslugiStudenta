#include "Uzytkownik.h"
#include <stdexcept>

// Definicja konstruktora
Uzytkownik::Uzytkownik(int _id, int _nrAlbumu,
                       const std::string& _imie,
                       const std::string& _haslo,
                       const std::string& _rola)
    : id(_id)
    , nrAlbumu(_nrAlbumu)
    , imie(_imie)
    , haslo(_haslo)
    , rola(_rola)
{
    if (imie.empty() || haslo.empty() || rola.empty())
        throw std::invalid_argument("Imi�, has�o i rola nie mog� by� puste");
    if (nrAlbumu <= 0)
        throw std::invalid_argument("Numer albumu musi by� dodatni");
}

// Definicja destruktora
Uzytkownik::~Uzytkownik() = default;

// Definicje getter�w
int Uzytkownik::getId() const { return id; }
int Uzytkownik::getNrAlbumu() const { return nrAlbumu; }
std::string Uzytkownik::getImie() const { return imie; }
std::string Uzytkownik::getHaslo() const { return haslo; }
std::string Uzytkownik::getRola() const { return rola; }


