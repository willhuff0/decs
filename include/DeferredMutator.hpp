#pragma once

#include "Types.hpp"

#include <queue>

struct Mutation {
    EntityId entityId;
    ComponentTypeId componentTypeId;
    std::function<void(void*)> function;
};

template<typename T>
class DeferredMutator {
public:
    DeferredMutator(std::queue<Mutation>& queue, EntityId entityId, const T& value) :
        queue(queue),
        entityId(entityId),
        value(value) { }

    const T& GetValue() const;
    void SetValue(std::function<void(T&)> mutator) const;

private:
    std::queue<Mutation>& queue;
    EntityId entityId;
    const T& value;
};

template<typename T>
const T& DeferredMutator<T>::GetValue() const {
    return value;
}

template<typename T>
void DeferredMutator<T>::SetValue(std::function<void(T&)> mutator) const {
    queue.push({
        entityId,
        ComponentTypeId::Get<T>(),
        [mutator](void* v) {
            mutator(*reinterpret_cast<T*>(v));
        }
    });
}
