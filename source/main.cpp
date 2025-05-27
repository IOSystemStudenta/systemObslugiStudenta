#include <iostream>
#include <sqlite3.h>
#include "DatabaseManager.h"
#include "ConsoleManager.h"

using namespace std;

int main() {
    DatabaseManager db("baza_uczelnia.db");
    db.initialize();
    
    ConsoleManager console(&db);
    console.showWelcomeMessage();
    console.showStartMenu();

    return 0;
}