#include <utility>
#include "Sorting.h"
#include "Utils.h"

// ============================================================================
//  LAB 8 - SORTING (Insertion Sort + Quick Sort) on arrays of Product*
// ============================================================================

bool priceAsc(const Product* a, const Product* b)  { return a->price < b->price; }
bool priceDesc(const Product* a, const Product* b) { return a->price > b->price; }
bool stockAsc(const Product* a, const Product* b)  { return a->stockQuantity < b->stockQuantity; }
bool stockDesc(const Product* a, const Product* b) { return a->stockQuantity > b->stockQuantity; }
bool brandAsc(const Product* a, const Product* b) {
    string x = toLower(a->brand), y = toLower(b->brand);
    if (x != y) return x < y;
    return toLower(a->name) < toLower(b->name);
}
bool brandDesc(const Product* a, const Product* b) { return brandAsc(b, a); }

// Stable, O(n^2) worst case, great on small / nearly sorted data.
void insertionSort(Product** a, int n, LessFn less) {
    for (int i = 1; i < n; i++) {
        Product* key = a[i];
        int j = i - 1;
        while (j >= 0 && less(key, a[j])) { a[j + 1] = a[j]; j--; }
        a[j + 1] = key;
    }
}

// Hoare-style partition with middle pivot, O(n log n) average.
void quickSort(Product** a, int lo, int hi, LessFn less) {
    if (lo >= hi) return;
    Product* pivot = a[lo + (hi - lo) / 2];
    int i = lo, j = hi;
    while (i <= j) {
        while (less(a[i], pivot)) i++;
        while (less(pivot, a[j])) j--;
        if (i <= j) { swap(a[i], a[j]); i++; j--; }
    }
    quickSort(a, lo, j, less);
    quickSort(a, i, hi, less);
}

Product** copyPointers(const Catalog& c) {
    Product** arr = new Product*[c.size() > 0 ? c.size() : 1];
    for (int i = 0; i < c.size(); i++) arr[i] = c.at(i);
    return arr;
}
