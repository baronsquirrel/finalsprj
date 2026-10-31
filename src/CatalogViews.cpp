#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include "CatalogViews.h"
#include "Utils.h"
#include "Sorting.h"
#include "Searching.h"
#include "BrandRing.h"

void showCatalog(const Inventory& inv) {
    cout << "\n--- COMPLETE CATALOG (" << inv.catalog.size() << " products, array capacity "
         << inv.catalog.cap() << ") ---\n";
    printHeader();
    for (int i = 0; i < inv.catalog.size(); i++) printRow(*inv.catalog.at(i));
    printLine();
}

void filterByBrand(const Inventory& inv) {
    // collect distinct brands
    vector<string> brands;
    for (int i = 0; i < inv.catalog.size(); i++) {
        const string& b = inv.catalog.at(i)->brand;
        bool seen = false;
        for (size_t k = 0; k < brands.size(); k++)
            if (toLower(brands[k]) == toLower(b)) { seen = true; break; }
        if (!seen) brands.push_back(b);
    }
    if (brands.empty()) { cout << "  The catalog is empty.\n"; return; }

    cout << "\n--- OFFICIAL PARTNER BRANDS ---\n";
    for (size_t i = 0; i < brands.size(); i++) cout << "  " << i + 1 << ". " << brands[i] << "\n";
    int choice = readInt("Select brand number: ", 1, (int)brands.size());
    const string& chosen = brands[choice - 1];

    cout << "\n--- " << chosen << " PRODUCTS ---\n";
    printHeader();
    int n = 0;
    for (int i = 0; i < inv.catalog.size(); i++) {
        if (toLower(inv.catalog.at(i)->brand) == toLower(chosen)) { printRow(*inv.catalog.at(i)); n++; }
    }
    printLine();
    cout << n << " product(s) from " << chosen << ".\n";
}

void searchMenu(const Inventory& inv) {
    cout << "\n--- SEARCH ---\n  1. Keyword (Linear Search)\n  2. Product ID (BST Search)\n  0. Back\n";
    int c = readInt("Choice: ", 0, 2);
    if (c == 1) {
        string key = readNonEmpty("Keyword (name / brand / category): ");
        Product** res = new Product*[inv.catalog.size() > 0 ? inv.catalog.size() : 1];
        int found = linearKeywordSearch(inv.catalog, key, res);
        if (found == 0) cout << "  No products match '" << key << "'.\n";
        else {
            printHeader();
            for (int i = 0; i < found; i++) printRow(*res[i]);
            printLine();
        }
        cout << "Linear search scanned " << inv.catalog.size() << " records (O(n)), "
             << found << " match(es).\n";
        delete[] res;
    } else if (c == 2) {
        int id = readInt("Product ID: ", 1, 2000000000);
        int steps = 0;
        Product* p = inv.find(id, &steps);
        if (!p) cout << "  No product with ID " << id << ".\n";
        else { printHeader(); printRow(*p); printLine(); }
        cout << "BST visited " << steps << " node(s); tree height " << inv.bst.height()
             << " for " << inv.catalog.size() << " products (O(log n)).\n";
    }
}

void priceRangeFilter(const Inventory& inv) {
    int n = inv.catalog.size();
    if (n == 0) { cout << "  The catalog is empty.\n"; return; }
    double lo = readDouble("Minimum price (PHP): ", 0, 1e9);
    double hi = readDouble("Maximum price (PHP): ", 0, 1e9);
    if (lo > hi) { double t = lo; lo = hi; hi = t; cout << "  (min/max swapped)\n"; }

    // Binary search needs sorted data: sort a copy by price with Quick Sort first.
    Product** sorted = copyPointers(inv.catalog);
    quickSort(sorted, 0, n - 1, priceAsc);
    int first = lowerBound(sorted, n, lo);
    int last = upperBound(sorted, n, hi);     // exclusive

    if (first >= last) cout << "  No products between PHP " << fixed << setprecision(2) << lo
                            << " and PHP " << hi << ".\n";
    else {
        printHeader();
        for (int i = first; i < last; i++) printRow(*sorted[i]);
        printLine();
        cout << (last - first) << " product(s) in range (found with two binary searches, O(log n)).\n";
    }
    delete[] sorted;
}

void brandShowcase(const Inventory& inv) {
    BrandRing ring;
    for (int i = 0; i < inv.catalog.size(); i++)
        ring.add(inv.catalog.at(i)->brand, taglineFor(inv.catalog.at(i)->brand));
    if (ring.size() == 0) { cout << "  No brands to showcase.\n"; return; }

    cout << "\n--- PARTNER BRAND SHOWCASE (circular list, " << ring.size() << " brands) ---\n";
    while (true) {
        int count = 0;
        for (int i = 0; i < inv.catalog.size(); i++)
            if (toLower(inv.catalog.at(i)->brand) == toLower(ring.brand())) count++;
        cout << "\n  ********************************************************\n"
             << "   FEATURED PARTNER: " << ring.brand() << "  (" << count << " products)\n"
             << "   " << ring.tagline() << "\n"
             << "  ********************************************************\n";
        ring.advance();
        if (ring.atStart()) cout << "  (cycled through every brand - wrapping back to the start)\n";
        string cmd = toLower(readLine("Press Enter for the next banner, or type q to stop: "));
        if (cmd == "q") break;
    }
}

void sortMenu(const Inventory& inv) {
    int n = inv.catalog.size();
    if (n == 0) { cout << "  The catalog is empty.\n"; return; }
    cout << "\n--- SORT INVENTORY ---\n  1. By price (Quick Sort)\n  2. By stock (Insertion Sort)\n"
         << "  3. By brand (Insertion Sort, ties by name)\n  0. Back\n";
    int c = readInt("Choice: ", 0, 3);
    if (c == 0) return;
    int dir = readInt("  1 = ascending, 2 = descending: ", 1, 2);

    Product** arr = copyPointers(inv.catalog);   // sort a copy; catalog order is untouched
    if (c == 1) {
        quickSort(arr, 0, n - 1, dir == 1 ? priceAsc : priceDesc);
        cout << "\nSorted by price (" << (dir == 1 ? "low to high" : "high to low") << ") using Quick Sort:\n";
    } else if (c == 2) {
        insertionSort(arr, n, dir == 1 ? stockAsc : stockDesc);
        cout << "\nSorted by stock (" << (dir == 1 ? "low to high" : "high to low") << ") using Insertion Sort:\n";
    } else {
        insertionSort(arr, n, dir == 1 ? brandAsc : brandDesc);
        cout << "\nSorted by brand (" << (dir == 1 ? "A to Z" : "Z to A") << ") using Insertion Sort:\n";
    }
    printHeader();
    for (int i = 0; i < n; i++) printRow(*arr[i]);
    printLine();
    delete[] arr;
}
