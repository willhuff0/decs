#pragma once

#include "Types.hpp"
#include "Archetype.hpp"
#include "DeferredExecutor.hpp"
#include "SystemManager.hpp"
#include "EntityBuilder.hpp"
#include "DeferredConstructor.hpp"

#include <unordered_map>
#include <memory>
#include <atomic>

class Decs : public std::enable_shared_from_this<Decs> {
    friend class EntityBuilder;
public:
    /// Creates a new Decs instance.
    /// @return A pointer to the Decs instance.
    static std::shared_ptr<Decs> Create();

    /// Creates an EntityBuilder.
    /// @return The new EntityBuilder.
    EntityBuilder CreateEntity();

    /// [Deferred] Deletes an entity.
    void DeleteEntity(EntityId id);

    /// Gets a copy of the component of type T associated with an entity. Prefer using component references provided by systems during iteration.
    /// @tparam T The type of component to get.
    /// @return A copy of the component T on entity id.
    template<typename T>
    T GetComponent(EntityId id);

    /// [Deferred] Sets the component of type T associated with an entity. Prefer altering component references provided by systems during iteration.
    /// @tparam T The type of component to set.
    /// @param id The Id of the entity to modify.
    /// @param value The new value of the component.
    template<typename T>
    void SetComponent(EntityId id, T value);

    /// [Deferred] Adds a component to an entity. The entity will be migrated to a different archetype. A new archetype may be created.
    /// @tparam T The type of component to add.
    /// @param id The Id of the entity to add to.
    /// @param args The arguments to be passed to the component constructor. Construction is deferred, so these are always copied by value.
    template<typename T, typename... Args>
    void AddComponent(EntityId id, Args... args);

    /// [Deferred] Removes the component of type T from an entity. The entity will be migrated to a different archetype. A new archetype may be created.
    /// @tparam T The type of component to remove.
    /// @param id The Id of the entity to remove from.
    template<typename T>
    void RemoveComponent(EntityId id);

private:
    /// Friend function for EntityBuilder.
    /// [Deferred] Creates a new entity with the built signature and initializes its components.
    /// @return The EntityId of the new entity.
    EntityId createEntity(Signature signature, std::unordered_map<ComponentTypeId, std::shared_ptr<IDeferredConstructor>> constructors);

private:
    Decs();
    ~Decs();

    std::atomic<EntityId> nextEntityId;

    std::unordered_map<Signature, std::unique_ptr<Archetype>> archetypes;
    std::unordered_map<EntityId, Signature> entities;

    DeferredExecutor deferredExecutor;
    SystemManager systemManager;
};

template<typename T>
T Decs::GetComponent(EntityId id) {
    return archetypes.at(entities.at(id))->GetComponent<T>(id);
}

template<typename T>
void Decs::SetComponent(EntityId id, T value) {
    deferredExecutor.PushFunc([this, id, value]() {
        archetypes.at(entities.at(id))->GetComponent<T>(id) = value;
    });
}

template<typename T, typename... Args>
void Decs::AddComponent(EntityId id, Args... args) {
    deferredExecutor.PushFunc([this, id, constructor = std::make_shared<DeferredConstructor>(std::forward<Args>(args)...)] {
        Signature oldSignature = entities.at(id);
        Signature newSignature = oldSignature;
        newSignature.set(ComponentTypeId::Get<T>().GetValue());
        entities[id] = newSignature;

        Archetype* dest;
        {
            auto iter = archetypes.find(newSignature);
            if (iter != archetypes.end()) {
                dest = iter->second.get();
            } else {
                auto [newIter, _] = archetypes.emplace(newSignature, std::make_unique<Archetype>(newSignature));
                dest = newIter->second.get();
                systemManager.OnArchetypeAdded(newSignature, *dest);
            }
        }
        Archetype* src = archetypes.at(oldSignature).get();

        src->MigrateEntity(id, *dest);
        dest->InitializeComponent(ComponentTypeId::Get<T>(), constructor);

        if (src->GetSize() == 0) {
            systemManager.OnArchetypeRemoved(oldSignature, *src);
            archetypes.erase(oldSignature);
        }
    });
}
