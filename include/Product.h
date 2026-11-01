#pragma once
#include <string>

// Product model (as required by the specification).
// The whole project uses the std namespace so the lab code reads cleanly.
using namespace std;

struct Product {
    int id;               // Unique Identifier
    string name;          // Product Title
    string category;      // Category (e.g., Electronics, Footwear)
    string brand;         // Official Partner Brand
    double price;         // Unit Price (PHP)
    int stockQuantity;    // Available Inventory Level
};
