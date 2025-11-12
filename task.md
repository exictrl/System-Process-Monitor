## "Системный монитор процессов с сетевыми возможностями"

**Цель:** Разработать приложение, которое мониторит запущенные процессы в Windows и предоставляет информацию по сети.

## Структура проекта:
```
ProcessMonitor/
PocessInfo.h/cpp     - Информация о процессах
ThreadSafeQueue.h     - Потокобезопасная очередь
NetworkServer.h/cpp   - Сетевой сервер
SecurityUtils.h/cpp   - Утилиты безопасности
main.cpp             - Главный модуль
```

## 1: Базовые структуры данных и классы

**Теория для изучения:**
- Классы и структуры в C++
- Конструкторы/деструкторы
- Rule of Three/Five
- Стандартная библиотека (vector, string, map)

**Практическое задание:**
```cpp
// ProcessInfo.h
#pragma once
#include <string>
#include <windows.h>
#include <memory>

class ProcessInfo {
private:
    DWORD m_pid;
    std::string m_name;
    std::string m_path;
    DWORD m_parentPid;
    
public:
    ProcessInfo(DWORD pid, const std::string& name, const std::string& path, DWORD parentPid);
    
    // Геттеры
    DWORD getPid() const;
    const std::string& getName() const;
    const std::string& getPath() const;
    
    // Статические методы
    static std::vector<std::shared_ptr<ProcessInfo>> getRunningProcesses();
    static std::shared_ptr<ProcessInfo> getProcessInfo(DWORD pid);
};
```

**Что сделать:**
- Реализовать класс ProcessInfo
- Написать метод getRunningProcesses() с использованием WinAPI (CreateToolhelp32Snapshot, Process32First, Process32Next)

 2: Управление памятью и умные указатели

**Теория для изучения:**
- Умные указатели (unique_ptr, shared_ptr, weak_ptr)
- RAII идиома
- Управление ресурсами WinAPI

**Практическое задание:**
```cpp
// ThreadSafeQueue.h
#pragma once
#include <queue>
#include <mutex>
#include <condition_variable>
#include <memory>

template<typename T>
class ThreadSafeQueue {
private:
    mutable std::mutex m_mutex;
    std::queue<T> m_queue;
    std::condition_variable m_condition;
    
public:
    void push(T value);
    bool try_pop(T& value);
    std::shared_ptr<T> try_pop();
    bool empty() const;
    size_t size() const;
};
```

**Что сделать:**
- Реализовать потокобезопасную очередь с использованием умных указателей
- Протестировать с разными типами данных

## 3: Многопоточность и синхронизация

**Теория для изучения:**
- std::thread, std::mutex, std::condition_variable
- Паттерн Producer-Consumer
- Проблемы гонки данных

**Практическое задание:**
```cpp
// NetworkServer.h
class NetworkServer {
private:
    SOCKET m_serverSocket;
    std::atomic<bool> m_running;
    std::thread m_workerThread;
    ThreadSafeQueue<std::string> m_messageQueue;
    
    void workerThread();
    void handleClient(SOCKET clientSocket);
    
public:
    NetworkServer();
    ~NetworkServer();
    
    bool start(int port);
    void stop();
};
```

**Что сделать:**
- Реализовать многопоточный TCP-сервер
- Использовать ThreadSafeQueue для передачи сообщений
- Обеспечить корректное завершение потоков

##  4: Системное программирование и безопасность

**Теория для изучения:**
- WinAPI для работы с процессами и сетью
- Winsock2 API
- Безопасная работа со строками
- Обработка ошибок

**Практическое задание:**
```cpp
// SecurityUtils.h
class SecurityUtils {
public:
    static bool validateProcessPath(const std::string& path);
    static bool isSystemProcess(DWORD pid);
    static std::string hashFile(const std::string& path); // MD5/SHA1
    static bool sanitizeInput(const std::string& input);
};
```

**Что сделать:**
- Добавить проверки безопасности
- Реализовать базовые функции безопасности
- Обработка ошибок WinAPI

## 5: Интеграция и тестирование

**Теория для изучения:**
- Интеграционное тестирование
- Отладка многопоточных приложений
- Работа с отладчиком

**Практическое задание:**
```cpp
// main.cpp
int main() {
    NetworkServer server;
    if (!server.start(8080)) {
        std::cerr << "Failed to start server" << std::endl;
        return 1;
    }
    
    std::cout << "Server running. Press Enter to stop..." << std::endl;
    std::cin.get();
    
    server.stop();
    return 0;
}
```

Протокол обмена данными:

```json
// Запрос от клиента
{
    "command": "get_processes",
    "filter": "all" // или "system", "user"
}

// Ответ сервера
{
    "status": "success",
    "processes": [
        {
            "pid": 1234,
            "name": "explorer.exe",
            "path": "C:\\Windows\\explorer.exe",
            "parent_pid": 456
        }
    ]
}
```

Технологии и API:

1. **WinAPI:** 
   - `CreateToolhelp32Snapshot`, `Process32First/Next`
   - `OpenProcess`, `GetModuleFileNameEx`
   - Работа с дескрипторами

2. **Сети:**
   - Winsock2 API (`socket`, `bind`, `listen`, `accept`)
   - Многопоточная обработка соединений

3. **Безопасность:**
   - Валидация входных данных
   - Безопасное управление памятью
   - Проверка прав доступа

4. **C++ Modern:**
   - Умные указатели
   - Многопоточность
   - Шаблоны
   - RAII

### Что получится в итоге:

- **Рабочее приложение** для мониторинга процессов
- **Сетевой интерфейс** для удаленного доступа
- **Потокобезопасную архитектуру**
- **Базовые механизмы безопасности**

