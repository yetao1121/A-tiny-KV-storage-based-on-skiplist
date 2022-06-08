/* ************************************************************************
> File Name:     skiplist.hpp
> Author:        Yetao
> Mail:          1020051524@njupt.edu.cn 
> Created Time:  Mon June 6 2020
> Description:   class for skiplist
 ************************************************************************/

#include <iostream>
#include <cstring>
#include <fstream>
#include <mutex>
#include "Node.hpp"
#define STROE_FILE "store/dumpFile"

std::mutex mtx;     // 后面可能会使用多线程来进行压力测试
std::string delimiter = ":";

/* 跳表类模板 */
template <typename K, typename V>
class skiplist {
public:
    skiplist(int);
    ~skiplist();
    int insert_element(K, V);
    void display_list();
    Node<K, V>* search_element(K);
    void delete_element(K);
    void dump_file();
    void load_file();
    int size();

private:
    Node<K, V>* create_node(K, V, int);
    int get_random_level();
    void get_key_value_from_string(const std::string& str, std::string* key, std::string* value);
    bool is_valid_string(const std::string& str);

private:
    // 跳表所允许的最大 level
    int max_level;

    // 跳表当前的最大 level
    int cur_level;

    // 虚拟头节点
    Node<K, V>* head;

    // 文件操作
    std::ofstream file_write;
    std::ifstream file_read;

    // 跳表当前的节点（键值对）个数
    int element_count;
};

template <typename K, typename V>
Node<K, V>* skiplist<K, V>::create_node(const K k, const V v, int level) {
    Node<K, V>* node = new Node<K, V>(k, v, level);
    return node;
}

// 将给定的键值对插入到跳表中
// 如果给定键值已存在，返回 1
// 成功插入返回 0
/* 
                           +------------+
                           |  insert 50 |
                           +------------+
level 4     +-->1+                                                      100
                 |
                 |                      insert +----+
level 3         1+-------->10+---------------> | 50 |          70       100
                                               |    |
                                               |    |
level 2         1          10         30       | 50 |          70       100
                                               |    |
                                               |    |
level 1         1    4     10         30       | 50 |          70       100
                                               |    |
                                               |    |
level 0         1    4   9 10         30   40  | 50 |  60      70       100
                                               +----+

*/

template <typename K, typename V>
int skiplist<K, V>::insert_element(const K key, const V value) {
    mtx.lock();
    Node<K, V>* current = this->head;

    // 创建 update 数组，并初始化为全 0
    // update[i] 为每一个 level 上待插入节点的前驱节点
    Node<K, V>* update[max_level + 1];
    memset(update, 0, sizeof(Node<K, V>*) * (max_level + 1));

    // 从最高 level 来寻找 update[i]
    for (int i = cur_level; i >= 0; i--) {
        while (current->forward[i] != nullptr && current->forward[i]->get_key() < key) {
            current = current->forward[i];
        }
        update[i] = current;
    }

    // 达到 level 0 之后向右移一个位置
    current = current->forward[0];

    // 如果 current 不为空且键值与要插入的键值相等，返回 1
    if (current != nullptr && current->get_key() == key) {
        std::cout << "key: " << key << ", exists" << std::endl;
        mtx.unlock();
        return 1;
    }

    // 如果 current 为空，说明我们要在尾节点插入该键值对
    // 如果 currnet 非空且键值不等于要插入的键值，则需要在update[i] 与 current 之间插入新的键值对
    if (current == nullptr || current->get_key() != key) {

        // 为要插入的 node 生成一个随机 level
        int random_level = get_random_level();

        // if random level is greater than skip list's current level, initialize update value with pointer to header
        // 如果生成的随机 level 大于跳表当前的 cur_level，在高出的上面的部分用 head 来指向（因为cur_level以上暂时只有新插入的节点）
        if (random_level > cur_level) {
            for (int i = cur_level + 1; i < random_level + 1; i++) {
                update[i] = head;
            }
            cur_level = random_level;
        }

        // 使用随机生成的level创建一个新的节点
        Node<K, V>* inserted_node = create_node(key, value, random_level);

        // 插入
        for (int i = 0; i <= random_level; i++) {
            inserted_node->forward[i] = update[i]->forward[i];
            update[i]->forward[i] = inserted_node;
        }
        std::cout << "Successfully inserted key:" << key << ", value:" << value << std::endl;
        this->element_count++;    
    }
    mtx.unlock();
    return 0;
}

// display skip list
template <typename K, typename V>
void skiplist<K, V>::display_list() {
    std::cout << "\n*****Skip List*****" << "\n";
    for (int i = cur_level; i >= 0; i--) {
        Node<K, V>* node = this->head->forward[i];
        std::cout << "Level " << i << ": ";
        while (node != nullptr) {
            std::cout << node->get_key() << ":" << node->get_value() << ";";
            node = node->forward[i];
        }
        std::cout << std::endl;
    }
}

// 将数据载入磁盘做持久化
template <typename K, typename V>
void skiplist<K, V>::dump_file() {
    std::cout << "dump_file-------------------------" << std::endl;
    this->file_write.open(STROE_FILE);
    Node<K, V>* node = this->head->forward[0];

    while (node != nullptr) {
        file_write << node->get_key() << ":" << node->get_value() << "\n";
        std::cout << node->get_key() << ":" << node->get_value() << ";\n";
        node = node->forward[0];
    }

    file_write.flush();
    file_write.close();
    return;
}

// 从磁盘将数据载入内存
template <typename K, typename V>
void skiplist<K, V>::load_file() {
    file_read.open(STROE_FILE);
    std::cout << "load_file-----------------------" << std::endl;
    std::string line;
    std::string* key = new std::string();
    std::string* value = new std::string();
    while (getline(file_read, line)) {
        get_key_value_from_string(line, key, value);
        if (key->empty() || value->empty()) {
            continue;
        }
        insert_element(*key, *value);
        std::cout << "key:" << *key << "value:" << *value << std::endl;
    }
    file_read.close();
    return;
}

// 获得当前跳表大小
template <typename K, typename V>
int skiplist<K, V>::size() {
    return element_count;
}

template <typename K, typename V>
void skiplist<K, V>::get_key_value_from_string(const std::string& str, std::string* key, std::string* value) {
    if (!is_valid_string(str)) {
        return;
    }
    *key = str.substr(0, str.find(delimiter));
    *value = str.substr(str.find(delimiter) + 1, str.length());
}

template <typename K, typename V>
bool skiplist<K, V>::is_valid_string(const std::string& str) {
    if (str.empty()) {
        return false;
    }
    if (str.find(delimiter) == std::string::npos) {
        return false;
    }
    return true;
}


// 从跳表中删除某个键值对
template <typename K, typename V>
void skiplist<K, V>::delete_element(K key) {
    mtx.lock();
    Node<K, V>* current = this->head;
    Node<K, V>* update[max_level + 1];
    memset(update, 0, sizeof(Node<K, V>*) * (max_level + 1));

    // 同样从当前最高level开始
    for (int i = cur_level; i >= 0; i--) {
        while (current->forward[i] != nullptr && current->forward[i]->get_key() < key) {
            current = current->forward[i];
        }
        update[i] = current;
    }

    current = current->forward[0];
    if (current != nullptr && current->get_key() == key) {

        // 从最低level开始，到最高时break
        for (int i = 0; i <= cur_level; i++) {
            
            // if at level i, next node is not target node, break the loop
            if (update[i]->forward[i] != current) {
                break;
            }

            update[i]->forward[i] = current->forward[i];
        }

        // 删除的节点可能会影响cur_level， 所以要更新以下cur_level
        while (cur_level > 0 && head->forward[cur_level] == 0) {
            cur_level--;
        }

        std::cout << "Successfully delete key " << key << std::endl;
        this->element_count--;
    }
    else {
        std::cout << key << "is not find in skiplist" << std::endl;
    }
    mtx.unlock();
    return;
}

// 从跳表中查找某个键值对
/*
                           +------------+
                           |  select 60 |
                           +------------+
level 4     +-->1+                                                      100
                 |
                 |
level 3         1+-------->10+------------------>50+           70       100
                                                   |
                                                   |
level 2         1          10         30         50|           70       100
                                                   |
                                                   |
level 1         1    4     10         30         50|           70       100
                                                   |
                                                   |
level 0         1    4   9 10         30   40    50+-->60      70       100
*/
template<typename K, typename V> 
Node<K, V>* skiplist<K, V>::search_element(K key) {
    std::cout << "search_element--------------------" << std::endl;
    Node<K, V>* current = head;

    // 同样是从当前最高的level来寻找update[i]
    for (int i = cur_level; i >= 0; i--) {
        while (current->forward[i] != nullptr && current->forward[i]->get_key() < key) {
            current = current->forward[i];
        }
    }

    // 找到前驱之后向右移一个位置
    current = current->forward[0];

    // 如果相等，说明已经找到了
    if (current != nullptr && current->get_key() == key ) {
        std::cout << "Found key: " << ", value: " << current->get_value() << std::endl;
        return current;
    }

    // 否则，说明没有要找的键值对
    std::cout << "Not Found Key:" << key << std::endl;
    return nullptr;
}

// 构造函数
template<typename K, typename V> 
skiplist<K, V>::skiplist(int max_level) {
    this->max_level = max_level;
    this->cur_level = 0;
    this->element_count = 0;

    // 使用最大的 max_level 来创建虚拟头节点，保证 head 能指向任意一个节点
    K key;
    V value;
    this->head = new Node<K, V>(key, value, max_level);
}

template<typename K, typename V>
skiplist<K, V>::~skiplist() {
    if (file_write.is_open()) {
        file_write.close();
    }
    if (file_read.is_open()) {
        file_read.close();
    }
    if (head) {
        delete head;
        head = nullptr;
    }
}

template<typename K, typename V>
int skiplist<K, V>::get_random_level() {
    int k = 1;
    while (rand() % 2) {
        k++;
    }
    k = (k < max_level) ? k : max_level;
    return k;
}
