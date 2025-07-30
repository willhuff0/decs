#pragma once

#include "DeferredConstructor.hpp"
#include "Types.hpp"

#include <cstdint>
#include <functional>
#include <memory>

class ComponentArray {
public:
    using Mover = std::function<void(void* src, void* dest)>;
    using Destructor = std::function<void(void* ptr)>;

    template<typename T>
    static ComponentArray Create();

    explicit ComponentArray(uint32_t elementSize, Mover mover, Destructor destructor);
    ComponentArray(ComponentArray&& other) noexcept;
    ComponentArray& operator=(ComponentArray&& other) noexcept;
    ComponentArray(const ComponentArray&) = delete;
    ComponentArray& operator=(const ComponentArray&) = delete;
    ~ComponentArray();

    void EmplaceBack(IDeferredConstructor* constructor);
    void MoveAndPop(ComponentIndex indexToRemove);

    template<typename T>
    T& Get(uint32_t index);

private:
    uint32_t elementSize;
    size_t elementCount = 0;
    size_t capacity = 0;
    std::unique_ptr<unsigned char[]> data;

    Mover mover;
    Destructor destructor;

    void grow();
    void reallocate(size_t newCapacity);
};

template<typename T>
ComponentArray ComponentArray::Create() {
    auto mover = [](void* src, void* dest) {
        new(dest) T(std::move(*static_cast<T*>(src)));
    };
    auto destructor = [](void* ptr) {
        static_cast<T*>(ptr)->~T();
    };
    return ComponentArray(sizeof(T), mover, destructor);
}

template<typename T>
T& ComponentArray::Get(uint32_t index) {
    return *static_cast<T*>(data.get() + index * elementSize);
}
