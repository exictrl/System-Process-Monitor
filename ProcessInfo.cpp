#include "ProcessInfo.h"
#include <tlhelp32.h> // Для функций процессов
#include <psapi.h>  // Для получения информации о процессах

ProcessInfo::ProcessInfo(DWORD pid, const std::string& name, 
            const std::string& path, DWORD parentPid) : m_pid(pid), m_name(name), m_path(path), m_parentPid(parentPid)
            {

            }

std::vector<std::shared_ptr<ProcessInfo>> ProcessInfo::getRunningProcesses() {
    
    std::vector <std::shared_ptr<ProcessInfo>> processes;

/*
Делает "снимок" текущего состояния системы (процессов, потоков и т.д.)
TH32CS_SNAPPROCESS - флаг "снимок процессов"
0 - ID процесса (0 = все процессы)
Возвращает HANDLE - дескриптор снимка
*/
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (snapshot == INVALID_HANDLE_VALUE) {
        return processes; //возращаем пустой вектор при ошибке
    }

    PROCESSENTRY32 processEntry;
    processEntry.dwSize = sizeof(PROCESSENTRY32);

/*
Process32First - получаем первый процесс из снимка
Process32Next - получаем следующий процесс
Цикл продолжается пока Process32Next возвращает TRUE
*/
    if (Process32First(snapshot, &processEntry)) {
        do {
            std::string fullPath = getProcessPath(processEntry.th32ProcessID);

            auto process = std::make_shared<ProcessInfo>(
                processEntry.th32ProcessID, //PID
                processEntry.szExeFile, //name of file
                fullPath, //path
                processEntry.th32ParentProcessID //Parent PID
            );
            processes.push_back(process);
        } while (Process32Next(snapshot, &processEntry));

    }
    CloseHandle(snapshot); //освобождаем дескриптор
    return processes;
}

/*
WinAPI функции: CreateToolhelp32Snapshot, Process32First/Next
Структура PROCESSENTRY32 - информация о процессе
Управление ресурсами: CloseHandle для освобождения дескрипторов
Умные указатели: std::make_shared для автоматического управления памятью
Список инициализации в конструкторе - эффективная инициализация
*/

std::string ProcessInfo::getProcessPath(DWORD pid) {
    HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, pid);
    if (!hProcess) {
        return "";
    }

    char path[MAX_PATH];
    DWORD pathSize = MAX_PATH;
    
    if(GetModuleFileNameExA(hProcess, NULL, path, pathSize)) {
        CloseHandle(hProcess);
        return std::string(path);
    }

    CloseHandle(hProcess);
    return "";
}