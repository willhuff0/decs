#include <Query.hpp>

#include "EntityBuilder.hpp"

Query::Query(Signature signature, std::vector<std::reference_wrapper<Archetype>> archetypes) :
    archetypes(std::move(archetypes)) {
    for (ComponentTypeId::Value id = 0; id < MAX_COMPONENTS; id++) {
        if (!signature.test(id)) continue;
        componentArrayVectors.emplace(ComponentTypeId(id), std::vector<std::reference_wrapper<ComponentArray>>());
    }

    for (Archetype& archetype : this->archetypes) {
        for (auto& [componentTypeId, componentArrays] : componentArrayVectors) {
            componentArrays.emplace_back(archetype.GetComponentArray(componentTypeId));
        }
    }
}

std::vector<std::reference_wrapper<Archetype>>& Query::GetArchetypes() {
    return archetypes;
}

std::vector<std::reference_wrapper<ComponentArray>>& Query::GetComponentArrays(ComponentTypeId componentTypeId) {
    return componentArrayVectors.at(componentTypeId);
}

std::unordered_map<ComponentTypeId, std::vector<std::reference_wrapper<ComponentArray>>>&
Query::GetComponentArrayVectors() {
    return componentArrayVectors;
}
