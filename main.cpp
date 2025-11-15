#include <iostream>
#include <algorithm>
#include "ProcessInfo.h"

#undef min
#undef max

int main() {
    std::cout << "Getting running processes..." << std::endl;

    auto processes = ProcessInfo::getRunningProcesses();

    if (processes.empty()) {
        std::cout << "No processes found or error occured!" << std::endl;
        return 1;
    }

    std::cout << "Found " << processes.size() << " processes:" << std::endl;

    for (size_t i = 0; i < std::min(processes.size(), size_t(10)); i++) {
        auto process = processes[i];
        std::cout << "PID: " << process->getPid()
        << ", Name: " << process->getName()
        << ", Parent: " << process->getParentPid() << 
        std::endl;
    }
    return 0;

}

/*
processes.size()    Количество элементов
processes[i]        Доступ по индексу
processes.empty()   Проверка на пустоту

auto process = processes[i];  // std::shared_ptr<ProcessInfo>
process->getPid()            // Доступ к методам через ->

auto processes = ...  // Компилятор сам определяет сложный тип

std::min(processes.size(), size_t(10))  // Защита от выхода за границы

*/