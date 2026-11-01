-- DELETES ALL DATA (products, brands, carts, wishlists AND order history) but keeps the
-- tables, so a new dataset can be loaded. Use it before loading a completely new seed file.
-- Run:  psql -U ecommerce_app -d ecommerce_db -f sql/reset_data.sql

TRUNCATE order_items, orders, cart_items, wishlist_items, products, brands RESTART IDENTITY;
