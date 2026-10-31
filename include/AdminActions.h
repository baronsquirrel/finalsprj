#pragma once
// Catalog changes that must reach the database, the in-memory structures and the
// cart together, plus the undo / redo logic (Lab 4) built on top of them.
#include "AppContext.h"

// --- the only three ways the product catalog changes ------------------------
bool addProductEverywhere(AppContext& ctx, const Product& prod);     // database, catalog, BST
bool setStockEverywhere(AppContext& ctx, int id, int newStock);      // database and catalog
bool removeProductEverywhere(AppContext& ctx, int id);               // database, cart, wishlist, catalog, BST

void adjustStock(AppContext& ctx, int id, int delta);
bool applyUndo(AppContext& ctx, const AdminAction& a);   // reverses an action, prints result
bool applyRedo(AppContext& ctx, const AdminAction& a);   // re-applies an undone action
