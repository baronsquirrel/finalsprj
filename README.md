# Brand-Affiliated E-Commerce Inventory & Search System (CS201)

**Course:** Data Structure and Algorithm Analysis (BS Computer Engineering) - Final Output
**Language:** C++17, console application
**Data:** products, brands and taglines live only in PostgreSQL, never in the C++ code

## Requirements

- g++ with C++17 and `make` (MSYS2 UCRT64 on Windows)
- PostgreSQL 18 (any recent version works) running locally
- The libpqxx client library. On MSYS2: `pacman -S --needed mingw-w64-ucrt-x86_64-libpqxx`

## Database setup (one time)

```sql
-- as the postgres superuser:  psql -U postgres
CREATE USER ecommerce_app WITH PASSWORD 'choose-a-password';
CREATE DATABASE ecommerce_db OWNER ecommerce_app;
```

Then create the tables from the project folder:

```bash
psql -U ecommerce_app -d ecommerce_db -f sql/schema.sql
```

Then load a dataset. The included sample (4 brands, 16 products) is `sql/seed.sql`:

```bash
psql -U ecommerce_app -d ecommerce_db -f sql/seed.sql
```

The program contains no product or brand data of its own; everything is read from PostgreSQL at
startup. To use a different dataset, wipe the old one with `sql/reset_data.sql`, load your own `.sql`
file (format rules in `sql/README.md`), then restart the program or choose **Admin panel > 6. Reload all
data from the database**. No code changes or rebuild are needed.

## Running (password and connection)

The password is **never** stored in the code or the repository. Set it for your terminal session first:

```powershell
$env:PGPASSWORD = "your-ecommerce_app-password"      # PowerShell
export PGPASSWORD="your-ecommerce_app-password"      # bash / MSYS2
```

By default the program connects to `localhost:5432`, database `ecommerce_db`, user `ecommerce_app`.
To use different settings, set `ECOMMERCE_DB_CONN`, for example:
`host=localhost port=5433 dbname=mydb user=me`.

If PostgreSQL cannot be reached (service stopped, wrong password, tables missing), the program prints
an error and exits with code 1. If the connection is lost while it is running, it exits the same way.

## Build and run

Open a terminal **inside the project folder**, then:

```bash
make                 # compiles into build/, writes the program to the project root (links -lpqxx -lpq)
./ecommerce          # Windows PowerShell: .\ecommerce.exe
```

Other commands: `make asan` (memory-safety build, `ecommerce_asan`) and `make clean` (removes `build/` and the programs).

No `make`? Compile everything with one command (the `-Iinclude` flag tells g++ where the headers are):

```bash
g++ -std=c++17 -Wall -Wextra -Iinclude -o ecommerce main.cpp src/Utils.cpp src/Sorting.cpp \
    src/Searching.cpp src/AdminActions.cpp src/CatalogViews.cpp \
    src/ShoppingMenus.cpp src/AdminMenu.cpp src/Database.cpp src/AppContext.cpp src/Pager.cpp -lpqxx -lpq
```

## Project structure

```
ecommerce-system/
├── main.cpp                main menu loop only
├── Makefile
├── README.md
├── ecommerce(.exe)         the built program (created by make)
├── include/                all headers
│   ├── Product.h           struct Product (the required data model)
│   ├── Pager.h             paged product tables (PRODUCTS_PER_PAGE = 16)
│   ├── Database.h          PostgreSQL layer (libpqxx), the only code that talks to the database
│   ├── AppContext.h        bundles the database + all data structures; passed to the menus
│   ├── Utils.h             input checking, toLower, table printing
│   ├── Inventory.h         Catalog + ProductBST kept in sync
│   ├── Catalog.h           Lab 1 - dynamic, resizable array
│   ├── Cart.h              Lab 2 - doubly linked list cart
│   ├── Wishlist.h          Lab 2 - singly linked list wishlist
│   ├── BrandRing.h         Lab 3 - circular linked list
│   ├── ActionStack.h       Lab 4 - LIFO stack + AdminAction record
│   ├── OrderQueue.h        Lab 5 - FIFO checkout queue
│   ├── ProductBST.h        Lab 6 - binary search tree by product ID
│   ├── Searching.h         Lab 7 - declarations
│   ├── Sorting.h           Lab 8 - declarations
│   ├── AdminActions.h      undo / redo declarations
│   ├── BrandTaglines.h     banner text per brand (loaded from the brands table)
│   ├── CatalogViews.h      menus 1, 2, 3, 4, 8, 9
│   ├── ShoppingMenus.h     menus 5, 6
│   └── AdminMenu.h         menu 7
├── src/                    implementations (everything except main.cpp)
│   ├── Utils.cpp
│   ├── Pager.cpp           shows long product lists one page at a time
│   ├── Searching.cpp       Lab 7 - linear keyword search, binary search bounds
│   ├── Sorting.cpp         Lab 8 - insertion sort, quick sort
│   ├── AdminActions.cpp    add / stock / remove helpers (database + memory) and undo / redo
│   ├── Database.cpp        all SQL lives here
│   ├── AppContext.cpp      loads everything from PostgreSQL at startup
│   ├── CatalogViews.cpp
│   ├── ShoppingMenus.cpp
│   └── AdminMenu.cpp
├── sql/
│   ├── schema.sql          the 6 tables (run once with psql)
│   ├── seed.sql            sample dataset: 4 brands, 16 products
│   ├── reset_data.sql      wipes all data (keeps the tables) before loading a new dataset
│   └── README.md           format rules for generating a new dataset
├── build/                  intermediate .o files (created by make)
└── .vscode/                editor include-path settings
```

Data-structure classes are header-only, so they live entirely in `include/`.
Algorithms and menu code have a header with declarations and a `.cpp` with the implementation.

## Product model

```cpp
struct Product {
    int id;  string name;  string category;
    string brand;  double price;  int stockQuantity;
};
```

## Lab integration (Labs 1-8)

| Lab | Concept | Where it is used in the program | Class / function (file) |
|---|---|---|---|
| 1 | Dynamic allocation | Catalog is a resizable array of `Product*` that doubles when full | `Catalog` (`Catalog.h`) |
| 2 | Singly / doubly linked lists | Shopping cart with next / previous navigation (doubly); wishlist (singly) | `Cart`, `Wishlist` (`Cart.h`, `Wishlist.h`) |
| 3 | Circular linked list | Partner brand banner showcase that wraps from last brand to first | `BrandRing` (`BrandRing.h`) |
| 4 | Stack (LIFO) | Admin undo / redo of add, remove and stock-update actions | `ActionStack` (`ActionStack.h`) |
| 5 | Queue (FIFO) | Checkout line: orders are enqueued, then processed oldest first | `OrderQueue` (`OrderQueue.h`) |
| 6 | Binary search tree | Product lookup by ID, O(log n); rebuilt balanced after each insert | `ProductBST` (`ProductBST.h`) |
| 7 | Searching | Linear keyword search vs. binary-search price range filter | `linearKeywordSearch`, `lowerBound`, `upperBound` (`Searching.cpp`) |
| 8 | Sorting | Quick Sort (price) and Insertion Sort (stock, brand) | `quickSort`, `insertionSort` (`Sorting.cpp`) |

No `std::list`, `std::stack`, `std::queue`, `std::sort` or `std::map` is used. `std::vector` appears only for
small temporary lists (distinct brand names, line items copied into an order).

## Menu

| # | Option | Structure / algorithm |
|---|---|---|
| 1 | View complete catalog (tabular, with brand column) | Catalog array |
| 2 | Filter by official brand | Distinct-brand list + scan |
| 3 | Search by keyword **or** product ID | Linear search / BST |
| 4 | Filter by price range | Quick Sort + two binary searches |
| 5 | Shopping cart and wishlist | Doubly / singly linked list |
| 6 | Checkout queue: place order, process next, view pending | FIFO queue |
| 7 | Admin panel: add, update stock, remove, **undo**, redo | LIFO stacks |
| 8 | Partner brand banner showcase | Circular list |
| 9 | Sort inventory display (price / stock / brand, asc or desc) | Quick / Insertion Sort |
| 10 | Exit | Destructors free every node |

## Design notes

- **Ownership:** `Catalog` owns every `Product`. The BST, cart and orders never own products, so there are no double deletes.
- **BST balance:** inserting sequential IDs would turn a plain BST into a linked list, so `Inventory::addProduct` rebuilds the tree perfectly balanced (O(n)). The search output prints the nodes visited and the tree height to show the O(log n) behavior.
- **Binary search precondition:** option 4 sorts a *copy* of the catalog by price with Quick Sort, then binary-searches the lower and upper bounds. The catalog's own order is never changed by sorting.
- **Undo / redo:** stock changes are saved as before/after values and applied as a delta, so units already sold at checkout are not restored by an undo.
- **Checkout:** processing is all-or-nothing. If any line is out of stock or the product was removed, the order is cancelled and no stock changes.
- **Cart merging:** adding the same product twice increases the quantity of the existing cart node.

## PostgreSQL integration

The Lab 1-8 data structures are unchanged. PostgreSQL sits underneath them: data is loaded into
the structures at startup, and every change is written to the database as it happens.

| Data | Stored in | Notes |
|---|---|---|
| Products | `products` | catalog array + BST are rebuilt from it at startup |
| Brand taglines | `brands` | optional; a brand without a row gets a generic banner line |
| Cart | `cart_items` | only product id and qty; name and price come from the catalog |
| Wishlist | `wishlist_items` | list order kept by a `position` column |
| Orders | `orders`, `order_items` | `pending` orders are reloaded into the FIFO queue in order |
| Undo / redo stacks | not stored | session history only |

- **Transactions:** placing an order (save order + lines + empty the cart) is one transaction.
  Processing an order (deduct stock for every line + mark completed) is one transaction, so it is all or nothing.
- **Database first:** every change is written to PostgreSQL before memory is updated, so a database failure never leaves memory ahead of the database.
- **Deleting a product** also removes it from the cart and wishlist (a database `ON DELETE CASCADE`, mirrored in memory).
- **Order history keeps a snapshot** of each line's name and price, so it stays correct if the product later changes.

## Paged lists

Every product table (catalog, brand filter, search, price range, sorted view) shows at most 16 rows
per screen, set by `PRODUCTS_PER_PAGE` in `include/Pager.h`. When a list is longer, the prompt accepts
`n` (next page), `p` (previous page), a page number, or Enter to go back to the menu. A list that fits on
one page just waits for Enter, as before.

## Memory management and safety

- Every `new` has a matching `delete` / `delete[]`, released in destructors (`~Catalog`, `~ProductBST`, `~Cart`, `~Wishlist`, `~BrandRing`, `~ActionStack`, `~OrderQueue`).
- The circular list's loop is broken before freeing so the destructor cannot loop forever.
- Copy constructors and assignment operators are deleted on all owning classes (no accidental shallow copies).
- Input is validated: non-numbers, out-of-range values and empty strings are rejected and re-prompted. Closing stdin (EOF) exits cleanly instead of looping.
- Edge cases handled: empty catalog / cart / queue / stacks, unknown IDs, out-of-stock items, duplicate wishlist entries, removing the last cart item, min / max price entered in reverse.
- Verified clean with AddressSanitizer and UBSan:

```bash
make asan
./ecommerce_asan
```

## Sample data

The sample names and prices in `sql/pc_component_inventory.sql` are illustrative placeholders (PHP), not live store prices.
