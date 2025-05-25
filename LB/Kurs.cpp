#include "Kurs.h"

Kurs::Kurs(const std::string& _nazwa, const std::string& _haslo, int _prowadzacyId)
    : nazwa(_nazwa), haslo(_haslo), prowadzacyId(_prowadzacyId) {}

std::string Kurs::getNazwa() const {
    return nazwa;
}

std::string Kurs::getHaslo() const {
    return haslo;
}

int Kurs::getProwadzacyId() const {
    return prowadzacyId;
}
