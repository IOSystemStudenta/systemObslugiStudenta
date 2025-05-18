#include "DatabaseManager.h"
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
