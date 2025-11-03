#pragma once

#include <decs/ConcurrentQueue.hpp>

#include "concurrentqueue.h"

#include <thread>

template<typename T>
class MoodycamelQueue : public ConcurrentQueue<T> {
public:
    ConcurrentQueueStatus Enqueue(const T& value) override;
    ConcurrentQueueStatus TryEnqueue(const T& value) override;

    ConcurrentQueueStatus Enqueue(T&& value) override;
    ConcurrentQueueStatus TryEnqueue(T&& value) override;

    ConcurrentQueueStatus Dequeue(T& out) override;
    ConcurrentQueueStatus TryDequeue(T& out) override;

    bool IsFull() const noexcept override;
    bool IsEmpty() const noexcept override;

    void Close() noexcept override;
    void Open() override;
    bool IsClosed() const noexcept override;

private:
    moodycamel::ConcurrentQueue<T> queue;
};

template<typename T>
ConcurrentQueueStatus MoodycamelQueue<T>::Enqueue(const T& value) {
    if (!queue.enqueue(value)) return ConcurrentQueueStatus::Full;
    return ConcurrentQueueStatus::Success;
}

template<typename T>
ConcurrentQueueStatus MoodycamelQueue<T>::TryEnqueue(const T& value) {
    if (!queue.try_enqueue(value)) return ConcurrentQueueStatus::Full;
    return ConcurrentQueueStatus::Success;
}

template<typename T>
ConcurrentQueueStatus MoodycamelQueue<T>::Enqueue(T&& value) {
    if (!queue.enqueue(value)) return ConcurrentQueueStatus::Full;
    return ConcurrentQueueStatus::Success;
}

template<typename T>
ConcurrentQueueStatus MoodycamelQueue<T>::TryEnqueue(T&& value) {
    if (!queue.try_enqueue(value)) return ConcurrentQueueStatus::Full;
    return ConcurrentQueueStatus::Success;
}

template<typename T>
ConcurrentQueueStatus MoodycamelQueue<T>::Dequeue(T& out) {
    while (!queue.try_dequeue(out)) {
        std::this_thread::yield();
    }
    return ConcurrentQueueStatus::Success;
}

template<typename T>
ConcurrentQueueStatus MoodycamelQueue<T>::TryDequeue(T& out) {
    if (!queue.try_dequeue(out)) return ConcurrentQueueStatus::Empty;
    return ConcurrentQueueStatus::Success;
}

template<typename T>
bool MoodycamelQueue<T>::IsFull() const noexcept {
    return false;
}

template<typename T>
bool MoodycamelQueue<T>::IsEmpty() const noexcept {
    return queue.size_approx() == 0;
}

template<typename T>
void MoodycamelQueue<T>::Close() noexcept {

}

template<typename T>
void MoodycamelQueue<T>::Open() {

}

template<typename T>
bool MoodycamelQueue<T>::IsClosed() const noexcept {
    return false;
}
