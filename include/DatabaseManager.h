#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <sqlite3.h>
#include <string>
#include <memory>
#include "Uzytkownik.h"

class DatabaseManager {
private:
    sqlite3* db;
    std::string dbName;

public:
    DatabaseManager(const std::string& name);
    ~DatabaseManager();

    bool connect();
    void initialize();
    void close();
    bool execute(const std::string& sql);
    void createTables();
    bool addUser(int nrAlbum, const std::string& imie, const std::string& haslo, const std::string& rola);
    sqlite3* getConnection() const;
    std::unique_ptr<Uzytkownik> login(int nrAlbum, const std::string& haslo);


};

#endif
