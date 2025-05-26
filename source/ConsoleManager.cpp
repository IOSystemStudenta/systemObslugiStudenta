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
            showStudentChooseKursPrompt();
            break;
        case 3:
            std::cout << "Wyswietlanie ocen..." << std::endl;
            showGrades();
            break;
        case 4:
            std::cout << "Wylogowywanie..." << std::endl;
            setCurrentUser(nullptr); // Wyloguj użytkownika
            showStartMenu(); // Powrót do menu startowego
            break;
        default:
            std::cout << "Niepoprawny wybor, sprobuj ponownie." << std::endl;
            showStudentMenu();
    }
}

void ConsoleManager::showJoinKursPrompt() {
    if (!currentUser) return;
    
    auto courses = db->getAvailableCourses(currentUser->getId()); // Pobieramy dostępne kursy
    
    std::cout << "\n--- DOSTĘPNE KURSY ---\n";
    
    if (courses.empty()) {
        std::cout << "Brak dostępnych kursów do dołączenia." << std::endl;
        showStudentMenu();
        return;
    }

    std::cout << "ID | Tytuł kursu\n";
    std::cout << "------------------\n";
    for (const auto& course : courses) {
        std::cout << course.first << " | " << course.second << std::endl;
    }

    std::cout << "Podaj ID kursu do dołączenia: ";
    int kursId;
    std::cin >> kursId;

    // Sprawdzamy, czy kurs jest dostępny
    auto it = std::find_if(courses.begin(), courses.end(), [kursId](const auto& c) {
        return c.first == kursId;
    });

    if (it == courses.end()) {
        std::cout << "Niepoprawne ID kursu! Spróbuj ponownie." << std::endl;
        showJoinKursPrompt();
        return;
    }

    // Korzystamy z funkcji `joinCourse` w `DatabaseManager`
    if (db->joinCourse(currentUser->getId(), kursId)) {
        std::cout << "Dołączono do kursu: " << it->second << " pomyślnie!" << std::endl;
    } else {
        std::cout << "Nie udało się dołączyć do kursu." << std::endl;
    }

    showStudentMenu();
}

void ConsoleManager::showStudentChooseKursPrompt() {
    std::cout << "\n--- WYBIERZ KURS ---\n";

    auto courses = db->getCoursesForStudent(currentUser->getId());

    if (courses.empty()) {
        std::cout << "Nie jesteś zapisany na żaden kurs." << std::endl;
        showStudentMenu();
        return;
    }

    std::cout << "ID | Tytuł kursu\n";
    std::cout << "------------------\n";
    for (const auto& course : courses) {
        std::cout << course.first << " | " << course.second << std::endl;
    }

    std::cout << "Podaj ID kursu, który chcesz otworzyć: ";
    int kursId;
    std::cin >> kursId;

    auto it = std::find_if(courses.begin(), courses.end(), [kursId](const auto& c) {
        return c.first == kursId;
    });

    if (it == courses.end()) {
        std::cout << "Niepoprawne ID kursu! Spróbuj ponownie." << std::endl;
        showStudentChooseKursPrompt();
        return;
    }

    showKursPrompt(kursId); // Przechodzimy do strony kursu
}



void ConsoleManager::showKursPrompt(int kursId) {
    std::cout << "\n--- STRONA KURSU ---\n";
    std::cout << "1. Wyświetl treści kursu" << std::endl;
    std::cout << "2. Prześlij pracę zaliczeniową" << std::endl;
    std::cout << "3. Powrót do menu" << std::endl;
    std::cout << "Wybierz opcję: ";

    int choice;
    std::cin >> choice;

    switch (choice) {
        case 1:
            showCourseContentsForStudent(kursId);
            break;
        case 2:
            showSubmitAssignmentPrompt(kursId);
            break;
        case 3:
            showStudentMenu();
            break;
        default:
            std::cout << "Niepoprawny wybór! Spróbuj ponownie." << std::endl;
            showKursPrompt(kursId);
    }
}



void ConsoleManager::showGrades() {
    std::cout << "\n--- TWOJE OCENY ---\n";

    auto grades = db->getStudentGrades(currentUser->getId());

    if (grades.empty()) {
        std::cout << "Nie masz jeszcze żadnych ocen." << std::endl;
        showStudentMenu();
        return;
    }

    std::cout << "Kurs | Ocena\n";
    std::cout << "------------------\n";
    for (const auto& grade : grades) {
        std::cout << grade.first << " | " << grade.second << std::endl;
    }

    std::cout << "\nNaciśnij Enter, aby wrócić do menu...";
    std::cin.ignore();
    std::cin.get();
    
    showStudentMenu();
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


void ConsoleManager::showInstructorManageKursPrompt(int kursId) {
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
            showAddCourseContentPrompt(kursId);
            break;
        case 2:
            showCourseContents(kursId);
            break;
        case 3:
            showGradePrompt(kursId);
            break;
        case 4:
            showCourseParticipants(kursId);
            break;
        case 5:
            showLecturerCourses();
            break;
        default:
            std::cout << "Niepoprawny wybór. Spróbuj ponownie." << std::endl;
            showInstructorManageKursPrompt(kursId);
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
        std::cout << "| Brak prowadzonych kursow                              |\n";
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
    std::cout << "\nPodaj ID kursu, aby zobaczyć szczegoly: ";
    int kursId;
    std::cin >> kursId;

    // Sprawdzenie poprawności wyboru
    auto it = std::find_if(kursy.begin(), kursy.end(), [kursId](const auto& kurs) {
        return kurs->getId() == kursId;
    });

    if (it != kursy.end()) {
        std::cout << "Przechodzenie do karty kursu: " << (*it)->getTytul() << std::endl;
        showInstructorManageKursPrompt(kursId); // Wywołanie funkcji do zarządzania kursem
    } else {
        std::cout << "Niepoprawne ID kursu! Sprobuj ponownie." << std::endl;
        showLecturerCourses();
    }
}


void ConsoleManager::showAllCourses() {
    auto kursy = db->getAllCourses();
    
    std::cout << "\nWSZYSTKIE KURSY:\n";
    std::cout << "=================================================================\n";
    std::cout << "| ID  | Tytul kursu\t\t| Wydzial\t| Prowadzacy     |\n";
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
    std::cout << "\n--- DODAWANIE TRESCI DO KURSU ---\n";
    
    std::string tytul, zawartosc;
    
    std::cout << "Podaj tytul tresci: ";
    std::cin.ignore();
    std::getline(std::cin, tytul);

    std::cout << "Podaj zawartosc tresci: ";
    std::getline(std::cin, zawartosc);

    if (db->addCourseContent(kursId, tytul, zawartosc)) {
        std::cout << "Tresc została dodana pomyslnie!" << std::endl;
    } else {
        std::cout << "Nie udalo się dodac tresci!" << std::endl;
    }

    showInstructorManageKursPrompt(kursId);
}

void ConsoleManager::showCourseContents(int kursId) {
    std::cout << "\n--- TRESCI KURSU ---\n";

    auto contents = db->getCourseContents(kursId);

    if (contents.empty()) {
        std::cout << "Brak dodanych tresci dla tego kursu." << std::endl;
    } else {
        for (const auto& content : contents) {
            std::cout << "\nTytul: " << std::get<0>(content) << std::endl;
            std::cout << "Tresc: " << std::get<1>(content) << std::endl;
            std::cout << "---------------------------------------" << std::endl;
        }
    }

    // Powrót do menu kursu
    showStudentChooseKursPrompt();
}

void ConsoleManager::showGradePrompt(int kursId) {
    std::cout << "\n--- WYSTAWIANIE OCEN ---\n";

    auto students = db->getStudentsInCourse(kursId);

    if (students.empty()) {
        std::cout << "Brak zapisanych studentow na ten kurs." << std::endl;
        showInstructorManageKursPrompt(kursId);
        return;
    }

    std::cout << "Lista studentow:\n";
    for (const auto& student : students) {
        std::cout << student.first << ". " << student.second << std::endl;
    }

    std::cout << "Podaj ID studenta, ktoremu chcesz wystawic ocene: ";
    int studentId;
    std::cin >> studentId;

    auto it = std::find_if(students.begin(), students.end(), [studentId](const auto& s) {
        return s.first == studentId;
    });

    if (it == students.end()) {
        std::cout << "Niepoprawne ID studenta! Sprobuj ponownie." << std::endl;
        showGradePrompt(kursId);
        return;
    }

    std::cout << "Podaj ocene dla " << it->second << ": ";
    int ocena;
    std::cin >> ocena;

    if (db->addGrade(kursId, studentId, ocena)) {
        std::cout << "Ocena została pomyslnie wystawiona!" << std::endl;
    } else {
        std::cout << "Nie udalo sie wystawic oceny!" << std::endl;
    }

    showInstructorManageKursPrompt(kursId);
}

void ConsoleManager::showCourseParticipants(int kursId) {
    std::cout << "\n--- LISTA UCZESTNIKOW KURSU ---\n";

    auto participants = db->getStudentsInCourse(kursId); // Używamy istniejącej funkcji!

    if (participants.empty()) {
        std::cout << "Brak studentow zapisanych na ten kurs." << std::endl;
    } else {
        std::cout << "ID | Imie studenta\n";
        std::cout << "------------------\n";
        for (const auto& participant : participants) {
            std::cout << participant.first << " | " << participant.second << std::endl;
        }
    }

    showInstructorManageKursPrompt(kursId);
}

/*void ConsoleManager::showJoinKursPrompt() {
    if (!currentUser || currentUser->getRola() != "student") return;

    std::cout << "\n--- DOSTĘPNE KURSY ---\n";

    auto courses = db->getAvailableCourses();

    if (courses.empty()) {
        std::cout << "Brak dostępnych kursów do dołączenia." << std::endl;
        showStudentMenu();
        return;
    }

    std::cout << "ID | Tytuł kursu\n";
    std::cout << "------------------\n";
    for (const auto& course : courses) {
        std::cout << course.first << " | " << course.second << std::endl;
    }

    std::cout << "Podaj ID kursu, do którego chcesz dołączyć: ";
    int kursId;
    std::cin >> kursId;

    auto it = std::find_if(courses.begin(), courses.end(), [kursId](const auto& c) {
        return c.first == kursId;
    });

    if (it == courses.end()) {
        std::cout << "Niepoprawne ID kursu! Spróbuj ponownie." << std::endl;
        showJoinKursPrompt();
        return;
    }

    if (db->joinCourse(currentUser->getId(), kursId)) {
        std::cout << "Dołączono do kursu: " << it->second << " pomyślnie!" << std::endl;
    } else {
        std::cout << "Nie udało się dołączyć do kursu." << std::endl;
    }

    showStudentMenu();
}*/

void ConsoleManager::showCourseContentsForStudent(int kursId) {
    std::cout << "\n--- DOSTĘPNE TREŚCI KURSU ---\n";

    auto contents = db->getCourseContents(kursId);

    if (contents.empty()) {
        std::cout << "Brak dodanych materiałów." << std::endl;
        showStudentMenu();
        return;
    }

    std::cout << "ID | Tytuł treści\n";
    std::cout << "------------------\n";
    for (const auto& content : contents) {
        std::cout << std::get<0>(content) << " | " << std::get<1>(content) << std::endl;
    }

    std::cout << "Podaj ID treści, aby zobaczyć szczegóły: ";
    int contentId;
    std::cin >> contentId;

    auto it = std::find_if(contents.begin(), contents.end(), [contentId](const auto& c) {
    return std::get<0>(c) == contentId;
});

    if (it == contents.end()) {
        std::cout << "Niepoprawne ID treści! Spróbuj ponownie." << std::endl;
        showCourseContentsForStudent(kursId);
        return;
    }

    showContentDetails(std::get<0>(*it), std::get<1>(*it), kursId);
}

void ConsoleManager::showContentDetails(int contentId, std::string tytul, int kursId) {
    std::cout << "\n--- SZCZEGÓŁY TREŚCI ---\n";
    std::cout << "📌 " << tytul << std::endl;

    auto contents = db->getCourseContents(kursId);
    auto it = std::find_if(contents.begin(), contents.end(), [contentId](const auto& c) {
        return std::get<0>(c) == contentId;
    });

    if (it != contents.end()) {
        std::cout << std::get<1>(*it) << std::endl;
    }

    std::cout << "---------------------------------------\n";
    std::cout << "1. Pobierz treść jako plik .txt\n";
    std::cout << "2. Powrót do listy treści\n";
    std::cout << "Wybierz opcję: ";

    int choice;
    std::cin >> choice;

    switch (choice) {
        case 1:
            saveSingleContentToFile(contentId, tytul, std::get<1>(*it));
            break;
        case 2:
            showCourseContentsForStudent(kursId);
            break;
        default:
            std::cout << "Niepoprawny wybór! Spróbuj ponownie." << std::endl;
            showContentDetails(contentId, tytul, kursId);
    }
}

#include <fstream>

void ConsoleManager::saveSingleContentToFile(int contentId, std::string tytul, std::string zawartosc) {
    std::string fileName = tytul + ".txt";
    std::ofstream file(fileName);

    if (file.is_open()) {
        file << zawartosc;
        file.close();
        std::cout << "Plik zapisany: " << fileName << std::endl;
    } else {
        std::cout << "Nie udało się utworzyć pliku: " << fileName << std::endl;
    }

    showStudentMenu();
}

void ConsoleManager::showAssignmentsForCourse(int kursId) {
    std::cout << "\n--- PRACE ZALICZENIOWE ---\n";

    auto assignments = db->getAssignments(kursId);

    if (assignments.empty()) {
        std::cout << "Brak przesłanych prac dla tego kursu." << std::endl;
        showInstructorManageKursPrompt(kursId);
        return;
    }

    std::cout << "ID | Student | Ścieżka pliku\n";
    std::cout << "--------------------------------------\n";
    for (const auto& assignment : assignments) {
        std::cout << std::get<0>(assignment) << " | " << std::get<1>(assignment) 
                  << " | " << std::get<2>(assignment) << std::endl;
    }

    std::cout << "\nPodaj ID pracy, aby zobaczyć szczegóły: ";
    int assignmentId;
    std::cin >> assignmentId;

    auto it = std::find_if(assignments.begin(), assignments.end(), [assignmentId](const auto& a) {
        return std::get<0>(a) == assignmentId;
    });

    if (it == assignments.end()) {
        std::cout << "Niepoprawne ID pracy! Spróbuj ponownie." << std::endl;
        showAssignmentsForCourse(kursId);
        return;
    }

    showAssignmentDetails(std::get<0>(*it), std::get<1>(*it), std::get<2>(*it), kursId);
}

void ConsoleManager::showAssignmentDetails(int assignmentId, std::string studentName, std::string filePath, int kursId) {
    std::cout << "\n--- SZCZEGÓŁY PRACY ---\n";
    std::cout << "📌 Student: " << studentName << std::endl;
    std::cout << "📄 Plik: " << filePath << std::endl;
    std::cout << "---------------------------------------\n";
    std::cout << "1. Pobierz pracę\n";
    std::cout << "2. Wystaw ocenę\n";
    std::cout << "3. Powrót do listy prac\n";
    std::cout << "Wybierz opcję: ";

    int choice;
    std::cin >> choice;

    switch (choice) {
        case 1:
            std::cout << "Pobieranie pliku: " << filePath << std::endl;
            // Możesz dodać kod do rzeczywistego pobierania pliku
            break;
        case 2:
            showGradePromptForAssignment(assignmentId, kursId);
            break;
        case 3:
            showAssignmentsForCourse(kursId);
            break;
        default:
            std::cout << "Niepoprawny wybór! Spróbuj ponownie." << std::endl;
            showAssignmentDetails(assignmentId, studentName, filePath, kursId);
    }
}

void ConsoleManager::showGradePromptForAssignment(int assignmentId, int kursId) {
    std::cout << "\nPodaj ocenę dla tej pracy: ";
    int ocena;
    std::cin >> ocena;

    if (db->addGradeForAssignment(assignmentId, ocena)) {
        std::cout << "Ocena wystawiona pomyślnie!" << std::endl;
    } else {
        std::cout << "Nie udało się wystawić oceny." << std::endl;
    }

    showAssignmentsForCourse(kursId);
}

void ConsoleManager::showSubmitAssignmentPrompt(int kursId) {
    std::cout << "\n--- PRZESYŁANIE PRACY ZALICZENIOWEJ ---\n";

    std::cout << "Podaj ścieżkę do pliku (np. C:/PraceZaliczeniowe/praca.pdf): ";
    std::string filePath;
    std::cin >> filePath;

    if (db->submitAssignment(kursId, currentUser->getId(), filePath)) {
        std::cout << "Praca zaliczeniowa została pomyślnie przesłana!" << std::endl;
    } else {
        std::cout << "Nie udało się przesłać pracy zaliczeniowej." << std::endl;
    }

    showStudentMenu();
}
