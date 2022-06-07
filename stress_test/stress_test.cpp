/* ************************************************************************
> File Name:     stress_test.cpp
> Author:        yetao
> Mail:          1020051524@njupt.edu.cn 
> Created Time:  Tue 7 Jun 2022
> Description:   The stress test for skip list
 ************************************************************************/

#include <iostream>
#include <cstdlib>
#include <pthread.h>
#include <time.h>
#include <chrono>
#include "../skiplist.hpp"

#define NUM_THREADS 1
#define TEST_COUNT 1000000

skiplist<int, std::string> skip_list(18);

void* insertElement(void* threadid) {
    long tid;
    tid = (long)threadid;
    //std::cout << tid << std::endl;
    int tmp = TEST_COUNT / NUM_THREADS;
    for (int i = tid * tmp, count = 0; count < tmp; i++) {
        count++;
        skip_list.insert_element(rand() % TEST_COUNT, "a");
    }
    pthread_exit(nullptr);
}

void* getElement(void* threadid) {
    long tid;
    tid = (long)threadid;
    // std::cout << tid << std::endl;
    int tmp = TEST_COUNT / NUM_THREADS;
    for (int i = tid * tmp, count = 0; count < tmp; i++) {
        count++;
        skip_list.search_element(rand() % TEST_COUNT);
    }
    pthread_exit(nullptr);
}

int main() {
    srand(time(nullptr));
    {

        pthread_t threads[NUM_THREADS];
        int rc;
        int i;

        auto start = std::chrono::high_resolution_clock::now();

        for (i = 0; i < NUM_THREADS; i++) {
            //std::cout << "main() : creating thread, " << i << std::endl;
            rc = pthread_create(&threads[i], nullptr, insertElement, (void*)i);
            /* 成功返回 0， 失败返回其他值 */
            if (rc) {
                std::cout << "Error: unable to create thread, " << rc << std::endl;
                exit(-1);
            }
        }

        void* ret;
        for (i = 0; i < NUM_THREADS; i++) {
            if (pthread_join(threads[i], &ret) != 0) {
                perror("pthread_create() error");
                exit(3);
            }
        }

        auto finish = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> elapsed = finish - start;
        std::cout << NUM_THREADS << " threads insert " << TEST_COUNT << " data elapsed: " << elapsed.count() << std::endl;
		//skip_list.dump_file();
    }

    {
        pthread_t threads[NUM_THREADS];
        int rc;
        int i;

        auto start = std::chrono::high_resolution_clock::now();

        for (i = 0; i < NUM_THREADS; i++) {
            // std::cout << "main() : creating thread, " << i << std::endl;
            rc = pthread_create(&threads[i], nullptr, getElement, (void*)i);
            /* 成功返回 0， 失败返回其他值 */
            if (rc) {
                // std::cout << "Error: unable to create thread, " << rc << std::endl;
                exit(-1);
            }
        }

        void* ret;
        for (i = 0; i < NUM_THREADS; i++) {
            if (pthread_join(threads[i], &ret) != 0) {
                // perror("pthread_create() error");
                exit(3);
            }
        }

        auto finish = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> elapsed = finish - start;
        std::cout << NUM_THREADS << " threads search " << TEST_COUNT << " data elapsed: " << elapsed.count() << std::endl;
    }

    pthread_exit(nullptr);
    return 0;
}
