#pragma once
#include <string>

class Kurs {
private:
    std::string nazwa;
    std::string haslo;
    int prowadzacyId; // identyfikator prowadz¹cego

public:
    Kurs(const std::string& _nazwa, const std::string& _haslo, int _prowadzacyId);

    int getId() const;
    std::string getNazwa() const;
    std::string getHaslo() const;
    int getProwadzacyId() const;
};
