#include "ConsoleManager.h"
#include "Uzytkownik.h"
#include <iostream>
#include <memory>
#include <limits>
#include <algorithm>

ConsoleManager::ConsoleManager(DatabaseManager* db) {
    this->db = db;
    currentUser = nullptr;
}

void ConsoleManager::setCurrentUser(std::unique_ptr<Uzytkownik> user) {
    currentUser = std::move(user);
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
    std::unique_ptr<Uzytkownik> user = db->login(nrAlbum, haslo);

    if (user) {
        setCurrentUser(std::move(user));
        showLoginSuccess(currentUser->getImie(), currentUser->getRola());
        
        if (currentUser->getRola() == "admin") {
            showAdminMenu();
        }
        else if (currentUser->getRola() == "student") {
            showStudentMenu();
        }
        else if (currentUser->getRola() == "prowadzacy") {
            showProwadzacyMenu();
        }
        else {
            std::cout << "Nieznana rola uzytkownika." << std::endl;
        }

    } else {
        showLoginFailure();
        showStartMenu();
    }
}

void ConsoleManager::showLoginSuccess(const std::string& imie, const std::string& rola) {
    std::cout << "Witaj, " << imie << " (" << rola << ")!" << std::endl;
}

void ConsoleManager::showLoginFailure() {
    std::cout << "Niepoprawny nr albumu lub haslo!" << std::endl;
}

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
            showAdminMenu(); 
            break;
        case 2:
            std::cout << "Dodawanie wydzialu..." << std::endl;
            showAddDepartmentPrompt();
            showAdminMenu();
            break;
        case 3:
            std::cout << "Wylogowywanie..." << std::endl;
            setCurrentUser(nullptr); // Wyloguj użytkownika
            showStartMenu(); // Powrót do menu startowego
            break;
        default:
            std::cout << "Niepoprawny wybor, sprobuj ponownie." << std::endl;
            showAdminMenu(); 
    }
}

void ConsoleManager::showAddUserPrompt() {
    std::string imie, rola, haslo;
    int nrAlbumu;
    
    std::cout << "Podaj imie: ";
    std::cin.ignore();
    std::getline(std::cin, imie);
    
    std::cout << "Podaj nr albumu: ";
    std::cin >> nrAlbumu;
    
    std::cout << "Podaj haslo: ";
    std::cin >> haslo;
    
    std::cout << "Podaj role (admin/student/prowadzacy): ";
    std::cin >> rola;
    
    // Dodaj użytkownika
    if(!db->addUser(nrAlbumu, imie, haslo, rola)) {
        std::cout << "Nie udalo sie dodac uzytkownika!\n";
        return;
    }
    
    // Pobierz ID nowo dodanego użytkownika
    int userId = sqlite3_last_insert_rowid(db->getConnection());
    
    // Jeśli to nie admin, przypisz do wydziału
    if (rola != "admin") {
        // Wyświetl dostępne wydziały
        std::cout << "\nDostepne wydzialy:\n";
        std::string query = "SELECT id, nazwa FROM Wydzial;";
        sqlite3_stmt* stmt;
        
        if (sqlite3_prepare_v2(db->getConnection(), query.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
            while (sqlite3_step(stmt) == SQLITE_ROW) {
                std::cout << sqlite3_column_int(stmt, 0) << ". " 
                          << sqlite3_column_text(stmt, 1) << "\n";
            }
            sqlite3_finalize(stmt);
        }
        
        std::cout << "Podaj ID wydzialu dla uzytkownika: ";
        int departmentId;
        std::cin >> departmentId;
        
        if (!db->assignUserToDepartment(userId, departmentId)) {
            std::cout << "Nie udalo sie przypisac uzytkownika do wydzialu!\n";
        } else {
            std::cout << "Uzytkownik dodany i przypisany do wydzialu pomyslnie!\n";
        }
    } else {
        std::cout << "Admin dodany pomyslnie!\n";
    }
}

void ConsoleManager::showAddDepartmentPrompt() {
    std::string nazwa;
    std::cout << "Podaj nazwe wydzialu: ";
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Ignoruje znaki nowej linii przed getline
    std::getline(std::cin, nazwa);
    if(!db->addDepartment(nazwa)) {
        std::cout << "Nie udalo sie dodac wydzialu!" << std::endl;
    } else {
        std::cout << "Wydzial dodany pomyslnie!" << std::endl;
    };
}

void ConsoleManager::showCreateKursPrompt() {
    if (!currentUser || currentUser->getRola() != "prowadzacy") return;

    // Pobierz wydziały prowadzącego
    auto prowadzacyWydzialy = db->getUserDepartments(currentUser->getId());
    
    if (prowadzacyWydzialy.empty()) {
        std::cout << "Nie jestes przypisany do zadnego wydzialu. Nie mozesz tworzyc kursow.\n";
        return;
    }

    // Jeśli prowadzący ma tylko jeden wydział, używamy go automatycznie
    int wybranyWydzialId = prowadzacyWydzialy[0];
    
    // Jeśli ma więcej wydziałów, dajmy wybór
    if (prowadzacyWydzialy.size() > 1) {
        std::cout << "Twoje wydzialy:\n";
        
        // Pobierz nazwy wydziałów
        for (int wydzialId : prowadzacyWydzialy) {
            std::string query = "SELECT nazwa FROM Wydzial WHERE id = ?;";
            sqlite3_stmt* stmt;
            
            if (sqlite3_prepare_v2(db->getConnection(), query.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
                sqlite3_bind_int(stmt, 1, wydzialId);
                if (sqlite3_step(stmt) == SQLITE_ROW) {
                    std::cout << wydzialId << ". " << sqlite3_column_text(stmt, 0) << "\n";
                }
                sqlite3_finalize(stmt);
            }
        }
        
        std::cout << "Wybierz ID wydzialu dla kursu: ";
        std::cin >> wybranyWydzialId;
        
        // Sprawdź czy wybrany wydział jest na liście prowadzącego
        if (std::find(prowadzacyWydzialy.begin(), prowadzacyWydzialy.end(), wybranyWydzialId) == prowadzacyWydzialy.end()) {
            std::cout << "Nie masz uprawnien do tego wydzialu!\n";
            return;
        }
    }

    std::string tytul;
    std::cout << "Podaj tytul kursu: ";
    std::cin.ignore();
    std::getline(std::cin, tytul);

    if (db->addKurs(tytul, currentUser->getId(), wybranyWydzialId)) {
        std::cout << "Kurs dodany pomyslnie do wydzialu o ID " << wybranyWydzialId << "!\n";
    } else {
        std::cout << "Nie udalo sie dodac kursu!\n";
    }
}


// Student
void ConsoleManager::showStudentMenu() {
    std::cout << "\n--- MENU STUDENTA ---" << std::endl;
    std::cout << "1. Dolacz do kursu" << std::endl;
    std::cout << "2. Wyswietl kurs" << std::endl;
    std::cout << "3. Wyswietl oceny" << std::endl;
    std::cout << "4. Wyloguj" << std::endl;
    std::cout << "Wybierz opcje: ";
    int choice;
    std::cin >> choice;
    switch (choice) {
        case 1:
            std::cout << "Wybierz kurs:" << std::endl;
            showJoinKursPrompt();
            break;
        case 2:
            std::cout << "Wybierz kurs:" << std::endl;
            showKursPrompt();
            break;
        case 3:
            std::cout << "Wyswietlanie ocen..." << std::endl;
            showGrades();
            break;
        case 4:
            std::cout << "Wylogowywanie..." << std::endl;
            break;
        default:
            std::cout << "Niepoprawny wybor, sprobuj ponownie." << std::endl;
            showStudentMenu();
    }
}

void ConsoleManager::showJoinKursPrompt() {
    if (!currentUser) return;

    sqlite3* conn = db->getConnection();
    std::string query = "SELECT id, tytul FROM Kurs;";
    sqlite3_stmt* stmt;
    
    if (sqlite3_prepare_v2(conn, query.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "Blad zapytania: " << sqlite3_errmsg(conn) << std::endl;
        return;
    }

    std::cout << "Dostepne kursy:\n";
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        int id = sqlite3_column_int(stmt, 0);
        std::string tytul = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        std::cout << id << ". " << tytul << "\n";
    }
    sqlite3_finalize(stmt);

    std::cout << "Podaj ID kursu do dolaczenia: ";
    int kursId;
    std::cin >> kursId;

    std::string insert = "INSERT INTO UczestnicyKursu (student_id, kurs_id) VALUES (?, ?);";
    if (sqlite3_prepare_v2(conn, insert.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "Blad przygotowania INSERT: " << sqlite3_errmsg(conn) << std::endl;
        return;
    }

    sqlite3_bind_int(stmt, 1, currentUser->getId());
    sqlite3_bind_int(stmt, 2, kursId);

    if (sqlite3_step(stmt) != SQLITE_DONE) {
        std::cerr << "Nie udalo sie dolaczyc do kursu: " << sqlite3_errmsg(conn) << std::endl;
    } else {
        std::cout << "Dolaczono do kursu pomyslnie!" << std::endl;
    }

    sqlite3_finalize(stmt);
}


void ConsoleManager::showKursPrompt() {

}

void ConsoleManager::showGrades() {

}

// Prowadzacy
void ConsoleManager::showProwadzacyMenu() {
    std::cout << "\n--- MENU PROWADZACEGO ---" << std::endl;
    std::cout << "1. Utworz kurs" << std::endl;
    std::cout << "2. Moje kursy" << std::endl;
    std::cout << "3. Wpisz oceny" << std::endl;
    std::cout << "4. Wyloguj" << std::endl;
    std::cout << "Wybierz opcje: ";
    int choice;
    std::cin >> choice;
    switch (choice) {
        case 1:
            std::cout << "Wpisz dane kursu:" << std::endl;
            showCreateKursPrompt();
            showProwadzacyMenu();
            break;
        case 2:
            std::cout << "Wyswietl kurs:" << std::endl;
            showLecturerCourses();
            //showChooseKursPrompt();
            showProwadzacyMenu();
            break;
        case 3:
            std::cout << "Wybierz kurs do wpisania ocen:" << std::endl;
            showLecturerCourses();
            //showKursy();
            showProwadzacyMenu();
            break;
        case 4:
            std::cout << "Wylogowywanie..." << std::endl;
            setCurrentUser(nullptr); // Wyloguj użytkownika
            showStartMenu(); // Powrót do menu startowego
            break;
        default:
            std::cout << "Niepoprawny wybor, sprobuj ponownie." << std::endl;
            showProwadzacyMenu();
    }
}


void ConsoleManager::showChooseKursPrompt(int kursId) {
    std::cout << "\n--- ZARZĄDZANIE KURSEM ---\n";
    std::cout << "1. Dodaj treść do kursu\n";
    std::cout << "2. Wyświetl istniejące treści\n";
    std::cout << "3. Wystaw oceny\n";
    std::cout << "4. Wyświetl uczestników\n";
    std::cout << "5. Powrót\n";
    std::cout << "Wybierz opcję: ";

    int choice;
    std::cin >> choice;

    switch (choice) {
        case 1:
            std::cout << "Dodawanie treści do kursu..." << std::endl;
            showAddCourseContentPrompt(kursId);
            break;
        case 2:
            std::cout << "Wyświetlanie dodanych treści..." << std::endl;
            //showCourseContents(kursId);
            break;
        case 3:
            std::cout << "Wystawianie ocen..." << std::endl;
            // Wywołanie funkcji do oceniania studentów
            break;
        case 4:
            std::cout << "Lista uczestników kursu:" << std::endl;
            // Wywołanie funkcji do wyświetlania uczestników
            break;
        case 5:
            std::cout << "Powrót do listy kursów..." << std::endl;
            showLecturerCourses();
            break;
        default:
            std::cout << "Niepoprawny wybór. Spróbuj ponownie." << std::endl;
            showChooseKursPrompt(kursId);
    }
}



void ConsoleManager::showLecturerCourses() {
    if (!currentUser || currentUser->getRola() != "prowadzacy") return;

    auto kursy = db->getCoursesByLecturer(currentUser->getId());

    std::cout << "\nTWOJE KURSY (" << currentUser->getImie() << "):\n";
    std::cout << "==========================================================\n";
    std::cout << "| ID  | Tytul kursu           | Wydzial        | Liczba studentow |\n";
    std::cout << "==========================================================\n";

    if (kursy.empty()) {
        std::cout << "| Brak prowadzonych kursów                              |\n";
    } else {
        for (const auto& kurs : kursy) {
            printf("| %-3d | %-20s | %-15s | %-15d |\n", 
                   kurs->getId(), 
                   kurs->getTytul().c_str(), 
                   kurs->getWydzialNazwa().c_str(), 
                   kurs->getLiczbaStudentow());
        }
    }
    std::cout << "==========================================================\n";

    // Zapytanie użytkownika o wybór kursu
    std::cout << "\nPodaj ID kursu, aby zobaczyć szczegóły: ";
    int kursId;
    std::cin >> kursId;

    // Sprawdzenie poprawności wyboru
    auto it = std::find_if(kursy.begin(), kursy.end(), [kursId](const auto& kurs) {
        return kurs->getId() == kursId;
    });

    if (it != kursy.end()) {
        std::cout << "Przechodzenie do karty kursu: " << (*it)->getTytul() << std::endl;
        showChooseKursPrompt(kursId); // Wywołanie funkcji do zarządzania kursem
    } else {
        std::cout << "Niepoprawne ID kursu! Spróbuj ponownie." << std::endl;
        showLecturerCourses();
    }
}


void ConsoleManager::showAllCourses() {
    auto kursy = db->getAllCourses();
    
    std::cout << "\nWSZYSTKIE KURSY:\n";
    std::cout << "=================================================================\n";
    std::cout << "| ID  | Tytuł kursu\t\t| Wydział\t| Prowadzący     |\n";
    std::cout << "=================================================================\n";
    
    for (const auto& kurs : kursy) {
        printf("| %-3d | %-20s | %-15s | %-15s |\n", 
               kurs->getId(), 
               kurs->getTytul().c_str(), 
               kurs->getWydzialNazwa().c_str(), 
               kurs->getProwadzacyImie().c_str());
    }
    std::cout << "=================================================================\n";
}

void ConsoleManager::showAddCourseContentPrompt(int kursId) {
    std::cout << "\n--- DODAWANIE TREŚCI DO KURSU ---\n";
    
    std::string tytul, zawartosc;
    
    std::cout << "Podaj tytuł treści: ";
    std::cin.ignore();
    std::getline(std::cin, tytul);

    std::cout << "Podaj zawartość treści: ";
    std::getline(std::cin, zawartosc);

    if (db->addCourseContent(kursId, tytul, zawartosc)) {
        std::cout << "Treść została dodana pomyślnie!" << std::endl;
    } else {
        std::cout << "Nie udało się dodać treści!" << std::endl;
    }

    showChooseKursPrompt(kursId);
}
