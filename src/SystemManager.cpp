#include <decs/SystemManager.hpp>

#include <decs/Decs.hpp>

SystemManager::SystemManager(Decs* decs) : decs(decs) { }

void SystemManager::UnregisterSystem(SystemKey key) {
    auto& pair = systems.at(key);
    signatureToSystems.erase(pair.first);
    systems.erase(key);
}

void SystemManager::OnArchetypeAdded(Signature signature, Archetype& archetype) {
    forEachSubsignature(signature, [this, &archetype](Signature sub) {
       auto iter = signatureToSystems.find(sub);
       if (iter != signatureToSystems.end()) {
           for (const auto& key: iter->second) {
               ISystem* system = systems.at(key).second.get();
               std::vector<std::reference_wrapper<Archetype>> archetypes = system->GetQuery().GetArchetypes();
               archetypes.emplace_back(archetype);
               system->SetQuery(Query(sub, archetypes));
           }
       }
    });
}

void SystemManager::OnArchetypeRemoved(Signature signature, Archetype& archetype) {
    forEachSubsignature(signature, [this, &archetype](Signature sub) {
        auto iter = signatureToSystems.find(sub);
        if (iter != signatureToSystems.end()) {
            for (const auto& key: iter->second) {
                ISystem* system = systems.at(key).second.get();
                std::vector<std::reference_wrapper<Archetype>> archetypes = system->GetQuery().GetArchetypes();
                archetypes.erase(std::remove_if(archetypes.begin(), archetypes.end(),
                                                [&archetype](const auto& other) {
                    return &other.get() == &archetype;
                }), archetypes.end());
                system->SetQuery(Query(sub, archetypes));
            }
        }
    });
}

void SystemManager::IterateAll() {
    for (const auto& [_, pair]: systems) {
        pair.second->Iterate();
    }
}

void SystemManager::IterateMutableAll(std::queue<Mutation>& mutations) {
    for (const auto& [_, pair]: systems) {
        pair.second->IterateMutable(mutations);
    }
}

std::vector<std::reference_wrapper<Archetype>> SystemManager::getArchetypesWithSupersignature(Signature signature) {
#if MAX_COMPONENTS <= 32
    using IntMask = uint32_t;
#elif MAX_COMPONENTS <= 64
    using IntMask = uint64_t;
#endif

    IntMask mask = signature.to_ullong();

    std::vector<std::reference_wrapper<Archetype>> archetypes;
    for (const auto& [sub, archetype] : decs->getArchetypes()) {
        IntMask subMask = sub.to_ullong();
        if ((subMask & mask) == mask) {
            archetypes.emplace_back(*archetype);
        }
    }
    return archetypes;
}
