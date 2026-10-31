#include "SeedData.h"

void loadSeedData(Inventory& inv) {
    struct Seed { const char* name; const char* cat; const char* brand; double price; int stock; };
    const Seed seeds[] = {
        {"iPhone 15 128GB",            "Smartphones", "Apple",   45990, 12},
        {"AirPods Pro (2nd Gen)",      "Audio",       "Apple",   14990, 30},
        {"iPad 10th Gen 64GB",         "Tablets",     "Apple",   24990,  9},
        {"Apple Watch SE 40mm",        "Wearables",   "Apple",   15990, 15},
        {"Galaxy S24 256GB",           "Smartphones", "Samsung", 49990, 10},
        {"Galaxy Buds2 Pro",           "Audio",       "Samsung",  8990, 25},
        {"Galaxy Tab S9 FE",           "Tablets",     "Samsung", 25990,  7},
        {"Odyssey G5 27in Monitor",    "Monitors",    "Samsung", 15990,  6},
        {"WH-1000XM5 Headphones",      "Audio",       "Sony",    18990, 14},
        {"WF-1000XM5 Earbuds",         "Audio",       "Sony",    14990, 18},
        {"PlayStation 5 Slim",         "Gaming",      "Sony",    33990,  5},
        {"Bravia 43in 4K TV",          "TV",          "Sony",    28990,  4},
        {"LOQ 15 Gaming Laptop",       "Laptops",     "Lenovo",  59990,  8},
        {"IdeaPad Slim 3 15in",        "Laptops",     "Lenovo",  31990, 20},
        {"Legion Go Handheld",         "Gaming",      "Lenovo",  42990,  3},
        {"ThinkPad E14 Gen 5",         "Laptops",     "Lenovo",  46990, 11},
    };
    for (const Seed& s : seeds) {
        Product* p = new Product{inv.nextId, s.name, s.cat, s.brand, s.price, s.stock};
        if (!inv.addProduct(p)) delete p;
    }
}
