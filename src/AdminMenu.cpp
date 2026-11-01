#include <iostream>
#include <string>
#include "AdminMenu.h"
#include "AdminActions.h"
#include "Utils.h"

bool adminMenu(AppContext& ctx) {
    while (true) {
        clearScreen();
        cout << "--- ADMIN PANEL (undo: " << ctx.undoStack.size() << ", redo: " << ctx.redoStack.size() << ") ---\n"
             << "  1. Add new product\n  2. Update stock quantity\n  3. Remove product\n"
             << "  4. Undo last action (stack pop)\n  5. Redo last undone action\n  6. Reload all data from the database\n  0. Back\n";
        int c = readInt("Choice: ", 0, 6);
        if (c == 0) return false;

        if (c == 1) {
            Product prod{ctx.inventory.nextId, "", "", "", 0, 0};
            prod.name = readNonEmpty("Product name: ");
            prod.category = readNonEmpty("Category: ");
            prod.brand = readNonEmpty("Official partner brand: ");
            prod.price = readDouble("Price (PHP): ", 0.01, 1e9);
            prod.stockQuantity = readInt("Stock quantity: ", 0, 1000000);
            if (!addProductEverywhere(ctx, prod)) { cout << "  ID collision, product not added.\n"; pauseForEnter(); continue; }
            AdminAction a;
            a.type = ACT_ADD;
            a.after = prod;
            ctx.undoStack.push(a);
            ctx.redoStack.clear();
            cout << "  Added '" << prod.name << "' with ID " << prod.id << ".\n";
        } else if (c == 2) {
            int id = readInt("Product ID: ", 1, 2000000000);
            Product* p = ctx.inventory.find(id);
            if (!p) { cout << "  No product with ID " << id << ".\n"; pauseForEnter(); continue; }
            cout << "  '" << p->name << "' currently has " << p->stockQuantity << " in stock.\n";
            int q = readInt("New stock quantity: ", 0, 1000000);
            AdminAction a;
            a.type = ACT_STOCK;
            a.before = *p;
            setStockEverywhere(ctx, id, q);
            a.after = *p;
            ctx.undoStack.push(a);
            ctx.redoStack.clear();
            cout << "  Stock for '" << p->name << "' set to " << q << ".\n";
        } else if (c == 3) {
            int id = readInt("Product ID: ", 1, 2000000000);
            Product* p = ctx.inventory.find(id);
            if (!p) { cout << "  No product with ID " << id << ".\n"; pauseForEnter(); continue; }
            string confirm = toLower(readLine("Remove '" + p->name + "'? (y/n): "));
            if (confirm != "y") { cout << "  Cancelled.\n"; pauseForEnter(); continue; }
            AdminAction a;
            a.type = ACT_REMOVE;
            a.before = *p;
            removeProductEverywhere(ctx, id);
            ctx.undoStack.push(a);
            ctx.redoStack.clear();
            cout << "  Removed '" << a.before.name << "'.\n";
        } else if (c == 4) {
            AdminAction a;
            if (!ctx.undoStack.pop(a)) { cout << "  Nothing to undo.\n"; pauseForEnter(); continue; }
            if (applyUndo(ctx, a)) ctx.redoStack.push(a);
            else cout << "  Could not undo (the product state has changed).\n";
        } else if (c == 5) {
            AdminAction a;
            if (!ctx.redoStack.pop(a)) { cout << "  Nothing to redo.\n"; pauseForEnter(); continue; }
            if (applyRedo(ctx, a)) ctx.undoStack.push(a);
            else cout << "  Could not redo (the product state has changed).\n";
        } else if (c == 6) {
            cout << "  Reloading from the database (undo/redo history will be cleared)...\n";
            return true;
        }
        pauseForEnter();     // every action ends with a pause so its message can be read
    }
}
