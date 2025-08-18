#include <SystemManager.hpp>

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
