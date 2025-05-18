#include <iostream>
#include <sqlite3.h>
#include "include\DatabaseManager.h"

using namespace std;
void mainmenu() {
    int choice;
    cout << "=============================" << endl;
    cout << " Witaj w systemie uczelni!" << endl;
    cout << " Zaloguj sie aby kontynuowac" << endl;
    cout << "=============================" << endl;
    cout << " 1.Logowanie" << endl;
    cout << " 2.Wyjdz" << endl;
    cin >> choice;
    switch (choice) {
        case 1:
            cout << "Logowanie..." << endl;
            // Tutaj dodaj kod logowania
            break;
        case 2:
            cout << "Wyjscie z programu..." << endl;
            exit(0);
        default:
            cout << "Niepoprawny wybor, sprobuj ponownie." << endl;
            mainmenu(); // Rekurencyjne wywołanie menu
    }
}
void database_init(DatabaseManager& db) {
    if (!db.connect()) {
        return;
    }
    db.createTables();
}

int main() {
    DatabaseManager db("baza_uczelnia.db");
    database_init(db);

    if (db.addUser(000000, "ADMIN", "haslo", "admin")) {
    std::cout << "Uzytkownik dodany pomyslnie." << std::endl;
    } else {
        std::cout << "Blad przy dodawaniu uzytkownika." << std::endl;
    }

    mainmenu();
    return 0;
}