#include <iostream>
#include <vector>
#include "AppContext.h"
#include "SeedData.h"

void AppContext::loadFromDatabase() {
    vector<Product> products = db.loadProducts();
    if (products.empty()) {
        loadSeedData(inventory);
        vector<Product> seeded;
        for (int i = 0; i < inventory.catalog.size(); i++) seeded.push_back(*inventory.catalog.at(i));
        db.insertProducts(seeded);
        cout << "First run: stored " << seeded.size() << " starting products in the database.\n";
    } else {
        for (size_t i = 0; i < products.size(); i++) {
            Product* p = new Product(products[i]);
            if (!inventory.addProduct(p)) delete p;
        }
        cout << "Loaded " << products.size() << " products from the database.\n";
    }

    // The cart and wishlist store only product ids; names and prices come from the
    // catalog we just loaded, so they are always current.
    vector<pair<int, int> > cartRows = db.loadCart();
    for (size_t i = 0; i < cartRows.size(); i++) {
        Product* p = inventory.find(cartRows[i].first);
        if (p) cart.addItem(*p, cartRows[i].second);
    }
    vector<int> wishRows = db.loadWishlist();
    for (size_t i = 0; i < wishRows.size(); i++) {
        Product* p = inventory.find(wishRows[i]);
        if (p) wishlist.add(p->id, p->name);
    }

    vector<Order> pending = db.loadPendingOrders();
    for (size_t i = 0; i < pending.size(); i++) orders.enqueue(pending[i]);

    nextOrderId = db.nextOrderId();

    cout << "Restored " << cart.size() << " cart line(s), " << pending.size()
         << " pending order(s).\n";
}
