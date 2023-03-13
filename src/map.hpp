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
        Key key;
        T data;
        tnode *left, *right, *parent;
        color col = RED;
        int siz;

        tnode() : key(), data(), siz(0) { left = right = parent = nullptr; }
        tnode(const value_type &_value, tnode *_parent, int _siz = 0) : tnode(_value.first, _value.second, _parent, _siz) {}
        tnode(const Key &_key, const T &_data, tnode *_parent, int _siz = 0)
            : key(_key), data(_data), siz(_siz), parent(_parent) {
            left = right = nullptr;
        }

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
        iterator operator++(int) {}
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
        bool operator==(const iterator &rhs) const {}
        bool operator==(const const_iterator &rhs) const {}
        /**
         * some other operator for iterator.
         */
        bool operator!=(const iterator &rhs) const {}
        bool operator!=(const const_iterator &rhs) const {}

        /**
         * for the support of it->first.
         * See <http://kelvinh.github.io/blog/2013/11/20/overloading-of-member-access-operator-dash-greater-than-symbol-in-cpp/>
         * for help.
         */
        value_type *operator->() const noexcept {}
    };
    class const_iterator {
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
        // And other methods in iterator.
        bool operator==(const const_iterator &rhs) { return ptr == rhs.ptr; }
        // And other methods in iterator.
        // And other methods in iterator.
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
            int comp = Compare()(cur->key, key) - Compare()(key, cur->key);
            if (!comp)
                break;
            if (comp < 0)
                cur = cur->left;
            else
                cur = cur->right;
        }
        if (cur == nullptr)
            throw index_out_of_bound();
        return cur->data;
    }
    const T &at(const Key &key) const {
        tnode *cur = rt;
        while (cur != nullptr) {
            int comp = Compare()(cur->key, key) - Compare()(key, cur->key);
            if (!comp)
                break;
            if (comp < 0)
                cur = cur->left;
            else
                cur = cur->right;
        }
        if (cur == nullptr)
            throw index_out_of_bound();
        return cur->data;
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
        } catch (index_out_of_bound()) {
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
    iterator end() {
        tnode *u = rt;
        if (u == nullptr)
            return iterator(this, nullptr);
        while (u->left != nullptr)
            u = u->left;
        return iterator(this, u);
    }
    const_iterator cend() const {
        tnode *u = rt;
        if (u == nullptr)
            return iterator(this, nullptr);
        while (u->right != nullptr)
            u = u->left;
        return iterator(this, u);
    }
    /**
     * checks whether the container is empty
     * return true if empty, otherwise false.
     */
    bool empty() const { return rt == nullptr; }
    /**
     * returns the number of elements.
     */
    size_t size() const { return rt->siz; }
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
            rt = cur = new tnode(value, nullptr);
            return {iterator(this, cur), true};
        }
        while (true) {
            int comp = Compare()(cur->key, value.first) - Compare()(value.first, cur->key);
            if (!comp) // Find the same element
                return {iterator(this, cur), false};
            if ((cur->left && cur->left->col == RED) && (cur->right && cur->right->col == RED)) {
                cur->col = RED;
                cur->left->col = cur->right->col = BLACK;
                insert_adjust(cur);
            }
            if (comp < 0) {
                if (cur->left == nullptr) {
                    cur = cur->left = new tnode(value, cur);
                    break;
                }
                cur = cur->left;
            } else {
                if (cur->right == nullptr) {
                    cur = cur->right = new tnode(value, cur);
                    break;
                }
                cur = cur->right;
            }
        }
        insert_adjust(cur);
        size_adjust(cur, 1);
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
        Key key = pos->first;
        if (rt->key == key && rt->left == nullptr && rt->right == nullptr) {
            delete rt;
            rt = nullptr;
            return;
        }
        tnode *cur = rt;
        while (true) {
            erase_adjust(cur);
            int comp = Compare()(cur->key, key) - Compare()(key, cur->key);
            if (!comp && cur->left != nullptr && cur->right != nullptr) {
                tnode *next = cur->right;
                while (next->left)
                    next = next->left;
                key = cur->key = next->key;
                cur->data = next->data;
                cur = cur->right;
                continue;
            }
            if (!comp) {
                if (is_left(cur))
                    cur->parent->left = nullptr;
                else
                    cur->parent->right = nullptr;
                
            }
        }
    }
    /**
     * Returns the number of elements with key
     *   that compares equivalent to the specified argument,
     *   which is either 1 or 0
     *     since this container does not allow duplicates.
     * The default method of check the equivalence is !(a < b || b > a)
     */
    size_t count(const Key &key) const { return find(key) == cend(); }
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
        while (cur && cur->key != key) {
            int comp = Compare()(cur->key, key) - Compare()(key, cur->key);
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
        while (cur && cur->key != key) {
            int comp = Compare()(cur->key, key) - Compare()(key, cur->key);
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
    /**
     * @brief Adjust the size of the nodes upward
     *
     * @param cur
     * @param delta
     */
    void size_adjust(tnode *cur, int delta) {
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
            if (!is_left(cur))
                left_rotate(cur);
            right_rotate(cur);
        } else {
            if (is_left(cur))
                right_rotate(cur);
            left_rotate(cur);
        }
        std::swap(cur->parent->col, sibling(cur)->col);
    }

    /**
     * @brief Adjust every node on the path to red node
     * 
     * @param cur 
     */
    void erase_adjust(tnode *cur) {

    }

    /**
     * @brief Rotate the selected node to its left child, with its right child replacing the current position
     *   rt              r0
     *  /  \            /  \
     * l0   r0  ---->  rt  r1
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
    }

    /**
     * @brief Rotate the selected node to its right child, with its left child replacing the current position
     *      rt             l0
     *     /  \           /  \
     *    l0  r0  ---->  l1  rt
     *   /  \               /  \
     *  l1  r1             r1  r0
     * @throw custom_exception when the selected node doesn't have a left child
     *
     * @param cur
     */
    void right_rotate(tnode *cur) {
        if (cur->left == nullptr)
            throw custom_exception("The node to rotate (left) doesn't have a right child.");
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
        tnode *tmp = new tnode(target->key, target->data, _parent, target->siz);
        tmp->left = copy(target->left, tmp);
        tmp->right = copy(target->right, tmp);
        return tmp;
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
