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
    void showGrades();
    void showKursPrompt();
    void showProwadzacyMenu();
    void showChooseKursPrompt(int kursId);
    void showLecturerCourses();
    void showAllCourses();
    void showAddCourseContentPrompt(int kursId);
    void showCourseContents(int kursId);
    void showGradePrompt(int kursId);
    void showCourseParticipants(int kursId);
    void showKursPrompt(int kursId);
    void showCourseContentsForStudent(int kursId);
    void showContentDetails(int contentId, std::string tytul, int kursId);
    void saveSingleContentToFile(int contentId, std::string tytul, std::string zawartosc);
    void showAssignmentsForCourse(int kursId);
    void showAssignmentDetails(int assignmentId, std::string studentName, std::string filePath, int kursId);
    void showGradePromptForAssignment(int assignmentId, int kursId);
    void showSubmitAssignmentPrompt(int kursId);
    void showStudentChooseKursPrompt();
    void showInstructorManageKursPrompt(int kursId);
};

#endif
