#pragma once

#include "ComponentTypeId.hpp"
#include "DeferredConstructor.hpp"
#include "Types.hpp"

#include <unordered_map>

class EntityBuilder {
public:
    ~EntityBuilder();

    /// Adds a component.
    /// @tparam T The type of component to add.
    /// @param args The arguments to be passed to the component constructor. Construction is deferred, so these are always copied by value.
    /// @return this
    template<typename T, typename... Args>
    EntityBuilder& AddComponent(Args&&... args);

    /// [Deferred] Creates a new entity with the built signature and initializes its components.
    /// @return The EntityId of the new entity.
    EntityId Build();

private:
    Signature signature;
    std::unordered_map<ComponentTypeId, IDeferredConstructor*> constructors;
};

template<typename T, typename... Args>
EntityBuilder& EntityBuilder::AddComponent(Args&&... args) {
    signature.set(ComponentTypeId::Get<T>().GetValue());
    constructors.emplace(ComponentTypeId::Get<T>(), new DeferredConstructor(std::forward<Args>(args)...));
    return *this;
}
