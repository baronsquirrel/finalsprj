#pragma once
// LAB 7 - SEARCHING
#include <string>
#include "Product.h"
#include "Catalog.h"

// Linear keyword search over name / brand / category. Fills results, returns match count.
int linearKeywordSearch(const Catalog& c, const string& key, Product** results);

// Binary searches on an array already sorted by price (ascending).
int lowerBound(Product** a, int n, double v);   // first index with price >= v
int upperBound(Product** a, int n, double v);   // first index with price >  v
