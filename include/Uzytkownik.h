#pragma once
#include <string>

class Uzytkownik {
protected:
    int id;
    int nrAlbumu;
    std::string imie;
    std::string haslo;
    std::string rola;
public:
    Uzytkownik(int _id, int _nrAlbumu, const std::string& _imie, const std::string& _haslo, const std::string& _rola)
        : id(_id), nrAlbumu(_nrAlbumu), imie(_imie), haslo(_haslo), rola(_rola) {}

    int getId() const { return id; }
    int getNrAlbumu() const { return nrAlbumu; }
    std::string getHaslo() const { return haslo; }
    std::string getRola() const { return rola; }
    std::string getImie() const { return imie; }

    virtual ~Uzytkownik() = default;
};