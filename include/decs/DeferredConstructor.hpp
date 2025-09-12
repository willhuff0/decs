#pragma once

#include <tuple>
#include <utility>

class IDeferredConstructor {
public:
    virtual ~IDeferredConstructor() = default;
    virtual void Construct(void* dest) = 0;
};

template<typename T, typename... Args>
class DeferredConstructor : public IDeferredConstructor {
public:
    explicit DeferredConstructor(Args&&... args)
            : storedArgs(std::forward<Args>(args)...) {}

    void Construct(void* dest) override {
        std::apply([dest](auto&&... args) {
            new(dest) T(std::forward<decltype(args)>(args)...);
        }, std::forward<decltype(storedArgs)>(storedArgs));
    }

private:
    std::tuple<std::decay_t<Args>...> storedArgs;
};
