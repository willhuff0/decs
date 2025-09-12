#include <DeferredExecutor.hpp>

DeferredExecutor::DeferredExecutor(std::unique_ptr<ConcurrentQueue<std::function<void()>>> queue)
    : queue(std::move(queue)) { }

void DeferredExecutor::PushFunc(std::function<void()>&& deferredFunc) {
    queue->Enqueue(deferredFunc);
}

void DeferredExecutor::ExecuteAll() {
    std::function<void()> func;
    while (queue->TryDequeue(func) == ConcurrentQueue<std::function<void()>>::Status::Success) {
        func();
    }
}
