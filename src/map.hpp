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

template <typename T> struct my_type_traits { using iterator_assignable = typename T::iterator_assignable; };

struct my_true_type {};
struct my_false_type {};

template <class Key, class T, class Compare = std::less<Key>> class RBTree {
  public:
    /**
     * the internal type of data.
     * it should have a default constructor, a copy constructor.
     * You can use sjtu::map as value_type by typedef.
     */
    typedef pair<const Key, T> value_type;

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
            : data(_data), left(nullptr), right(nullptr), parent(_parent), col(_col), siz(_siz) {}
    } * rt;

  public:
    RBTree() { rt = nullptr; }
    RBTree(const RBTree<Key, T, Compare> &other) { rt = node_copy(other.rt); }

    RBTree &operator=(const RBTree &other) {
        if (this == &other)
            return *this;
        rt = node_copy(other.rt);
        return *this;
    }

    ~RBTree() { node_destruct(rt); }

  public:
    /**
     * @brief checks whether the container is empty
     *
     * @return true if empty
     * @return false otherwise
     */
    bool empty() { return rt == nullptr; }
    /**
     * @brief returns the number of elements.
     *
     * @return size_t
     */
    size_t size() const { return rt ? rt->siz : 0; }
    /**
     * @brief clear the contents
     *
     */
    void clear() { node_destruct(rt); }

  public:
    tnode *find(const Key &key) const {
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
        return cur;
    }

    tnode *first() const {
        tnode *u = rt;
        if (u == nullptr)
            return nullptr;
        while (u->left != nullptr)
            u = u->left;
        return u;
    }

    tnode *last() const {
        tnode *u = rt;
        if (u == nullptr)
            return nullptr;
        while (u->right != nullptr)
            u = u->right;
        return u;
    }

    tnode *prev(tnode *ptr) const {
        if (ptr->left) {
            ptr = ptr->left;
            while (ptr->right != nullptr)
                ptr = ptr->right;
        } else {
            while (ptr != rt && is_left(ptr))
                ptr = ptr->parent;
            if (ptr->parent == nullptr)
                throw invalid_iterator();
            ptr = ptr->parent;
        }
        return ptr;
    }

    tnode *next(tnode *ptr) const {
        if (ptr == nullptr)
            throw invalid_iterator();
        if (ptr->right) {
            ptr = ptr->right;
            while (ptr->left != nullptr)
                ptr = ptr->left;
        } else {
            while (ptr != rt && !is_left(ptr))
                ptr = ptr->parent;
            ptr = ptr->parent;
        }
        return ptr;
    }

  public:
    pair<tnode *, bool> insert(const value_type &value) {
        tnode *cur = rt, *next;
        if (cur == nullptr) { // If the tree is empty
            // Create a new root node, with col = RED, size = 1 and no links to other node
            rt = cur = new tnode(value, nullptr, BLACK, 1);
            return {cur, true};
        }
        // Here we try to ensure the node we found cannot have a red sibling,
        // which requires that every node on the path doesn't have two red descendants.
        while (true) {
            int comp = Compare()(cur->data.first, value.first) - Compare()(value.first, cur->data.first);
            if (!comp) // Find the same element
                return {cur, false};
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
        // Change the size backward
        size_adjust_upward(cur, 1);
        // After inserted, fix the red-red link again
        insert_adjust(cur);
        return {cur, true};
    }

    void erase(const Key &key) {
        if (!Compare()(rt->data.first, key) && !Compare()(rt->data.first, key) && rt->left == nullptr && rt->right == nullptr) {
            delete rt;
            rt = nullptr;
            return;
        }
        tnode *cur = rt;
        // Here we try to ensure every node we met is red, which then conducts the node we want to delete is red
        while (true) {
            if (cur == nullptr)
                return;
            // Change the current node to red
            erase_adjust(cur, key);
            int comp = Compare()(cur->data.first, key) - Compare()(key, cur->data.first);
            // If we find the node with two descendents,
            // swap the data with its 'next' node and delete that node then
            if (!comp && cur->left != nullptr && cur->right != nullptr) {
                tnode *next = cur->right;
                while (next->left)
                    next = next->left;
                node_swap(cur, next);
                cur = next->right;
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

  private:
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

    void size_adjust(tnode *cur) {
        cur->siz = 1;
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
                 *      B1                       B2
                 *     / \                      / \
                 *    R2  B3     ------->  (cur)R  R1
                 *   /                              \
                 *  R(cur)                           B3
                 */
                right_rotate(cur->parent->parent);
                std::swap(cur->parent->col, sibling(cur)->col);
            } else {
                /** Change the tree by
                 *      B1                     B1            R(cur)         B(cur)
                 *     / \                    / \           / \            /  \
                 *    R2  B3     -----> (cur)R   B3 -----> R2  B1  -----> R2  R1
                 *     \                   /                    \               \
                 *      R(cur)            R2                     B3             B3
                 */
                left_rotate(cur->parent);
                right_rotate(cur->parent);
                std::swap(cur->col, cur->right->col);
            }
        } else {
            if (is_left(cur)) {
                /** Change the tree by
                 *      B1               B1                 R(cur)          B(cur)
                 *     / \              / \                / \             / \
                 *    B2  R3    -----> B2  R(cur) ----->  B1  R3   -----> R1  R3
                 *       /                  \            /               /
                 *      R(cur)               R3         B2              B2
                 */
                right_rotate(cur->parent);
                left_rotate(cur->parent);
                std::swap(cur->col, cur->left->col);
            } else {
                /** Change the tree by
                 *       B1                R3                B3
                 *      / \               / \               /  \
                 *     B2  R3  ------->  B1  R(cur) -----> R1  R(cur)
                 *          \           /                 /
                 *          R(cur)     B2                B2
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
        /**
         * if del < cur->key, comp = 0 - 1 = -1
         * if del = cur->key, comp = 0 - 0 = 0
         * if del > cur->key, comp = 1 - 0 = 1
         */
        int comp = Compare()(cur->data.first, del) - Compare()(del, cur->data.first);
        if (has_black_descendants(cur)) {
            // note that sib == nullptr suggest it has no siblings or it's the root
            tnode *sib = sibling(cur);
            // If the (black) sibling node has two black descendants or it doesn't have a sibling
            /** Case 1-1: cur (black, black, black), sib null or (black, black, black)
             *  That would not change the structure of the tree
             */
            if (sib == nullptr || has_black_descendants(sib)) {
                if (cur->parent)
                    cur->parent->col = BLACK;
                if (sib)
                    sib->col = RED;
                cur->col = RED;
                return;
            }
            // If the current node has a sibling which has red descendent
            /** Case 1-2: cur (black, black, black) at left, sib with outer red at right
             *      R(par)                    B(sib)                 R(sib)
             *     /   \                     /  \                   /   \
             *    B(cur)B(sib) -------->  R(par) R2 -------->     B(par) B2
             *           \               /                       /
             *            R2            B(cur)                R(cur)
             *  That is a left_rotate on par and change color then
             */
            if (is_left(cur) && sib->right && sib->right->col == RED) {
                left_rotate(cur->parent);
                sib->col = RED;
                cur->parent->col = BLACK;
                sib->right->col = BLACK;
                cur->col = RED;
                return;
            }
            /** Case 1-3: cur (black, black, black) at right, sib with outer red at left
             *     R(par)                   B(sib)                 R(sib)
             *    /    \                   /  \                   /  \
             *   B(sib) B(cur) -------->  R1   R(par)  ------->  B1  B(par)
             *  /                                \                     \
             * R1                                 B(cur)                R(cur)
             *  That is a right_rotate on par and change color then
             */
            if (!is_left(cur) && sib->left && sib->left->col == RED) {
                right_rotate(cur->parent);
                sib->col = RED;
                sib->left->col = BLACK;
                cur->parent->col = BLACK;
                cur->col = RED;
            }
            /** Case 1-4: cur (black, black, black) at left, sib with inner red at left
             *      R(par)                    R(par)                  R1                   R1
             *     /    \                     /  \                   /  \                 /  \
             *    B(cur) B(sib) -------->  B(cur) R1 -------->    R(par) B(sib) -----> B(par) B(sib)
             *          /                          \             /                    /
             *         R1                          B(sib)       B(cur)              R(cur)
             *  That is a right_rotate on sib, left_rotate on par and change color then
             */
            if (is_left(cur) && sib->left && sib->left->col == RED) {
                right_rotate(sib);
                left_rotate(cur->parent);
                std::swap(cur->col, cur->parent->col);
            }
            /** Case 1-5: cur (black, black, black) at right, sib with inner red at right
             *      R(par)                      R(par)                 R1                   B1
             *     /    \                      /  \                   /  \                 /  \
             *    B(sib) B(cur) -------->    R1   B(cur) --------> B(sib) R(par) -----> B(sib) B(sib)
             *     \                        /                               \                   \
             *      R1                    B(sib)                           B(cur)                R(cur)
             *  That is a left_rotate on sib, right_rotate on par and change color then
             */
            if (!is_left(cur) && sib->right && sib->right->col == RED) {
                left_rotate(sib);
                right_rotate(cur->parent);
                std::swap(cur->col, cur->parent->col);
            }
        } else {
            // If the current node is the node to be deleted
            if (!comp) {
                // If the current node has two descendents
                if (cur->left && cur->right) {
                    // If the right descendent is black
                    /** Case 2-1: cur (black, red, black), which implies that left node has two black descendents
                     *     B(cur)      R1         B1
                     *    /     ----->  \  ----->  \
                     *   R1              B(cur)    R(cur)
                     */
                    if (cur->right->col == BLACK) {
                        right_rotate(cur);
                        std::swap(cur->col, cur->parent->col);
                    }
                    /** Case 2-2: cur (black, black, red), we'll reach its right node then, so don't need to change anything
                     */
                    else
                        ;
                    return;
                }
                /** Case 2-2: cur (black, red, null)
                 *    B(cur)     R1           B1
                 *   /    ----->  \   ------>  \
                 *  R1             B(cur)      R(cur)
                 */
                if (cur->left) {
                    right_rotate(cur);
                    std::swap(cur->col, cur->parent->col);
                }
                /** Case 2-3: cur (black, null, red)
                 *   B(cur)        R1          B1
                 *    \    -----> /    -----> /
                 *    R1         B(cur)      R(cur)
                 */
                if (cur->right) {
                    left_rotate(cur);
                    std::swap(cur->col, cur->parent->col);
                    return;
                }
            }
            // If the current node isn't the node to be deleted
            else {
                /** Case 2-3: we'll reach a red node or nullptr then, so don't need to change anything
                 */
                if ((comp < 0 && (!cur->left || cur->left->col == RED)) ||
                    (comp > 0 && (!cur->right || cur->right->col == RED)))
                    return;
                /** Case 2-4: cur (black, black, red) and we'll reach its left node then
                 *      B(cur)          R2              B2
                 *     / \              /              /
                 *    B1 R2    -----> B(cur) ----->   R(cur)
                 *                   /               /
                 *                  B1              B1
                 * That is a left_rotate on cur and change color then
                 */
                if (comp < 0 && cur->left->col == BLACK) {
                    left_rotate(cur);
                    std::swap(cur->col, cur->parent->col);
                    return;
                }
                /** Case 2-5: cur (black, black, red) and we'll reach its left node then
                 *      B(cur)       R1              B1
                 *     / \            \               \
                 *    R1 B2    ----->  B(cur) ----->   R(cur)
                 *                      \               \
                 *                      B2               B2
                 * That is a left_rotate on cur and change color then
                 */
                if (comp > 0 && cur->right->col == BLACK) {
                    right_rotate(cur);
                    std::swap(cur->col, cur->parent->col);
                    return;
                }
            }
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
    tnode *node_copy(tnode *target, tnode *_parent = nullptr) {
        if (target == nullptr)
            return nullptr;
        tnode *tmp = new tnode(target->data, _parent, target->col, target->siz);
        tmp->left = node_copy(target->left, tmp);
        tmp->right = node_copy(target->right, tmp);
        return tmp;
    }

    /**
     * @brief swap the node with another tree node, since the Key type doesn't even support the f**king assignment operation
     *
     * @param cur
     * @param target
     */
    void node_swap(tnode *cur, tnode *target) {
        // parent
        int cur_is_left = target->parent ? is_left(target) : 0;
        int tar_is_left = cur->parent ? is_left(cur) : 0;
        std::swap(cur->parent, target->parent);
        if (cur->parent) {
            if (cur_is_left)
                cur->parent->left = cur;
            else
                cur->parent->right = cur;
        } else {
            rt = cur;
        }
        if (target->parent) {
            if (tar_is_left)
                target->parent->left = target;
            else
                target->parent->right = target;
        } else {
            rt = target;
        }
        // left
        std::swap(cur->left, target->left);
        if (cur->left)
            cur->left->parent = cur;
        if (target->left)
            target->left->parent = target;
        // right
        std::swap(cur->right, target->right);
        if (cur->right)
            cur->right->parent = cur;
        if (target->right)
            target->right->parent = target;
        // color and size
        std::swap(cur->col, target->col);
        std::swap(cur->siz, target->siz);
    }

    /**
     * @brief destruct a tree node and all its progenies by recursion
     *
     * @param target
     */
    void node_destruct(tnode *&target) {
        if (target == nullptr)
            return;
        node_destruct(target->left);
        node_destruct(target->right);
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
    bool is_left(tnode *cur) const {
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
    tnode *sibling(tnode *cur) {
        if (cur == rt)
            return nullptr;
        return is_left(cur) ? cur->parent->right : cur->parent->left;
    }
};

template <class Key, class T, class Compare = std::less<Key>> class map : public sjtu::RBTree<Key, T, Compare> {
  public:
    /**
     * see BidirectionalIterator at CppReference for help.
     *
     * if there is anything wrong throw invalid_iterator.
     *     like it = map.begin(); --it;
     *       or it = map.end(); ++end();
     */
    using tnode = typename RBTree<Key, T, Compare>::tnode;
    using value_type = typename RBTree<Key, T, Compare>::value_type;

    class const_iterator;
    class iterator {
        friend class map;
        friend class const_iterator;

      private:
        /**
         * TODO add data members
         *   just add whatever you want.
         */
        const map *iter;
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
        using value_type = typename RBTree<Key, T, Compare>::value_type;
        using pointer = value_type *;
        using reference = value_type &;
        using iterator_category = std::output_iterator_tag;

        // If you are interested in type_traits, toy_traits_test provides a place to
        // practice. But the method used in that test is old and rarely used, so you
        // may explore on your own.
        // Notice: you may add some code in here and class const_iterator and namespace sjtu to implement toy_traits_test,
        // this part is only for bonus.
        using iterator_assignable = my_true_type;

        iterator() : iter(nullptr), ptr(nullptr) {}
        iterator(const iterator &other) : iter(other.iter), ptr(other.ptr) {}
        iterator(const map *_iter, tnode *_ptr) : iter(_iter), ptr(_ptr) {}
        /**
         * TODO iter++
         */
        iterator operator++(int) {
            if (ptr == nullptr)
                throw invalid_iterator();
            iterator cp = *this;
            ptr = iter->next(ptr);
            return cp;
        }
        /**
         * TODO ++iter
         */
        iterator &operator++() {
            if (ptr == nullptr)
                throw invalid_iterator();
            ptr = iter->next(ptr);
            return *this;
        }
        /**
         * TODO iter--
         */
        iterator operator--(int) {
            iterator cp = *this;
            if (ptr == nullptr)
                ptr = iter->last();
            else
                ptr = iter->prev(ptr);
            if (ptr == nullptr)
                throw invalid_iterator();
            return cp;
        }
        /**
         * TODO --iter
         */
        iterator &operator--() {
            if (ptr == nullptr)
                ptr = iter->last();
            else
                ptr = iter->prev(ptr);
            if (ptr == nullptr)
                throw invalid_iterator();
            return *this;
        }
        /**
         * a operator to check whether two iterators are same (pointing to the same memory).
         */
        value_type &operator*() const { return ptr->data; }
        bool operator==(const iterator &rhs) const { return iter == rhs.iter && ptr == rhs.ptr; }
        bool operator==(const const_iterator &rhs) const { return iter == rhs.iter && ptr != rhs.ptr; }
        /**
         * some other operator for iterator.
         */
        bool operator!=(const iterator &rhs) const { return iter != rhs.iter || ptr != rhs.ptr; }
        bool operator!=(const const_iterator &rhs) const { return iter != rhs.iter || ptr != rhs.ptr; }

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
        const map *iter;
        tnode *ptr;

      public:
        using iterator_assignable = my_false_type;
        
        const_iterator() : iter(nullptr), ptr(nullptr) {}
        const_iterator(const const_iterator &other) : iter(other.iter), ptr(other.ptr) {}
        const_iterator(const iterator &other) : iter(other.iter), ptr(other.ptr) {}
        const_iterator(const map *_iter, tnode *_ptr) : iter(_iter), ptr(_ptr) {}

        const_iterator operator++(int) {
            if (ptr == nullptr)
                throw invalid_iterator();
            const_iterator cp = *this;
            ptr = iter->next(ptr);
            return cp;
        }
        const_iterator &operator++() {
            if (ptr == nullptr)
                throw invalid_iterator();
            ptr = iter->next(ptr);
            return *this;
        }
        const_iterator operator--(int) {
            const_iterator cp = *this;
            if (ptr == nullptr)
                ptr = iter->last();
            else
                ptr = iter->prev(ptr);
            if (ptr == nullptr)
                throw invalid_iterator();
            return cp;
        }
        const_iterator &operator--() {
            if (ptr == nullptr)
                ptr = iter->last();
            else
                ptr = iter->prev(ptr);
            if (ptr == nullptr)
                throw invalid_iterator();
            return *this;
        }

        const value_type &operator*() const { return ptr->data; }

        // And other methods in iterator.
        bool operator==(const const_iterator &rhs) { return iter == rhs.iter && ptr == rhs.ptr; }
        // And other methods in iterator.
        bool operator!=(const const_iterator &rhs) { return iter != rhs.iter || ptr != rhs.ptr; }
        // And other methods in iterator.
        const value_type *operator->() const noexcept { return &ptr->data; }
    };
    /**
     * TODO two constructors
     */
    map() : RBTree<Key, T, Compare>() {}
    map(const map &other) : RBTree<Key, T, Compare>(other) {}
    /**
     * TODO assignment operator
     */
    map &operator=(const map &other) {
        RBTree<Key, T, Compare>::operator=(other);
        return *this;
    }
    /**
     * TODO Destructors
     */
    ~map() {}
    /**
     * TODO
     * access specified element with bounds checking
     * Returns a reference to the mapped value of the element with key equivalent to key.
     * If no such element exists, an exception of type `index_out_of_bound'
     */
    T &at(const Key &key) {
        tnode *res = RBTree<Key, T, Compare>::find(key);
        if (res == nullptr)
            throw index_out_of_bound();
        return RBTree<Key, T, Compare>::find(key)->data.second;
    }
    const T &at(const Key &key) const {
        tnode *res = RBTree<Key, T, Compare>::find(key);
        if (res == nullptr)
            throw index_out_of_bound();
        return RBTree<Key, T, Compare>::find(key)->data.second;
    }
    /**
     * TODO
     * access specified element
     * Returns a reference to the value that is mapped to a key equivalent to key,
     *   performing an insertion if such key does not already exist.
     */
    T &operator[](const Key &key) { return (RBTree<Key, T, Compare>::insert({key, T()}).first->data).second; }
    /**
     * behave like at() throw index_out_of_bound if such key does not exist.
     */
    const T &operator[](const Key &key) const { return at(key); }
    /**
     * return a iterator to the beginning
     */
    iterator begin() { return iterator(this, RBTree<Key, T, Compare>::first()); }
    const_iterator cbegin() const { return const_iterator(this, RBTree<Key, T, Compare>::first()); }
    /**
     * return a iterator to the end
     * in fact, it returns past-the-end.
     */
    iterator end() { return iterator{this, nullptr}; }
    const_iterator cend() const { return const_iterator(this, nullptr); }

  public:
    /**
     * insert an element.
     * return a pair, the first of the pair is
     *   the iterator to the new element (or the element that prevented the insertion),
     *   the second one is true if insert successfully, or false.
     */
    pair<iterator, bool> insert(const value_type &value) {
        auto res = RBTree<Key, T, Compare>::insert(value);
        return {iterator(this, res.first), res.second};
    }
    /**
     * erase the element at pos.
     *
     * throw if pos pointed to a bad element (pos == this->end() || pos points an element out of this)
     */
    void erase(iterator pos) {
        if (pos.iter != this || pos.ptr == nullptr)
            throw index_out_of_bound();
        RBTree<Key, T, Compare>::erase(pos.ptr->data.first);
    }

  public:
    /**
     * Finds an element with key equivalent to key.
     * key value of the element to search for.
     * Iterator to an element with key equivalent to key.
     *   If no such element is found, past-the-end (see end()) iterator is returned.
     */
    iterator find(const Key &key) { return iterator(this, RBTree<Key, T, Compare>::find(key)); }
    const_iterator find(const Key &key) const { return iterator(this, RBTree<Key, T, Compare>::find(key)); }

    /**
     * Returns the number of elements with key
     *   that compares equivalent to the specified argument,
     *   which is either 1 or 0
     *     since this container does not allow duplicates.
     * The default method of check the equivalence is !(a < b || b > a)
     */
    size_t count(const Key &key) const { return find(key) == cend() ? 0 : 1; }
};

template class map<std::string, int>;

} // namespace sjtu

#endif
