#pragma once

#include <atomic>
#include <cstdint>
#include <functional>

class ComponentTypeId {
public:
    template<typename T>
    static ComponentTypeId Get();

    bool operator==(const ComponentTypeId &rhs) const;
    bool operator!=(const ComponentTypeId &rhs) const;

    [[nodiscard]] uint32_t GetId() const;

private:
    static std::atomic<uint32_t> nextId;

    explicit ComponentTypeId(uint32_t id);
    uint32_t id;
};

template<typename T>
ComponentTypeId ComponentTypeId::Get() {
    static ComponentTypeId id(nextId++);
    return id;
}

inline std::atomic<uint32_t> ComponentTypeId::nextId = 0;

inline ComponentTypeId::ComponentTypeId(uint32_t id) : id(id) {}

inline bool ComponentTypeId::operator==(const ComponentTypeId &rhs) const {
    return id == rhs.id;
}

inline bool ComponentTypeId::operator!=(const ComponentTypeId &rhs) const {
    return !(rhs == *this);
}

inline uint32_t ComponentTypeId::GetId() const {
    return id;
}

namespace std {
    template<>
    struct hash<ComponentTypeId> {
        std::size_t operator()(const ComponentTypeId& k) const {
            return hash<uint32_t>()(k.GetId());
        }
    };
}