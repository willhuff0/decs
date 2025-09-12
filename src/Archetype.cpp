#include <decs/Archetype.hpp>

void Archetype::CreateEntity(EntityId id, const std::unordered_map<ComponentTypeId, IDeferredConstructor*>& constructors) {
    ComponentIndex newIndex = componentArraySize++;

    entityIdToComponentIndex.emplace(id, newIndex);
    componentIndexToEntityId.emplace(newIndex, id);

    for (auto& [componentTypeId, componentArray]: componentArrays) {
        componentArray.EmplaceBack(constructors.at(componentTypeId));
    }
}

void Archetype::DeleteEntity(EntityId id) {
    ComponentIndex indexToRemove = entityIdToComponentIndex.at(id);
    ComponentIndex lastIndex = --componentArraySize;

    if (indexToRemove != lastIndex) {
        EntityId lastEntityId = componentIndexToEntityId.at(lastIndex);

        entityIdToComponentIndex[lastEntityId] = indexToRemove;
        componentIndexToEntityId[indexToRemove] = lastEntityId;
    }

    entityIdToComponentIndex.erase(id);
    componentIndexToEntityId.erase(lastIndex);

    for (auto& [componentTypeId, componentArray]: componentArrays) {
        componentArray.MoveAndPop(indexToRemove);
    }
}

ComponentArray& Archetype::GetComponentArray(ComponentTypeId componentTypeId) {
    return componentArrays.at(componentTypeId);
}

EntityId Archetype::GetEntityId(ComponentIndex componentIndex) const {
    return componentIndexToEntityId.at(componentIndex);
}

ComponentIndex Archetype::GetSize() const {
    return componentArraySize;
}