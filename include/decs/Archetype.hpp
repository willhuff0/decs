#pragma once

#include "ComponentTypeId.hpp"
#include "ComponentArray.hpp"

#include <unordered_map>
#include <memory>

class Archetype {
public:
    explicit Archetype(const std::vector<ComponentTypeId>& componentTypeIds);

    Archetype(Archetype&&) = delete;
    Archetype& operator=(Archetype&&) = delete;
    Archetype(const Archetype&) = delete;
    Archetype& operator=(const Archetype&) = delete;

    void CreateEntity(EntityId id, const std::unordered_map<ComponentTypeId, IDeferredConstructor*>& constructors);
    void DeleteEntity(EntityId id);

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
