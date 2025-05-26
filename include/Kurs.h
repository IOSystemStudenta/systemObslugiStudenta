#pragma once
#include <string>

class Kurs {
private:
    int id;
    std::string tytul;
    int prowadzacyId;
    int wydzialId;
    std::string wydzialNazwa;
    std::string prowadzacyImie;
    int liczbaStudentow;
    
public:
    Kurs(int _id, const std::string& _tytul, int _prowadzacyId, int _wydzialId, 
         const std::string& _wydzialNazwa = "", const std::string& _prowadzacyImie = "", 
         int _liczbaStudentow = 0)
        : id(_id), tytul(_tytul), prowadzacyId(_prowadzacyId), wydzialId(_wydzialId),
          wydzialNazwa(_wydzialNazwa), prowadzacyImie(_prowadzacyImie), 
          liczbaStudentow(_liczbaStudentow) {}

    // Gettery
    int getId() const { return id; }
    std::string getTytul() const { return tytul; }
    int getProwadzacyId() const { return prowadzacyId; }
    int getWydzialId() const { return wydzialId; }
    std::string getWydzialNazwa() const { return wydzialNazwa; }
    std::string getProwadzacyImie() const { return prowadzacyImie; }
    int getLiczbaStudentow() const { return liczbaStudentow; }
};