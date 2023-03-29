/**
 * implement a container like std::map
 */
#ifndef SJTU_MAP_HPP
#define SJTU_MAP_HPP

// only for std::less<T>
#include "exceptions.hpp"
#include "utility.hpp"
#include <cstddef>
#include <functional>

namespace sjtu {

template <class Key, class T, class Compare = std::less<Key>> class map {

  public:
    /**
     * the internal type of data.
     * it should have a default constructor, a copy constructor.
     * You can use sjtu::map as value_type by typedef.
     */
    typedef pair<const Key, T> value_type;

  protected:
    /**
     * the main data of red-black tree
     *
     */
    enum color { BLACK, RED };
    struct tnode {
        value_type data;
        tnode *left, *right, *parent;
        color col;
        int siz;

        tnode(const value_type &_data, tnode *_parent, color _col, int _siz = 0)
            : data(_data), parent(_parent), col(_col), siz(_siz) {}
    } * rt;

    friend class map_enabled_testing;

  public:
    /**
     * see BidirectionalIterator at CppReference for help.
     *
     * if there is anything wrong throw invalid_iterator.
     *     like it = map.begin(); --it;
     *       or it = map.end(); ++end();
     */
    class const_iterator;
    class iterator {
      private:
        /**
         * TODO add data members
         *   just add whatever you want.
         */
        const map<Key, T, Compare> *iter;
        tnode *ptr;

      public:
        // The following code is written for the C++ type_traits library.
        // Type traits is a C++ feature for describing certain properties of a type.
        // For instance, for an iterator, iterator::value_type is the type that the
        // iterator points to.
        // STL algorithms and containers may use these type_traits (e.g. the following
        // typedef) to work properly.
        // See these websites for more information:
        // https://en.cppreference.com/w/cpp/header/type_traits
        // About value_type: https://blog.csdn.net/u014299153/article/details/72419713
        // About iterator_category: https://en.cppreference.com/w/cpp/iterator
        using difference_type = std::ptrdiff_t;
        using value_type = map::value_type;
        using pointer = value_type *;
        using reference = value_type &;
        using iterator_category = std::output_iterator_tag;
        // If you are interested in type_traits, toy_traits_test provides a place to
        // practice. But the method used in that test is old and rarely used, so you
        // may explore on your own.
        // Notice: you may add some code in here and class const_iterator and namespace sjtu to implement toy_traits_test,
        // this part is only for bonus.

        iterator() : iter(nullptr), ptr(nullptr) {}
        iterator(const iterator &other) : iter(other.iter), ptr(other.ptr) {}
        iterator(const map<Key, T, Compare> *_iter, tnode *_ptr) : iter(_iter), ptr(_ptr) {}
        /**
         * TODO iter++
         */
        iterator operator++(int) { return *this; }
        /**
         * TODO ++iter
         */
        iterator &operator++() {}
        /**
         * TODO iter--
         */
        iterator operator--(int) {}
        /**
         * TODO --iter
         */
        iterator &operator--() {}
        /**
         * a operator to check whether two iterators are same (pointing to the same memory).
         */
        value_type &operator*() const {}
        bool operator==(const iterator &rhs) const { return ptr == rhs.ptr; }
        bool operator==(const const_iterator &rhs) const { return ptr != rhs.ptr; }
        /**
         * some other operator for iterator.
         */
        bool operator!=(const iterator &rhs) const { return ptr != rhs.ptr; }
        bool operator!=(const const_iterator &rhs) const { return ptr != rhs.ptr; }

        /**
         * for the support of it->first.
         * See <http://kelvinh.github.io/blog/2013/11/20/overloading-of-member-access-operator-dash-greater-than-symbol-in-cpp/>
         * for help.
         */
        value_type *operator->() const noexcept { return &ptr->data; }
    };
    class const_iterator {
        friend class iterator;
        // it should has similar member method as iterator.
        //  and it should be able to construct from an iterator.
      private:
        // data members.
        const map<Key, T, Compare> *iter;
        tnode *ptr;

      public:
        const_iterator() {
            // TODO
        }
        const_iterator(const const_iterator &other) {
            // TODO
        }
        const_iterator(const iterator &other) {
            // TODO
        }
        const_iterator(const map<Key, T, Compare> *_iter, tnode *_ptr) : iter(_iter), ptr(_ptr) {}

        const_iterator operator++(int) {}
        const_iterator &operator++() {}
        const_iterator operator--(int) {}
        const_iterator &operator--() {}

        const value_type &operator*() const {}

        // And other methods in iterator.
        bool operator==(const const_iterator &rhs) { return ptr == rhs.ptr; }
        // And other methods in iterator.
        bool operator!=(const const_iterator &rhs) { return ptr != rhs.ptr; }
        // And other methods in iterator.
        const value_type *operator->() const noexcept { return &ptr->data; }
    };
    /**
     * TODO two constructors
     */
    map() : rt(nullptr) {}
    map(const map &other) { rt = copy(other.rt); }
    /**
     * TODO assignment operator
     */
    map &operator=(const map &other) {
        if (this == &other)
            return *this;
        rt = copy(other.rt);
        return *this;
    }
    /**
     * TODO Destructors
     */
    ~map() { destruct(rt); }
    /**
     * TODO
     * access specified element with bounds checking
     * Returns a reference to the mapped value of the element with key equivalent to key.
     * If no such element exists, an exception of type `index_out_of_bound'
     */
    T &at(const Key &key) {
        tnode *cur = rt;
        while (cur != nullptr) {
            /**
             * if key < cur->key, comp = 0 - 1 = -1
             * if key = cur->key, comp = 0 - 0 = 0
             * if key > cur->key, comp = 1 - 0 = 1
             * (same below)
             */
            int comp = Compare()(cur->data.first, key) - Compare()(key, cur->data.first);
            if (!comp)
                break;
            if (comp < 0)
                cur = cur->left;
            else
                cur = cur->right;
        }
        if (cur == nullptr)
            throw index_out_of_bound();
        return cur->data.second;
    }
    const T &at(const Key &key) const {
        tnode *cur = rt;
        while (cur != nullptr) {
            int comp = Compare()(cur->data.first, key) - Compare()(key, cur->data.first);
            if (!comp)
                break;
            if (comp < 0)
                cur = cur->left;
            else
                cur = cur->right;
        }
        if (cur == nullptr)
            throw index_out_of_bound();
        return cur->data.second;
    }
    /**
     * TODO
     * access specified element
     * Returns a reference to the value that is mapped to a key equivalent to key,
     *   performing an insertion if such key does not already exist.
     */
    T &operator[](const Key &key) {
        try {
            return at(key);
        } catch (...) {
            value_type new_value(key, T());
            auto insert_iter = insert(new_value).first;
            return insert_iter->second;
        }
    }
    /**
     * behave like at() throw index_out_of_bound if such key does not exist.
     */
    const T &operator[](const Key &key) const { return at(key); }
    /**
     * return a iterator to the beginning
     */
    iterator begin() {
        tnode *u = rt;
        if (u == nullptr)
            return iterator(this, nullptr);
        while (u->left != nullptr)
            u = u->left;
        return iterator(this, u);
    }
    const_iterator cbegin() const {
        tnode *u = rt;
        if (u == nullptr)
            return iterator(this, nullptr);
        while (u->right != nullptr)
            u = u->left;
        return iterator(this, u);
    }
    /**
     * return a iterator to the end
     * in fact, it returns past-the-end.
     */
    iterator end() { return iterator{this, nullptr}; }
    const_iterator cend() const { return iterator(this, nullptr); }
    /**
     * checks whether the container is empty
     * return true if empty, otherwise false.
     */
    bool empty() const { return rt == nullptr; }
    /**
     * returns the number of elements.
     */
    size_t size() const { return rt == nullptr ? 0 : rt->siz; }
    /**
     * clears the contents
     */
    void clear() { destruct(rt); }
    /**
     * insert an element.
     * return a pair, the first of the pair is
     *   the iterator to the new element (or the element that prevented the insertion),
     *   the second one is true if insert successfully, or false.
     */
    pair<iterator, bool> insert(const value_type &value) {
        tnode *cur = rt, *next;
        if (cur == nullptr) { // If the tree is empty
            // Create a new root node, with col = RED, size = 1 and no links to other node
            rt = cur = new tnode(value, nullptr, BLACK);
            return {iterator(this, cur), true};
        }
        // Here we try to ensure the node we found cannot have a red sibling,
        // which requires that every node on the path doesn't have two red descendants.
        while (true) {
            int comp = Compare()(cur->data.first, value.first) - Compare()(value.first, cur->data.first);
            if (!comp) // Find the same element
                return {iterator(this, cur), false};
            // If the current node has two red descendeants, we should change them to black.
            if ((cur->left && cur->left->col == RED) && (cur->right && cur->right->col == RED)) {
                cur->col = RED;
                cur->left->col = cur->right->col = BLACK;
                // fix the situation if there's a red-red link to its parent.
                insert_adjust(cur);
            }
            if (comp < 0) {
                if (cur->left == nullptr) {
                    cur = cur->left = new tnode(value, cur, RED);
                    break;
                }
                cur = cur->left;
            } else {
                if (cur->right == nullptr) {
                    cur = cur->right = new tnode(value, cur, RED);
                    break;
                }
                cur = cur->right;
            }
        }
        // After inserted, fix the red-red link again
        insert_adjust(cur);
        // Change the size backward
        size_adjust_upward(cur, 1);
        return {iterator(this, cur), true};
    }
    /**
     * erase the element at pos.
     *
     * throw if pos pointed to a bad element (pos == this->end() || pos points an element out of this)
     */
    void erase(iterator pos) {
        iterator target = find(pos->first);
        if (target == end() || target != pos)
            return;
        Key *key = new Key(pos->first);
        if (!Compare()(rt->data.first, *key) && !Compare()(rt->data.first, *key) && rt->left == nullptr &&
            rt->right == nullptr) {
            delete rt;
            rt = nullptr;
            return;
        }
        tnode *cur = rt;
        // Here we try to ensure every node we met is red, which then conducts the node we want to delete is red
        while (true) {
            // Change the current node to red
            erase_adjust(cur, *key);
            int comp = Compare()(cur->data.first, *key) - Compare()(*key, cur->data.first);
            // If we find the node with two descendents,
            // swap the data with its 'next' node and delete that node then
            if (!comp && cur->left != nullptr && cur->right != nullptr) {
                tnode *next = cur->right;
                while (next->left)
                    next = next->left;
                move(cur, next);
                delete key;
                key = new Key(next->data.first);
                cur = cur->right;
                continue;
            }
            // If we find the node
            if (!comp) {
                tnode *replacement = cur->left == nullptr ? cur->right : cur->left;
                if (is_left(cur))
                    cur->parent->left = replacement;
                else
                    cur->parent->right = replacement;
                size_adjust_upward(cur, -1);
                delete cur;
                return;
            }
            // Go to the next node
            cur = comp > 0 ? cur->right : cur->left;
        }
    }
    /**
     * Returns the number of elements with key
     *   that compares equivalent to the specified argument,
     *   which is either 1 or 0
     *     since this container does not allow duplicates.
     * The default method of check the equivalence is !(a < b || b > a)
     */
    size_t count(const Key &key) const { return find(key) != cend(); }
    /**
     * Finds an element with key equivalent to key.
     * key value of the element to search for.
     * Iterator to an element with key equivalent to key.
     *   If no such element is found, past-the-end (see end()) iterator is returned.
     */
    iterator find(const Key &key) {
        tnode *cur = rt;
        if (cur == nullptr)
            return end();
        while (cur) {
            int comp = Compare()(cur->data.first, key) - Compare()(key, cur->data.first);
            if (!comp)
                return iterator(this, cur);
            if (comp < 0)
                cur = cur->left;
            else
                cur = cur->right;
        }
        return cur == nullptr ? end() : iterator(this, cur);
    }
    const_iterator find(const Key &key) const {
        tnode *cur = rt;
        if (cur == nullptr)
            return cend();
        while (cur) {
            int comp = Compare()(cur->data.first, key) - Compare()(key, cur->data.first);
            if (!comp)
                return iterator(this, cur);
            if (comp < 0)
                cur = cur->left;
            else
                cur = cur->right;
        }
        return cur == nullptr ? cend() : iterator(this, cur);
    }

  protected:
    /**
     * @brief custom exceptions for the protected and private functions of map
     *
     */
    class custom_exception {
      public:
        custom_exception(const std::string &_msg) : msg(_msg) {}
        std::string what() { return msg; }

      private:
        std::string msg;
    };

  protected:
    void size_adjust(tnode *cur) {
        cur->siz = 0;
        if (cur->left != nullptr)
            cur->siz += cur->left->siz;
        if (cur->right != nullptr)
            cur->siz += cur->right->siz;
    }
    /**
     * @brief Adjust the size of the nodes upward
     *
     * @param cur
     * @param delta
     */
    void size_adjust_upward(tnode *cur, int delta) {
        while (cur != nullptr) {
            cur->siz += delta;
            cur = cur->parent;
        }
    }

    /**
     * @brief Fix the situation when there's a red-red link between the selected node and its parent
     *
     * @param cur
     */
    void insert_adjust(tnode *cur) {
        if (cur->parent == nullptr || cur->parent->col == BLACK)
            return;
        if (cur->parent == rt) {
            cur->parent->col = BLACK;
            return;
        }
        if (is_left(cur->parent)) {
            if (is_left(cur)) {
                /** Change the tree by
                 *      B                         B
                 *     / \                       / \
                 *    R   B     ------->  (cur) R   R
                 *   /                               \
                 *  R (cur)                           B
                 */
                right_rotate(cur->parent->parent);
                std::swap(cur->parent->col, sibling(cur)->col);
            } else {
                /** Change the tree by
                 *      B                     B             B (cur)
                 *     / \                   / \           / \
                 *    R   B     ----->(cur) R   R ----->  R   R
                 *     \                   /                   \
                 *      R (cur)           R                     B
                 */
                left_rotate(cur->parent);
                right_rotate(cur->parent);
                std::swap(cur->col, cur->right->col);
            }
        } else {
            if (is_left(cur)) {
                /** Change the tree by
                 *      B                B                  B (cur)
                 *     / \              / \                / \
                 *    B   R     -----> B   R (cur) -----> R   R
                 *       /                  \            /
                 *      R (cur)              R          B
                 */
                right_rotate(cur->parent);
                left_rotate(cur->parent);
                std::swap(cur->col, cur->left->col);
            } else {
                /** Change the tree by
                 *      B                   B
                 *     / \                 / \
                 *    B   R     ------->  R   R (cur)
                 *         \             /
                 *          R (cur)     B
                 */
                left_rotate(cur->parent->parent);
                std::swap(cur->parent->col, sibling(cur)->col);
            }
        }
    }

    /**
     * @brief Adjust every node on the path to red node
     *
     * @param cur
     */
    void erase_adjust(tnode *cur, Key del) {
        // If the current node is red, we don't need to change it.
        // Notice: it only happens when current node is root.
        if (cur->col == RED)
            return;
        // If the current node is root
        if (cur == rt) {
            if (cur->left && cur->right && cur->left->col == cur->right->col) {
                cur->col = RED;
                cur->left->col = cur->right->col = BLACK;
                return;
            } else {

            }
        }
        // Notice: if the current node is a black node but not root, it must have a sibling
        if (has_black_descendants(cur)) {
            // tnode *sib = 
        }
    }

    /**
     * @brief Rotate the selected node to its left child, with its right child replacing the current position
     *   cur              r0
     *  /  \             /  \
     * l0   r0  ---->  cur  r1
     *     /  \       /  \
     *    l1  r1     l0  l1
     * @throw custom_exception when the selected node doesn't have a right child
     *
     * @param cur
     */
    void left_rotate(tnode *cur) {
        if (cur->right == nullptr)
            throw custom_exception("The node to rotate (left) doesn't have a right child.");
        if (cur->parent) {
            if (is_left(cur))
                cur->parent->left = cur->right;
            else
                cur->parent->right = cur->right;
        } else {
            rt = cur->right;
        }
        tnode *tmp = cur->right;
        cur->right->parent = cur->parent;
        cur->right = tmp->left;
        if (tmp->left)
            tmp->left->parent = cur;
        tmp->left = cur;
        cur->parent = tmp;
        size_adjust(cur);
        size_adjust(tmp);
    }

    /**
     * @brief Rotate the selected node to its right child, with its left child replacing the current position
     *      cur            l0
     *     /  \           /  \
     *    l0  r0  ---->  l1  cur
     *   /  \               /  \
     *  l1  r1             r1  r0
     * @throw custom_exception when the selected node doesn't have a left child
     *
     * @param cur
     */
    void right_rotate(tnode *cur) {
        if (cur->left == nullptr)
            throw custom_exception("The node to rotate (right) doesn't have a left child.");
        if (cur->parent) {
            if (is_left(cur))
                cur->parent->left = cur->left;
            else
                cur->parent->right = cur->left;
        } else {
            rt = cur->left;
        }
        tnode *tmp = cur->left;
        tmp->parent = cur->parent;
        cur->left = tmp->right;
        if (tmp->right)
            tmp->right->parent = cur;
        tmp->right = cur;
        cur->parent = tmp;
        size_adjust(cur);
        size_adjust(tmp);
    }

  private:
    /**
     * @brief copy a tree node
     *
     * @param target
     * @param _parent
     * @return return the copy of selected tree node
     */
    tnode *copy(tnode *target, tnode *_parent = nullptr) {
        if (target == nullptr)
            return nullptr;
        tnode *tmp = new tnode(target->data, _parent, target->col, target->siz);
        tmp->left = copy(target->left, tmp);
        tmp->right = copy(target->right, tmp);
        return tmp;
    }

    /**
     * @brief move the data from another tree node, since the Key type doesn't even support the f**king assignment operation
     *
     * @param cur
     * @param target
     */
    void move(tnode *&cur, tnode *target) {
        tnode *_left = cur->left, *_right = cur->right, *_parent = cur->parent;
        color _col = cur->col;
        bool _is_left = cur == rt ? 0 : is_left(cur);
        int _siz = cur->siz;
        delete cur;
        cur = new tnode(target->data, _parent, _col, _siz);
        if (_parent) {
            if (_is_left)
                _parent->left = cur;
            else
                _parent->right = cur;
        }
        if (_left) {
            cur->left = _left;
            _left->parent = cur;
        }
        if (_right) {
            cur->right = _right;
            _right->parent = cur;
        }
    }

    /**
     * @brief destruct a tree node and all its progenies by recursion
     *
     * @param target
     */
    void destruct(tnode *&target) {
        if (target == nullptr)
            return;
        destruct(target->left);
        destruct(target->right);
        delete target;
        target = nullptr;
    }

    /**
     * @brief Judge if a node is the left child of its parent
     * @throw custom_exception when passing the root node
     *
     * @param cur
     * @return true when is the left child
     * @return false when is the right child
     */
    bool is_left(tnode *cur) {
        if (cur->parent == nullptr)
            throw custom_exception("Unexpected operations on the root node.");
        return cur->parent->left == cur;
    }

    bool has_black_descendants(tnode *cur) {
        return ((cur->left && cur->left->col == BLACK) || cur->left == nullptr) &&
               ((cur->right && cur->right->col == BLACK) || cur->right == nullptr);
    }

    /**
     * @brief Get the sibling of the selected node
     * @throw custom_exception by is_left() when passing the root node
     *
     * @param cur
     * @return tnode*
     */
    tnode *sibling(tnode *cur) { return is_left(cur) ? cur->parent->right : cur->parent->left; }

    /**
     * @brief Get the grand parent of the selected node
     * @throw custom_exception when passing the root node
     *
     * @param cur
     * @return tnode*
     */
    tnode *grand_parent(tnode *cur) {
        if (cur->parent == nullptr)
            throw custom_exception("Unexpected operations on the root node.");
        return cur->parent->parent;
    }

    /**
     * @brief Get the aunt of the selected node
     * @throw custom_exception by is_left() when passing the root node
     *
     * @param cur
     * @return tnode*
     */
    tnode *aunt(tnode *cur) {
        if (cur->parent == nullptr)
            throw custom_exception("Unexpected operations on the root node.");
        return sibling(cur->parent);
    }
};

template class map<std::string, int>;

} // namespace sjtu

#endif
