////实现方法：红黑树
////参考资料：《数据结构：思想与实现》、《算法导论》、OI-Wiki
////增加 type_traits判断迭代器的 assignable 特性
#ifndef SJTU_MAP_HPP
#define SJTU_MAP_HPP

#include <functional>
#include <cstddef>
#include "utility.hpp"
#include "exceptions.hpp"

namespace sjtu {

template<typename T>
struct my_type_traits{
    using iterator_assignable = typename T::iterator_assignable;
};

struct my_true_type{};
struct my_false_type{};

template<
	class Key,
	class T,
	class Compare = std::less<Key>
> class map {
public:
	typedef pair<const Key, T> value_type;
    enum NodeColor { RED, BLACK };
    struct RedBlackNode{
        value_type *data;
        RedBlackNode *p, *son[2];
        NodeColor color;
        RedBlackNode():color(RED){
            p = son[0] = son[1] = nullptr;
            data = nullptr;
        }
        explicit RedBlackNode(const value_type &tmp_data, RedBlackNode *tmp_p = nullptr, NodeColor c = RED){
            son[0] = son[1] = nullptr;
            p = tmp_p;
            color = c;
            data = (value_type*) malloc(sizeof(value_type));
            new (data) value_type(tmp_data);
        }
        RedBlackNode(const RedBlackNode &rhs, RedBlackNode *tmp_p):p(tmp_p), color(rhs.color){
            son[0] = son[1] = nullptr;
            if(!rhs.data) data = nullptr;
            else {
                data = (value_type*) malloc(sizeof(value_type));
                new (data) value_type (*rhs.data);
            }
        }
        ~RedBlackNode(){
            if(data){
                (*data).~value_type();
                free(data);
                data = nullptr;
            }
        }
    };
    class RedBlackTree{
    private:
        RedBlackNode *root;
        bool isEqual(const Key &a, const Key &b) const {return !(Compare()(a, b) || Compare()(b, a));}
    public:
        size_t size;
        RedBlackNode *endNode;
        RedBlackTree() : root(nullptr), size(0){
            endNode = new RedBlackNode();
        }
        RedBlackTree &operator = (const RedBlackTree &other){
            if(this == &other) return *this;
            delete endNode;
            endNode = new RedBlackNode();
            treeClear(root);
            treeClone(other.root, root, nullptr);
            size = other.size;
            return *this;
        }
        ~RedBlackTree(){
            treeClear(root);
            delete endNode;
            size = 0;
        }
        void findNext(RedBlackNode * &t) const {
            if(t->son[1]){
                t = t->son[1];
                while(t->son[0]) t = t->son[0];
            }
            else{
                while(t->p && t->p->son[1] == t) t = t->p;
                t = t->p;
            }
            if(t == nullptr) t = endNode;
        }
        void findLast(RedBlackNode * &t) const {
            if(t == endNode){
                t = root;
                if(t) while(t->son[1]) t = t->son[1];
            }
            else{
                if(t->son[0]){
                    t = t->son[0];
                    while(t->son[1]) t = t->son[1];
                }
                else{
                    while(t->p && t->p->son[0] == t) t = t->p;
                    t = t->p;
                }
            }
        }
        RedBlackNode *getMin() const {
            RedBlackNode *tmp = root;
            if(root == nullptr) return endNode;
            while(tmp->son[0]) tmp = tmp->son[0];
            return tmp;
        }
        RedBlackNode *find(const Key &x) const{
            RedBlackNode *t = root;
            while(t != nullptr && !isEqual(t->data->first, x))
                Compare()(x, t->data->first) ? t = t->son[0] : t = t->son[1];
            return t;
        }
        void treeMakeEmpty(){
            treeClear(root);
            size = 0;
        }
        void treeInsert(const value_type &x){
            if(!root){
                root = new RedBlackNode(x, nullptr, BLACK);
                size++;
                return;
            }
            RedBlackNode *t = root, *fa = nullptr;
            while(true){
                if(t){
                    if(t->son[0] && t->son[1] && t->son[0]->color == RED && t->son[1]->color == RED){
                        t->son[0]->color = t->son[1]->color = BLACK;
                        t->color = RED;
                        insertAdjust(t);
                    }
                    fa = t;
                    t = (Compare()(x.first, t->data->first) ? t->son[0] : t->son[1]);
                }
                else {
                    t = new RedBlackNode(x, fa, RED);
                    size++;
                    Compare()(x.first, fa->data->first) ? fa->son[0] = t : fa->son[1] = t;
                    t->p = fa;
                    insertAdjust(t);
                    root->color = BLACK; //如果根节点是被旋转上去的红结点，需要改变根节点的颜色。
                    return;
                }
            }
        }
        void treeRemove(const Key &x){
            RedBlackNode *t, *p, *c;
            if(!root) return;
            if(isEqual(root->data->first, x) && root->son[0] == nullptr && root->son[1] == nullptr){
                delete root;
                root = nullptr;
                size--;
                return;
            }
            p = c = t = root;
            while(true){
                removeAdjust(p, c, t, x);
                if(isEqual(c->data->first, x) && c->son[0] && c->son[1]){
                    RedBlackNode *alter = c->son[1], *alt_fa, *c_fa, *tmp_node;
                    NodeColor alt_color;
                    while(alter->son[0]) alter = alter->son[0];
                    alt_fa = alter->p, c_fa = c->p;
                    alt_color = alter->color;
                    if(alt_fa == c){//处理特殊情况
                        alter->color = c->color, c->color = alt_color;
                        if(c_fa) c == c_fa->son[0] ? c_fa->son[0] = alter : c_fa->son[1] = alter;
                        alter->p = c_fa;
                        tmp_node = c->son[0];
                        c->son[0] = alter->son[0]; if(c->son[0]) c->son[0]->p = c;
                        c->son[1] = alter->son[1]; if(c->son[1]) c->son[1]->p = c;
                        alter->son[0] = tmp_node, alter->son[0]->p = alter;
                        alter->son[1] = c, c->p = alter;
                    }
                    else{
                        alter->color = c->color, c->color = alt_color;
                        if(c_fa) c == c_fa->son[0] ? c_fa->son[0] = alter : c_fa->son[1] = alter;
                        alter->p = c_fa;
                        alt_fa->son[0] = c, c->p = alt_fa;
                        RedBlackNode *alter_sonR = alter->son[1];
                        alter->son[0] = c->son[0], c->son[0]->p = alter;
                        alter->son[1] = c->son[1], c->son[1]->p = alter;
                        c->son[0] = nullptr;
                        c->son[1] = alter_sonR; if(c->son[1]) c->son[1]->p = c;
                    }
                    if(c == root) root = alter;
                    p = alter;
                    c = alter->son[1];
                    t = alter->son[0];
                    continue;
                }
                if(isEqual(c->data->first, x)){
                    p->son[0] == c ? p->son[0] = c->son[1] : p->son[1] = c->son[1];
                    delete c;
                    c = nullptr;
                    size--;
                    root->color = BLACK;
                    return;
                }
                p = c;
                c = (Compare()(x, c->data->first) ? c->son[0] : c->son[1]);
                t = (p->son[0] == c ? p->son[1] : p->son[0]);
            }
        }
    private:
        bool allBLACK(RedBlackNode *t){
            return !((t->son[0] && t->son[0]->color == RED) || (t->son[1] && t->son[1]->color == RED));
        }
        void treeClone(const RedBlackNode *rhs, RedBlackNode * &cur, RedBlackNode *f){
            if(rhs == nullptr) return;
            cur = new RedBlackNode(*rhs, f);
            treeClone(rhs->son[0], cur->son[0], cur);
            treeClone(rhs->son[1], cur->son[1], cur);
        }
        void treeClear(RedBlackNode * &rt){
            if(rt == nullptr) return;
            treeClear(rt->son[0]);
            treeClear(rt->son[1]);
            delete rt;
            rt = nullptr;
        }
        void insertAdjust(RedBlackNode *t){
            RedBlackNode *f = t->p, *gf;
            gf = (f == nullptr ? nullptr : f->p);
            if(f == nullptr || f->color == BLACK) return;
            if(f == root){
                f->color = BLACK;
                return;
            }
            if(gf && gf->son[0] == f){
                if(f->son[0] == t) LL(gf);
                else LR(gf);
            }
            else {
                if(f->son[1] == t) RR(gf);
                else RL(gf);
            }
        }
        void removeAdjust(RedBlackNode * &p, RedBlackNode * &c, RedBlackNode * &t,const Key &del){
            if(c->color == RED) return;
            if(c == root){
                if(c->son[0] && c->son[1] && c->son[0]->color == c->son[1]->color){
                    c->color = RED;
                    c->son[0]->color = c->son[1]->color = BLACK;
                    return;
                }
            }
            if(allBLACK(c)){//x 有两个黑儿子
                if(allBLACK(t)){//t 有两个黑儿子
                    c->color = RED;
                    t->color = RED;
                    p->color = BLACK;
                }
                else{//t 有红色儿子
                    if(p->son[0] == t){//t 为 p 的左儿子
                        if(t->son[0] && t->son[0]->color == RED){//t 有外侧红儿子
                            LL(p);
                            c->color = RED;
                            t->color = RED;
                            t->son[0]->color = BLACK;
                            p->color = BLACK;
                            t = p->son[0];
                        }
                        else{//t 有内侧红儿子
                            LR(p);
                            p->p->color = RED;
                            c->color = RED;
                            p->color = BLACK;
                            t->color = BLACK;
                            t = p->son[0];
                        }
                    }
                    else{// t 为 p 的右儿子
                        if(t->son[1] && t->son[1]->color == RED){//t 有外侧红儿子
                            RR(p);
                            c->color = RED;
                            t->color = RED;
                            t->son[1]->color = BLACK;
                            p->color = BLACK;
                            t = p->son[1];
                        }
                        else{//t 有内侧红儿子
                            RL(p);
                            p->p->color = RED;
                            c->color = RED;
                            t->color = BLACK;
                            p->color = BLACK;
                            t = p->son[1];
                        }
                    }
                }
            }
            else{//x 至少有一个红儿子
                if(isEqual(c->data->first, del)){//x 是被删节点
                    if(c->son[0] && c->son[1]){//x 有两个儿子
                        if(c->son[1]->color == BLACK){
                            LL(c);
                            c->color = RED;
                            c->p->color = BLACK;
                            p = c->p;
                            t = p->son[0];
                        }
                        return;
                    }
                    if(c->son[0]){//x 只有左儿子
                        LL(c);
                        c->color = RED;
                        c->p->color = BLACK;
                        p = c->p;
                        t = p->son[0];
                    }
                    else{//x 只有右儿子
                        RR(c);
                        c->color = RED;
                        c->p->color = BLACK;
                        p = c->p;
                        t = p->son[1];
                    }
                }
                else{//x 不是被删节点
                    //往下走一层
                    p = c;
                    c = (Compare()(del, p->data->first) ? p->son[0] : p->son[1]);
                    t = (c == p->son[0] ? p->son[1] : p->son[0]);
                    if(c->color == BLACK){//如果新的 x 结点为黑结点
                        if(t == p->son[1]){//新的 x 是左儿子
                            RR(p);
                            p->color = RED;
                            t->color = BLACK;
                        }
                        else{//新的 x 是右儿子
                            LL(p);
                            p->color = RED;
                            t->color = BLACK;
                        }
                        c = p;
                        p = t;
                        t = (c == p->son[0] ? p->son[1] : p->son[0]);
                        removeAdjust(p, c, t, del);
                    }
                }
            }
        }
        void LL(RedBlackNode *gf){
            RedBlackNode *p = gf->son[0], *fa = gf->p;
            if(fa) fa->son[0] == gf ? fa->son[0] = p : fa->son[1] = p; p->p = fa;
            gf->son[0] = p->son[1]; if(gf->son[0]) gf->son[0]->p = gf;
            p->son[1] = gf, gf->p = p;
            p->color = BLACK, gf->color = RED;
            while(root->p) root = root->p;
        }
        void RR(RedBlackNode *gf){
            RedBlackNode *p = gf->son[1], *fa = gf->p;
            if(fa) fa->son[0] == gf ? fa->son[0] = p : fa->son[1] = p; p->p = fa;
            gf->son[1] = p->son[0]; if(gf->son[1]) gf->son[1]->p = gf;
            p->son[0] = gf, gf->p = p;
            p->color = BLACK, gf->color = RED;
            while(root->p) root = root->p;
        }
        void LR(RedBlackNode *gf){
            RedBlackNode *p = gf->son[0], *t = p->son[1], *fa = gf->p;
            if(fa) fa->son[0] == gf ? fa->son[0] = t : fa->son[1] = t; t->p = fa;
            gf->son[0] = t->son[1]; if(gf->son[0]) gf->son[0]->p = gf;
            p->son[1] = t->son[0]; if(p->son[1]) p->son[1]->p = p;
            t->son[0] = p, p->p = t;
            t->son[1] = gf, gf->p = t;
            t->color = BLACK, gf->color = RED;
            while(root->p) root = root->p;
        }
        void RL(RedBlackNode *gf){
            RedBlackNode *p = gf->son[1], *t = p->son[0], *fa = gf->p;
            if(fa) fa->son[0] == gf ? fa->son[0] = t : fa->son[1] = t; t->p = fa;
            gf->son[1] = t->son[0]; if(gf->son[1]) gf->son[1]->p = gf;
            p->son[0] = t->son[1]; if(p->son[0]) p->son[0]->p = p;
            t->son[1] = p, p->p = t;
            t->son[0] = gf, gf->p = t;
            t->color = BLACK, gf->color = RED;
            while(root->p) root = root->p;
        }
    };
    RedBlackTree Tree;
	class const_iterator;
	class iterator {
    public:
        RedBlackNode *p;
        const map *from;
        using iterator_assignable = my_true_type;
	public:
		iterator() : p(nullptr), from(nullptr){}
        iterator(RedBlackNode *tmp_p, const map *tmp_from){
            p = tmp_p;
            from = tmp_from;
        }
		iterator(const iterator &other) {
			p = other.p;
            from = other.from;
		}
        iterator &operator = (const iterator &other){
            if(this == &other) return *this;
            p = other.p;
            from = other.from;
            return *this;
        }
		iterator operator++(int) {
            iterator tmp(*this);
            if(p == from->Tree.endNode || p == nullptr) throw invalid_iterator();
            from->Tree.findNext(p);
            return tmp;
        }
		iterator & operator++() {
            if(p == from->Tree.endNode || p == nullptr) throw invalid_iterator();
            from->Tree.findNext(p);
            return *this;
        }
		iterator operator--(int) {
            iterator tmp(*this);
            if(*this == from->cbegin() || p == nullptr) throw invalid_iterator();
            from->Tree.findLast(p);
            return tmp;
        }
		iterator & operator--() {
            if(*this == from->cbegin() || p == nullptr) throw invalid_iterator();
            from->Tree.findLast(p);
            return *this;
        }
		value_type & operator*() const {
            if(p == from->Tree.endNode || p == nullptr) throw invalid_iterator();
            return *(p->data);
        }
		bool operator==(const iterator &rhs) const {
            return p == rhs.p;
        }
		bool operator==(const const_iterator &rhs) const {
            return p == rhs.p;
        }
		bool operator!=(const iterator &rhs) const {
            return !(*this == rhs);
        }
		bool operator!=(const const_iterator &rhs) const {
            return !(*this == rhs);
        }
		value_type* operator->() const noexcept {
            if(p == from->Tree.endNode || p == nullptr) throw invalid_iterator();
            return p->data;
        }
	};
	class const_iterator {
    public:
        RedBlackNode *p;
        const map *from;
        using iterator_assignable = my_false_type;
    public:
        const_iterator() :p(nullptr), from(nullptr) {}
        const_iterator(RedBlackNode *tmp_p, const map *tmp_from){
            p = tmp_p;
            from = tmp_from;
        }
        const_iterator(const iterator &other){
            p = other.p;
            from = other.from;
        }
        const_iterator(const const_iterator &other) {
            p = other.p;
            from = other.from;
        }
        const_iterator &operator = (const iterator &other){
            p = other.p;
            from = other.from;
            return *this;
        }
        const_iterator &operator = (const const_iterator &other){
            if(this == &other) return *this;
            p = other.p;
            from = other.from;
            return *this;
        }
        const_iterator operator++(int) {
            const_iterator tmp(*this);
            if(p == from->Tree.endNode || p == nullptr) throw invalid_iterator();
            from->Tree.findNext(p);
            return tmp;
        }
        const_iterator & operator++() {
            if(p == from->Tree.endNode || p == nullptr) throw invalid_iterator();
            from->Tree.findNext(p);
            return *this;
        }
        const_iterator operator--(int) {
            const_iterator tmp(*this);
            if(*this == from->cbegin() || p == nullptr) throw invalid_iterator();
            from->Tree.findLast(p);
            return tmp;
        }
        const_iterator & operator--() {
            if(*this == from->cbegin() || p == nullptr) throw invalid_iterator();
            from->Tree.findLast(p);
            return *this;
        }
        const value_type & operator*() const {
            if(p == nullptr || p == from->Tree.endNode) throw invalid_iterator();
            return *(p->data);
        }
        bool operator==(const iterator &rhs) const {
            return p == rhs.p;
        }
        bool operator==(const const_iterator &rhs) const {
            return p == rhs.p;
        }
        bool operator!=(const iterator &rhs) const {
            return !(*this == rhs);
        }
        bool operator!=(const const_iterator &rhs) const {
            return !(*this == rhs);
        }
        const value_type* operator->() const noexcept {
            if(p == nullptr || p == from->Tree.endNode) throw invalid_iterator();
            return p->data;
        }
	};
	map() = default;
	map(const map &other){
        Tree = other.Tree;
    }
	map & operator=(const map &other) {
        Tree = other.Tree;
        return *this;
    }
	~map() {}
	T & at(const Key &key) {
        RedBlackNode *res = Tree.find(key);
        if(res == nullptr) throw index_out_of_bound();
        return res->data->second;
    }
	const T & at(const Key &key) const {
        RedBlackNode *res = Tree.find(key);
        if(res == nullptr) throw index_out_of_bound();
        return res->data->second;
    }
	T & operator[](const Key &key) {
        RedBlackNode *res = Tree.find(key);
        if(res == nullptr)
            return insert(value_type(key, T())).first.p->data->second;
        else return res->data->second;
    }
	const T & operator[](const Key &key) const {
        RedBlackNode *res = Tree.find(key);
        if(res == nullptr) throw index_out_of_bound();
        return res->data->second;
    }
	iterator begin() {
        return iterator(Tree.getMin(), this);
    }
	const_iterator cbegin() const {
        return const_iterator(Tree.getMin(), this);
    }
	iterator end() {
        return iterator(Tree.endNode, this);
    }
	const_iterator cend() const {
        return const_iterator(Tree.endNode, this);
    }
	bool empty() const {return !Tree.size;}
	size_t size() const {return Tree.size;}
	void clear() {Tree.treeMakeEmpty();}
	pair<iterator, bool> insert(const value_type &value) {
        RedBlackNode *res = Tree.find(value.first);
        if(res != nullptr) return pair<iterator, bool>(iterator(res, this), 0);
        Tree.treeInsert(value);
        res = Tree.find(value.first);
        return pair<iterator, bool>(iterator(res, this), 1);
    }
	void erase(iterator pos) {
        if(pos == this->end() || pos.p == nullptr || pos.from != this) throw invalid_iterator();
        Tree.treeRemove(pos.p->data->first);
    }
	size_t count(const Key &key) const {
        RedBlackNode *res = Tree.find(key);
        return res == nullptr ? 0 : 1;
    }
	iterator find(const Key &key) {
        RedBlackNode *res = Tree.find(key);
        if(res == nullptr) return end();
        return iterator(res, this);
    }
	const_iterator find(const Key &key) const {
        RedBlackNode *res = Tree.find(key);
        if(res == nullptr) return cend();
        return const_iterator(res, this);
    }
};

}

#endif
