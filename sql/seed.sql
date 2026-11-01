-- Sample dataset: 4 tech brands, 16 products (prices in PHP are placeholders).
-- Safe to run more than once (existing ids / brands are skipped).
-- Run:  psql -U ecommerce_app -d ecommerce_db -f sql/seed.sql

BEGIN;

INSERT INTO brands (name, tagline) VALUES
    ('Apple', 'Think different. Premium design, seamless ecosystem.'),
    ('Samsung', 'Do what you can''t. Bold screens, everyday innovation.'),
    ('Sony', 'Be moved. Studio-grade sound and gaming.'),
    ('Lenovo', 'Smarter technology for all. Laptops built to perform.')
ON CONFLICT DO NOTHING;

INSERT INTO products (id, name, category, brand, price, stock_quantity) VALUES
    (1001, 'iPhone 15 128GB', 'Smartphones', 'Apple', 45990, 12),
    (1002, 'AirPods Pro (2nd Gen)', 'Audio', 'Apple', 14990, 30),
    (1003, 'iPad 10th Gen 64GB', 'Tablets', 'Apple', 24990, 9),
    (1004, 'Apple Watch SE 40mm', 'Wearables', 'Apple', 15990, 15),
    (1005, 'Galaxy S24 256GB', 'Smartphones', 'Samsung', 49990, 10),
    (1006, 'Galaxy Buds2 Pro', 'Audio', 'Samsung', 8990, 25),
    (1007, 'Galaxy Tab S9 FE', 'Tablets', 'Samsung', 25990, 7),
    (1008, 'Odyssey G5 27in Monitor', 'Monitors', 'Samsung', 15990, 6),
    (1009, 'WH-1000XM5 Headphones', 'Audio', 'Sony', 18990, 14),
    (1010, 'WF-1000XM5 Earbuds', 'Audio', 'Sony', 14990, 18),
    (1011, 'PlayStation 5 Slim', 'Gaming', 'Sony', 33990, 5),
    (1012, 'Bravia 43in 4K TV', 'TV', 'Sony', 28990, 4),
    (1013, 'LOQ 15 Gaming Laptop', 'Laptops', 'Lenovo', 59990, 8),
    (1014, 'IdeaPad Slim 3 15in', 'Laptops', 'Lenovo', 31990, 20),
    (1015, 'Legion Go Handheld', 'Gaming', 'Lenovo', 42990, 3),
    (1016, 'ThinkPad E14 Gen 5', 'Laptops', 'Lenovo', 46990, 11)
ON CONFLICT DO NOTHING;

COMMIT;
