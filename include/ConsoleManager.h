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
    //Funkcje główne do zarządzania konsolą
    void showWelcomeMessage();
    void showStartMenu();
    void showLoginPrompt();
    void showLoginSuccess(const std::string& imie, const std::string& rola);
    void showLoginFailure();
    //Menu dla różnych ról użytkowników
    void showAdminMenu();
    void showStudentMenu();
    void showProwadzacyMenu();
    //Opcje dla administratora
    void showAddUserPrompt();
    void showAddDepartmentPrompt();
    //Opcje dla studenta
    void showStudentChooseKursPrompt();
    void showJoinKursPrompt();
    void showCourseContentsForStudent(int kursId);
    void showSubmitAssignmentPrompt(int kursId);
    void showGrades();
    //Opcje dla prowadzącego
    void showInstructorManageKursPrompt(int kursId);
    void showCreateKursPrompt();
    void showLecturerCourses();
    int selectCourseForGrading();
    void showAssignmentsForGrading(int kursId);
    void showAssignmentsForCourse(int kursId);
    void showAssignmentDetails(int assignmentId, std::string studentName, std::string filePath, int kursId);
    void showGradePromptForAssignment(int assignmentId, int kursId);
    //Obsługa kursów  
    void showCourseContents(int kursId);
    void showKursPrompt(int kursId);
    void showAddCourseContentPrompt(int kursId);
    void showCourseParticipants(int kursId);
    void showGradePrompt(int kursId);
    void showContentDetails(int contentId, std::string tytul, int kursId);
    //Pliki
    void saveSingleContentToFile(int contentId, std::string tytul, std::string zawartosc);
};

#endif
