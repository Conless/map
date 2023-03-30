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
        printf("(%d, %c, %d, %d) ", cur->data.first, cur->col == RED ? 'R' : 'B', cur->left ? cur->left->data.first : 0, cur->right ? cur->right->data.first : 0);
        inorder_traverse(cur->right);
    }
    void inorder_output() {
        inorder_traverse(rt);
        printf("\n");
    }
} m;

// map<int, int> m;

int main() {
    int opt, key, data = 0;
    // int n = 100000;
    // for (int i = 1; i <= n; i++)
    //     m.insert({i, 0});
    // for (int i = 1; i <= n; i++)
    //     m.erase(m.find(i));
    // return 0;
    while (std::cin >> opt >> key) {
        if (opt == 1)
            m.insert({key, data});
        else
            m.erase(m.find(key));
        m.inorder_output();
    }
    return 0;
}