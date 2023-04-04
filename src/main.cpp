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
        printf("(%d, %c, %d, %d) ", cur->data.first, cur->col == RED ? 'R' : 'B', cur->left ? cur->left->data.first : 0,
               cur->right ? cur->right->data.first : 0);
        inorder_traverse(cur->right);
    }
    void inorder_output() {
        inorder_traverse(rt);
        printf("\n");
    }
} m;

template <class T>
void test_by_bool(T iter, bool assignable) {
    std::cout << "The iterator pointing to (" << iter->first << ", " << iter->second << ")" << " is judged " << (assignable ? "" : "not ") << "assignable by bool value." << std::endl;
}

template <class T> void test_by_type(T iter, sjtu::my_true_type) {
    std::cout << "The iterator pointing to (" << iter->first << ", " << iter->second << ")" << " is judged assignable by true_type." << std::endl;
}

template <class T> void test_by_type(T iter, sjtu::my_false_type) {
    std::cout << "The iterator pointing to (" << iter->first << ", " << iter->second << ")" << " is judged not assignable by false_type." << std::endl;
}

void assignable();

template <class T> void type_traits_test(T iter) {
    using iterator_assignable = typename sjtu::my_type_traits<T>::iterator_assignable;
    test_by_bool(iter, iterator_assignable::value);
    test_by_type(iter, iterator_assignable());
}

int main() {
    sjtu::map<std::string, int> m;
    m.insert({"one", 1});
    m.insert({"two", 2});
    type_traits_test(m.cbegin());
    type_traits_test(--m.end());
    return 0;
}