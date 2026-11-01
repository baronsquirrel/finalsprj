#include <iostream>
#include <iomanip>
#include <sstream>
#include <cctype>
#include <cstdlib>
#include "Utils.h"

string readLine(const string& prompt) {
    cout << prompt;
    string s;
    if (!getline(cin, s)) throw InputEnded();
    return s;
}

int readInt(const string& prompt, int lo, int hi) {
    while (true) {
        istringstream ss(readLine(prompt));
        int v;
        char extra;
        if ((ss >> v) && !(ss >> extra) && v >= lo && v <= hi) return v;
        cout << "  Invalid input. Enter a whole number from " << lo << " to " << hi << ".\n";
    }
}

double readDouble(const string& prompt, double lo, double hi) {
    while (true) {
        istringstream ss(readLine(prompt));
        double v;
        char extra;
        if ((ss >> v) && !(ss >> extra) && v >= lo && v <= hi) return v;
        cout << "  Invalid input. Enter a number from " << lo << " to " << hi << ".\n";
    }
}

string readNonEmpty(const string& prompt) {
    while (true) {
        string s = readLine(prompt);
        size_t a = s.find_first_not_of(" \t");
        size_t b = s.find_last_not_of(" \t");
        if (a != string::npos) return s.substr(a, b - a + 1);
        cout << "  This field cannot be empty.\n";
    }
}

void clearScreen() {
    cout.flush();                          // get earlier text out before the screen is wiped
#ifdef _WIN32
    int rc = system("cls");
    (void)rc;
#else
    cout << "\033[2J\033[H" << flush;      // ANSI clear + cursor home for non-Windows terminals
#endif
}

void pauseForEnter(const string& message) {
    cout << "\n";
    readLine(message);
}

string toLower(string s) {
    for (size_t i = 0; i < s.size(); i++) s[i] = (char)tolower((unsigned char)s[i]);
    return s;
}

bool containsCI(const string& text, const string& key) {
    return toLower(text).find(toLower(key)) != string::npos;
}

void printLine(char c) { cout << string(LINE_W, c) << "\n"; }

void printHeader() {
    cout << left << setw(7) << "ID" << setw(29) << "Name" << setw(13) << "Category"
         << setw(11) << "Brand" << right << setw(12) << "Price (PHP)" << setw(7) << "Stock" << "\n";
    printLine();
}

void printRow(const Product& p) {
    string n = p.name;
    if (n.size() > 27) n = n.substr(0, 24) + "...";
    cout << left << setw(7) << p.id << setw(29) << n << setw(13) << p.category
         << setw(11) << p.brand << right << setw(12) << fixed << setprecision(2) << p.price
         << setw(7) << p.stockQuantity << "\n";
}
