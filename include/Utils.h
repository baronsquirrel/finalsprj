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

string toLower(string s);
bool   containsCI(const string& text, const string& key);   // case-insensitive substring

void printLine(char c = '-');
void printHeader();                   // column titles for product tables
void printRow(const Product& p);      // one formatted product row
