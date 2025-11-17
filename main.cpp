#include <iostream>
#include <algorithm>
#include "ProcessInfo.h"
#include "SecurityUtils.h"

#undef min
#undef max

int main() {
    std::cout << "Getting running processes..." << std::endl;

    // ТЕСТИРУЕМ SecurityUtils ПЕРВЫМ ДЕЛОМ
    std::cout << "\n=== Testing SecurityUtils ===" << std::endl;
    
    // 1. Проверка прав администратора
    bool isAdmin = SecurityUtils::isRunningAsAdmin();
    std::cout << "Running as Administrator: " << (isAdmin ? "YES" : "NO") << std::endl;
    
    // 2. Вычисление хеша текущего исполняемого файла
    char exePath[MAX_PATH];
    GetModuleFileNameA(NULL, exePath, MAX_PATH);
    std::cout << "Current executable: " << exePath << std::endl;
    
    std::string hash = SecurityUtils::calculateFileHash(exePath);
    if (!hash.empty()) {
        std::cout << "SHA-256 Hash: " << hash << std::endl;
    } else {
        std::cout << "Failed to calculate hash!" << std::endl;
    }

    // ОСТАЛЬНОЙ КОД
    std::cout << "\n=== Process Info ===" << std::endl;
    auto processes = ProcessInfo::getRunningProcesses();

    if (processes.empty()) {
        std::cout << "No processes found or error occurred!" << std::endl;
        return 1;
    }

    std::cout << "Found " << processes.size() << " processes:" << std::endl;

    for (size_t i = 0; i < std::min(processes.size(), size_t(10)); i++) {
        auto process = processes[i];
        std::cout << "PID: " << process->getPid()
        << ", Name: " << process->getName()
        << ", Parent: " << process->getParentPid() 
        << std::endl;
    }
    return 0;
}