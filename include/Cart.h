#pragma once
#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include "Product.h"
#include "Utils.h"

// ============================================================================
//  LAB 2 - LISTS: Shopping cart (DOUBLY linked list)
// ============================================================================
struct CartItem {
    int productId;
    string name;
    double price;
    int qty;
};

class Cart {
    struct Node {
        CartItem item;
        Node* prev;
        Node* next;
        explicit Node(const CartItem& i) : item(i), prev(nullptr), next(nullptr) {}
    };
    Node* head;
    Node* tail;
    Node* cur;      // cursor used for next / previous navigation
    int count;

public:
    Cart() : head(nullptr), tail(nullptr), cur(nullptr), count(0) {}
    ~Cart() { clear(); }
    Cart(const Cart&) = delete;
    Cart& operator=(const Cart&) = delete;

    int size() const { return count; }

    void addItem(const Product& p, int qty) {
        for (Node* n = head; n; n = n->next) {          // merge duplicates
            if (n->item.productId == p.id) { n->item.qty += qty; cur = n; return; }
        }
        Node* node = new Node(CartItem{p.id, p.name, p.price, qty});
        if (!tail) head = tail = node;
        else { tail->next = node; node->prev = tail; tail = node; }
        cur = node;
        count++;
    }

    bool moveNext() { if (cur && cur->next) { cur = cur->next; return true; } return false; }
    bool movePrev() { if (cur && cur->prev) { cur = cur->prev; return true; } return false; }

    bool removeCurrent() {
        if (!cur) return false;
        Node* victim = cur;
        if (victim->prev) victim->prev->next = victim->next; else head = victim->next;
        if (victim->next) victim->next->prev = victim->prev; else tail = victim->prev;
        cur = victim->next ? victim->next : victim->prev;
        delete victim;
        count--;
        return true;
    }

    // Removes the line for a product (used when an admin deletes that product).
    // The cursor stays where it is unless it was sitting on the removed line.
    bool removeProduct(int productId) {
        for (Node* n = head; n; n = n->next) {
            if (n->item.productId != productId) continue;
            if (n == cur) return removeCurrent();
            if (n->prev) n->prev->next = n->next; else head = n->next;
            if (n->next) n->next->prev = n->prev; else tail = n->prev;
            delete n;
            count--;
            return true;
        }
        return false;
    }

    const CartItem* current() const { return cur ? &cur->item : nullptr; }

    double total() const {
        double t = 0;
        for (Node* n = head; n; n = n->next) t += n->item.price * n->item.qty;
        return t;
    }

    void display() const {
        if (!head) { cout << "  (cart is empty)\n"; return; }
        printLine();
        cout << left << setw(4) << "" << setw(7) << "ID" << setw(32) << "Item" << right
             << setw(12) << "Price" << setw(6) << "Qty" << setw(14) << "Subtotal" << "\n";
        printLine();
        for (Node* n = head; n; n = n->next) {
            string nm = fitText(n->item.name, 30);      // UTF-8 safe: padded or cut with "..."
            cout << left << setw(4) << (n == cur ? " >>" : "") << setw(7) << n->item.productId
                 << nm << "  " << right << fixed << setprecision(2) << setw(12) << n->item.price
                 << setw(6) << n->item.qty << setw(14) << n->item.price * n->item.qty << "\n";
        }
        printLine();
        cout << right << setw(LINE_W - 14) << "TOTAL (PHP):" << setw(14) << fixed << setprecision(2)
             << total() << "\n";
    }

    void copyItems(vector<CartItem>& out) const {
        for (Node* n = head; n; n = n->next) out.push_back(n->item);
    }

    void clear() {
        while (head) { Node* t = head; head = head->next; delete t; }
        tail = cur = nullptr;
        count = 0;
    }
};
