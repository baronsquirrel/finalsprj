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
            if (!readTextOrCancel("Product name", prod.name) ||
                !readTextOrCancel("Category", prod.category) ||
                !readTextOrCancel("Official partner brand", prod.brand) ||
                !readDoubleOrCancel("Price (PHP)", 0.01, 1e9, prod.price) ||
                !readIntOrCancel("Stock quantity", 0, 1000000, prod.stockQuantity))
                continue;                         // cancelled: nothing was added
            if (!addProductEverywhere(ctx, prod)) { cout << "  ID collision, product not added.\n"; pauseForEnter(); continue; }
            AdminAction a;
            a.type = ACT_ADD;
            a.after = prod;
            ctx.undoStack.push(a);
            ctx.redoStack.clear();
            cout << "  Added '" << prod.name << "' with ID " << prod.id << ".\n";
        } else if (c == 2) {
            int id = 0;
            if (!readIntOrCancel("Product ID", 1, 2000000000, id)) continue;
            Product* p = ctx.inventory.find(id);
            if (!p) { cout << "  No product with ID " << id << ".\n"; pauseForEnter(); continue; }
            cout << "  '" << p->name << "' currently has " << p->stockQuantity << " in stock.\n";
            int q = 0;
            if (!readIntOrCancel("New stock quantity", 0, 1000000, q)) continue;
            AdminAction a;
            a.type = ACT_STOCK;
            a.before = *p;
            setStockEverywhere(ctx, id, q);
            a.after = *p;
            ctx.undoStack.push(a);
            ctx.redoStack.clear();
            cout << "  Stock for '" << p->name << "' set to " << q << ".\n";
        } else if (c == 3) {
            int id = 0;
            if (!readIntOrCancel("Product ID", 1, 2000000000, id)) continue;
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
