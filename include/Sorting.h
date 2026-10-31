#pragma once
// LAB 8 - SORTING (Insertion Sort + Quick Sort) on arrays of Product*
#include "Product.h"
#include "Catalog.h"

typedef bool (*LessFn)(const Product*, const Product*);

// Comparators (return true when a should come before b)
bool priceAsc(const Product* a, const Product* b);
bool priceDesc(const Product* a, const Product* b);
bool stockAsc(const Product* a, const Product* b);
bool stockDesc(const Product* a, const Product* b);
bool brandAsc(const Product* a, const Product* b);
bool brandDesc(const Product* a, const Product* b);

void insertionSort(Product** a, int n, LessFn less);
void quickSort(Product** a, int lo, int hi, LessFn less);

// Shallow copy of the catalog's pointers so sorting never reorders the catalog itself.
// The caller must delete[] the returned array (the Products stay owned by the Catalog).
Product** copyPointers(const Catalog& c);
