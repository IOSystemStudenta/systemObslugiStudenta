#include "ConsoleManager.h"
#include "Uzytkownik.h"
#include <iostream>
#include <memory>
#include <limits>

void ConsoleManager::showWelcomeMessage() {
    std::cout << "=============================" << std::endl;
    std::cout << " Witaj w systemie uczelni!" << std::endl;
    std::cout << "=============================" << std::endl;
}

void ConsoleManager::showStartMenu(DatabaseManager& db) {
    std::cout << "1. Zaloguj sie" << std::endl;
    std::cout << "2. Wyjdz" << std::endl;
    std::cout << "Wybierz opcje: ";
    int choice;
    std::cin >> choice;
    switch (choice) {
        case 1:
            std::cout << "Logowanie..." << std::endl;
            showLoginPrompt(db);
            break;
        case 2:
            std::cout << "Wyjscie z programu..." << std::endl;
            exit(0);
        default:
            std::cout << "Niepoprawny wybor, sprobuj ponownie." << std::endl;
            showStartMenu(db); 
    }
}

void ConsoleManager::showLoginPrompt(DatabaseManager& db) {
    int nrAlbum;
    std::string haslo;
    std::cout << "Podaj Nr albumu: ";
    std::cin >> nrAlbum;
    std::cout << "Podaj haslo: ";
    std::cin >> haslo;
    std::string imie, rola;
    std::unique_ptr<Uzytkownik> user = db.login(nrAlbum, haslo);

    if (user) {
        showLoginSuccess(user->getImie(), user->getRola());
        
        if (user->getRola() == "admin") {
            showAdminMenu(db);
        }
        else if (user->getRola() == "student") {
            //showStudentMenu();
        }
        else if (user->getRola() == "prowadzacy") {
            //showProwadzacyMenu();
        }
        else {
            std::cout << "Nieznana rola uzytkownika." << std::endl;
        }

    } else {
        showLoginFailure();
        showStartMenu(db);
    }
}

void ConsoleManager::showLoginSuccess(const std::string& imie, const std::string& rola) {
    std::cout << "Witaj, " << imie << " (" << rola << ")!" << std::endl;
}

void ConsoleManager::showLoginFailure() {
    std::cout << "Niepoprawny nr albumu lub haslo!" << std::endl;
}

void ConsoleManager::showStudentMenu(DatabaseManager& db) {
    std::cout << "\n--- MOJE STUDIA ---" << std::endl;
    std::cout << "1. Przegladaj wydzialy i kursy" << std::endl;
    std::cout << "2. Moje kursy" << std::endl;
    std::cout << "3. Wyloguj" << std::endl;
    std::cout << "Wybierz opcje: ";
    int choice;
    std::cin >> choice;
    switch (choice) {
    case 1:
        showAllDepartments(db);
        // TODO: logika wybierania wydzia�u i wy�wietlenie dla niego kurs�w LUB zmiana powy�szej metody, aby wy�wietla�a kursy pogrupowane wydzia�ami
        break;
    case 2:
        // showAssignedCourses(db);
        // TODO: mo�liwo�� przegl�dania zawarto�ci kurs�w i wchodzenia z nimi w interakcje
        break;
    case 3:
        std::cout << "Wylogowywanie..." << std::endl;
        break;
    default:
        std::cout << "Niepoprawny wybor, sprobuj ponownie." << std::endl;
        showStudentMenu(db);
    }
}

void ConsoleManager::showAllDepartments(DatabaseManager& db) {
    std::vector<std::string> departments = db.getAllDepartments();
    int id = 1;
    for (std::vector<std::string> department : departments) {
        std::cout << id << ". " << department << std::endl;
        id++;
    }
}

void ConsoleManager::showAdminMenu(DatabaseManager& db) {
    std::cout << "\n--- MENU ADMINA ---" << std::endl;
    std::cout << "1. Dodaj uzytkownika" << std::endl;
    std::cout << "2. Dodaj wydzial" << std::endl;
    std::cout << "3. Wyloguj" << std::endl;
    std::cout << "Wybierz opcje: ";
    int choice;
    std::cin >> choice;
    switch (choice) {
        case 1:
            std::cout << "Dodawanie uzytkownika..." << std::endl;
            showAddUserPrompt(db);
            break;
        case 2:
            std::cout << "Dodawanie wydzialu..." << std::endl;
            showAddDepartmentPrompt(db);
            break;
        case 3:
            std::cout << "Wylogowywanie..." << std::endl;
            break;
        default:
            std::cout << "Niepoprawny wybor, sprobuj ponownie." << std::endl;
            showAdminMenu(db); 
    }
}

void ConsoleManager::showAddUserPrompt(DatabaseManager& db) {
    std::string imie, rola;
    std::string haslo;
    int nrAlbumu;
    std::cout << "Podaj imie: ";
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Ignoruje znaki nowej linii przed getline
    std::getline(std::cin, imie);
    std::cout << "Podaj nr albumu: ";
    std::cin >> nrAlbumu;
    std::cout << "Podaj haslo: ";
    std::cin >> haslo;
    std::cout << "Podaj role (admin/student/prowadzacy): ";
    std::cin >> rola;
    if(!db.addUser(nrAlbumu, imie, haslo, rola)) {
        std::cout << "Nie udalo sie dodac uzytkownika!" << std::endl;
    } else {
        std::cout << "Uzytkownik dodany pomyslnie!" << std::endl;
    };
}

void ConsoleManager::showAddDepartmentPrompt(DatabaseManager& db) {
    std::string nazwa;
    std::cout << "Podaj nazwe wydzialu: ";
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Ignoruje znaki nowej linii przed getline
    std::getline(std::cin, nazwa);
    if(!db.addDepartment(nazwa)) {
        std::cout << "Nie udalo sie dodac wydzialu!" << std::endl;
    } else {
        std::cout << "Wydzial dodany pomyslnie!" << std::endl;
    };
}