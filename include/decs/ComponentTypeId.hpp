#pragma once

#include <atomic>
#include <cstdint>
#include <functional>

class ComponentTypeId {
public:
    using Value = uint32_t;

    explicit ComponentTypeId(Value id);

    template<typename T>
    static ComponentTypeId Get();

    bool operator==(const ComponentTypeId &rhs) const;
    bool operator!=(const ComponentTypeId &rhs) const;

    [[nodiscard]] Value GetValue() const;

private:
    static std::atomic<Value> nextId;

    Value value;
};

template<typename T>
ComponentTypeId ComponentTypeId::Get() {
    static ComponentTypeId id(nextId++);
    return id;
}

inline std::atomic<ComponentTypeId::Value> ComponentTypeId::nextId = 0;

inline ComponentTypeId::ComponentTypeId(Value id) : value(id) {}

inline bool ComponentTypeId::operator==(const ComponentTypeId &rhs) const {
    return value == rhs.value;
}

inline bool ComponentTypeId::operator!=(const ComponentTypeId &rhs) const {
    return !(rhs == *this);
}

inline ComponentTypeId::Value ComponentTypeId::GetValue() const {
    return value;
}

template<>
struct std::hash<ComponentTypeId> {
    std::size_t operator()(const ComponentTypeId& k) const noexcept {
        return hash<ComponentTypeId::Value>()(k.GetValue());
    }
};
