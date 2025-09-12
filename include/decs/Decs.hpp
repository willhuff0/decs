#pragma once

#include "Types.hpp"
#include "Archetype.hpp"
#include "DeferredExecutor.hpp"
#include "SystemManager.hpp"
#include "EntityBuilder.hpp"

#include <unordered_map>
#include <memory>

class Decs : public std::enable_shared_from_this<Decs> {
public:
    /// Creates a new Decs instance.
    /// @return A pointer to the Decs instance.
    static std::shared_ptr<Decs> Create();

    /// Creates an EntityBuilder.
    /// @return The new EntityBuilder.
    EntityBuilder CreateEntity();
    void DeleteEntity(EntityId id);

    /// Gets a copy of the component of type T associated with an entity. Prefer using component references provided by systems during iteration.
    /// @return A copy of the component T on entity id.
    template<typename T>
    T GetComponent(EntityId id);

    /// [Deferred] Sets the component of type T associated with an entity. Prefer altering component references provided by systems during iteration.
    template<typename T>
    void SetComponent(EntityId id, T value);

    /// [Deferred] Adds the component of type T to an entity and sets its value. The entity will be migrated to a different archetype. A new archetype may be created.
    template<typename T>
    void AddComponent(EntityId id, T value);

    /// [Deferred] Removes the component of type T from an entity. The entity will be migrated to a different archetype. A new archetype may be created.
    template<typename T>
    void RemoveComponent(EntityId id);

private:
    /// Friend function for EntityBuilder.
    /// [Deferred] Creates a new entity with the built signature and initializes its components.
    /// @return The EntityId of the new entity.
    EntityId createEntity(std::unordered_map<ComponentTypeId, IDeferredConstructor*> constructors);

private:
    Decs();

    std::unordered_map<Signature, Archetype> archetypes;
    std::unordered_map<EntityId, Signature> entities;

    DeferredExecutor deferredExecutor;
    SystemManager systemManager;
};