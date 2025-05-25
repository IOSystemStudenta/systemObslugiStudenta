#include "Kurs.h"
#include "Prowadzacy.h"
#include <iostream>
#include <string>

Wydzial::Wydzial(std::string nazwa {
	this->nazwa = nazwa;
}

Kurs::void dodajProwadzacego(Prowadzacy prowadzacy) {
	prowadzacy.push_back(prowadzacy);
}

Kurs::void dodajKurs(Kurs kurs) {
	kursy.push_back(kurs);
}