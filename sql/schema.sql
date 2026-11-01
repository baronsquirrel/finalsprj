-- Schema for the e-commerce inventory system (PostgreSQL).
-- Safe to run more than once (IF NOT EXISTS).
-- Run:  psql -U ecommerce_app -d ecommerce_db -f sql/schema.sql

-- Partner brands and their banner taglines (menu 8). This table is OPTIONAL data:
-- the brand list itself always comes from products.brand, and a brand that has no
-- row here simply gets a generic tagline. products.brand is deliberately not a
-- foreign key, so an admin can add a product with a brand that is not listed yet.
CREATE TABLE IF NOT EXISTS brands (
    name     TEXT  PRIMARY KEY,
    tagline  TEXT  NOT NULL
);
CREATE UNIQUE INDEX IF NOT EXISTS brands_name_lower_idx ON brands (lower(name));

-- The catalog. IDs are assigned by the application (not by the database) so the
-- BST index and the undo/redo history keep working exactly as before.
CREATE TABLE IF NOT EXISTS products (
    id              INTEGER        PRIMARY KEY,
    name            TEXT           NOT NULL,
    category        TEXT           NOT NULL,
    brand           TEXT           NOT NULL,
    price           NUMERIC(12,2)  NOT NULL CHECK (price >= 0),
    stock_quantity  INTEGER        NOT NULL CHECK (stock_quantity >= 0)
);

-- One row per customer order. 'pending' orders are the checkout queue.
CREATE TABLE IF NOT EXISTS orders (
    id          INTEGER        PRIMARY KEY,
    customer    TEXT           NOT NULL,
    total       NUMERIC(12,2)  NOT NULL CHECK (total >= 0),
    status      TEXT           NOT NULL DEFAULT 'pending'
                CHECK (status IN ('pending', 'completed', 'cancelled')),
    created_at  TIMESTAMPTZ    NOT NULL DEFAULT now()
);

-- The lines of an order. Name and price are copied (a snapshot) so an order keeps
-- its history even if the product is later renamed, repriced or removed.
-- product_id is deliberately NOT a foreign key for the same reason.
CREATE TABLE IF NOT EXISTS order_items (
    order_id    INTEGER        NOT NULL REFERENCES orders(id) ON DELETE CASCADE,
    line_no     INTEGER        NOT NULL,
    product_id  INTEGER        NOT NULL,
    name        TEXT           NOT NULL,
    price       NUMERIC(12,2)  NOT NULL CHECK (price >= 0),
    qty         INTEGER        NOT NULL CHECK (qty > 0),
    PRIMARY KEY (order_id, line_no)
);

-- The shopping cart. The program has no user accounts, so there is one cart.
-- Only product_id and qty are stored; name and price are read from products when
-- the cart is loaded, so a cart never shows a stale price. 'position' is assigned
-- by the database and keeps the linked-list order. Deleting a product also
-- removes it from the cart.
CREATE TABLE IF NOT EXISTS cart_items (
    position    INTEGER  GENERATED ALWAYS AS IDENTITY PRIMARY KEY,
    product_id  INTEGER  NOT NULL UNIQUE REFERENCES products(id) ON DELETE CASCADE,
    qty         INTEGER  NOT NULL CHECK (qty > 0)
);

-- The wishlist, same idea (singly linked list order, one row per product).
CREATE TABLE IF NOT EXISTS wishlist_items (
    position    INTEGER  GENERATED ALWAYS AS IDENTITY PRIMARY KEY,
    product_id  INTEGER  NOT NULL UNIQUE REFERENCES products(id) ON DELETE CASCADE
);
