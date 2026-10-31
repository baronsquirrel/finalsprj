#pragma once
#include <vector>
#include "Product.h"

// ============================================================================
//  LAB 6 - BINARY SEARCH TREE keyed by product ID (nodes point at Products)
// ============================================================================
struct BSTNode {
    Product* data;
    BSTNode* left;
    BSTNode* right;
    explicit BSTNode(Product* p) : data(p), left(nullptr), right(nullptr) {}
};

class ProductBST {
    BSTNode* root;

    static BSTNode* insertRec(BSTNode* n, Product* p) {
        if (!n) return new BSTNode(p);
        if (p->id < n->data->id) n->left = insertRec(n->left, p);
        else if (p->id > n->data->id) n->right = insertRec(n->right, p);
        return n;
    }
    static BSTNode* removeRec(BSTNode* n, int id) {
        if (!n) return nullptr;
        if (id < n->data->id) n->left = removeRec(n->left, id);
        else if (id > n->data->id) n->right = removeRec(n->right, id);
        else {
            if (!n->left) { BSTNode* r = n->right; delete n; return r; }
            if (!n->right) { BSTNode* l = n->left; delete n; return l; }
            BSTNode* s = n->right;                 // in-order successor
            while (s->left) s = s->left;
            n->data = s->data;
            n->right = removeRec(n->right, s->data->id);
        }
        return n;
    }
    static void destroy(BSTNode* n) {
        if (!n) return;
        destroy(n->left);
        destroy(n->right);
        delete n;
    }
    static void inorder(BSTNode* n, vector<Product*>& out) {
        if (!n) return;
        inorder(n->left, out);
        out.push_back(n->data);
        inorder(n->right, out);
    }
    static BSTNode* buildBalanced(const vector<Product*>& v, int lo, int hi) {
        if (lo > hi) return nullptr;
        int mid = lo + (hi - lo) / 2;
        BSTNode* n = new BSTNode(v[mid]);
        n->left = buildBalanced(v, lo, mid - 1);
        n->right = buildBalanced(v, mid + 1, hi);
        return n;
    }
    static int heightRec(BSTNode* n) {
        if (!n) return 0;
        int l = heightRec(n->left), r = heightRec(n->right);
        return 1 + (l > r ? l : r);
    }

public:
    ProductBST() : root(nullptr) {}
    ~ProductBST() { destroy(root); }
    ProductBST(const ProductBST&) = delete;
    ProductBST& operator=(const ProductBST&) = delete;

    void insert(Product* p) { root = insertRec(root, p); }
    void remove(int id) { root = removeRec(root, id); }

    // steps (optional) reports how many nodes were visited -> shows O(log n)
    Product* find(int id, int* steps = nullptr) const {
        BSTNode* cur = root;
        int s = 0;
        while (cur) {
            s++;
            if (id == cur->data->id) { if (steps) *steps = s; return cur->data; }
            cur = (id < cur->data->id) ? cur->left : cur->right;
        }
        if (steps) *steps = s;
        return nullptr;
    }

    // Plain BSTs degrade to linked lists on sorted inserts, so after each
    // insertion we rebuild the tree perfectly balanced (O(n)) to keep O(log n).
    void rebuildBalanced() {
        vector<Product*> v;
        inorder(root, v);
        destroy(root);
        root = buildBalanced(v, 0, (int)v.size() - 1);
    }
    int height() const { return heightRec(root); }
};
