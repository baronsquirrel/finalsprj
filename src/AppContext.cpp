#include <iostream>
#include <vector>
#include "AppContext.h"

void AppContext::loadFromDatabase() {
    vector<Product> products = db.loadProducts();
    for (size_t i = 0; i < products.size(); i++) {
        Product* p = new Product(products[i]);
        if (!inventory.addProduct(p)) delete p;
    }

    vector<pair<string, string> > brandRows = db.loadBrandTaglines();
    for (size_t i = 0; i < brandRows.size(); i++) taglines.add(brandRows[i].first, brandRows[i].second);

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

    cout << "Loaded " << products.size() << " product(s), " << brandRows.size() << " brand tagline(s), "
         << cart.size() << " cart line(s), " << pending.size() << " pending order(s).\n";
    if (products.empty())
        cout << "\nThe catalog is empty. Load a dataset with:\n"
             << "  psql -U ecommerce_app -d ecommerce_db -f sql/seed.sql\n"
             << "or add products from the Admin panel (menu 7).\n";
}
