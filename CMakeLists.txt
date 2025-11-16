cmake_minimum_required(VERSION 3.10)
project(QueueTest)

# Явно указываем компилятор C++
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Для Windows
if(WIN32)
    add_compile_definitions(_WIN32_WINNT=0x0600 NOMINMAX)
    set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
endif()

# Создаем исполняемый файл
add_executable(QueueTest 
    test_queue.cpp 
    ProcessInfo.cpp
)

# Подключаем системные библиотеки Windows
if(WIN32)
    target_link_libraries(QueueTest 
        psapi
        kernel32
        user32
        advapi32
    )
    target_link_libraries(QueueTest pthread)  # Для MinGW
endif()

# Включаем текущую директорию для заголовков
target_include_directories(QueueTest PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})