#include <iostream>

using namespace std;
void mainmenu() {
    int choice;
    cout << "=============================" << endl;
    cout << " Witaj w systemie uczelni!" << endl;
    cout << " Zaloguj sie aby kontynuowac" << endl;
    cout << "=============================" << endl;
    cout << " 1.Logowanie" << endl;
    cout << " 2.Wyjdz" << endl;
    cin >> choice;
    switch (choice) {
        case 1:
            cout << "Logowanie..." << endl;
            // Tutaj dodaj kod logowania
            break;
        case 2:
            cout << "Wyjscie z programu..." << endl;
            exit(0);
        default:
            cout << "Niepoprawny wybor, sprobuj ponownie." << endl;
            mainmenu(); // Rekurencyjne wywołanie menu
    }
}

int main() {
    mainmenu();
    return 0;
}