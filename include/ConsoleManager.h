#ifndef CONSOLEMANAGER_H
#define CONSOLEMANAGER_H

#include <string>
#include "DatabaseManager.h"

class ConsoleManager {
private:
    DatabaseManager* db;

public:
    ConsoleManager(DatabaseManager* db);
    ~ConsoleManager();

    void showWelcomeMessage();
    void showStartMenu();
    void showLoginPrompt();
    void showAdminMenu();
    void showStudentMenu(std::unique_ptr<Uzytkownik> user);
    void showAddUserPrompt();
    void showAddDepartmentPrompt();
    void showAllDepartments();
    void showAssignedCourses(std::unique_ptr<Uzytkownik> user);
};

#endif
