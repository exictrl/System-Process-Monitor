cmake_minimum_required(VERSION 3.10)
project(ProcessMonitor)

# Явно указываем компилятор C++
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Для Windows
if(WIN32)
    add_compile_definitions(_WIN32_WINNT=0x0600)
    set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)  # Для экспорта символов
endif()

# Создаем исполняемый файл
add_executable(ProcessMonitor 
    main.cpp 
    ProcessInfo.cpp
)

# Подключаем системные библиотеки Windows
if(WIN32)
    target_link_libraries(ProcessMonitor 
        psapi
        kernel32
        user32
        advapi32  # Может понадобиться для некоторых WinAPI функций
    )
endif()

# Включаем текущую директорию для заголовков
target_include_directories(ProcessMonitor PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})