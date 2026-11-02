#pragma once
// Input / output helpers shared by every menu.
#include <string>
#include "Product.h"

struct InputEnded {};          // thrown when stdin closes so destructors still run

constexpr int LINE_W = 79;     // width of table rules

string readLine(const string& prompt);
int    readInt(const string& prompt, int lo, int hi);
double readDouble(const string& prompt, double lo, double hi);
string readNonEmpty(const string& prompt);

// Cancel-aware input for prompts that are not menus. Each prints "<label> (Enter to cancel): ";
// pressing Enter on an empty line cancels and returns false (the value is left untouched).
bool readIntOrCancel(const string& label, int lo, int hi, int& out);
bool readDoubleOrCancel(const string& label, double lo, double hi, double& out);
bool readTextOrCancel(const string& label, string& out);        // any non-empty text

string trim(const string& s);                                   // strips spaces / tabs at both ends

// Wipes the console so each screen starts fresh (cls on Windows).
void clearScreen();

// Waits for the user to press Enter, so a result stays on screen until they are done with it.
void pauseForEnter(const string& message = "Press Enter to continue...");

string toLower(string s);
bool   containsCI(const string& text, const string& key);   // case-insensitive substring

void printLine(char c = '-');
// ---- text helpers that understand UTF-8, so accented names still line up -------------
int textWidth(const string& s);               // characters, not bytes
string fitText(const string& s, int width);   // exactly `width` characters: padded, or cut with "..."
void setupConsole();                          // Windows: make the console show UTF-8 correctly

// ---- product tables: column widths adapt to the data being shown ---------------------
struct ProductTableLayout {
    int name = 24, category = 12, brand = 10;     // widths of the three text columns
    int width() const;                            // total table width
};
ProductTableLayout makeLayout(Product** items, int count);   // measures every item once
void printHeader(const ProductTableLayout& t);
void printRow(const Product& p, const ProductTableLayout& t);
void printRule(const ProductTableLayout& t, char c = '-');
