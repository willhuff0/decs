#pragma once

#include "ComponentTypeId.hpp"
#include "ComponentArray.hpp"

class Archetype {
public:
    void CreateEntity(EntityId id, const std::unordered_map<ComponentTypeId, IDeferredConstructor*>& constructors);
    void DeleteEntity(EntityId id);

    template<typename T>
    T& GetComponent(EntityId id);

private:
    std::unordered_map<EntityId, ComponentIndex> entityIdToComponentIndex;
    std::unordered_map<ComponentIndex, EntityId> componentIndexToEntityId;
    std::unordered_map<ComponentTypeId, ComponentArray> componentArrays;
    ComponentIndex componentArraySize = 0;
};

template<typename T>
T& Archetype::GetComponent(EntityId id) {
    return componentArrays.at(ComponentTypeId::Get<T>()).Get<T>(entityIdToComponentIndex.at(id));
}
