#include "ConsoleManager.h"
#include "Uzytkownik.h"
#include <iostream>
#include <memory>
#include <limits>

ConsoleManager::ConsoleManager(DatabaseManager* db) {
    this->db = db;
}

ConsoleManager::~ConsoleManager() {
    delete db;
}

void ConsoleManager::showWelcomeMessage() {
    std::cout << "=============================" << std::endl;
    std::cout << " Witaj w systemie uczelni!" << std::endl;
    std::cout << "=============================" << std::endl;
}

void ConsoleManager::showStartMenu() {
    std::cout << "1. Zaloguj sie" << std::endl;
    std::cout << "2. Wyjdz" << std::endl;
    std::cout << "Wybierz opcje: ";
    int choice;
    std::cin >> choice;
    switch (choice) {
        case 1:
            std::cout << "Logowanie..." << std::endl;
            showLoginPrompt();
            break;
        case 2:
            std::cout << "Wyjscie z programu..." << std::endl;
            exit(0);
        default:
            std::cout << "Niepoprawny wybor, sprobuj ponownie." << std::endl;
            showStartMenu(); 
    }
}

void ConsoleManager::showLoginPrompt() {
    int nrAlbum;
    std::string haslo;
    std::cout << "Podaj Nr albumu: ";
    std::cin >> nrAlbum;
    std::cout << "Podaj haslo: ";
    std::cin >> haslo;
    std::string imie, rola;
    std::unique_ptr<Uzytkownik> user = db.login(nrAlbum, haslo);

    if (user) {
        std::cout << "Witaj, " << imie << " (" << rola << ")!" << std::endl;
        
        if (user->getRola() == "admin") {
            showAdminMenu();
        }
        else if (user->getRola() == "student") {
            showStudentMenu();
        }
        else if (user->getRola() == "prowadzacy") {
            //showProwadzacyMenu();
        }
        else {
            std::cout << "Nieznana rola uzytkownika." << std::endl;
        }

    } else {
        std::cout << "Niepoprawny nr albumu lub haslo!" << std::endl;
        showStartMenu();
    }
}

void ConsoleManager::showStudentMenu(std::unique_ptr<Uzytkownik> user) {
    std::cout << "\n--- MOJE STUDIA ---" << std::endl;
    std::cout << "1. Przegladaj wydzialy i kursy" << std::endl;
    std::cout << "2. Moje kursy" << std::endl;
    std::cout << "3. Wyloguj" << std::endl;
    std::cout << "Wybierz opcje: ";
    int choice;
    std::cin >> choice;
    switch (choice) {
    case 1:
        showAllDepartments();
        // TODO: logika wybierania wydzia³u i wyœwietlenie dla niego kursów LUB zmiana powy¿szej metody, aby wyœwietla³a kursy pogrupowane wydzia³ami
        break;
    case 2:
        showAssignedCourses(user);
        // TODO: mo¿liwoœæ przegl¹dania zawartoœci kursów i wchodzenia z nimi w interakcje
        break;
    case 3:
        std::cout << "Wylogowywanie..." << std::endl;
        break;
    default:
        std::cout << "Niepoprawny wybor, sprobuj ponownie." << std::endl;
        showStudentMenu();
    }
}

void ConsoleManager::showAllDepartments() {
    std::vector<std::string> departments = db.getAllDepartments();
    int number = 1;
    for (std::vector<std::string> department : departments) {
        std::cout << number << ". " << department << std::endl;
        number++;
    }
}

void ConsoleManager::showAssignedCourses(std::unique_ptr<Uzytkownik> user) {
    std::vector<std::string> courses = db.getAssignedCourses(user->getNrAlbumu());
    int number = 1;
    for (std::vector<std::string> course : courses) {
        std::cout << number << ". " << course << std::endl;
        number++;
    }
}

void ConsoleManager::showCoursePrompt() {
    std::string name;
    std::cout << "Podaj nazwe kursu: ";
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Ignoruje znaki nowej linii przed getline
    std::getline(std::cin, name);
    if (!db.getCourse(name)) {
        std::cout << "Nie znaleziono kursu!" << std::endl;
    }
    else {
        Course course(db.getCourse(name));
        std::cout << course.name << std::endl;
        std::cout << course.owner << std::endl;
        std::cout << course.department << std::endl;
        for (Content c : course.content) {
            c.show();
        }
    }
}



// admin panel functionalities (not typically used during end-user application usage)
void ConsoleManager::showAdminMenu() {
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
            showAddUserPrompt();
            break;
        case 2:
            std::cout << "Dodawanie wydzialu..." << std::endl;
            showAddDepartmentPrompt();
            break;
        case 3:
            std::cout << "Wylogowywanie..." << std::endl;
            break;
        default:
            std::cout << "Niepoprawny wybor, sprobuj ponownie." << std::endl;
            showAdminMenu(); 
    }
}

void ConsoleManager::showAddUserPrompt() {
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

void ConsoleManager::showAddDepartmentPrompt() {
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