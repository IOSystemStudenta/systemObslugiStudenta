#include "Uzytkownik.h"

class Admin : public Uzytkownik {
    public:
        Admin(int id, int nrAlbumu, const std::string& imie, const std::string& haslo)
            : Uzytkownik(id, nrAlbumu, imie, haslo, "admin") {}

        void dodajUzytkownika() {
            // Implementacja zarządzania użytkownikami
        }

        void dodajWydzial() {
            // Implementacja zarządzania wydziałami
        }

        void dodajKurs() {
            // Implementacja dodawania kursu
        }

};