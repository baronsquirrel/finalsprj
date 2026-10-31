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

struct AppContext {
    Database db;
    Inventory inventory;       // Lab 1 catalog + Lab 6 BST
    Cart cart;                 // Lab 2 doubly linked list
    Wishlist wishlist;         // Lab 2 singly linked list
    OrderQueue orders;         // Lab 5 FIFO queue
    ActionStack undoStack;     // Lab 4 stacks
    ActionStack redoStack;
    int nextOrderId;

    explicit AppContext(const std::string& connInfo) : db(connInfo), nextOrderId(5001) {}

    // Fills the structures above from PostgreSQL. On the very first run the products
    // table is empty, so the 16 seed products are stored in the database instead.
    void loadFromDatabase();
};
