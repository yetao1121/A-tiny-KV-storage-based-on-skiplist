/* ************************************************************************
> File Name:     Node.hpp
> Author:        Yetao
> Mail:          1020051524@njupt.edu.cn 
> Created Time:  Mon June 6 2020
> Description:   class for skiplist node
 ************************************************************************/

#include <iostream>

/* 跳表节点的类模板 */
template <typename K, typename V>
class Node {
public:

    Node() {}

    Node(K k, V v, int);

    ~Node();

    K get_key() const;

    V get_value() const;

    void set_value(V);

    Node<K, V>** forward;

    int node_level;

private:
    K key;
    V value;
};

template <typename K, typename V>
Node<K, V>::Node(K k, V v, int level) {
    this->key = k;
    this->value = v;
    this->node_level = level;

    this->forward = new Node<K, V>*[level + 1];

    memset(this->forward, 0, sizeof(Node<K, V>*) * (level + 1));
}

template <typename K, typename V>
Node<K, V>::~Node() {
    delete[] forward;
}

template <typename K, typename V>
K Node<K, V>::get_key() const {
    return key;
}

template <typename K, typename V>
V Node<K, V>::get_value() const {
    return value;
}

template <typename K, typename V>
void Node<K, V>::set_value(V value) {
    this->value = value;
}