#pragma once
// Everything the running program owns, in one place, so menus receive a single
// parameter instead of five. The database is declared FIRST: if PostgreSQL cannot
// be reached, the constructor throws before anything else is built.
#include <string>
#include "Database.h"
#include "Inventory.h"
#include "Cart.h"
#include "Wishlist.h"
#include "OrderQueue.h"
#include "ActionStack.h"
#include "BrandTaglines.h"

struct AppContext {
    Database db;
    Inventory inventory;       // Lab 1 catalog + Lab 6 BST
    Cart cart;                 // Lab 2 doubly linked list
    Wishlist wishlist;         // Lab 2 singly linked list
    OrderQueue orders;         // Lab 5 FIFO queue
    ActionStack undoStack;     // Lab 4 stacks
    ActionStack redoStack;
    BrandTaglines taglines;    // banner text for menu 8, from the 'brands' table
    int nextOrderId;

    explicit AppContext(const std::string& connInfo) : db(connInfo), nextOrderId(5001) {}

    // Fills the structures above from PostgreSQL. The program contains no product or brand
    // data of its own: whatever is in the database is what gets shown.
    void loadFromDatabase();
};
