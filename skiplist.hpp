/* ************************************************************************
> File Name:     skiplist.hpp
> Author:        Yetao
> Mail:          1020051524@njupt.edu.cn 
> Created Time:  Mon June 6 2020
> Description:   class for skiplist
 ************************************************************************/

#include "Node.hpp"
#include <iostream>
#include <cstring>
#include <fstream>
#include <mutex>
#define STROE_FILE "store/dumpFile"

std::mutex mtx;     // mutex for critical section
std::string delimiter = ":";

/* 跳表类模板 */
template <typename K, typename V>
class skiplist {
public:
    skiplist(int);
    ~skiplist();
    int insert_element(K, V);
    void display_list();
    bool search_element(K);
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
    // Max level of the skip list
    int max_level;

    // current level of skip list
    int cur_level;

    // pointer to haed node
    Node<K, V>* head;

    // file operator
    std::ofstream file_write;
    std::ifstream file_read;

    // skip list current element count
    int element_count;
};

template <typename K, typename V>
Node<K, V>* skiplist<K, V>::create_node(const K k, const V v, int level) {
    Node<K, V>* node = new Node<K, V>(k, v, level);
    return node;
}

// Insert given key and value in skip list 
// return 1 means element exists  
// return 0 means insert successfully
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

    // create update array and initialize it
    // update is array which put node that the node->forward[i] should be operated later
    Node<K, V>* update[max_level + 1];
    memset(update, 0, sizeof(Node<K, V>*) * (max_level + 1));

    // start from highest level of skip list
    for (int i = cur_level; i >= 0; i--) {
        while (current->forward[i] != nullptr && current->forward[i]->get_key() < key) {
            current = current->forward[i];
        }
        update[i] = current;
    }

    // reached level 0 and forward pointer to right node, which is desired to insert key
    current = current->forward[0];

    // if current node have key equal to search key, we get it
    if (current != nullptr && current->get_key() == key) {
        //std::cout << "key: " << key << ", exists" << std::endl;
        mtx.unlock();
        return 1;
    }

    // if current is nullptr that means we have reached to the end of the level
    // if current's key is not equal to key that means we have to insert node between update[0] and current node
    if (current == nullptr || current->get_key() != key) {

        // generate a random level for insert node
        int random_level = get_random_level();

        // if random level is greater than skip list's current level, initialize update value with pointer to header
        if (random_level > cur_level) {
            for (int i = cur_level + 1; i < random_level + 1; i++) {
                update[i] = head;
            }
            cur_level = random_level;
        }

        // create new node with random level generated
        Node<K, V>* inserted_node = create_node(key, value, random_level);

        // insert node
        for (int i = 0; i <= random_level; i++) {
            inserted_node->forward[i] = update[i]->forward[i];
            update[i]->forward[i] = inserted_node;
        }
        //std::cout << "Successfully inserted key:" << key << ", value:" << value << std::endl;
        this->element_count++;    
    }
    mtx.unlock();
    return 0;
}

// display skip list
template <typename K, typename V>
void skiplist<K, V>::display_list() {
    std::cout << "\n*****Skip List*****" << "\n";
    for (int i = 0; i <= cur_level; i++) {
        Node<K, V>* node = this->head->forward[i];
        std::cout << "Level " << i << ": ";
        while (node != nullptr) {
            std::cout << node->get_key() << ":" << node->get_value() << ";";
            node = node->forward[i];
        }
        std::cout << std::endl;
    }
}

// dump data in memory to file
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

// load data from disk
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

// get current skip list size
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


// delete element from skip list
template <typename K, typename V>
void skiplist<K, V>::delete_element(K key) {
    mtx.lock();
    Node<K, V>* current = this->head;
    Node<K, V>* update[max_level + 1];
    memset(update, 0, sizeof(Node<K, V>*) * (max_level + 1));

    // start from highest level of skip list
    for (int i = cur_level; i >= 0; i--) {
        while (current->forward[i] != nullptr && current->forward[i]->get_key() < key) {
            current = current->forward[i];
        }
        update[i] = current;
    }

    current = current->forward[0];
    if (current != nullptr && current->get_key() == key) {

        // start for lowest level and delete the current node of each level
        for (int i = 0; i <= cur_level; i++) {
            
            // if at level i, next node is not target node, break the loop
            if (update[i]->forward[i] != current) {
                break;
            }

            update[i]->forward[i] = current->forward[i];
        }

        // update cur_level, remove levels which have no elements
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

// Search for element in skip list 
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
bool skiplist<K, V>::search_element(K key) {
    // std::cout << "search_element--------------------" << std::endl;
    Node<K, V>* current = head;

    // start from highest level of skip list
    for (int i = cur_level; i >= 0; i--) {
        while (current->forward[i] != nullptr && current->forward[i]->get_key() < key) {
            current = current->forward[i];
        }
    }

    // reached level 0 and advance pointer to right node, which we search
    current = current->forward[0];

    // if current node's key equal to searched key, we get it
    if (current != nullptr && current->get_key() == key ) {
        // std::cout << "Found key: " << ", value: " << current->get_value() << std::endl;
        return true;
    }

    // std::cout << "Not Found Key:" << key << std::endl;
    return false;
}

// construct skip list
template<typename K, typename V> 
skiplist<K, V>::skiplist(int max_level) {
    this->max_level = max_level;
    this->cur_level = 0;
    this->element_count = 0;

    // create header node and initialize key and value to null
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
