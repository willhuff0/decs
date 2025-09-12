#pragma once

#include "ConcurrentQueue.hpp"

#include <memory>
#include <functional>

class DeferredExecutor {
public:
    explicit DeferredExecutor(std::unique_ptr<ConcurrentQueue<std::function<void()>>> queue);

    void PushFunc(std::function<void()>&& deferredFunc);
    void ExecuteAll();

private:
    std::unique_ptr<ConcurrentQueue<std::function<void()>>> queue;
};