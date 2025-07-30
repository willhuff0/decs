#include <ComponentArray.hpp>

void ComponentArray::Push() {
    data.reserve(data.size() + elementSize);
}

void ComponentArray::Pop() {
    data.resize(data.size() - elementSize);
}

unsigned char* ComponentArray::Get(ComponentIndex index) {
    return data.data() + index * elementSize;
}

void ComponentArray::Set(ComponentIndex index, const unsigned char* element) {
    std::memcpy(data.data() + index * elementSize, element, elementSize);
}