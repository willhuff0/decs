#pragma once

#include "Types.hpp"

#include <cstring>
#include <vector>

class ComponentArray {
public:
    explicit ComponentArray(uint32_t elementSize) : elementSize(elementSize) { }

    void Push();
    void Pop();

    unsigned char* Get(ComponentIndex index);

    template<typename T>
    T& GetRef(ComponentIndex index);

    void Set(ComponentIndex index, const unsigned char* element);

    template<typename T>
    void Set(ComponentIndex index, const T& element);

private:
    uint32_t elementSize;
    std::vector<unsigned char> data;
};

template<typename T>
T& ComponentArray::GetRef(ComponentIndex index) {
   return *reinterpret_cast<T*>(data.data() + index * elementSize);
}

template<typename T>
void ComponentArray::Set(ComponentIndex index, const T& element) {
    std::memcpy(data.data() + index * elementSize, &element, elementSize);
}