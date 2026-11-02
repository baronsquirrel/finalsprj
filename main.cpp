#include <cstdlib>
#include <iostream>
#include <memory>
#include <string>
#include "AppContext.h"
#include "Utils.h"
#include "CatalogViews.h"
#include "ShoppingMenus.h"
#include "AdminMenu.h"

void printMenu() {
    clearScreen();
    printLine('=');
    cout << " BRAND-AFFILIATED E-COMMERCE INVENTORY & SEARCH SYSTEM\n";
    printLine('=');
    cout << "1. View Complete Catalog (Tabular View with Brand Affiliations)\n"
         << "2. Filter Products by Official Brand (Partner Brands in the Catalog)\n"
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

// libpq reads password from the PGPASSWORD environment variable (or pgpass.conf).
// Set ECOMMERCE_DB_CONN to point the program at a different database.
string connectionString() {
    const char* env = getenv("ECOMMERCE_DB_CONN");
    if (env && *env) return env;
    return "host=localhost port=5432 dbname=ecommerce_db user=ecommerce_app";
}

void run() {
    cout << "Connecting to PostgreSQL...\n";
    unique_ptr<AppContext> ctx(new AppContext(connectionString()));   // throws DbConnectError if the database is down
    ctx->loadFromDatabase();
    pauseForEnter("Press Enter to open the main menu...");

    while (true) {
        printMenu();
        int choice = readInt("Enter choice (1-10): ", 1, 10);
        switch (choice) {
            case 1:  showCatalog(ctx->inventory); break;
            case 2:  filterByBrand(ctx->inventory); break;
            case 3:  searchMenu(ctx->inventory); break;
            case 4:  priceRangeFilter(ctx->inventory); break;
            case 5:  cartMenu(*ctx); break;
            case 6:  checkoutMenu(*ctx); break;
            case 7:
                if (adminMenu(*ctx)) {                 // the admin chose "Reload all data"
                    ctx.reset();                       // closes the old connection and frees every structure
                    ctx.reset(new AppContext(connectionString()));
                    ctx->loadFromDatabase();           // picks up anything changed in the database
                    pauseForEnter("Reload complete. Press Enter to open the main menu...");
                }
                break;
            case 8:  brandShowcase(ctx->inventory, ctx->taglines); break;
            case 9:  sortMenu(ctx->inventory); break;
            case 10:
                clearScreen();
                cout << "Thank you for using the system. Freeing all memory... Goodbye!\n";
                return;     // every container's destructor releases its nodes
        }
    }
}

int main() {
    setupConsole();
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
