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
    // Deklaracja konstruktora
    Uzytkownik(int _id, int _nrAlbumu,
               const std::string& _imie,
               const std::string& _haslo,
               const std::string& _rola);

    virtual ~Uzytkownik();

    // Deklaracje getterów
    int getId() const;
    int getNrAlbumu() const;
    std::string getImie() const;
    std::string getHaslo() const;
    std::string getRola() const;

    // Metoda czysto wirtualna – klasa staje siê abstrakcyjna
    virtual void wyswietlMenu() const {

    }
};
