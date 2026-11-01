#pragma once
// Browsing features: view, filter, search, price range, brand showcase, sort.
#include "Inventory.h"
#include "BrandTaglines.h"

void showCatalog(const Inventory& inv);        // menu 1
void filterByBrand(const Inventory& inv);      // menu 2
void searchMenu(const Inventory& inv);         // menu 3  (linear search / BST)
void priceRangeFilter(const Inventory& inv);   // menu 4  (binary search)
void brandShowcase(const Inventory& inv, const BrandTaglines& taglines);   // menu 8  (circular list)
void sortMenu(const Inventory& inv);           // menu 9  (insertion / quick sort)
