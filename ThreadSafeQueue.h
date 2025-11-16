#pragma once
#include <queue>
#include <mutex>
#include <condition_variable>
#include <memory> //std::shared_ptr

template<typename T>
class ThreadSafeQueue {
    private: 
        mutable std::mutex m_mutex; 
        std::queue<T> m_queue;
        std::condition_variable m_condition;
/*m_mutex - как дверной замок. Прежде чем работать с очередью, поток должен "взять замок"
Только один поток может владеть мьютексом
mutable - позволяет изменять мьютекс в const-методах
m_condition_variable - позволяет потокам ждать пока в очереди появятся элементы
 */

    public:
        ThreadSafeQueue() = default; //конструктор по умолчанию

        //Запрещаем копирование (потокобезопасная очередь)
        ThreadSafeQueue(const ThreadSafeQueue&) = delete;
        ThreadSafeQueue& operator=(const ThreadSafeQueue&) = delete;

        //Добавление элемента в очередь
        void push(T value) {
            {
                std::lock_guard<std::mutex> lock(m_mutex);
                m_queue.push(std::move(value));
            }
        m_condition.notify_one(); //уведомляем один ждущий поток
        }
/*
{
    std::lock_guard<std::mutex> lock(m_mutex);  // ЗАМОК - другие потоки ждут
    // ... критическая секция ...
}  // АВТОМАТИЧЕСКИЙ ОТПУСК ЗАМКА когда lock выходит из scope
  */

        //Попытка извлечения элемента (неблокирующая) из очереди когда не нужно ждать
        bool try_pop(T& value) {
            std::lock_guard<std::mutex> lock(m_mutex);
            if (m_queue.empty()) {
                return false;
            }
            value = std::move(m_queue.front());
            m_queue.pop();
            return true;
        }

        //Попытка извлечения элемента из очереди с возвратом shared_ptr когда нужны умные указатели
        std::shared_ptr<T> try_pop() {
            std::lock_guard<std::mutex> lock(m_mutex);
            if (m_queue.empty()) {
                return nullptr;
            }

            auto result = std::make_shared<T>(std::move(m_queue.front()));
            m_queue.pop();
            return result;
        }
    //Блокирующее извлеченеи когда можно ждать данные
    T wait_and_pop() {
        std::unique_lock<std::mutex> lock(m_mutex);

        //Ждем пока очередь не станет не пустой
        m_condition.wait(lock, [this]() {return !m_queue.empty();});

        T value = std::move(m_queue.front());
        m_queue.pop();
        return value;
    }
/*
m_condition.wait(lock, условие);
// Поток засыпает и ждет пока:
// 1. Его разбудят через notify_one() И
// 2. Условие станет true
*/

    //Блокирующее извлечение с возвратом shared_ptr ждем данные и умные указатели
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



