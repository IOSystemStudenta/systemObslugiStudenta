#include "Course.h"
#include "Prowadzacy.h"
#include <iostream>
#include <string>

Kurs::Kurs(std::string nazwa, std::string nazwiskoProwadzacego, std::string nazwaWydzialu) {
	this->nazwa = nazwa;
	this->nazwiskoProwadzacego = nazwiskoProwadzacego;
	this->nazwaWydzialu = nazwaWydzialu;
}

Kurs::void dodajZadanie(Zadanie zadanie) {
	tresci.push_back(zadanie);
}

Kurs::void dodajZasob(Zasob zasob) {
	tresci.push_back(zasob);
}