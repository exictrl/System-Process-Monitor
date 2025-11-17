#include <iostream>
#include <thread>
#include <vector>
#include <chrono>
#include <atomic>
#include "ThreadSafeQueue.h"
#include "ProcessInfo.h"
#include "SecurityUtils.h"

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

// Тестирование SecurityUtils
void test_security_utils() {
    std::cout << "\n=== Testing Security Utils ===" << std::endl;
    
    // 1. Проверка прав администратора
    std::cout << "1. Admin check:" << std::endl;
    bool isAdmin = SecurityUtils::isRunningAsAdmin();
    std::cout << "   Running as Administrator: " << (isAdmin ? "YES" : "NO") << std::endl;
    
    // 2. Вычисление хеша текущего исполняемого файла
    std::cout << "2. File hash calculation:" << std::endl;
    char exePath[MAX_PATH];
    GetModuleFileNameA(NULL, exePath, MAX_PATH);
    std::cout << "   File: " << exePath << std::endl;
    
    std::string hash = SecurityUtils::calculateFileHash(exePath);
    if (!hash.empty()) {
        std::cout << "   SHA-256 Hash: " << hash.substr(0, 32) << "..." << std::endl;
        std::cout << "   Hash length: " << hash.length() << " characters" << std::endl;
    } else {
        std::cout << "   Failed to calculate hash!" << std::endl;
    }
    
    // 3. Проверка системных файлов
    std::cout << "3. System file hashes:" << std::endl;
    const char* systemFiles[] = {
        "C:\\Windows\\System32\\notepad.exe",
        "C:\\Windows\\System32\\calc.exe",
        "C:\\Windows\\explorer.exe"
    };
    
    for (const char* file : systemFiles) {
        std::string fileHash = SecurityUtils::calculateFileHash(file);
        if (!fileHash.empty()) {
            std::cout << "   " << file << " : " << fileHash.substr(0, 16) << "..." << std::endl;
        } else {
            std::cout << "   " << file << " : [Access denied or not found]" << std::endl;
        }
    }
    
    // 4. Простая демонстрация с ProcessInfo
    std::cout << "4. Process security demo:" << std::endl;
    auto processes = ProcessInfo::getRunningProcesses();
    if (!processes.empty()) {
        std::cout << "   First process: " << processes[0]->getName() << " (PID: " << processes[0]->getPid() << ")" << std::endl;
        if (!processes[0]->getPath().empty()) {
            std::string processHash = SecurityUtils::calculateFileHash(processes[0]->getPath());
            if (!processHash.empty()) {
                std::cout << "   Process hash: " << processHash.substr(0, 16) << "..." << std::endl;
            }
        }
    }
}



int main() {
    test_basic_types();
    test_smart_pointers();
    test_multithreaded();
    test_wait_and_pop_multithreaded();
    test_security_utils();  // Добавляем тестирование SecurityUtils
    return 0;
}