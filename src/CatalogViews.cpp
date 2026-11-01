#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>
#include <vector>
#include "CatalogViews.h"
#include "Utils.h"
#include "Pager.h"
#include "Sorting.h"
#include "Searching.h"
#include "BrandRing.h"

// Every product table below is shown through showProductPages(), which splits long
// lists into pages of PRODUCTS_PER_PAGE rows (see Pager.h).

void showCatalog(const Inventory& inv) {
    Product** items = copyPointers(inv.catalog);
    ostringstream heading;
    heading << "--- COMPLETE CATALOG (" << inv.catalog.size() << " products, array capacity "
            << inv.catalog.cap() << ") ---";
    showProductPages(items, inv.catalog.size(), heading.str(), "", "  The catalog is empty.");
    delete[] items;
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
    if (brands.empty()) { cout << "  The catalog is empty.\n"; pauseForEnter(); return; }

    // Alphabetical (ignoring case) so a long brand list is easy to scan. Insertion sort, as in Lab 8.
    for (size_t i = 1; i < brands.size(); i++) {
        string key = brands[i];
        size_t j = i;
        while (j > 0 && toLower(key) < toLower(brands[j - 1])) { brands[j] = brands[j - 1]; j--; }
        brands[j] = key;
    }

    // Paged pick-list (16 per page) with a way out: 0 goes back to the main menu.
    int pick = pickFromList(brands, "--- OFFICIAL PARTNER BRANDS ---");
    if (pick < 0) return;
    const string chosen = brands[pick];

    Product** items = new Product*[inv.catalog.size()];
    int n = 0;
    for (int i = 0; i < inv.catalog.size(); i++)
        if (toLower(inv.catalog.at(i)->brand) == toLower(chosen)) items[n++] = inv.catalog.at(i);

    showProductPages(items, n, "--- " + chosen + " PRODUCTS ---",
                     to_string(n) + " product(s) from " + chosen + ".");
    delete[] items;
}

void searchMenu(const Inventory& inv) {
    clearScreen();
    cout << "--- SEARCH ---\n  1. Keyword (Linear Search)\n  2. Product ID (BST Search)\n  0. Back\n";
    int c = readInt("Choice: ", 0, 2);
    if (c == 1) {
        string key;
        if (!readTextOrCancel("Keyword (name / brand / category)", key)) return;
        Product** res = new Product*[inv.catalog.size() > 0 ? inv.catalog.size() : 1];
        int found = linearKeywordSearch(inv.catalog, key, res);
        ostringstream footer;
        footer << "Linear search scanned " << inv.catalog.size() << " records (O(n)), "
               << found << " match(es).";
        showProductPages(res, found, "--- KEYWORD SEARCH: '" + key + "' ---", footer.str(),
                         "  No products match '" + key + "'.");
        delete[] res;
    } else if (c == 2) {
        int id = 0;
        if (!readIntOrCancel("Product ID", 1, 2000000000, id)) return;
        int steps = 0;
        Product* p = inv.find(id, &steps);
        Product* one[1];
        int found = 0;
        if (p) one[found++] = p;
        ostringstream footer;
        footer << "BST visited " << steps << " node(s); tree height " << inv.bst.height()
               << " for " << inv.catalog.size() << " products (O(log n)).";
        showProductPages(one, found, "--- PRODUCT ID SEARCH: " + to_string(id) + " ---", footer.str(),
                         "  No product with ID " + to_string(id) + ".");
    }
}

void priceRangeFilter(const Inventory& inv) {
    int n = inv.catalog.size();
    if (n == 0) { cout << "  The catalog is empty.\n"; pauseForEnter(); return; }
    clearScreen();
    cout << "--- PRICE RANGE FILTER ---\n";
    double lo = 0, hi = 0;
    if (!readDoubleOrCancel("Minimum price (PHP)", 0, 1e9, lo) ||
        !readDoubleOrCancel("Maximum price (PHP)", 0, 1e9, hi))
        return;                                   // cancelled
    bool swapped = false;
    if (lo > hi) { double t = lo; lo = hi; hi = t; swapped = true; }

    // Binary search needs sorted data: sort a copy by price with Quick Sort first.
    Product** sorted = copyPointers(inv.catalog);
    quickSort(sorted, 0, n - 1, priceAsc);
    int first = lowerBound(sorted, n, lo);
    int last = upperBound(sorted, n, hi);     // exclusive
    int found = last > first ? last - first : 0;

    ostringstream range;
    range << fixed << setprecision(2) << "PHP " << lo << " and PHP " << hi;
    string heading = "--- PRODUCTS BETWEEN " + range.str() + " ---";
    if (swapped) heading += "\n  (minimum and maximum were swapped)";
    showProductPages(sorted + first, found, heading,
                     to_string(found) + " product(s) in range (found with two binary searches, O(log n)).",
                     "  No products between " + range.str() + ".");
    delete[] sorted;
}

void brandShowcase(const Inventory& inv, const BrandTaglines& taglines) {
    BrandRing ring;
    for (int i = 0; i < inv.catalog.size(); i++)
        ring.add(inv.catalog.at(i)->brand, taglines.lookup(inv.catalog.at(i)->brand));
    if (ring.size() == 0) { cout << "  No brands to showcase.\n"; pauseForEnter(); return; }

    while (true) {
        clearScreen();
        cout << "--- PARTNER BRAND SHOWCASE (circular list, " << ring.size() << " brands) ---\n";
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
    if (n == 0) { cout << "  The catalog is empty.\n"; pauseForEnter(); return; }
    clearScreen();
    cout << "--- SORT INVENTORY ---\n  1. By price (Quick Sort)\n  2. By stock (Insertion Sort)\n"
         << "  3. By brand (Insertion Sort, ties by name)\n  0. Back\n";
    int c = readInt("Choice: ", 0, 3);
    if (c == 0) return;
    int dir = 0;
    if (!readIntOrCancel("  1 = ascending, 2 = descending", 1, 2, dir)) return;

    Product** arr = copyPointers(inv.catalog);   // sort a copy; catalog order is untouched
    string heading;
    if (c == 1) {
        quickSort(arr, 0, n - 1, dir == 1 ? priceAsc : priceDesc);
        heading = string("Sorted by price (") + (dir == 1 ? "low to high" : "high to low") + ") using Quick Sort:";
    } else if (c == 2) {
        insertionSort(arr, n, dir == 1 ? stockAsc : stockDesc);
        heading = string("Sorted by stock (") + (dir == 1 ? "low to high" : "high to low") + ") using Insertion Sort:";
    } else {
        insertionSort(arr, n, dir == 1 ? brandAsc : brandDesc);
        heading = string("Sorted by brand (") + (dir == 1 ? "A to Z" : "Z to A") + ") using Insertion Sort:";
    }
    showProductPages(arr, n, heading);
    delete[] arr;
}
