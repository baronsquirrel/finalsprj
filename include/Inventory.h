#pragma once
#include "Catalog.h"
#include "ProductBST.h"

// ============================================================================
//  Inventory = Catalog (Lab 1) + BST index (Lab 6), kept in sync
// ============================================================================
class Inventory {
public:
    Catalog catalog;
    ProductBST bst;
    int nextId;

    Inventory() : nextId(1001) {}

    // Takes ownership of p. Returns false (caller must delete p) on duplicate ID.
    bool addProduct(Product* p) {
        if (bst.find(p->id)) return false;
        catalog.add(p);
        bst.insert(p);
        bst.rebuildBalanced();
        if (p->id >= nextId) nextId = p->id + 1;
        return true;
    }

    // Unlinks product from catalog + BST and hands ownership to the caller.
    Product* detach(int id) {
        Product* p = bst.find(id);
        if (!p) return nullptr;
        bst.remove(id);
        catalog.detach(p);
        return p;
    }

    Product* find(int id, int* steps = nullptr) const { return bst.find(id, steps); }
};
