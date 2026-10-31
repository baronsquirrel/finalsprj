#include "Searching.h"
#include "Utils.h"

// ============================================================================
//  LAB 7 - SEARCHING
// ============================================================================

// Linear keyword search: matches name, brand or category (case-insensitive).
int linearKeywordSearch(const Catalog& c, const string& key, Product** results) {
    int found = 0;
    for (int i = 0; i < c.size(); i++) {
        const Product* p = c.at(i);
        if (containsCI(p->name, key) || containsCI(p->brand, key) || containsCI(p->category, key))
            results[found++] = c.at(i);
    }
    return found;
}

// Binary searches on an array already sorted by price (ascending).
int lowerBound(Product** a, int n, double v) {   // first index with price >= v
    int lo = 0, hi = n;
    while (lo < hi) {
        int mid = lo + (hi - lo) / 2;
        if (a[mid]->price < v) lo = mid + 1; else hi = mid;
    }
    return lo;
}
int upperBound(Product** a, int n, double v) {   // first index with price > v
    int lo = 0, hi = n;
    while (lo < hi) {
        int mid = lo + (hi - lo) / 2;
        if (a[mid]->price <= v) lo = mid + 1; else hi = mid;
    }
    return lo;
}
