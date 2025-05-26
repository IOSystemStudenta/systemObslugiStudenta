#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <sqlite3.h>
#include <string>
#include <memory>
#include "Uzytkownik.h"
#include "Kurs.h"
#include <vector>

class DatabaseManager {
private:
    sqlite3* db;
    std::string dbName;

public:
    DatabaseManager(const std::string& name);
    ~DatabaseManager();

    bool connect();
    void initialize();
    void close();
    bool execute(const std::string& sql);
    void createTables();
    bool addUser(int nrAlbum, const std::string& imie, const std::string& haslo, const std::string& rola);
    bool assignUserToDepartment(int userId, int departmentId);
    std::vector<int> getUserDepartments(int userId);
    bool addDepartment(const std::string& nazwa);
    sqlite3* getConnection() const;
    std::unique_ptr<Uzytkownik> login(int nrAlbum, const std::string& haslo);
    bool addKurs(const std::string& tytul, int prowadzacyId, int wydzialId);
    std::vector<std::shared_ptr<Kurs>> getCoursesByLecturer(int lecturerId);
    std::vector<std::shared_ptr<Kurs>> getAllCourses();
    bool addCourseContent(int kursId, const std::string& tytul, const std::string& zawartosc);
};

#endif
