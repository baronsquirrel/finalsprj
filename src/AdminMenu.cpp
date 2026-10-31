#include <iostream>
#include <string>
#include "AdminMenu.h"
#include "AdminActions.h"
#include "Utils.h"

void adminMenu(Inventory& inv, ActionStack& undoStack, ActionStack& redoStack) {
    while (true) {
        cout << "\n--- ADMIN PANEL (undo: " << undoStack.size() << ", redo: " << redoStack.size() << ") ---\n"
             << "  1. Add new product\n  2. Update stock quantity\n  3. Remove product\n"
             << "  4. Undo last action (stack pop)\n  5. Redo last undone action\n  0. Back\n";
        int c = readInt("Choice: ", 0, 5);
        if (c == 0) return;

        if (c == 1) {
            Product* p = new Product{inv.nextId, "", "", "", 0, 0};
            p->name = readNonEmpty("Product name: ");
            p->category = readNonEmpty("Category: ");
            p->brand = readNonEmpty("Official partner brand: ");
            p->price = readDouble("Price (PHP): ", 0.01, 1e9);
            p->stockQuantity = readInt("Stock quantity: ", 0, 1000000);
            AdminAction a;
            a.type = ACT_ADD;
            a.after = *p;
            if (!inv.addProduct(p)) { delete p; cout << "  ID collision, product not added.\n"; continue; }
            undoStack.push(a);
            redoStack.clear();
            cout << "  Added '" << a.after.name << "' with ID " << a.after.id << ".\n";
        } else if (c == 2) {
            int id = readInt("Product ID: ", 1, 2000000000);
            Product* p = inv.find(id);
            if (!p) { cout << "  No product with ID " << id << ".\n"; continue; }
            cout << "  '" << p->name << "' currently has " << p->stockQuantity << " in stock.\n";
            int q = readInt("New stock quantity: ", 0, 1000000);
            AdminAction a;
            a.type = ACT_STOCK;
            a.before = *p;
            p->stockQuantity = q;
            a.after = *p;
            undoStack.push(a);
            redoStack.clear();
            cout << "  Stock for '" << p->name << "' set to " << q << ".\n";
        } else if (c == 3) {
            int id = readInt("Product ID: ", 1, 2000000000);
            Product* p = inv.find(id);
            if (!p) { cout << "  No product with ID " << id << ".\n"; continue; }
            string confirm = toLower(readLine("Remove '" + p->name + "'? (y/n): "));
            if (confirm != "y") { cout << "  Cancelled.\n"; continue; }
            AdminAction a;
            a.type = ACT_REMOVE;
            a.before = *p;
            Product* gone = inv.detach(id);
            delete gone;
            undoStack.push(a);
            redoStack.clear();
            cout << "  Removed '" << a.before.name << "'.\n";
        } else if (c == 4) {
            AdminAction a;
            if (!undoStack.pop(a)) { cout << "  Nothing to undo.\n"; continue; }
            if (applyUndo(inv, a)) redoStack.push(a);
            else cout << "  Could not undo (the product state has changed).\n";
        } else if (c == 5) {
            AdminAction a;
            if (!redoStack.pop(a)) { cout << "  Nothing to redo.\n"; continue; }
            if (applyRedo(inv, a)) undoStack.push(a);
            else cout << "  Could not redo (the product state has changed).\n";
        }
    }
}
