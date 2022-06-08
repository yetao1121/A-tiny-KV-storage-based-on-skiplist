## 基于跳跃表的KV存储引擎

众所周知，非关系型数据库redis等，其核心存储引擎的数据结构就是跳越表。

本项目就是基于跳表实现的轻量级键值型存储引擎，使用C++实现。插入数据、删除数据、查询数据、数据展示、数据落盘、文件加载数据，以及数据库大小显示。

在随机写读情况下，该项目每秒可处理啊请求数（QPS）: 97.24w，每秒可处理读请求数（QPS）: 88.245w

### 提供的接口

```c++
int insert_element(K, V);  // 插入键值对，成功返回0，返回 1 代表跳表中已有该键
```

```c++
void delete_element(K);    // 删除键值为 key 的键值对
```

```c++
Node<K, V>* search_element(K);    // 查找键值为 key 的键值对，成功返回该键值对节点，失败返回 nullptr
```

```c++
int size();                // 返回当前跳跃表的大小（键值对个数）
```

```c++
void display_list();       // 显示当前跳跃表
```

```c++
void dump_file();          // 持久化
```

```c++
void load_file();          // 载入内存
```

### 压力测试

#### 插入操作

跳表 max_level : 18，采用单线程随机插入数据测试。在去除所有 std::cout 操作的情况下测试结果如下：

| 插入数据量（万条） | 耗时（秒） |
| :----------------: | :--------: |
|         20         |  0.149572  |
|         50         |  0.550042  |
|        100         |  1.49016   |

#### 读取操作

与插入操作相同，也是跳表 max_level : 18，采用单线程随机插入数据测试。在去除所有 std::cout 操作的情况下测试结果如下：

| 读取数据量（万条） | 耗时（秒） |
| :----------------: | :--------: |
|         20         |  0.156308  |
|         50         |  0.635806  |
|        100         |  1.71985   |

![image-20220607221440027](https://github.com/yetao1121/A-tiny-KV-storage-based-on-skiplist/blob/master/%E6%8F%92%E5%85%A5%E8%AF%BB%E5%8F%96%E5%8E%8B%E5%8A%9B%E6%B5%8B%E8%AF%95.png)

可见，在插入、读取数据量不超过一百万的情况下，平均每秒可以处理写请求数：97.24万条，平均每秒可以处理读请求数：88.245条。

Remark：此压力测试去除了所有的std::cout操作，仅做插入读取操作。

### 项目运行方式

```
// 接口测试
make
./bin/main

// 压力测试
cd ./stress_test
make
./stress_test
```

copy Node.hpp 和 skiplist.hpp 两个文件即可使用此基于跳跃表的 KV 键值对存储引擎



#### 参考：

非常感谢：

[youngyangyang04](https://github.com/youngyangyang04)

《Redis设计与实战》
