#pragma once
#include <string>
#include <windows.h>

class SecurityUtils {
    public:
    // 1. Проверка прав администратора
    static bool isRunningAsAdmin();

    // 2. Проверка цифровой подписи файла
    static bool verifyDigitalSignature(const std::string& filePath);
    
    // 3. Проверка системного процесса
    static bool isSystemProcess(DWORD pid);

    // 4. Проверка подозрительных процессов
    static bool isSuspiciousProcess(const std::string& processName);
    
    // 5. Вычисление хеша файла (SHA-256)
    static std::string calculateFileHash(const std::string& filePath);
/*
    // 4. Безопасное логирование
    static void secureLog(const std::string& message, const std::string& logFile = "security.log");
    
    // 6. Проверка целостности пути
    static bool validatePath(const std::string& path);
    
    // 7. Получение информации о владельце процесса
    static std::string getProcessOwner(DWORD pid);
*/    

};
