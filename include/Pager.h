#pragma once
// Paged product tables: at most PRODUCTS_PER_PAGE rows per screen, with
// next / previous / go-to-page navigation. Every product list in the program
// goes through this one function, so the page size is changed in one place.
#include <string>
#include "Product.h"

constexpr int PRODUCTS_PER_PAGE = 16;

// Shows items[0..count) one page at a time. For every page it clears the screen,
// prints `heading`, the table, a "Page X of Y" line, then `footer` (if any).
// With a single page it simply waits for Enter, exactly like the other screens.
// The Products are not copied or modified; the caller still owns the items array.
void showProductPages(Product** items, int count, const string& heading,
                      const string& footer = "",
                      const string& emptyMessage = "  No products to show.");
