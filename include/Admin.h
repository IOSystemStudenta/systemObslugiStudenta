#include "Uzytkownik.h"

class Admin : public Uzytkownik {
    public:
        Admin(int id, int nrAlbumu, const std::string& haslo, const std::string& rola)
            : Uzytkownik(id, nrAlbumu, haslo, rola) {}

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