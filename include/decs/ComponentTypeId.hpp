#pragma once

#include <atomic>
#include <cstdint>
#include <functional>
#include <unordered_map>
#include <tuple>
#include <shared_mutex>

using ComponentSize = uint32_t;
using Mover = std::function<void(void* src, void* dest)>;
using Destructor = std::function<void(void* ptr)>;
using TypeErasedOps = std::tuple<ComponentSize, Mover, Destructor>;

class ComponentTypeId {
public:
    using Value = uint32_t;

    template<typename T>
    static ComponentTypeId Get();

    static ComponentTypeId GetExisting(Value value);

    bool operator==(const ComponentTypeId& rhs) const;
    bool operator!=(const ComponentTypeId& rhs) const;

    /// The index into Signature bitset
    [[nodiscard]] Value GetValue() const;

    [[nodiscard]] TypeErasedOps GetTypeErasedOps() const;

private:
    ComponentTypeId() = default;

    static std::atomic<Value> nextId;

    Value value;
};

template<>
struct std::hash<ComponentTypeId> {
    std::size_t operator()(const ComponentTypeId& k) const noexcept {
        return hash<ComponentTypeId::Value>()(k.GetValue());
    }
};

class TypeErasedOpsStore {
public:
    static std::shared_mutex typeErasedOpsMutex;
    static std::unordered_map<ComponentTypeId, TypeErasedOps> typeErasedOps;
};

template<typename T>
ComponentTypeId ComponentTypeId::Get() {
    static ComponentTypeId id;
    static std::once_flag flag;
    std::call_once(flag, [](ComponentTypeId& id){
        id.value = nextId++;

        auto componentSize = sizeof(T);
        auto mover = [](void* src, void* dest) {
            new(dest) T(std::move(*static_cast<T*>(src)));
        };
        auto destructor = [](void* ptr) {
            static_cast<T*>(ptr)->~T();
        };

        std::unique_lock lock(TypeErasedOpsStore::typeErasedOpsMutex);
        TypeErasedOpsStore::typeErasedOps.emplace(id, std::make_tuple(componentSize, mover, destructor));
    }, std::ref(id));
    return id;
}