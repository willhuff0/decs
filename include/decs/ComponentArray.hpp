#pragma once

#include "DeferredConstructor.hpp"
#include "Types.hpp"

#include <cstdint>
#include <functional>
#include <memory>

class ComponentArray {
public:
    explicit ComponentArray(ComponentTypeId componentTypeId);
    ComponentArray(uint32_t elementSize, Mover mover, Destructor destructor);

    ~ComponentArray();

    ComponentArray(ComponentArray&& other) noexcept;
    ComponentArray& operator=(ComponentArray&& other) noexcept;
    ComponentArray(const ComponentArray&) = delete;
    ComponentArray& operator=(const ComponentArray&) = delete;

    void EmplaceBack(const std::shared_ptr<IDeferredConstructor>& constructor);
    void MoveAndPop(ComponentIndex indexToRemove);

    /// Moves a component from this ComponentArray to the back of another.
    /// @param fromIndex The index into this ComponentArray to remove from.
    /// @param other The ComponentArray to append to.
    void Migrate(ComponentIndex fromIndex, ComponentArray& other);

    void ConstructInPlace();
    void DestructInPlace();

    template<typename T>
    T& Get(ComponentIndex index);

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
T& ComponentArray::Get(ComponentIndex index) {
    return *reinterpret_cast<T*>(data.get() + index * elementSize);
}
