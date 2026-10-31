#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include "ShoppingMenus.h"
#include "Utils.h"

void cartMenu(AppContext& ctx) {
    Inventory& inv = ctx.inventory;
    Cart& cart = ctx.cart;
    Wishlist& wish = ctx.wishlist;
    while (true) {
        cout << "\n--- SHOPPING CART (" << cart.size() << " line(s)) ---\n"
             << "  1. Add item to cart\n  2. View cart\n  3. Browse cart (next / previous)\n"
             << "  4. Remove highlighted item\n  5. Add item to wishlist\n  6. View wishlist\n"
             << "  7. Remove item from wishlist\n  0. Back\n";
        int c = readInt("Choice: ", 0, 7);
        if (c == 0) return;

        if (c == 1) {
            int id = readInt("Product ID: ", 1, 2000000000);
            Product* p = inv.find(id);
            if (!p) { cout << "  No product with ID " << id << ".\n"; continue; }
            if (p->stockQuantity == 0) { cout << "  Sorry, '" << p->name << "' is out of stock.\n"; continue; }
            int qty = readInt("Quantity (1-" + to_string(p->stockQuantity) + "): ", 1, p->stockQuantity);
            cart.addItem(*p, qty);
            ctx.db.setCartItem(p->id, cart.current()->qty);   // qty after merging duplicates
            cout << "  Added " << qty << " x " << p->name << " to your cart.\n";
        } else if (c == 2) {
            cart.display();
        } else if (c == 3) {
            if (cart.size() == 0) { cout << "  (cart is empty)\n"; continue; }
            while (true) {
                cart.display();
                string cmd = toLower(readLine("[n]ext / [p]revious / [b]ack: "));
                if (cmd == "n") { if (!cart.moveNext()) cout << "  Already at the last item.\n"; }
                else if (cmd == "p") { if (!cart.movePrev()) cout << "  Already at the first item.\n"; }
                else if (cmd == "b") break;
                else cout << "  Enter n, p or b.\n";
            }
        } else if (c == 4) {
            const CartItem* it = cart.current();
            if (!it) { cout << "  (cart is empty)\n"; continue; }
            string nm = it->name;
            int pid = it->productId;
            cart.removeCurrent();
            ctx.db.removeCartItem(pid);
            cout << "  Removed '" << nm << "' from the cart.\n";
        } else if (c == 5) {
            int id = readInt("Product ID: ", 1, 2000000000);
            Product* p = inv.find(id);
            if (!p) { cout << "  No product with ID " << id << ".\n"; continue; }
            bool added = wish.add(p->id, p->name);
            if (added) ctx.db.addWishlistItem(p->id);
            cout << (added ? "  Added to wishlist.\n" : "  Already on your wishlist.\n");
        } else if (c == 6) {
            wish.display();
        } else if (c == 7) {
            int id = readInt("Product ID to remove: ", 1, 2000000000);
            bool removed = wish.remove(id);
            if (removed) ctx.db.removeWishlistItem(id);
            cout << (removed ? "  Removed from wishlist.\n" : "  Not on your wishlist.\n");
        }
    }
}

void processNextOrder(AppContext& ctx) {
    Inventory& inv = ctx.inventory;
    Order o;
    if (!ctx.orders.dequeue(o)) { cout << "  No pending orders.\n"; return; }
    cout << "\nProcessing Order " << o.orderId << " for " << o.customer << "...\n";

    // Pass 1: validate every line before touching stock (all-or-nothing)
    bool ok = true;
    for (size_t i = 0; i < o.items.size(); i++) {
        Product* p = inv.find(o.items[i].productId);
        if (!p) {
            cout << "  FAILED: '" << o.items[i].name << "' is no longer in the catalog.\n";
            ok = false;
        } else if (p->stockQuantity < o.items[i].qty) {
            cout << "  FAILED: only " << p->stockQuantity << " of '" << p->name << "' left (ordered "
                 << o.items[i].qty << ").\n";
            ok = false;
        }
    }
    if (!ok) {
        ctx.db.cancelOrder(o.orderId);
        cout << "  Order " << o.orderId << " was cancelled. No stock was changed.\n";
        return;
    }

    // Pass 2: ONE database transaction deducts every line and marks the order completed
    // (all or nothing). The database re-checks the stock itself.
    if (!ctx.db.completeOrder(o)) {
        ctx.db.cancelOrder(o.orderId);
        cout << "  FAILED: the database reports not enough stock. Order " << o.orderId
             << " was cancelled. No stock was changed.\n";
        return;
    }
    // Mirror the committed change in memory
    for (size_t i = 0; i < o.items.size(); i++) {
        Product* p = inv.find(o.items[i].productId);
        p->stockQuantity -= o.items[i].qty;
    }
    cout << "  Order " << o.orderId << " completed. Total charged: PHP " << fixed << setprecision(2)
         << o.total << ". Stock updated.\n";
}

void checkoutMenu(AppContext& ctx) {
    Cart& cart = ctx.cart;
    OrderQueue& queue = ctx.orders;
    while (true) {
        cout << "\n--- CHECKOUT QUEUE (" << queue.size() << " pending) ---\n"
             << "  1. Place order from cart (enqueue)\n  2. Process next order (dequeue)\n"
             << "  3. View pending orders\n  0. Back\n";
        int c = readInt("Choice: ", 0, 3);
        if (c == 0) return;
        if (c == 1) {
            if (cart.size() == 0) { cout << "  Your cart is empty. Add items first.\n"; continue; }
            Order o;
            o.orderId = ctx.nextOrderId++;
            string name = readLine("Customer name: ");
            o.customer = name.empty() ? "Guest" : name;
            cart.copyItems(o.items);
            o.total = cart.total();
            ctx.db.placeOrder(o);      // one transaction: save the order and empty the cart rows
            queue.enqueue(o);
            cart.clear();
            cout << "  Order " << o.orderId << " queued (position " << queue.size() << ").\n";
        } else if (c == 2) {
            processNextOrder(ctx);
        } else if (c == 3) {
            queue.display();
        }
    }
}
