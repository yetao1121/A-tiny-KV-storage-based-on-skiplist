/* ************************************************************************
> File Name:     main.cpp
> Author:        Yetao
> Mail:          1020051524@njupt.edu.cn 
> Created Time:  Mon June 7 2020
> Description:   Test relevant interfaces of the skip meter
 ************************************************************************/

#include <iostream>
#include "skiplist.hpp"
#define FILE_PATH "./store/dumpFile"

int main() {
    skiplist<std::string, std::string> skip_list(10);
    skip_list.insert_element("1", "a");
    skip_list.insert_element("2", "b");
    skip_list.insert_element("3", "c");
    skip_list.insert_element("4", "d");
    skip_list.insert_element("7", "ye");
    skip_list.insert_element("8", "tao");
    skip_list.insert_element("9", "zhen");
    skip_list.insert_element("19", "hhh");
    skip_list.insert_element("19", "hhh");
    skip_list.insert_element("19", "kkk");
    skip_list.insert_element("19", "lll");

    std::cout << "skiplist size: " << skip_list.size() << std::endl;

    skip_list.dump_file();

    skip_list.search_element("9");
    skip_list.search_element("18");

    skip_list.display_list();

    skip_list.delete_element("3");
    skip_list.delete_element("7");

    std::cout << "skiplist size: " << skip_list.size() << std::endl;

    skip_list.display_list();

    return 0;
}