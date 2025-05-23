#include <iostream>
#include <sqlite3.h>
#include "DatabaseManager.h"
#include "ConsoleManager.h"

using namespace std;

int main() {
    DatabaseManager db("baza_uczelnia.db");
    db.initialize();

    ConsoleManager console;
    console.showWelcomeMessage();
    console.showStartMenu(db);
    return 0;
}