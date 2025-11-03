#include <decs/Decs.hpp>

#include <queue>
#include <utility>

std::shared_ptr<Decs> Decs::Create(std::unique_ptr<ConcurrentQueue<std::function<void()>>> queue) {
    return std::shared_ptr<Decs>(new Decs(std::move(queue)));
}

Decs::Decs(std::unique_ptr<ConcurrentQueue<std::function<void()>>> queue)
  : deferredExecutor(std::move(queue)), systemManager(this) { }

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

void Decs::UnregisterSystem(SystemKey key) {
    systemManager.UnregisterSystem(key);
}

void Decs::ExecuteDeferredFunctions() {
    deferredExecutor.ExecuteAll();
}

void Decs::IterateSystems() {
    systemManager.IterateAll();

    std::queue<Mutation> mutations;
    systemManager.IterateMutableAll(mutations);
    while (!mutations.empty()) {
        const auto& mutation = mutations.front();

        void* comp = archetypes.at(entities.at(mutation.entityId))->GetComponent(mutation.entityId, mutation.componentTypeId);
        mutation.function(comp);

        mutations.pop();
    }
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

std::vector<EntityId> Decs::createEntity(Signature signature, std::unordered_map<ComponentTypeId, std::shared_ptr<IDeferredConstructor>> constructors, uint32_t clones) {
    std::vector<EntityId> ids;
    ids.reserve(clones);
    for(uint32_t i = 0; i < clones; i++) {
        ids.push_back(nextEntityId++);
    }
    deferredExecutor.PushFunc([this, ids = std::move(ids), signature, constructors = std::move(constructors)]() {
        for (const auto& id : ids) {
            entities.emplace(id, signature);
        }
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
        archetype->CreateEntity(ids, constructors);
    });
    return ids;
}

const std::unordered_map<Signature, std::unique_ptr<Archetype>>& Decs::getArchetypes() {
    return archetypes;
}
