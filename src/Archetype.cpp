#include <decs/Archetype.hpp>

Archetype::Archetype(Signature signature) {
    for (ComponentTypeId::Value id = 0; id < MAX_COMPONENTS; id++) {
        if (!signature.test(id)) continue;
        componentArrays.emplace(ComponentTypeId::GetExisting(id), ComponentTypeId::GetExisting(id));
    }
}

void Archetype::CreateEntity(EntityId id, const std::unordered_map<ComponentTypeId, std::shared_ptr<IDeferredConstructor>>& constructors) {
    ComponentIndex newIndex = componentArraySize++;

    entityIdToComponentIndex.emplace(id, newIndex);
    componentIndexToEntityId.emplace(newIndex, id);

    for (auto& [componentTypeId, componentArray] : componentArrays) {
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

    for (auto& [componentTypeId, componentArray] : componentArrays) {
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

void Archetype::MigrateEntity(EntityId id, Archetype& other) {
    ComponentIndex indexToRemove = entityIdToComponentIndex.at(id);

    {
        ComponentIndex lastIndex = --componentArraySize;

        if (indexToRemove != lastIndex) {
            EntityId lastEntityId = componentIndexToEntityId.at(lastIndex);

            entityIdToComponentIndex[lastEntityId] = indexToRemove;
            componentIndexToEntityId[indexToRemove] = lastEntityId;
        }

        entityIdToComponentIndex.erase(id);
        componentIndexToEntityId.erase(lastIndex);
    }

    {
        ComponentIndex newIndex = other.componentArraySize++;

        other.entityIdToComponentIndex.emplace(id, newIndex);
        other.componentIndexToEntityId.emplace(newIndex, id);
    }

    for (auto& [componentTypeId, componentArray] : componentArrays) {
        auto iter = other.componentArrays.find(componentTypeId);
        if (iter == other.componentArrays.end()) continue;
        componentArray.Migrate(indexToRemove, iter->second);
    }
}

void Archetype::InitializeComponent(ComponentTypeId componentTypeId, const std::shared_ptr<IDeferredConstructor>& constructor) {
    componentArrays.at(componentTypeId).EmplaceBack(constructor);
}

void Archetype::DeinitializeComponent(EntityId entityId, ComponentTypeId componentTypeId) {
    componentArrays.at(componentTypeId).MoveAndPop(entityIdToComponentIndex.at(entityId));
}