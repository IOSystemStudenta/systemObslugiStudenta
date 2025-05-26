#include "DatabaseManager.h"
#include "Uzytkownik.h"
#include "Admin.h"
#include "Student.h"
#include "Prowadzacy.h"
#include "Kurs.h"
#include <iostream>

DatabaseManager::DatabaseManager(const std::string& name) {
    dbName = name;
    db = nullptr;
}

DatabaseManager::~DatabaseManager() {
    close();
}

bool DatabaseManager::connect() {
    int result = sqlite3_open(dbName.c_str(), &db);
    if (result != SQLITE_OK) {
        std::cerr << "Blad polaczenia z baza: " << sqlite3_errmsg(db) << std::endl;
        return false;
    }
    std::cout << "Polaczono z baza danych: " << dbName << std::endl;
    return true;
}

void DatabaseManager::initialize() {
    if (!connect()) {
        return;
    }
    createTables();
}

void DatabaseManager::close() {
    if (db) {
        sqlite3_close(db);
        db = nullptr;
    }
}

bool DatabaseManager::execute(const std::string& sql) {
    char* errorMessage = nullptr;
    int result = sqlite3_exec(db, sql.c_str(), nullptr, nullptr, &errorMessage);

    if (result != SQLITE_OK) {
        std::cerr << "Blad SQL: " << errorMessage << std::endl;
        sqlite3_free(errorMessage);
        return false;
    }
    return true;
}

void DatabaseManager::createTables() {
    std::string uzytkownikTable = R"(
        CREATE TABLE IF NOT EXISTS Uzytkownik (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            nrAlbum INTEGER UNIQUE,
            imie TEXT,
            haslo TEXT,
            rola TEXT CHECK(rola IN ('admin', 'prowadzacy', 'student'))
        );
    )";

    std::string wydzialTable = R"(
        CREATE TABLE IF NOT EXISTS Wydzial (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            nazwa TEXT
        );
    )";

    std::string wydzialUzytkownikTable = R"(
        CREATE TABLE IF NOT EXISTS WydzialUzytkownik (
            uzytkownik_id INTEGER,
            wydzial_id INTEGER,
            FOREIGN KEY (uzytkownik_id) REFERENCES Uzytkownik(id),
            FOREIGN KEY (wydzial_id) REFERENCES Wydzial(id)
        );
    )";

    std::string kursTable = R"(
        CREATE TABLE IF NOT EXISTS Kurs (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            tytul TEXT,
            prowadzacy_id INTEGER,
            wydzial_id INTEGER,
            FOREIGN KEY (prowadzacy_id) REFERENCES Uzytkownik(id),
            FOREIGN KEY (wydzial_id) REFERENCES Wydzial(id)
        );
    )";

    std::string uczestnicyKursuTable = R"(
        CREATE TABLE IF NOT EXISTS UczestnicyKursu (
            student_id INTEGER,
            kurs_id INTEGER,
            FOREIGN KEY (student_id) REFERENCES Uzytkownik(id),
            FOREIGN KEY (kurs_id) REFERENCES Kurs(id)
        );
    )";

    std::string zadaniaTable = R"(
        CREATE TABLE IF NOT EXISTS Zadania (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            kurs_id INTEGER,
            student_id INTEGER,
            file_path TEXT,
            ocena INTEGER,
            FOREIGN KEY (kurs_id) REFERENCES Kurs(id),
            FOREIGN KEY (student_id) REFERENCES Uzytkownik(id)
        );
    )";

    std::string trescKursuTable = R"(
        CREATE TABLE IF NOT EXISTS TrescKursu (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            kurs_id INTEGER,
            tytul TEXT,
            zawartosc TEXT,
            FOREIGN KEY (kurs_id) REFERENCES Kurs(id)
        );
    )";

    // Tworzenie tabel po kolei
    execute(uzytkownikTable);
    execute(wydzialTable);
    execute(wydzialUzytkownikTable);
    execute(kursTable);
    execute(uczestnicyKursuTable);
    execute(zadaniaTable);
    execute(trescKursuTable);

    std::cout << "Wszystkie tabele zostaly utworzone." << std::endl;
}

bool DatabaseManager::addUser(int nrAlbum, const std::string& imie, const std::string& haslo, const std::string& rola) {
    std::string sql = "INSERT INTO Uzytkownik (nrAlbum, imie, haslo, rola) VALUES (?, ?, ?, ?);";
    sqlite3_stmt* stmt = nullptr;

    int rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        std::cerr << "Blad przygotowania zapytania: " << sqlite3_errmsg(db) << std::endl;
        return false;
    }

    sqlite3_bind_int(stmt, 1, nrAlbum);
    sqlite3_bind_text(stmt, 2, imie.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 3, haslo.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 4, rola.c_str(), -1, SQLITE_TRANSIENT);

    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE) {
        std::cerr << "Blad wykonania zapytania: " << sqlite3_errmsg(db) << std::endl;
        sqlite3_finalize(stmt);
        return false;
    }

    sqlite3_finalize(stmt);
    return true;
}

bool DatabaseManager::assignUserToDepartment(int userId, int departmentId) {
    std::string sql = "INSERT INTO WydzialUzytkownik (uzytkownik_id, wydzial_id) VALUES (?, ?);";
    sqlite3_stmt* stmt;
    
    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        return false;
    }
    
    sqlite3_bind_int(stmt, 1, userId);
    sqlite3_bind_int(stmt, 2, departmentId);
    
    bool result = sqlite3_step(stmt) == SQLITE_DONE;
    sqlite3_finalize(stmt);
    return result;
}

std::vector<int> DatabaseManager::getUserDepartments(int userId) {
    std::vector<int> departments;
    std::string sql = "SELECT wydzial_id FROM WydzialUzytkownik WHERE uzytkownik_id = ?;";
    sqlite3_stmt* stmt;
    
    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_int(stmt, 1, userId);
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            departments.push_back(sqlite3_column_int(stmt, 0));
        }
        sqlite3_finalize(stmt);
    }
    return departments;
}

sqlite3* DatabaseManager::getConnection() const {
    return db;
}

std::unique_ptr<Uzytkownik> DatabaseManager::login(int nrAlbum, const std::string& haslo) {
    std::string sql = "SELECT id, imie, rola FROM Uzytkownik WHERE nrAlbum = ? AND haslo = ?;";
    sqlite3_stmt* stmt = nullptr;

    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "Blad przygotowania zapytania SQL: " << sqlite3_errmsg(db) << std::endl;
        return nullptr;
    }

    sqlite3_bind_int(stmt, 1, nrAlbum);
    sqlite3_bind_text(stmt, 2, haslo.c_str(), -1, SQLITE_TRANSIENT);

    std::unique_ptr<Uzytkownik> user = nullptr;

    if (sqlite3_step(stmt) == SQLITE_ROW) {
        int id = sqlite3_column_int(stmt, 0);
        std::string imie = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        std::string rola = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));

        if (rola == "admin") {
            user = std::make_unique<Admin>(id, nrAlbum, imie, haslo);
        } else if (rola == "student") {
            user = std::make_unique<Student>(id, nrAlbum, imie, haslo);
        } else if (rola == "prowadzacy") {
            user = std::make_unique<Prowadzacy>(id, nrAlbum, imie, haslo);
        }
    }

    sqlite3_finalize(stmt);
    return user;
}

bool DatabaseManager::addDepartment(const std::string& nazwa) {
    std::string sql = "INSERT INTO Wydzial (nazwa) VALUES (?);";
    sqlite3_stmt* stmt = nullptr;

    int rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        std::cerr << "Blad przygotowania zapytania: " << sqlite3_errmsg(db) << std::endl;
        return false;
    }

    sqlite3_bind_text(stmt, 1, nazwa.c_str(), -1, SQLITE_TRANSIENT);

    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE) {
        std::cerr << "Blad wykonania zapytania: " << sqlite3_errmsg(db) << std::endl;
        sqlite3_finalize(stmt);
        return false;
    }

    sqlite3_finalize(stmt);
    return true;
}

bool DatabaseManager::addKurs(const std::string& tytul, int prowadzacyId, int wydzialId) {
    // Sprawdź czy prowadzący jest przypisany do tego wydziału
    std::string checkSql = "SELECT 1 FROM WydzialUzytkownik WHERE uzytkownik_id = ? AND wydzial_id = ?;";
    sqlite3_stmt* checkStmt;
    
    if (sqlite3_prepare_v2(db, checkSql.c_str(), -1, &checkStmt, nullptr) != SQLITE_OK) {
        return false;
    }
    
    sqlite3_bind_int(checkStmt, 1, prowadzacyId);
    sqlite3_bind_int(checkStmt, 2, wydzialId);
    
    if (sqlite3_step(checkStmt) != SQLITE_ROW) {
        sqlite3_finalize(checkStmt);
        return false; // Prowadzący nie jest przypisany do tego wydziału
    }
    sqlite3_finalize(checkStmt);
    std::string sql = "INSERT INTO Kurs (tytul, prowadzacy_id, wydzial_id) VALUES (?, ?, ?);";
    sqlite3_stmt* stmt = nullptr;

    int rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        std::cerr << "Blad przygotowania zapytania SQL: " << sqlite3_errmsg(db) << std::endl;
        return false;
    }

    sqlite3_bind_text(stmt, 1, tytul.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 2, prowadzacyId);
    sqlite3_bind_int(stmt, 3, wydzialId);

    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE) {
        std::cerr << "Blad wykonania zapytania SQL: " << sqlite3_errmsg(db) << std::endl;
        sqlite3_finalize(stmt);
        return false;
    }

    sqlite3_finalize(stmt);
    return true;
}

std::vector<std::shared_ptr<Kurs>> DatabaseManager::getCoursesByLecturer(int lecturerId) {
    std::vector<std::shared_ptr<Kurs>> kursy;
    
    std::string query = R"(
        SELECT K.id, K.tytul, K.wydzial_id, W.nazwa, 
               U.imie, COUNT(UK.student_id)
        FROM Kurs K
        JOIN Wydzial W ON K.wydzial_id = W.id
        JOIN Uzytkownik U ON K.prowadzacy_id = U.id
        LEFT JOIN UczestnicyKursu UK ON K.id = UK.kurs_id
        WHERE K.prowadzacy_id = ?
        GROUP BY K.id
        ORDER BY K.id;
    )";
    
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_int(stmt, 1, lecturerId);
        
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            auto kurs = std::make_shared<Kurs>(
                sqlite3_column_int(stmt, 0),
                reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1)),
                lecturerId,
                sqlite3_column_int(stmt, 2),
                reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3)),
                reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4)),
                sqlite3_column_int(stmt, 5)
            );
            kursy.push_back(kurs);
        }
        sqlite3_finalize(stmt);
    }
    return kursy;
}

std::vector<std::pair<int, std::string>> DatabaseManager::getCoursesForStudent(int studentId) {
    std::vector<std::pair<int, std::string>> courses;
    std::string sql = R"(
        SELECT K.id, K.tytul FROM UczestnicyKursu UK
        JOIN Kurs K ON UK.kurs_id = K.id
        WHERE UK.student_id = ?;
    )";

    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_int(stmt, 1, studentId);

        while (sqlite3_step(stmt) == SQLITE_ROW) {
            int id = sqlite3_column_int(stmt, 0);
            std::string tytul = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
            courses.emplace_back(id, tytul);
        }

        sqlite3_finalize(stmt);
    }

    return courses;
}


std::vector<std::shared_ptr<Kurs>> DatabaseManager::getAllCourses() {
    std::vector<std::shared_ptr<Kurs>> kursy;
    
    std::string query = R"(
        SELECT K.id, K.tytul, K.prowadzacy_id, K.wydzial_id, 
               W.nazwa, U.imie, COUNT(UK.student_id)
        FROM Kurs K
        JOIN Wydzial W ON K.wydzial_id = W.id
        JOIN Uzytkownik U ON K.prowadzacy_id = U.id
        LEFT JOIN UczestnicyKursu UK ON K.id = UK.kurs_id
        GROUP BY K.id
        ORDER BY K.id;
    )";
    
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            auto kurs = std::make_shared<Kurs>(
                sqlite3_column_int(stmt, 0),
                reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1)),
                sqlite3_column_int(stmt, 2),
                sqlite3_column_int(stmt, 3),
                reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4)),
                reinterpret_cast<const char*>(sqlite3_column_text(stmt, 5)),
                sqlite3_column_int(stmt, 6)
            );
            kursy.push_back(kurs);
        }
        sqlite3_finalize(stmt);
    }
    return kursy;
}

bool DatabaseManager::addCourseContent(int kursId, const std::string& tytul, const std::string& zawartosc) {
    std::string sql = "INSERT INTO TrescKursu (kurs_id, tytul, zawartosc) VALUES (?, ?, ?);";
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "Błąd SQL: " << sqlite3_errmsg(db) << std::endl;
        return false;
    }

    sqlite3_bind_int(stmt, 1, kursId);
    sqlite3_bind_text(stmt, 2, tytul.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 3, zawartosc.c_str(), -1, SQLITE_TRANSIENT);

    bool success = sqlite3_step(stmt) == SQLITE_DONE;
    sqlite3_finalize(stmt);

    return success;
}

std::vector<std::tuple<int, std::string, std::string>> DatabaseManager::getCourseContents(int kursId) {
    std::vector<std::tuple<int, std::string, std::string>> contents;
    std::string sql = "SELECT id, tytul, zawartosc FROM TrescKursu WHERE kurs_id = ?;";
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_int(stmt, 1, kursId);

        while (sqlite3_step(stmt) == SQLITE_ROW) {
            int id = sqlite3_column_int(stmt, 0);
            std::string tytul = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
            std::string zawartosc = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
            contents.emplace_back(id, tytul, zawartosc);
        }

        sqlite3_finalize(stmt);
    }

    return contents;
}


std::vector<std::pair<int, std::string>> DatabaseManager::getStudentsInCourse(int kursId) {
    std::vector<std::pair<int, std::string>> students;
    std::string sql = R"(
        SELECT U.id, U.imie FROM UczestnicyKursu UK
        JOIN Uzytkownik U ON UK.student_id = U.id
        WHERE UK.kurs_id = ?;
    )";

    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_int(stmt, 1, kursId);

        while (sqlite3_step(stmt) == SQLITE_ROW) {
            int id = sqlite3_column_int(stmt, 0);
            std::string imie = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
            students.emplace_back(id, imie);
        }

        sqlite3_finalize(stmt);
    }

    return students;
}

bool DatabaseManager::addGrade(int kursId, int studentId, int ocena) {
    std::string sql = "INSERT INTO Zadania (kurs_id, student_id, ocena) VALUES (?, ?, ?);";
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "Błąd SQL: " << sqlite3_errmsg(db) << std::endl;
        return false;
    }

    sqlite3_bind_int(stmt, 1, kursId);
    sqlite3_bind_int(stmt, 2, studentId);
    sqlite3_bind_int(stmt, 3, ocena);

    bool success = sqlite3_step(stmt) == SQLITE_DONE;
    sqlite3_finalize(stmt);

    return success;
}

std::vector<std::pair<int, std::string>> DatabaseManager::getAvailableCourses(int studentId) {
    std::vector<std::pair<int, std::string>> courses;
    std::string sql = R"(
        SELECT K.id, K.tytul FROM Kurs K
        WHERE K.id NOT IN (
            SELECT UK.kurs_id FROM UczestnicyKursu UK WHERE UK.student_id = ?
        );
    )";

    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_int(stmt, 1, studentId);

        while (sqlite3_step(stmt) == SQLITE_ROW) {
            int id = sqlite3_column_int(stmt, 0);
            std::string tytul = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
            courses.emplace_back(id, tytul);
        }

        sqlite3_finalize(stmt);
    }
    return courses;
}


bool DatabaseManager::joinCourse(int studentId, int kursId) {
    std::string sql = "INSERT INTO UczestnicyKursu (student_id, kurs_id) VALUES (?, ?);";
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "Błąd SQL: " << sqlite3_errmsg(db) << std::endl;
        return false;
    }

    sqlite3_bind_int(stmt, 1, studentId);
    sqlite3_bind_int(stmt, 2, kursId);

    bool success = sqlite3_step(stmt) == SQLITE_DONE;
    sqlite3_finalize(stmt);

    return success;
}

std::vector<std::tuple<int, std::string, std::string>> DatabaseManager::getAssignments(int kursId) {
    std::vector<std::tuple<int, std::string, std::string>> assignments;
    std::string sql = R"(
        SELECT Z.id, U.imie, Z.file_path 
        FROM Zadania Z
        JOIN Uzytkownik U ON Z.student_id = U.id
        WHERE Z.kurs_id = ?;
    )";

    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_int(stmt, 1, kursId);

        while (sqlite3_step(stmt) == SQLITE_ROW) {
            int id = sqlite3_column_int(stmt, 0);
            std::string studentName = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
            std::string filePath = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
            assignments.emplace_back(id, studentName, filePath);
        }

        sqlite3_finalize(stmt);
    }

    return assignments;
}

bool DatabaseManager::addGradeForAssignment(int assignmentId, int ocena) {
    std::string sql = "UPDATE Zadania SET ocena = ? WHERE id = ?;";
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "Błąd SQL: " << sqlite3_errmsg(db) << std::endl;
        return false;
    }

    sqlite3_bind_int(stmt, 1, ocena);
    sqlite3_bind_int(stmt, 2, assignmentId);

    bool success = sqlite3_step(stmt) == SQLITE_DONE;
    sqlite3_finalize(stmt);

    return success;
}

bool DatabaseManager::submitAssignment(int kursId, int studentId, const std::string& filePath) {
    std::string sql = "INSERT INTO Zadania (kurs_id, student_id, file_path) VALUES (?, ?, ?);";
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "Błąd SQL: " << sqlite3_errmsg(db) << std::endl;
        return false;
    }

    sqlite3_bind_int(stmt, 1, kursId);
    sqlite3_bind_int(stmt, 2, studentId);
    sqlite3_bind_text(stmt, 3, filePath.c_str(), -1, SQLITE_TRANSIENT);

    bool success = sqlite3_step(stmt) == SQLITE_DONE;
    sqlite3_finalize(stmt);

    return success;
}

std::vector<std::pair<std::string, int>> DatabaseManager::getStudentGrades(int studentId) {
    std::vector<std::pair<std::string, int>> grades;
    std::string sql = R"(
        SELECT K.tytul, Z.ocena 
        FROM Zadania Z
        JOIN Kurs K ON Z.kurs_id = K.id
        WHERE Z.student_id = ? AND Z.ocena IS NOT NULL;
    )";

    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_int(stmt, 1, studentId);

        while (sqlite3_step(stmt) == SQLITE_ROW) {
            std::string kursTytul = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
            int ocena = sqlite3_column_int(stmt, 1);
            grades.emplace_back(kursTytul, ocena);
        }

        sqlite3_finalize(stmt);
    }

    return grades;
}


