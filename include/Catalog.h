#pragma once
#include "Product.h"

// ============================================================================
//  LAB 1 - DYNAMIC ALLOCATION: resizable catalog (array of Product*)
//  The catalog OWNS every Product it holds and deletes them in its destructor.
// ============================================================================
class Catalog {
    Product** items;
    int count;
    int capacity;

    void grow() {                         // double the capacity (amortized O(1) add)
        int newCap = capacity * 2;
        Product** bigger = new Product*[newCap];
        for (int i = 0; i < count; i++) bigger[i] = items[i];
        delete[] items;
        items = bigger;
        capacity = newCap;
    }

public:
    Catalog() : items(new Product*[4]), count(0), capacity(4) {}
    ~Catalog() {
        for (int i = 0; i < count; i++) delete items[i];
        delete[] items;
    }
    Catalog(const Catalog&) = delete;
    Catalog& operator=(const Catalog&) = delete;

    void add(Product* p) {
        if (count == capacity) grow();
        items[count++] = p;
    }

    // Removes p from the array (does NOT delete it). Caller takes ownership.
    bool detach(const Product* p) {
        for (int i = 0; i < count; i++) {
            if (items[i] == p) {
                for (int j = i; j < count - 1; j++) items[j] = items[j + 1];
                count--;
                return true;
            }
        }
        return false;
    }

    int size() const { return count; }
    int cap() const { return capacity; }
    Product* at(int i) const { return items[i]; }
};
