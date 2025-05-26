#ifndef CONSOLEMANAGER_H
#define CONSOLEMANAGER_H

#include <string>
#include "DatabaseManager.h"

class ConsoleManager {
private:
    DatabaseManager* db;
    std::unique_ptr<Uzytkownik> currentUser;

public:
    ConsoleManager(DatabaseManager* db);
    void setCurrentUser(std::unique_ptr<Uzytkownik> user);

    void showWelcomeMessage();
    void showStartMenu();
    void showLoginPrompt();
    void showLoginSuccess(const std::string& imie, const std::string& rola);
    void showLoginFailure();
    void showAdminMenu();
    void showAddUserPrompt();
    void showAddDepartmentPrompt();
    void printLine(const std::string& msg);
    void showStudentMenu();
    void showCreateKursPrompt();
    void showJoinKursPrompt();
    void showKursPrompt();
    void showGrades();
    void showProwadzacyMenu();
    void showChooseKursPrompt(int kursId);
    void showLecturerCourses();
    void showAllCourses();
    void showAddCourseContentPrompt(int kursId);
};

#endif
