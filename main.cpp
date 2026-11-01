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
//    Storage                     -> Database.h/.cpp (PostgreSQL), AppContext.h/.cpp
//    Paging                      -> Pager.h/.cpp (16 products per screen)
//
//  Build:  run `make` in the project folder (see README.md for the g++ command)
// ============================================================================

#include <cstdlib>
#include <iostream>
#include <string>
#include "AppContext.h"
#include "Utils.h"
#include "CatalogViews.h"
#include "ShoppingMenus.h"
#include "AdminMenu.h"

void printMenu() {
    clearScreen();
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

// Connection settings. The password is never stored in the code or the repository:
// libpq reads it from the PGPASSWORD environment variable (or pgpass.conf).
// Set ECOMMERCE_DB_CONN to point the program at a different database.
string connectionString() {
    const char* env = getenv("ECOMMERCE_DB_CONN");
    if (env && *env) return env;
    return "host=localhost port=5432 dbname=ecommerce_db user=ecommerce_app";
}

void run() {
    cout << "Connecting to PostgreSQL...\n";
    AppContext ctx(connectionString());      // throws DbConnectError if the database is down
    ctx.loadFromDatabase();
    pauseForEnter("Press Enter to open the main menu...");

    while (true) {
        printMenu();
        int choice = readInt("Enter choice (1-10): ", 1, 10);
        switch (choice) {
            case 1:  showCatalog(ctx.inventory); break;
            case 2:  filterByBrand(ctx.inventory); break;
            case 3:  searchMenu(ctx.inventory); break;
            case 4:  priceRangeFilter(ctx.inventory); break;
            case 5:  cartMenu(ctx); break;
            case 6:  checkoutMenu(ctx); break;
            case 7:  adminMenu(ctx); break;
            case 8:  brandShowcase(ctx.inventory); break;
            case 9:  sortMenu(ctx.inventory); break;
            case 10:
                clearScreen();
                cout << "Thank you for using the system. Freeing all memory... Goodbye!\n";
                return;     // every container's destructor releases its nodes
        }
    }
}

int main() {
    try {
        run();
    } catch (const InputEnded&) {
        cout << "\nInput closed. Exiting...\n";
    } catch (const DbConnectError& e) {
        cerr << "\nERROR: could not connect to PostgreSQL.\n" << e.what()
             << "\nCheck that the PostgreSQL service is running and that PGPASSWORD is set."
             << "\nThe system cannot run without its database. Exiting.\n";
        return 1;
    } catch (const exception& e) {
        cerr << "\nDATABASE ERROR: " << e.what()
             << "\nThe program cannot continue and will now exit.\n";
        return 1;
    }
    return 0;
}
