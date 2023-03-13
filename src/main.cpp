#include <cstdio>
#include <iostream>

#include "map.hpp"

using namespace sjtu;

class map_enabled_testing : public map<int, int> {
  public:
    void inorder_traverse(tnode *cur) {
        if (cur == nullptr)
            return;
        inorder_traverse(cur->left);
        printf("(%d, %c, %d, %d) ", cur->key, cur->col == RED ? 'R' : 'B', cur->left ? cur->left->key : 0, cur->right ? cur->right->key : 0);
        inorder_traverse(cur->right);
    }
    void inorder_output() {
        inorder_traverse(rt);
        printf("\n");
    }
} m;

int main() {
    int key, data = 0;
    while (std::cin >> key) {
        m.insert({key, data});
        m.inorder_output();
    }
}