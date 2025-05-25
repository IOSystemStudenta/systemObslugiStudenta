#pragma once
#include <string>

class Uzytkownik {
protected:
    std::string login;

public:
    Uzytkownik(std::string login) {}

    std::string getLogin() const { return login; }

    virtual ~Uzytkownik() = default;
};