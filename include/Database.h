#pragma once
// PostgreSQL persistence layer.
// This is the ONLY code that talks to the database, and src/Database.cpp is the only
// file that includes <pqxx/pqxx>. Every method runs in its own transaction and throws
// a std::exception if something goes wrong.
#include <memory>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>
#include "Product.h"
#include "Cart.h"          // CartItem
#include "OrderQueue.h"    // Order

// Thrown by the constructor when PostgreSQL cannot be reached or the login fails.
struct DbConnectError : public std::runtime_error {
    using std::runtime_error::runtime_error;
};

class Database {
public:
    // connInfo example: "host=localhost dbname=ecommerce_db user=ecommerce_app"
    // The password is NOT part of it: libpq reads the PGPASSWORD environment variable.
    explicit Database(const std::string& connInfo);
    ~Database();
    Database(const Database&) = delete;
    Database& operator=(const Database&) = delete;

    // ---- products ----------------------------------------------------------
    std::vector<Product> loadProducts();                     // ordered by id
    void insertProducts(const std::vector<Product>& items);  // one transaction
    void insertProduct(const Product& p);
    void updateStock(int productId, int newStock);
    void deleteProduct(int productId);   // cart / wishlist rows cascade away

    // ---- cart and wishlist (list order = the database 'position' column) ----
    std::vector<std::pair<int, int>> loadCart();             // (productId, qty)
    void setCartItem(int productId, int qty);                // insert or update the qty
    void removeCartItem(int productId);
    std::vector<int> loadWishlist();                         // product ids
    void addWishlistItem(int productId);
    void removeWishlistItem(int productId);

    // ---- orders -------------------------------------------------------------
    int nextOrderId();                                       // highest id + 1 (at least 5001)
    std::vector<Order> loadPendingOrders();                  // oldest first = queue order
    void placeOrder(const Order& o);                         // save order + lines AND empty the cart rows
    bool completeOrder(const Order& o);                      // deduct stock + mark completed, all or nothing
    void cancelOrder(int orderId);

private:
    struct Impl;                       // holds the pqxx connection (kept out of this header)
    std::unique_ptr<Impl> impl_;
};
