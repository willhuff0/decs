#include <decs/ComponentTypeId.hpp>

ComponentTypeId ComponentTypeId::GetExisting(ComponentTypeId::Value value) {
    ComponentTypeId componentTypeId{};
    componentTypeId.value = value;
    return componentTypeId;
}

bool ComponentTypeId::operator==(const ComponentTypeId& rhs) const {
    return value == rhs.value;
}

bool ComponentTypeId::operator!=(const ComponentTypeId& rhs) const {
    return !(rhs == *this);
}

ComponentTypeId::Value ComponentTypeId::GetValue() const {
    return value;
}

std::tuple<ComponentSize, Mover, Destructor> ComponentTypeId::GetTypeErasedOps() const {
    std::lock_guard lock(typeErasedOpsMutex);
    return typeErasedOps.at(*this);
}

std::atomic<ComponentTypeId::Value> ComponentTypeId::nextId = 0;
