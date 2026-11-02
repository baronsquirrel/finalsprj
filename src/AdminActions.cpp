#include <iostream>
#include "AdminActions.h"

// Every helper writes to PostgreSQL first. If the database call throws, nothing in
// memory has changed yet, so the two never disagree.

bool addProductEverywhere(AppContext& ctx, const Product& prod) {
    if (ctx.inventory.find(prod.id)) return false;
    ctx.db.insertProduct(prod);
    ctx.inventory.addProduct(new Product(prod));
    return true;
}

bool setStockEverywhere(AppContext& ctx, int id, int newStock) {
    Product* p = ctx.inventory.find(id);
    if (!p) return false;
    ctx.db.updateStock(id, newStock);
    p->stockQuantity = newStock;
    return true;
}

bool removeProductEverywhere(AppContext& ctx, int id) {
    if (!ctx.inventory.find(id)) return false;
    ctx.db.deleteProduct(id);            // its cart / wishlist rows cascade away in the database
    ctx.cart.removeProduct(id);          // keep the in-memory cart and wishlist in step
    ctx.wishlist.remove(id);
    delete ctx.inventory.detach(id);     // unlinks from the catalog array and the BST
    return true;
}

// Undo / redo helpers. Stock changes are stored as before/after values and applied as a
// delta, so units already sold at checkout are never "un-sold" by an undo.
void adjustStock(AppContext& ctx, int id, int delta) {
    Product* p = ctx.inventory.find(id);
    if (!p) return;
    int target = p->stockQuantity + delta;
    if (target < 0) {
        target = 0;
        cout << "  (note: stock would be negative, clamped to 0)\n";
    }
    setStockEverywhere(ctx, id, target);
}

bool applyUndo(AppContext& ctx, const AdminAction& a) {
    switch (a.type) {
        case ACT_ADD:
            if (!removeProductEverywhere(ctx, a.after.id)) return false;
            cout << "  Undid: add of '" << a.after.name << "' (removed from catalog).\n";
            return true;
        case ACT_REMOVE:
            if (!addProductEverywhere(ctx, a.before)) return false;
            cout << "  Undid: removal of '" << a.before.name << "' (restored to catalog).\n";
            return true;
        case ACT_STOCK:
            if (!ctx.inventory.find(a.before.id)) return false;
            adjustStock(ctx, a.before.id, a.before.stockQuantity - a.after.stockQuantity);
            cout << "  Undid: stock change on '" << a.before.name << "'.\n";
            return true;
    }
    return false;
}

bool applyRedo(AppContext& ctx, const AdminAction& a) {
    switch (a.type) {
        case ACT_ADD:
            if (!addProductEverywhere(ctx, a.after)) return false;
            cout << "  Redid: add of '" << a.after.name << "'.\n";
            return true;
        case ACT_REMOVE:
            if (!removeProductEverywhere(ctx, a.before.id)) return false;
            cout << "  Redid: removal of '" << a.before.name << "'.\n";
            return true;
        case ACT_STOCK:
            if (!ctx.inventory.find(a.before.id)) return false;
            adjustStock(ctx, a.before.id, a.after.stockQuantity - a.before.stockQuantity);
            cout << "  Redid: stock change on '" << a.before.name << "'.\n";
            return true;
    }
    return false;
}
