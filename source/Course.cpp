#include "Course.h"
#include "Prowadzacy.h"
#include <iostream>
#include <string>

Course::Course(std::string name, std::string prowadzacy, std::string department) {
	this->name = name;
	this->prowadzacy = prowadzacy;
	this->department = department;
}

Course::void addInformation(Information information) {
	content.push_back(information);
}

Course::void addTask(Task task) {
	content.push_back(task);
}

Course::void addTest(Test test) {
	content.push_back(test);
}