#include <iostream>
#include "AdminActions.h"

// delta, so units already sold at checkout are never "un-sold" by an undo.
void adjustStock(Inventory& inv, int id, int delta) {
    Product* p = inv.find(id);
    if (!p) return;
    p->stockQuantity += delta;
    if (p->stockQuantity < 0) {
        p->stockQuantity = 0;
        cout << "  (note: stock would be negative, clamped to 0)\n";
    }
}

bool applyUndo(Inventory& inv, const AdminAction& a) {
    switch (a.type) {
        case ACT_ADD: {
            Product* p = inv.detach(a.after.id);
            if (!p) return false;
            delete p;
            cout << "  Undid: add of '" << a.after.name << "' (removed from catalog).\n";
            return true;
        }
        case ACT_REMOVE: {
            Product* p = new Product(a.before);
            if (!inv.addProduct(p)) { delete p; return false; }
            cout << "  Undid: removal of '" << a.before.name << "' (restored to catalog).\n";
            return true;
        }
        case ACT_STOCK:
            if (!inv.find(a.before.id)) return false;
            adjustStock(inv, a.before.id, a.before.stockQuantity - a.after.stockQuantity);
            cout << "  Undid: stock change on '" << a.before.name << "'.\n";
            return true;
    }
    return false;
}

bool applyRedo(Inventory& inv, const AdminAction& a) {
    switch (a.type) {
        case ACT_ADD: {
            Product* p = new Product(a.after);
            if (!inv.addProduct(p)) { delete p; return false; }
            cout << "  Redid: add of '" << a.after.name << "'.\n";
            return true;
        }
        case ACT_REMOVE: {
            Product* p = inv.detach(a.before.id);
            if (!p) return false;
            delete p;
            cout << "  Redid: removal of '" << a.before.name << "'.\n";
            return true;
        }
        case ACT_STOCK:
            if (!inv.find(a.before.id)) return false;
            adjustStock(inv, a.before.id, a.after.stockQuantity - a.before.stockQuantity);
            cout << "  Redid: stock change on '" << a.before.name << "'.\n";
            return true;
    }
    return false;
}
