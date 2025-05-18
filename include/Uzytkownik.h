#include <string>

class Uzytkownik {
protected:
    int id;
    int nrAlbumu;
    std::string haslo;
    std::string rola;
public:
    Uzytkownik(int _id, int _nrAlbumu, const std::string& _haslo, const std::string& _rola)
        : id(_id), nrAlbumu(_nrAlbumu), haslo(_haslo), rola(_rola) {}

    int getId() const { return id; }
    int getNrAlbumu() const { return nrAlbumu; }
    std::string getHaslo() const { return haslo; }
    std::string getRola() const { return rola; }

    virtual ~Uzytkownik() = default;
};