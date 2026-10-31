#pragma once
#include "Product.h"

// ============================================================================
//  LAB 4 - STACK (LIFO): admin action history for Undo / Redo
// ============================================================================
enum ActionType { ACT_ADD, ACT_REMOVE, ACT_STOCK };

struct AdminAction {
    ActionType type = ACT_ADD;
    Product before{};     // state before the action (REMOVE / STOCK)
    Product after{};      // state after the action  (ADD / STOCK)
};

class ActionStack {
    struct Node {
        AdminAction act;
        Node* next;
        Node(const AdminAction& a, Node* n) : act(a), next(n) {}
    };
    Node* topNode;
    int count;

public:
    ActionStack() : topNode(nullptr), count(0) {}
    ~ActionStack() { clear(); }
    ActionStack(const ActionStack&) = delete;
    ActionStack& operator=(const ActionStack&) = delete;

    void push(const AdminAction& a) { topNode = new Node(a, topNode); count++; }
    bool pop(AdminAction& out) {
        if (!topNode) return false;
        Node* t = topNode;
        out = t->act;
        topNode = t->next;
        delete t;
        count--;
        return true;
    }
    bool empty() const { return topNode == nullptr; }
    int size() const { return count; }
    void clear() { AdminAction d; while (pop(d)) {} }
};

// Undo / redo helpers. Stock changes are stored as before/after and applied as a
