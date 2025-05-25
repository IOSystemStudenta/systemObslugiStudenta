#pragma once
#include <string>
#include <vector>
#include <memory>

class Kurs;        // forward declaration
class Prowadzacy;  // forward declaration

class Wydzial {
private:
    int id;
    std::string nazwa;
    std::vector<std::shared_ptr<Kurs>> kursy;
    std::vector<std::shared_ptr<Prowadzacy>> prowadzacy;

public:
    Wydzial(int id, const std::string& nazwa);

    int getId() const;
    std::string getNazwa() const;

    void dodajProwadzacego(std::shared_ptr<Prowadzacy> prow);

    void dodajKurs(std::shared_ptr<Kurs> kurs);

    const std::vector<std::shared_ptr<Kurs>>& getKursy() const;
    const std::vector<std::shared_ptr<Prowadzacy>>& getProwadzacy() const;
};
