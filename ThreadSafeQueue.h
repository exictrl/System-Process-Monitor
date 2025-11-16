#pragma once
#include <queue>
#include <mutex>
#include <condition_variable>
#include <memory>
#include <optional>

template<typename T>
class ThreadSafeQueue {
    private 
        mutable std::mutex m_mutex; 
        std::queue<T> m_queue;
        std::condition_variable m_condition;

    public
        ThreadSafeQueue() = default;

        //Запрещаем копирование (потокобезопасная очередь)
        ThreadSafeQueue(const ThreadSafeQueue&) = delete;
        ThreadSafeQueue& operator=(const ThreadSafeQueue&) = delete;

        //Добавление элемента в очередь
        void push(T value) {
            {
                std::lock_guard<std::mutex> lock(m_mutex);
                m_queue.push(std::move(value));
            }
        }
        m_condition.notify_one(); //уведомление один ждущий поток

        //Попытка извлечения элемента (неблокирующая)
        bool try_pop(T& value) {
            std::lock_guard<std::mutex> lock(m_mutex);
            if (m_queue.empty()) {
                return false;
            }
            value = std::move(m_queue.front());
            m_queue.pop();
            return true;
        }

        //Попытка извлечения с возвратом shared_ptr
        std::shared_ptr<T> try_pop() {
            std::lock_guard<std::mutex> lock(m_mutex);
            if (m_queue.empty()) {
                return nullptr;
            }

            auto result = std::makeshared<T>(std::move(m_queue.front()));
            m_queue.pop();
            return result;
        }
    //Блокирующее извлеченеи
    T wait_and_pop() {
        std::unique_lock<std::mutex> lock<m_mutex>;

        //Ждем пока очередь не станет пустой
        m_condition.wait(lock, [this]() {return !m_queue.empty();});

        T value = std::move(m_queue.front());
        m_queue.pop();
        return value;
    }

    //Блокирующее извлечение с возвратом shared_ptr
    std::shared_ptr<T> wait_and_pop_ptr() {
        std::unique_lock<std::mutex> lock(m_mutex);
        m_condition.wait(lock, [this]() {return !m_queue.empty();});

        auto result = std::make_shared<T>(std::move(m_queue.front()));
        m_queue.pop();
        return result;
    }

    //проверка на пустоту
    bool empty() const {
        std::lock_guard<std::mutex> lock(m_mutex);
        return m_queue.empty();
    }

    //Размер очереди
    size_t size() const {
        std::lock_guard<std::mutex> lock(m_mutex);
        return m_queue.size();

    }
};



