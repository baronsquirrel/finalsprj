#pragma once
#include <iostream>
#include <string>

// ============================================================================
//  LAB 2 - LISTS: Customer wishlist (SINGLY linked list)
//  Like the cart it has a "highlighted" cursor for browsing. Because each node only
//  knows its NEXT node, moving to the previous item (and removing the highlighted
//  one) means walking again from the head: O(n), unlike the doubly linked cart.
// ============================================================================

class Wishlist {
    struct Node {
        int productId;
        string name;
        Node* next;
    };
    Node* head;
    Node* cur;        // the highlighted item
    int count;

    // The node in front of `target` (nullptr when target is the head). Walks from the head.
    Node* before(const Node* target) const {
        Node* prev = nullptr;
        for (Node* n = head; n && n != target; n = n->next) prev = n;
        return prev;
    }

public:
    Wishlist() : head(nullptr), cur(nullptr), count(0) {}
    ~Wishlist() {
        while (head) { Node* t = head; head = head->next; delete t; }
    }
    Wishlist(const Wishlist&) = delete;
    Wishlist& operator=(const Wishlist&) = delete;

    int size() const { return count; }

    bool add(int id, const string& name) {
        Node* last = nullptr;
        for (Node* n = head; n; n = n->next) {
            if (n->productId == id) return false;
            last = n;
        }
        Node* node = new Node{id, name, nullptr};
        if (last) last->next = node; else head = node;
        if (!cur) cur = node;
        count++;
        return true;
    }

    bool remove(int id) {
        Node* prev = nullptr;
        for (Node* n = head; n; prev = n, n = n->next) {
            if (n->productId == id) {
                if (n == cur) cur = n->next ? n->next : prev;     // keep the cursor on a real node
                if (prev) prev->next = n->next; else head = n->next;
                delete n;
                count--;
                return true;
            }
        }
        return false;
    }

    // ---- browsing ----
    void moveToFirst() { cur = head; }
    bool moveNext() { if (cur && cur->next) { cur = cur->next; return true; } return false; }
    bool movePrev() {
        if (!cur || cur == head) return false;
        cur = before(cur);
        return true;
    }
    bool current(int& id, string& name) const {
        if (!cur) return false;
        id = cur->productId;
        name = cur->name;
        return true;
    }
    bool removeCurrent() { return cur ? remove(cur->productId) : false; }

    void display(bool showCursor = false) const {
        if (!head) { cout << "  (wishlist is empty)\n"; return; }
        int i = 1;
        for (Node* n = head; n; n = n->next, i++)
            cout << (showCursor && n == cur ? " >> " : "    ") << i << ". [" << n->productId << "] " << n->name << "\n";
    }
};
