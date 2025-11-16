// ProcessInfo.h
#pragma once
#include <string>
#include <windows.h> //WinAPI
#include <memory> //shared_ptr
#include <vector> 

class ProcessInfo;

class ProcessInfo {

private:
    DWORD m_pid; //DWORD - 32-bit unsigned number (4 bytes)
    std::string m_name; //m - member
    std::string m_path;
    DWORD m_parentPid;

public:
//Constructor вызывается при создании объекта
    ProcessInfo (DWORD pid, const std::string& name, 
        const std::string& path, DWORD parentPid);

//Getter для безопасного доступа к private данным
    DWORD getPid() const {return m_pid;}
    const std::string& getName() const {return m_name;} //const - метод не меняет объект
    const std::string& getPath() const {return m_path;}
    DWORD getParentPid() const { return m_parentPid;}

//Static methods work without creating an object 
//static - принадлжеит к классу, а не объекту; умный указатель автоматически удаляется когда не нужен; вектор возвращает список процессов;
    static std::vector <std::shared_ptr<ProcessInfo>> getRunningProcesses(); 
    static std::shared_ptr<ProcessInfo> getProcessInfo(DWORD pid);
};

/*
    DWORD m_pid;           // Встроенный тип - автоматическое управление
    std::string m_name;    // RAII класс - сам управляет памятью
    std::string m_path;    // RAII класс - сам управляет памятью  
    DWORD m_parentPid;     // Встроенный тип - автоматическое управление
*/