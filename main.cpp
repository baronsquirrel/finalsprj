// ============================================================================
//  BRAND-AFFILIATED E-COMMERCE INVENTORY & SEARCH SYSTEM (CS201)
//  Data Structure and Algorithm Analysis - Final Output
//  Domain: Tech & Electronics | Partner brands: Apple, Samsung, Sony, Lenovo
//
//  This file only holds the main menu loop. Each lab lives in its own file (headers in include/, sources in src/):
//    Lab 1  Dynamic Allocation   -> Catalog.h      (resizable array)
//    Lab 2  Singly/Doubly Lists  -> Cart.h, Wishlist.h
//    Lab 3  Circular List        -> BrandRing.h
//    Lab 4  Stack (LIFO)         -> ActionStack.h, AdminActions.cpp
//    Lab 5  Queue (FIFO)         -> OrderQueue.h
//    Lab 6  BST                  -> ProductBST.h
//    Lab 7  Searching            -> Searching.cpp
//    Lab 8  Sorting              -> Sorting.cpp
//
//  Build:  run `make` in the project folder (see README.md for the g++ command)
// ============================================================================

#include <iostream>
#include "Inventory.h"
#include "Cart.h"
#include "Wishlist.h"
#include "OrderQueue.h"
#include "ActionStack.h"
#include "Utils.h"
#include "SeedData.h"
#include "CatalogViews.h"
#include "ShoppingMenus.h"
#include "AdminMenu.h"

void printMenu() {
    cout << "\n";
    printLine('=');
    cout << " BRAND-AFFILIATED E-COMMERCE INVENTORY & SEARCH SYSTEM (CS201)\n";
    printLine('=');
    cout << "1. View Complete Catalog (Tabular View with Brand Affiliations)\n"
         << "2. Filter Products by Official Brand (e.g., Apple, Samsung, Sony)\n"
         << "3. Search Products by Keyword (Linear Search) / ID (BST)\n"
         << "4. Filter Products by Price Range (Binary Search)\n"
         << "5. Add Item to Shopping Cart (Doubly Linked List Navigation)\n"
         << "6. Process Customer Checkout Queue (FIFO Order Queue)\n"
         << "7. Perform Admin Undo Action (LIFO Stack Pop)\n"
         << "8. Display Partner Brand Banner Showcase (Circular List Cycle)\n"
         << "9. Sort Inventory Display (By Price, Stock, or Brand)\n"
         << "10. Exit\n";
    printLine('=');
}

void run() {
    Inventory inventory;
    Cart cart;
    Wishlist wishlist;
    OrderQueue orders;
    ActionStack undoStack, redoStack;
    int nextOrderId = 5001;

    loadSeedData(inventory);

    while (true) {
        printMenu();
        int choice = readInt("Enter choice (1-10): ", 1, 10);
        switch (choice) {
            case 1:  showCatalog(inventory); break;
            case 2:  filterByBrand(inventory); break;
            case 3:  searchMenu(inventory); break;
            case 4:  priceRangeFilter(inventory); break;
            case 5:  cartMenu(inventory, cart, wishlist); break;
            case 6:  checkoutMenu(inventory, cart, orders, nextOrderId); break;
            case 7:  adminMenu(inventory, undoStack, redoStack); break;
            case 8:  brandShowcase(inventory); break;
            case 9:  sortMenu(inventory); break;
            case 10:
                cout << "\nThank you for using the system. Freeing all memory... Goodbye!\n";
                return;     // every container's destructor releases its nodes
        }
    }
}

int main() {
    try {
        run();
    } catch (const InputEnded&) {
        cout << "\nInput closed. Exiting...\n";
    }
    return 0;
}
