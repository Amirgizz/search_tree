#pragma once

#include <vector>
#include <cstddef>

template<class T>
class Node {
public:
    std::vector<T> keys;

    std::vector<Node *> c;

    Node *p;

    Node() : p(nullptr) {}

    explicit Node<T>(T value) : keys({value}), p(nullptr) {}

    Node(Node<T> *f, Node<T> *s) : c({f, s}), p(nullptr) {}

    Node<T>(Node<T> &original) : keys(original.keys), c(original.c), p(original.p) {
        for (auto e: c) {
            e->p = this;
        }
    }

    explicit Node<T>(Node<T> *original){
        if (original == nullptr) {
            p = nullptr;
        } else {
            keys = original->keys;
            c = original->c;
            p = original->p;
            for (auto e: c) {
                e->p = this;
            }
        }
    }

    ~Node() = default;
};

template<class T>
T get_max(Node<T> *v) {
    return v->keys.back();
}


template<class T>
class Set {

    Node<T> *root;

    size_t sz;

    void splitParent(Node<T> *t) {
        if (t->c.size() > 3) {
            auto *right = new Node<T>(t->c[2], t->c[3]);
            right->c[0]->p = right->c[1]->p = right;
            t->c.pop_back();
            t->c.pop_back();

            update(right);
            update(t);

            auto p = t->p;
            if (p != nullptr) {
                addChild(p, right);
                splitParent(p);
            } else {
                root = new Node<T>(t, right);
                t->p = right->p = root;
                update(root);
            }
        } else {
            update(t);
            while (t->p != nullptr) {
                t = t->p;
                update(t);
            }
        }
    }

    void update(Node<T> *t) {
        t->keys.resize(t->c.size());
        for (int i = 0; i < t->c.size(); ++i) {
            t->keys[i] = get_max(t->c[i]);
        }
    }

    void addChild(Node<T> *p, Node<T> *n) {
        p->c.push_back(n);
        n->p = p;
        for (int i = int(p->c.size()) - 2; i >= 0 && get_max(p->c[i + 1]) < get_max(p->c[i]); --i) {
            std::swap(p->c[i], p->c[i + 1]);
        }
        update(p);
    }

    Node<T> *findNode(T x) const {
        if (root == nullptr) return nullptr;
        auto t = root;
        while (t->c.size() > 0) {
            if (t->c.size() == 2) {
                if (t->keys[0] < x) {
                    t = t->c[1];
                } else {
                    t = t->c[0];
                }
            } else if (t->c.size() == 3) {
                if (t->keys[1] < x) {
                    t = t->c[2];
                } else if (t->keys[0] < x) {
                    t = t->c[1];
                } else {
                    t = t->c[0];
                }
            } else {
                t = t->c[0];
            }
        }
        return t;
    }

public:
    class iterator {
        Node<T> *n;
        bool is_end;
    public:
        iterator():n(nullptr), is_end(true){
        }
        explicit iterator(Node<T> *n, bool is_end = false) : n(n), is_end(is_end) {}

        iterator(iterator const &original) : n(original.n), is_end(original.is_end) {}

        iterator& operator=(iterator s) {
            n = s.n;
            is_end = s.is_end;
            return *this;
        }

        iterator operator++() {
            auto cur = n;
            while (cur->p != nullptr && cur->p->c.back() == cur) {
                cur = cur->p;
            }
            if (cur->p == nullptr) {
                this->is_end = true;
                return *this;
            }
            if (cur->p->c[0] == cur) {
                cur = cur->p->c[1];
            } else {
                cur = cur->p->c[2];
            }
            while (cur->c.size() > 0) {
                cur = cur->c[0];
            }
            n = cur;
            return *this;
        }

        iterator operator++(int) {
            iterator s(*this);
            ++*this;
            return s;
        }

        iterator operator--() {
            if (is_end) {
                is_end = false;
                return *this;
            }
            auto cur = n;
            while (cur->p != nullptr && cur->p->c[0] == cur) {
                cur = cur->p;
            }
            if (cur->p == nullptr) {
                // There should be an error?
                this->is_end = true;
                return *this;
            }
            if (cur->p->c[1] == cur) {
                cur = cur->p->c[0];
            } else {
                cur = cur->p->c[1];
            }
            while (cur->c.size() > 0) {
                cur = cur->c.back();
            }
            n = cur;
            return *this;
        }

        iterator operator--(int) {
            iterator s(*this);
            --*this;
            return s;
        }

        T operator*() {
            return n->keys[0];
        }

        T const *operator->() const {
            return &n->keys[0];
        }

        bool operator==(iterator other) const {
            if (is_end || other.is_end) {
                return is_end == other.is_end;
            }
            return n == other.n;
        }

        bool operator!=(iterator other) const {
            if (is_end || other.is_end) {
                return is_end != other.is_end;
            }
            return n != other.n;
        }
    };

public:
    Set() : root(nullptr), sz(0) {
    }

    template<typename Container>
    Set(Container c):root(nullptr), sz(0) {
        for (auto e: c) {
            insert(e);
        }
    }

    template <typename Iterator>
    Set(Iterator b, Iterator e):root(nullptr), sz(0) {
        while (b != e) {
            insert(*b);
            ++b;
        }
    }

    Set(std::initializer_list<T> list):root(nullptr), sz(0) {
        for (auto e: list) {
            insert(e);
        }
    }

    Set(Set<T> &original) :root(nullptr), sz(0){
        for (auto e: original) {
            insert(e);
        }
    }

    void rec_del(Node<T> *v) {
        if (v == nullptr) return;
        for (auto e: v->c) {
            rec_del(e);
        }
        delete v;
    }

    Set &operator=(const Set<T> &original) {
        if (&original == this) {
            return *this;
        }
        rec_del(root);
        sz = 0;
        root = nullptr;

        for (auto e: original) {
            insert(e);
        }
        return *this;
    }


    ~Set() {
        rec_del(root);
    }

    size_t size() const {
        return sz;
    }

    bool empty() const {
        return sz == 0;
    }

    void insert(T x) {
        auto n = new Node<T>(x);
        if (root == nullptr) {
            root = n;
            ++sz;
            return;
        }
        auto a = findNode(x);

        if (!(a->keys[0] < x) && !(x < a->keys[0])) {
            delete n;
            return;
        }

        ++sz;
        if (a->p != nullptr) {
            addChild(a->p, n);
            splitParent(a->p);
        } else {
            auto p = new Node<T>(a, n);
            a->p = n->p = p;
            if (get_max(p->c[1]) < get_max(p->c[0])) {
                std::swap(p->c[0], p->c[1]);
            }
            update(p);
            root = p;
        }
    }

    void erase(T x) {
        auto t = findNode(x);
        if (t == nullptr) {
            return;
        }
        if (t->keys[0] < x || x < t->keys[0]) {
            return;
        }

        --sz;

        if (t->p == nullptr) {
            delete t;
            root = nullptr;
            return;
        }
        while (true) {
            auto p = t->p;
            auto it = p->c.begin();
            while (it != p->c.end()) {
                if (*it == t) {
                    delete *it;
                    p->c.erase(it);
                    break;
                }
                ++it;
            }
            update(p);

            if (p->c.size() == 1) {
                if (p->p == nullptr) {
                    root = p->c[0];
                    root->p = nullptr;
                    delete p;
                    return;
                }

                int id = (p->p->c[0] == p ? 0 : p->p->c[1] == p ? 1 : 2);
                if (id > 0) {
                    auto l = p->p->c[id - 1];
                    if (l->c.size() == 3) {
                        auto adopted = l->c[2];
                        addChild(p, adopted);
                        l->c.pop_back();
                        update(l);
                        update(p->p);
                        return;
                    }
                }
                if (id + 1 < p->p->c.size()) {
                    auto r = p->p->c[id + 1];
                    if (r->c.size() == 3) {
                        auto adopted = r->c[0];
                        addChild(p, adopted);
                        r->c.erase(r->c.begin());
                        update(r);
                        update(p->p);
                        return;
                    }
                }

                if (id > 0) {
                    auto l = p->p->c[id - 1];
                    auto adopted = p->c[0];
                    addChild(l, adopted);
                    p->c.pop_back();
                    update(p);
                    update(l);
                    update(p->p);
                } else {
                    auto r = p->p->c[id + 1];
                    auto adopted = p->c[0];
                    addChild(r, adopted);
                    p->c.pop_back();
                    update(p);
                    update(r);
                    update(p->p);
                }
                t = p;
            } else if (p->c.size() == 2) {
                return;
            }
        }
    }

    iterator begin() const {
        if (root == nullptr) {
            return end();
        }
        Node<T> *t = root;
        while (t->c.size() > 0) {
            t = t->c[0];
        }
        return iterator(t);
    }

    iterator end() const {
        if (root == nullptr) {
            return iterator(nullptr, true);
        }
        Node<T> *t = root;
        while (t->c.size() > 0) {
            t = t->c.back();
        }
        return iterator(t, true);
    }

    iterator find(T x) const {
        auto t = findNode(x);
        if (t == nullptr) {
            return end();
        }
        if (!(t->keys[0] < x) && !(x < t->keys[0])) {
            return iterator(t);
        }
        return end();
    }

    iterator lower_bound(T x) const {
        auto t = findNode(x);
        if (t == nullptr) {
            return end();
        }
        if (t->keys[0] < x) {
            return end();
        }
        return iterator(t);
    }
};