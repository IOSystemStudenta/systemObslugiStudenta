#include "Wydzial.h"
#include "Kurs.h"
#include "Prowadzacy.h"
#include <algorithm>

Wydzial::Wydzial(int id, const std::string& nazwa) : id(id), nazwa(nazwa) {}

int Wydzial::getId() const {
    return id;
}

std::string Wydzial::getNazwa() const {
    return nazwa;
}

void Wydzial::dodajProwadzacego(std::shared_ptr<Prowadzacy> prow) {
    prowadzacy.push_back(prow);
}

void Wydzial::dodajKurs(std::shared_ptr<Kurs> kurs) {
    kursy.push_back(kurs);
}

const std::vector<std::shared_ptr<Kurs>>& Wydzial::getKursy() const {
    return kursy;
}

const std::vector<std::shared_ptr<Prowadzacy>>& Wydzial::getProwadzacy() const {
    return prowadzacy;
}
