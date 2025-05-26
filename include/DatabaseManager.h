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
    std::vector<std::pair<int, std::string>> getCoursesForStudent(int studentId);
    std::vector<std::shared_ptr<Kurs>> getAllCourses();
    bool addCourseContent(int kursId, const std::string& tytul, const std::string& zawartosc);
    std::vector<std::tuple<int, std::string, std::string>> getCourseContents(int kursId);
    std::vector<std::pair<int, std::string>> getStudentsInCourse(int kursId);
    bool addGrade(int kursId, int studentId, int ocena);
    std::vector<std::pair<int, std::string>> getAvailableCourses(int studentId);
    bool joinCourse(int studentId, int kursId);
    bool submitAssignment(int kursId, int studentId, const std::string& filePath);
    std::vector<std::tuple<int, std::string, std::string>> getAssignments(int kursId);
    bool addGradeForAssignment(int assignmentId, int ocena);
    std::vector<std::pair<std::string, int>> getStudentGrades(int studentId);
};

#endif
