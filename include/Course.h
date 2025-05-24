#pragma once
#include <string>

class Course {
protected:
    std::string name;
    std::string prowadzacy;
    std::string department;
    std::vector<Content> content;
public:
    Course(std::string name, std::string prowadzacy, std::string department);

    std::string getName() const { return name; }
    std::string getProwadzacy() const { return prowadzacy; }
    std:string getDepartment() const { return department; }
    std::vector<Content> getContent() const { return content; }
    void addInformation(Information information);
    void addTask(Task task);
    void addTest(Test test);

    virtual ~Course() = default;
};