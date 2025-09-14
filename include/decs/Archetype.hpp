#pragma once

#include "ComponentTypeId.hpp"
#include "ComponentArray.hpp"

#include <unordered_map>
#include <memory>

class Archetype {
public:
    explicit Archetype(Signature signature);

    Archetype(Archetype&&) = delete;
    Archetype& operator=(Archetype&&) = delete;
    Archetype(const Archetype&) = delete;
    Archetype& operator=(const Archetype&) = delete;

    void CreateEntity(EntityId id, const std::unordered_map<ComponentTypeId, std::shared_ptr<IDeferredConstructor>>& constructors);
    void DeleteEntity(EntityId id);

    /// Moves a component from this Archetype to another.
    /// @note Caller must handle non-overlapping components via (Initialize/Deinitialize)Component.
    /// @procedure Before calling MigrateEntity, on the source archetype, deinitialize each component which is not present in the destination archetype.
    /// @procedure After calling MigrateEntity, on the destination archetype, initialize each component which was not present in the source archetype.
    /// @param id The Id of the entity to migrate.
    /// @param other The Archetype to append to.
    void MigrateEntity(EntityId id, Archetype& other);

    /// Initializes a non-overlapping component after an entity has been migrated to this archetype. Must be called immediately after MigrateEntity.
    /// @param componentTypeId The TypeId of the component to initialize.
    /// @param constructor The constructor to use for initialization.
    void InitializeComponent(ComponentTypeId componentTypeId, const std::shared_ptr<IDeferredConstructor>& constructor);

    /// Deinitializes a non-overlapping component before an entity is migrated from this archetype. Must be called immediately before MigrateEntity.
    /// @param entityId The Id of the entity planned for migration.
    /// @param componentTypeId The TypeId of the component to deinitialize.
    void DeinitializeComponent(EntityId entityId, ComponentTypeId componentTypeId);

    template<typename T>
    T& GetComponent(EntityId id);

    ComponentArray& GetComponentArray(ComponentTypeId componentTypeId);
    EntityId GetEntityId(ComponentIndex componentIndex) const;
    ComponentIndex GetSize() const;

private:
    std::unordered_map<EntityId, ComponentIndex> entityIdToComponentIndex;
    std::unordered_map<ComponentIndex, EntityId> componentIndexToEntityId;
    std::unordered_map<ComponentTypeId, ComponentArray> componentArrays;
    ComponentIndex componentArraySize = 0;
};

template<typename T>
T& Archetype::GetComponent(EntityId id) {
    return componentArrays.at(ComponentTypeId::Get<T>()).template Get<T>(entityIdToComponentIndex.at(id));
}
