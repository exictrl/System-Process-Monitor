#include <iostream>
#include <thread>
#include <vector>
#include <chrono>
#include <atomic>
#include "ThreadSafeQueue.h"
#include "ProcessInfo.h"

#ifndef NOMINMAX
#define NOMINMAX
#endif

//Тест с простыми типами данных
void test_basic_types() {
    std::cout << "=== Testing with basic types ===" << std::endl;

    ThreadSafeQueue<int> queue;

    //производитель
    queue.push(1);
    queue.push(2);
    queue.push(3);

    //Потребитель
    int value;
    while (queue.try_pop(value)) {
        std::cout << "Popped: " << value << std::endl; 
    }
    std::cout << "Queue empty: " << std::boolalpha << queue.empty() << std::endl;
}

//Тест с умными указателями:
void test_smart_pointers() {
    std::cout << "\n=== Testing with smart pointers ===" << std::endl;

    ThreadSafeQueue<std::shared_ptr<std::string>> queue;

    //добавление строки через shared ptr
    queue.push(std::make_shared<std::string> ("Hello"));
    queue.push(std::make_shared<std::string> ("World"));
    queue.push(std::make_shared<std::string> ("Test"));

    //Извлекаем

    std::shared_ptr<std::string> str_ptr;
    while (queue.try_pop(str_ptr)) {
        std::cout << "String: " << *str_ptr << std::endl;
        std::cout << "Reference count: " << str_ptr.use_count() << std::endl;
    }
}

//Многопточный тест
void test_multithreaded() {
    std::cout << "\n=== Testing multithreaded scenario ===" << std::endl;

    ThreadSafeQueue<int> queue;
    std::atomic<bool> done{false};
    std::vector<int> consumed;

    //поток-производители
    std::thread producer( [&queue, &done]() {
        for (int i=0; i < 10; ++i) {
            queue.push(i);
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
        done = true;
    }); 

    //Поток потребитель
    std::thread consumer([&queue, &done, &consumed]() {
        while (!done || !queue.empty()) {
            int value;
            if (queue.try_pop(value)) {
                consumed.push_back(value);
                std::cout << "Consumed: " << value << std::endl;
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(5));
        }
    });

    producer.join();
    consumer.join();

    std::cout << "Total consumed: " << consumed.size() << std::endl;
    
}


// Дополнительный тест для демонстрации wait_and_pop в многопоточном сценарии
void test_wait_and_pop_multithreaded() {
    std::cout << "\n=== Testing wait_and_pop in multithread scenario ===" << std::endl;
    ThreadSafeQueue<std::string> queue;
    std::atomic<bool> done{false};
    std::vector<std::string> consumed;

    //поток-производитель с задержкой
    std::thread producer([&queue, &done]() {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        for (int i=0; i<3; ++i) {
            queue.push("Task " + std::to_string(i));
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
        }
        done = true;
    });

    //поток-потребитель
    std::thread consumer([&queue, &done, &consumed]() {
        while (!done || !queue.empty()) {
            std::string task;
            if (queue.try_pop(task)) {
                consumed.push_back(task);
                std::cout << "Processed: " << task << std::endl;
            } else {
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
            }

        }
    });

    producer.join();
    consumer.join();

    std::cout << "Total tasks processed: " << consumed.size() << std::endl;
}   



int main() {
    test_basic_types();
    test_smart_pointers();
    test_multithreaded();
    test_wait_and_pop_multithreaded();
    return 0;
}



