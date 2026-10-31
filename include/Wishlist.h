#pragma once
#include <iostream>
#include <string>

// ============================================================================
//  LAB 2 - LISTS: Customer wishlist (SINGLY linked list)
// ============================================================================

class Wishlist {
    struct Node {
        int productId;
        string name;
        Node* next;
    };
    Node* head;

public:
    Wishlist() : head(nullptr) {}
    ~Wishlist() {
        while (head) { Node* t = head; head = head->next; delete t; }
    }
    Wishlist(const Wishlist&) = delete;
    Wishlist& operator=(const Wishlist&) = delete;

    bool add(int id, const string& name) {
        Node* last = nullptr;
        for (Node* n = head; n; n = n->next) {
            if (n->productId == id) return false;
            last = n;
        }
        Node* node = new Node{id, name, nullptr};
        if (last) last->next = node; else head = node;
        return true;
    }
    bool remove(int id) {
        Node* prev = nullptr;
        for (Node* n = head; n; prev = n, n = n->next) {
            if (n->productId == id) {
                if (prev) prev->next = n->next; else head = n->next;
                delete n;
                return true;
            }
        }
        return false;
    }
    void display() const {
        if (!head) { cout << "  (wishlist is empty)\n"; return; }
        int i = 1;
        for (Node* n = head; n; n = n->next, i++)
            cout << "  " << i << ". [" << n->productId << "] " << n->name << "\n";
    }
};
