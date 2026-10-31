#pragma once
#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include "Cart.h"

// ============================================================================
//  LAB 5 - QUEUE (FIFO): checkout / order processing line
// ============================================================================
struct Order {
    int orderId = 0;
    string customer;
    vector<CartItem> items;
    double total = 0;
};

class OrderQueue {
    struct Node {
        Order order;
        Node* next;
        explicit Node(const Order& o) : order(o), next(nullptr) {}
    };
    Node* frontN;
    Node* rearN;
    int count;

public:
    OrderQueue() : frontN(nullptr), rearN(nullptr), count(0) {}
    ~OrderQueue() { Order d; while (dequeue(d)) {} }
    OrderQueue(const OrderQueue&) = delete;
    OrderQueue& operator=(const OrderQueue&) = delete;

    void enqueue(const Order& o) {
        Node* n = new Node(o);
        if (!rearN) frontN = rearN = n;
        else { rearN->next = n; rearN = n; }
        count++;
    }
    bool dequeue(Order& out) {
        if (!frontN) return false;
        Node* t = frontN;
        out = t->order;
        frontN = t->next;
        if (!frontN) rearN = nullptr;
        delete t;
        count--;
        return true;
    }
    int size() const { return count; }
    void display() const {
        if (!frontN) { cout << "  (no pending orders)\n"; return; }
        int pos = 1;
        for (Node* n = frontN; n; n = n->next, pos++) {
            cout << "  #" << pos << "  Order " << n->order.orderId << " | " << n->order.customer
                 << " | " << n->order.items.size() << " line(s) | PHP " << fixed << setprecision(2)
                 << n->order.total << (pos == 1 ? "   <-- next to be processed" : "") << "\n";
        }
    }
};
