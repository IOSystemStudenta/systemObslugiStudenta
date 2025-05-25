#ifndef CONSOLEMANAGER_H
#define CONSOLEMANAGER_H

#include <string>
#include "DatabaseManager.h"

class ConsoleManager {
public:
    void showWelcomeMessage();
    void showStartMenu(DatabaseManager& db);
    void showLoginPrompt(DatabaseManager& db);
    void showLoginSuccess(const std::string& imie, const std::string& rola);
    void showLoginFailure();
    void showAdminMenu(DatabaseManager& db);
    void showAddUserPrompt(DatabaseManager& db);
    void showAddDepartmentPrompt(DatabaseManager& db);
    void printLine(const std::string& msg);
    void showStudentMenu(DatabaseManager& db);
    void showJoinKursPrompt(DatabaseManager& db);
    void showKursPrompt(DatabaseManager& db);
    void showGrades(DatabaseManager& db);
    void showProwadzacyMenu(DatabaseManager& db);
    void showCreateKursPrompt(DatabaseManager& db);
    void showChooseKursPrompt(DatabaseManager& db);
    void showKursy(DatabaseManager& db);
};

#endif
