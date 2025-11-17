cmake_minimum_required(VERSION 3.15)
project(ProcessMonitor)

# Настройка компилятора C++
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Для Windows специфичные настройки
if(WIN32)
    add_compile_definitions(_WIN32_WINNT=0x0600 NOMINMAX WIN32_LEAN_AND_MEAN)
    set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
endif()

# Включение всех предупреждений компилятора
if(MSVC)
    add_compile_options(/W4 /WX)
else()
    add_compile_options(-Wall -Wextra -Wpedantic -Werror)
endif()

# Основное приложение - монитор процессов
add_executable(ProcessMonitor
    main.cpp
    ProcessInfo.cpp
    SecurityUtils.cpp
)

# Тестовое приложение для очереди
add_executable(QueueTest
    test_queue.cpp
    ProcessInfo.cpp
    SecurityUtils.cpp
)

# Подключаем системные библиотеки Windows
if(WIN32)
    target_link_libraries(ProcessMonitor 
        psapi
        kernel32
        user32
        advapi32
        crypt32
        wintrust
    )
    
    target_link_libraries(QueueTest 
        psapi
        kernel32
        user32
        advapi32
        crypt32
        wintrust
    )
endif()

# Включаем текущую директорию для заголовков
target_include_directories(ProcessMonitor PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(QueueTest PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

# Настройка для отладки
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_definitions(ProcessMonitor PRIVATE DEBUG=1)
    target_compile_definitions(QueueTest PRIVATE DEBUG=1)
endif()

# Информация о проекте
message(STATUS "Project: ${PROJECT_NAME}")
message(STATUS "C++ Standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")