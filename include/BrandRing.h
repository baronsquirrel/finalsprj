#pragma once
#include <string>
#include "Utils.h"

// ============================================================================
//  LAB 3 - CIRCULAR LINKED LIST: partner brand banner showcase
// ============================================================================
class BrandRing {
    struct Node {
        string brand;
        string tagline;
        Node* next;
    };
    Node* tail;     // tail->next is the head, which makes the list circular
    Node* cur;
    int count;

public:
    BrandRing() : tail(nullptr), cur(nullptr), count(0) {}
    ~BrandRing() {
        if (!tail) return;
        Node* n = tail->next;
        tail->next = nullptr;                 // break the circle, then free linearly
        while (n) { Node* t = n->next; delete n; n = t; }
    }
    BrandRing(const BrandRing&) = delete;
    BrandRing& operator=(const BrandRing&) = delete;

    void add(const string& brand, const string& tagline) {
        if (tail) {
            Node* n = tail;
            do {
                if (toLower(n->brand) == toLower(brand)) return;   // already listed
                n = n->next;
            } while (n != tail);
        }
        Node* node = new Node{brand, tagline, nullptr};
        if (!tail) { node->next = node; tail = node; cur = node; }
        else { node->next = tail->next; tail->next = node; tail = node; }
        count++;
    }
    int size() const { return count; }
    const string& brand() const { return cur->brand; }
    const string& tagline() const { return cur->tagline; }
    bool atStart() const { return cur == tail->next; }
    void advance() { cur = cur->next; }          // wraps automatically at the end
};

inline string taglineFor(const string& brand) {
    string b = toLower(brand);
    if (b == "apple") return "Think different. Premium design, seamless ecosystem.";
    if (b == "samsung") return "Do what you can't. Bold screens, everyday innovation.";
    if (b == "sony") return "Be moved. Studio-grade sound and gaming.";
    if (b == "lenovo") return "Smarter technology for all. Laptops built to perform.";
    return "Official partner brand.";
}
