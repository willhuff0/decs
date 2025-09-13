#include <decs/Decs.hpp>

EntityBuilder Decs::CreateEntity() {
    return EntityBuilder { shared_from_this() };
}

void Decs::DeleteEntity(EntityId id) {
    deferredExecutor.PushFunc([this, id]() {
        Signature signature = entities.at(id);
        entities.erase(id);

        std::unique_ptr<Archetype>& archetype = archetypes.at(signature);
        if (archetype->GetSize() == 1) {
            systemManager.OnArchetypeRemoved(signature, *archetype);
            archetypes.erase(signature);
        } else {
            archetype->DeleteEntity(id);
        }
    });
}

EntityId Decs::createEntity(std::unordered_map<ComponentTypeId, std::shared_ptr<IDeferredConstructor>> constructors) {
    EntityId id = nextEntityId++;
    deferredExecutor.PushFunc([this, id, constructors = std::move(constructors)]() {
        Signature signature = entities.at(id);
        Archetype* archetype;
        {
            auto iter = archetypes.find(signature);
            if (iter != archetypes.end()) {
                archetype = iter->second.get();
            } else {
                std::vector<ComponentTypeId> componentTypeIds;
                for (const auto& [componentTypeId, _] : constructors) {
                    componentTypeIds.push_back(componentTypeId);
                }
                archetypes.emplace(signature, std::make_unique<Archetype>(componentTypeIds));
                systemManager.OnArchetypeAdded(signature, *archetypes.at(signature));
            }
        }
        archetype->CreateEntity(id, constructors);
    });
    return id;
}