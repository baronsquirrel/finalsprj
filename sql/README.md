# Data guide (for generating a new dataset)

The C++ program contains **no product or brand data**. Everything shown comes from PostgreSQL, so a
new dataset is just a `.sql` file. After loading it, restart the program or use
**Admin panel > 6. Reload all data from the database**.

## Tables you fill in

```sql
brands   (name TEXT PRIMARY KEY, tagline TEXT NOT NULL)        -- optional, used by the brand banner screen
products (id INTEGER PRIMARY KEY,
          name TEXT NOT NULL, category TEXT NOT NULL, brand TEXT NOT NULL,
          price NUMERIC(12,2) NOT NULL CHECK (price >= 0),
          stock_quantity INTEGER NOT NULL CHECK (stock_quantity >= 0))
```

## Rules for the generated file

1. Output ONE file, wrapped in `BEGIN;` ... `COMMIT;`, containing a `brands` INSERT and a `products` INSERT.
2. `id`: unique positive whole numbers (for example 1001, 1002, ...). The app assigns the next id after the highest one.
3. `brand` in every product must match a `brands.name` exactly (same spelling and capitalization).
4. `price`: a plain number such as `45990` or `1299.50`. No currency symbol, no commas. Currency is PHP.
5. `stock_quantity`: a whole number, 0 or more.
6. Text values go in single quotes; an apostrophe inside text is doubled: `'Levi''s 501 Jeans'`.
7. Keep names under about 40 characters, categories under 20, taglines under about 70.
8. Prefer plain English letters and digits. Accented letters work if the file is saved as UTF-8; avoid emoji and symbols.
9. Use a small number of consistent categories (for example `Shoes`, `Apparel`, `Bags`).

## Example

```sql
BEGIN;
INSERT INTO brands (name, tagline) VALUES
    ('Nike', 'Just do it. Performance sportswear.'),
    ('Adidas', 'Impossible is nothing.');
INSERT INTO products (id, name, category, brand, price, stock_quantity) VALUES
    (1001, 'Air Zoom Pegasus 40', 'Shoes', 'Nike', 6495, 25),
    (1002, 'Ultraboost Light', 'Shoes', 'Adidas', 9500, 12);
COMMIT;
```

## Loading it (PowerShell, from the project folder)

```powershell
$env:PGCLIENTENCODING = "UTF8"
psql -U ecommerce_app -d ecommerce_db -f sql/reset_data.sql    # only when REPLACING the old data (deletes everything)
psql -U ecommerce_app -d ecommerce_db -f sql/seed.sql          # or your new file
```

`seed.sql` in this folder is the original sample dataset (tech brands) and can be used as a template.
