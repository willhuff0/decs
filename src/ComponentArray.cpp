#include <decs/ComponentArray.hpp>

#include <tuple>

ComponentArray::ComponentArray(ComponentTypeId componentTypeId) {
    auto typeErasedOps = componentTypeId.GetTypeErasedOps();
    elementSize = std::get<ComponentSize>(typeErasedOps);
    mover = std::get<Mover>(typeErasedOps);
    destructor = std::get<Destructor>(typeErasedOps);
}

ComponentArray::ComponentArray(uint32_t elementSize, Mover mover, Destructor destructor) :
    elementSize(elementSize),
    mover(std::move(mover)),
    destructor(std::move(destructor)) { }

ComponentArray::ComponentArray(ComponentArray&& other) noexcept :
    elementSize(other.elementSize),
    elementCount(other.elementCount),
    capacity(other.capacity),
    data(std::move(other.data)),
    mover(std::move(other.mover)),
    destructor(std::move(other.destructor)) {
    other.elementCount = 0;
    other.capacity = 0;
}

ComponentArray& ComponentArray::operator=(ComponentArray&& other) noexcept {
    if (this == &other) return *this;

    for (size_t i = 0; i < elementCount; i++) {
        destructor(data.get() + i * elementSize);
    }

    elementSize = other.elementSize;
    elementCount = other.elementCount;
    capacity = other.capacity;
    data = std::move(other.data);
    mover = std::move(other.mover);
    destructor = std::move(other.destructor);

    other.elementCount = 0;
    other.capacity = 0;

    return *this;
}

ComponentArray::~ComponentArray() {
    for (size_t i = 0; i < elementCount; i++) {
        destructor(data.get() + i * elementSize);
    }
}

void ComponentArray::EmplaceBack(IDeferredConstructor *constructor) {
    if (elementCount == capacity) {
        grow();
    }
    constructor->Construct(data.get() + elementCount * elementSize);
    elementCount++;
}

void ComponentArray::MoveAndPop(ComponentIndex indexToRemove) {
    ComponentIndex lastIndex = elementCount - 1;

    void* dest = data.get() + indexToRemove * elementSize;
    destructor(dest);

    if (indexToRemove != lastIndex) {
        void* src = data.get() + lastIndex * elementSize;
        mover(src, dest);
        destructor(src);
    }
    elementCount--;
}

void ComponentArray::grow() {
    reallocate(capacity == 0 ? 8 : capacity * 2);
}

void ComponentArray::reallocate(size_t newCapacity) {
    auto newData = std::make_unique<unsigned char[]>(newCapacity * elementSize);
    for (size_t i = 0; i < elementCount; i++) {
        void* src = data.get() + i * elementSize;
        void* dest = newData.get() + i * elementSize;
        mover(src, dest);
        destructor(src);
    }
    data = std::move(newData);
    capacity = newCapacity;
}
