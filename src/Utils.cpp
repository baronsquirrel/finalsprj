#include <iostream>
#include <iomanip>
#include <sstream>
#include <cctype>
#include <cstdlib>
#ifdef _WIN32
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#ifndef NOMINMAX            // MinGW's headers may already define it
#define NOMINMAX
#endif
#include <windows.h>
#endif
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

// ---------------------------------------------------------------------------
//  UTF-8 aware text helpers. Product names come from the database as UTF-8, so a
//  letter with an accent is ONE character but two bytes; counting bytes would push
//  the table columns out of line.
// ---------------------------------------------------------------------------
int textWidth(const string& s) {
    int n = 0;
    for (size_t i = 0; i < s.size(); i++)
        if ((static_cast<unsigned char>(s[i]) & 0xC0) != 0x80) n++;    // skip continuation bytes
    return n;
}

// Byte offset just after the first `chars` characters of s (never cuts a character in half).
static size_t bytesForChars(const string& s, int chars) {
    int seen = 0;
    for (size_t i = 0; i < s.size(); i++) {
        if ((static_cast<unsigned char>(s[i]) & 0xC0) != 0x80) {
            if (seen == chars) return i;
            seen++;
        }
    }
    return s.size();
}

string fitText(const string& s, int width) {
    int w = textWidth(s);
    if (w <= width) return s + string(width - w, ' ');
    if (width <= 3) return s.substr(0, bytesForChars(s, width));
    return s.substr(0, bytesForChars(s, width - 3)) + "...";
}

void setupConsole() {
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);      // names read from PostgreSQL are UTF-8
#endif
}

// ---------------------------------------------------------------------------
//  Product tables
// ---------------------------------------------------------------------------
static int clampInt(int v, int lo, int hi) { return v < lo ? lo : (v > hi ? hi : v); }

int ProductTableLayout::width() const { return 7 + name + category + brand + 13 + 7; }

ProductTableLayout makeLayout(Product** items, int count) {
    int n = 0, c = 0, b = 0;
    for (int i = 0; i < count; i++) {
        int w = textWidth(items[i]->name);      if (w > n) n = w;
        w = textWidth(items[i]->category);      if (w > c) c = w;
        w = textWidth(items[i]->brand);         if (w > b) b = w;
    }
    ProductTableLayout t;                       // +2 on each = the gap between columns
    t.name = clampInt(n + 2, 20, 40);           // longer text is cut with "..."
    t.category = clampInt(c + 2, 12, 18);
    t.brand = clampInt(b + 2, 10, 18);
    return t;
}

void printRule(const ProductTableLayout& t, char c) { cout << string(t.width(), c) << "\n"; }

void printHeader(const ProductTableLayout& t) {
    cout << left << setw(7) << "ID" << fitText("Name", t.name) << fitText("Category", t.category)
         << fitText("Brand", t.brand) << right << setw(13) << "Price (PHP)" << setw(7) << "Stock" << "\n";
    printRule(t);
}

void printRow(const Product& p, const ProductTableLayout& t) {
    cout << left << setw(7) << p.id << fitText(p.name, t.name - 2) << "  "
         << fitText(p.category, t.category - 2) << "  " << fitText(p.brand, t.brand - 2) << "  "
         << right << fixed << setprecision(2) << setw(13) << p.price << setw(7) << p.stockQuantity << "\n";
}
