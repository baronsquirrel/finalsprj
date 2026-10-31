// PostgreSQL persistence layer (libpqxx). See include/Database.h.
#include <pqxx/pqxx>
#include "Database.h"

using namespace std;

struct Database::Impl {
    pqxx::connection conn;
    explicit Impl(const string& info) : conn(info) {}
};

Database::Database(const string& connInfo) {
    try {
        impl_.reset(new Impl(connInfo));
    } catch (const pqxx::broken_connection& e) {
        throw DbConnectError(e.what());
    }

    // Fail early with a clear message if sql/schema.sql has not been run yet.
    pqxx::work tx(impl_->conn);
    pqxx::row r = tx.exec(
        "SELECT count(*) FROM information_schema.tables WHERE table_schema = 'public' "
        "AND table_name IN ('products','orders','order_items','cart_items','wishlist_items')").one_row();
    if (r[0].as<int>() != 5)
        throw runtime_error("The database tables were not found. Create them with:\n"
                            "  psql -U ecommerce_app -d ecommerce_db -f sql/schema.sql");
    tx.commit();
}

Database::~Database() = default;

// ---------------------------------------------------------------------------
//  products
// ---------------------------------------------------------------------------
static void insertProductRow(pqxx::work& tx, const Product& p) {
    tx.exec("INSERT INTO products (id, name, category, brand, price, stock_quantity) "
            "VALUES ($1, $2, $3, $4, $5, $6)",
            pqxx::params{p.id, p.name, p.category, p.brand, p.price, p.stockQuantity});
}

vector<Product> Database::loadProducts() {
    pqxx::work tx(impl_->conn);
    pqxx::result res = tx.exec(
        "SELECT id, name, category, brand, price, stock_quantity FROM products ORDER BY id");
    vector<Product> out;
    for (const pqxx::row& row : res)
        out.push_back(Product{row[0].as<int>(), row[1].as<string>(), row[2].as<string>(),
                              row[3].as<string>(), row[4].as<double>(), row[5].as<int>()});
    tx.commit();
    return out;
}

void Database::insertProducts(const vector<Product>& items) {
    pqxx::work tx(impl_->conn);
    for (size_t i = 0; i < items.size(); i++) insertProductRow(tx, items[i]);
    tx.commit();
}

void Database::insertProduct(const Product& p) {
    pqxx::work tx(impl_->conn);
    insertProductRow(tx, p);
    tx.commit();
}

void Database::updateStock(int productId, int newStock) {
    pqxx::work tx(impl_->conn);
    tx.exec("UPDATE products SET stock_quantity = $2 WHERE id = $1", pqxx::params{productId, newStock});
    tx.commit();
}

void Database::deleteProduct(int productId) {
    pqxx::work tx(impl_->conn);
    tx.exec("DELETE FROM products WHERE id = $1", pqxx::params{productId});
    tx.commit();
}

// ---------------------------------------------------------------------------
//  cart and wishlist
// ---------------------------------------------------------------------------
vector<pair<int, int>> Database::loadCart() {
    pqxx::work tx(impl_->conn);
    pqxx::result res = tx.exec("SELECT product_id, qty FROM cart_items ORDER BY position");
    vector<pair<int, int>> out;
    for (const pqxx::row& row : res) out.push_back(make_pair(row[0].as<int>(), row[1].as<int>()));
    tx.commit();
    return out;
}

void Database::setCartItem(int productId, int qty) {
    pqxx::work tx(impl_->conn);
    tx.exec("INSERT INTO cart_items (product_id, qty) VALUES ($1, $2) "
            "ON CONFLICT (product_id) DO UPDATE SET qty = EXCLUDED.qty",
            pqxx::params{productId, qty});
    tx.commit();
}

void Database::removeCartItem(int productId) {
    pqxx::work tx(impl_->conn);
    tx.exec("DELETE FROM cart_items WHERE product_id = $1", pqxx::params{productId});
    tx.commit();
}

vector<int> Database::loadWishlist() {
    pqxx::work tx(impl_->conn);
    pqxx::result res = tx.exec("SELECT product_id FROM wishlist_items ORDER BY position");
    vector<int> out;
    for (const pqxx::row& row : res) out.push_back(row[0].as<int>());
    tx.commit();
    return out;
}

void Database::addWishlistItem(int productId) {
    pqxx::work tx(impl_->conn);
    tx.exec("INSERT INTO wishlist_items (product_id) VALUES ($1) ON CONFLICT (product_id) DO NOTHING",
            pqxx::params{productId});
    tx.commit();
}

void Database::removeWishlistItem(int productId) {
    pqxx::work tx(impl_->conn);
    tx.exec("DELETE FROM wishlist_items WHERE product_id = $1", pqxx::params{productId});
    tx.commit();
}

// ---------------------------------------------------------------------------
//  orders
// ---------------------------------------------------------------------------
int Database::nextOrderId() {
    pqxx::work tx(impl_->conn);
    pqxx::row r = tx.exec("SELECT GREATEST(COALESCE(MAX(id), 0) + 1, 5001) FROM orders").one_row();
    tx.commit();
    return r[0].as<int>();
}

vector<Order> Database::loadPendingOrders() {
    pqxx::work tx(impl_->conn);
    vector<Order> orders;

    pqxx::result heads = tx.exec(
        "SELECT id, customer, total FROM orders WHERE status = 'pending' ORDER BY id");
    for (const pqxx::row& row : heads) {
        Order o;
        o.orderId = row[0].as<int>();
        o.customer = row[1].as<string>();
        o.total = row[2].as<double>();
        orders.push_back(o);
    }

    // Both queries are sorted by order id, so one forward pass attaches every line.
    pqxx::result lines = tx.exec(
        "SELECT order_id, product_id, name, price, qty FROM order_items "
        "WHERE order_id IN (SELECT id FROM orders WHERE status = 'pending') "
        "ORDER BY order_id, line_no");
    size_t at = 0;
    for (const pqxx::row& row : lines) {
        int orderId = row[0].as<int>();
        while (at < orders.size() && orders[at].orderId != orderId) at++;
        if (at == orders.size()) break;
        orders[at].items.push_back(CartItem{row[1].as<int>(), row[2].as<string>(),
                                            row[3].as<double>(), row[4].as<int>()});
    }
    tx.commit();
    return orders;
}

void Database::placeOrder(const Order& o) {
    pqxx::work tx(impl_->conn);
    tx.exec("INSERT INTO orders (id, customer, total, status) VALUES ($1, $2, $3, 'pending')",
            pqxx::params{o.orderId, o.customer, o.total});
    for (size_t i = 0; i < o.items.size(); i++) {
        const CartItem& it = o.items[i];
        tx.exec("INSERT INTO order_items (order_id, line_no, product_id, name, price, qty) "
                "VALUES ($1, $2, $3, $4, $5, $6)",
                pqxx::params{o.orderId, (int)i + 1, it.productId, it.name, it.price, it.qty});
    }
    tx.exec("DELETE FROM cart_items");     // the cart has become this order
    tx.commit();
}

bool Database::completeOrder(const Order& o) {
    pqxx::work tx(impl_->conn);
    for (size_t i = 0; i < o.items.size(); i++) {
        pqxx::result r = tx.exec(
            "UPDATE products SET stock_quantity = stock_quantity - $2 "
            "WHERE id = $1 AND stock_quantity >= $2",
            pqxx::params{o.items[i].productId, o.items[i].qty});
        if (r.affected_rows() != 1) return false;      // not enough stock: tx is rolled back
    }
    tx.exec("UPDATE orders SET status = 'completed' WHERE id = $1", pqxx::params{o.orderId});
    tx.commit();
    return true;
}

void Database::cancelOrder(int orderId) {
    pqxx::work tx(impl_->conn);
    tx.exec("UPDATE orders SET status = 'cancelled' WHERE id = $1", pqxx::params{orderId});
    tx.commit();
}
