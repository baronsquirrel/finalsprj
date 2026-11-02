#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include "ShoppingMenus.h"
#include "Utils.h"
#include "Pager.h"
#include "Sorting.h"

// Shows the whole catalog (paged) and lets the user pick a product by its ID, so nobody has
// to remember IDs. Returns nullptr if the user cancels.
static Product* chooseProduct(const Inventory& inv, const string& heading) {
    Product** items = copyPointers(inv.catalog);
    Product* chosen = pickProductFromPages(items, inv.catalog.size(), heading, "  The catalog is empty.");
    delete[] items;
    return chosen;
}

// Browse the cart (doubly linked list: next / previous are both O(1)) and remove the highlighted line.
static void browseCart(AppContext& ctx) {
    Cart& cart = ctx.cart;
    string note;
    while (true) {
        clearScreen();
        cout << "--- BROWSE CART ---\n";
        if (cart.size() == 0) {
            cout << "  (cart is empty)\n";
            if (!note.empty()) cout << note << "\n";
            pauseForEnter();
            return;
        }
        cart.display();
        if (!note.empty()) { cout << note << "\n"; note.clear(); }

        string cmd = toLower(trim(readLine("\n[n] next   [p] previous   [r] remove highlighted   [b] back: ")));
        if (cmd.empty() || cmd == "b" || cmd == "back") return;
        if (cmd == "n") { if (!cart.moveNext()) note = "  Already at the last item."; }
        else if (cmd == "p") { if (!cart.movePrev()) note = "  Already at the first item."; }
        else if (cmd == "r") {
            const CartItem* it = cart.current();
            if (it) {
                string nm = it->name;
                int pid = it->productId;
                cart.removeCurrent();
                ctx.db.removeCartItem(pid);
                note = "  Removed '" + nm + "' from the cart.";
            }
        }
        else note = "  Enter n, p, r or b (or press Enter to go back).";
    }
}

// Browse the wishlist (singly linked list) and remove the highlighted item.
static void browseWishlist(AppContext& ctx) {
    Wishlist& wish = ctx.wishlist;
    wish.moveToFirst();
    string note;
    while (true) {
        clearScreen();
        cout << "--- BROWSE WISHLIST ---\n";
        if (wish.size() == 0) {
            cout << "  (wishlist is empty)\n";
            if (!note.empty()) cout << note << "\n";
            pauseForEnter();
            return;
        }
        wish.display(true);
        if (!note.empty()) { cout << note << "\n"; note.clear(); }

        string cmd = toLower(trim(readLine("\n[n] next   [p] previous   [r] remove highlighted   [b] back: ")));
        if (cmd.empty() || cmd == "b" || cmd == "back") return;
        if (cmd == "n") { if (!wish.moveNext()) note = "  Already at the last item."; }
        else if (cmd == "p") { if (!wish.movePrev()) note = "  Already at the first item."; }
        else if (cmd == "r") {
            int pid = 0;
            string nm;
            if (wish.current(pid, nm)) {
                wish.removeCurrent();
                ctx.db.removeWishlistItem(pid);
                note = "  Removed '" + nm + "' from the wishlist.";
            }
        }
        else note = "  Enter n, p, r or b (or press Enter to go back).";
    }
}

void cartMenu(AppContext& ctx) {
    Inventory& inv = ctx.inventory;
    Cart& cart = ctx.cart;
    Wishlist& wish = ctx.wishlist;
    while (true) {
        clearScreen();
        cout << "--- SHOPPING CART (" << cart.size() << " line(s), " << wish.size() << " on wishlist) ---\n"
             << "  1. Add item to cart (choose from the catalog)\n"
             << "  2. View cart\n"
             << "  3. Browse cart (next / previous / remove)\n"
             << "  4. Add item to wishlist (choose from the catalog)\n"
             << "  5. View wishlist\n"
             << "  6. Browse wishlist (next / previous / remove)\n"
             << "  0. Back\n";
        int c = readInt("Choice: ", 0, 6);
        if (c == 0) return;

        if (c == 1) {
            Product* p = chooseProduct(inv, "--- ADD TO CART: choose a product ---");
            if (!p) continue;                                   // cancelled
            if (p->stockQuantity == 0) { cout << "  Sorry, '" << p->name << "' is out of stock.\n"; pauseForEnter(); continue; }
            int qty = 0;
            if (!readIntOrCancel("Quantity (1-" + to_string(p->stockQuantity) + ")", 1, p->stockQuantity, qty)) continue;
            cart.addItem(*p, qty);
            ctx.db.setCartItem(p->id, cart.current()->qty);   // qty after merging duplicates
            cout << "  Added " << qty << " x " << p->name << " to your cart.\n";
        } else if (c == 2) {
            cart.display();
        } else if (c == 3) {
            browseCart(ctx);
            continue;                                           // no extra pause: the user just left the browser
        } else if (c == 4) {
            Product* p = chooseProduct(inv, "--- ADD TO WISHLIST: choose a product ---");
            if (!p) continue;                                   // cancelled
            bool added = wish.add(p->id, p->name);
            if (added) ctx.db.addWishlistItem(p->id);
            cout << (added ? "  Added '" + p->name + "' to your wishlist.\n" : "  '" + p->name + "' is already on your wishlist.\n");
        } else if (c == 5) {
            wish.display();
        } else if (c == 6) {
            browseWishlist(ctx);
            continue;
        }
        pauseForEnter();     // every action ends with a pause so its message can be read
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
        clearScreen();
        cout << "--- CHECKOUT QUEUE (" << queue.size() << " pending) ---\n"
             << "  1. Place order from cart (enqueue)\n  2. Process next order (dequeue)\n"
             << "  3. View pending orders\n  0. Back\n";
        int c = readInt("Choice: ", 0, 3);
        if (c == 0) return;
        if (c == 1) {
            if (cart.size() == 0) { cout << "  Your cart is empty. Add items first.\n"; pauseForEnter(); continue; }
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
        pauseForEnter();
    }
}
