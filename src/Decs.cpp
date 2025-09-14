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

EntityId Decs::createEntity(Signature signature, std::unordered_map<ComponentTypeId, std::shared_ptr<IDeferredConstructor>> constructors) {
    EntityId id = nextEntityId++;
    deferredExecutor.PushFunc([this, id, signature, constructors = std::move(constructors)]() {
        entities.emplace(id, signature);
        Archetype* archetype;
        {
            auto iter = archetypes.find(signature);
            if (iter != archetypes.end()) {
                archetype = iter->second.get();
            } else {
                auto [newIter, _] = archetypes.emplace(signature, std::make_unique<Archetype>(signature));
                archetype = newIter->second.get();
                systemManager.OnArchetypeAdded(signature, *archetype);
            }
        }
        archetype->CreateEntity(id, constructors);
    });
    return id;
}
