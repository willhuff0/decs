#pragma once

#include "ComponentTypeId.hpp"
#include "ComponentArray.hpp"
#include "Archetype.hpp"
#include "DeferredMutator.hpp"

#include <unordered_map>
#include <queue>

class Query {
public:
    explicit Query(Signature signature, std::vector<std::reference_wrapper<Archetype>> archetypes);

    template<typename... Comps, typename Func,
        typename = std::enable_if_t<
            std::is_invocable_v<Func, Archetype&, ComponentIndex, const Comps&...> ||
            std::is_invocable_v<Func, Archetype&, const Comps&...> ||
            std::is_invocable_v<Func, ComponentIndex, const Comps&...> ||
            std::is_invocable_v<Func, const Comps&...>
        >>
    void Iterate(Func func);

    template<typename... Comps, typename Func,
        typename = std::enable_if_t<
            std::is_invocable_v<Func, EntityId, const DeferredMutator<Comps>&...> ||
            std::is_invocable_v<Func, const DeferredMutator<Comps>&...>
        >>
    std::queue<Mutation> IterateMutable(Func func);

    std::vector<std::reference_wrapper<Archetype>>& GetArchetypes();
    std::vector<std::reference_wrapper<ComponentArray>>& GetComponentArrays(ComponentTypeId componentTypeId);
    std::unordered_map<ComponentTypeId, std::vector<std::reference_wrapper<ComponentArray>>>& GetComponentArrayVectors();

private:
    std::vector<std::reference_wrapper<Archetype>> archetypes;
    std::unordered_map<ComponentTypeId, std::vector<std::reference_wrapper<ComponentArray>>> componentArrayVectors;
};

template<typename... Comps, typename Func, typename>
void Query::Iterate(Func func) {
    auto activeComponentArrayVectors = std::make_tuple(
        std::ref(componentArrayVectors.at(ComponentTypeId::Get<Comps>()))...
    );

    for (size_t i = 0; i < archetypes.size(); ++i) {
        Archetype& archetype = archetypes[i];

        auto componentArraysTuple = std::apply([i](auto&... vecs) {
            return std::make_tuple(std::ref(vecs[i].get())...);
        }, activeComponentArrayVectors);

        for (ComponentIndex j = 0; j < archetype.GetSize(); ++j) {
            std::apply([&](auto&... arrays) {
                if constexpr (std::is_invocable_v<Func, Archetype&, ComponentIndex, const Comps&...>) func(archetype, j, arrays.template Get<Comps>(j)...);
                else if constexpr (std::is_invocable_v<Func, Archetype&, const Comps&...>) func(archetype, arrays.template Get<Comps>(j)...);
                else if constexpr (std::is_invocable_v<Func, ComponentIndex, const Comps&...>) func(j, arrays.template Get<Comps>(j)...);
                else if constexpr (std::is_invocable_v<Func, const Comps&...>) func(arrays.template Get<Comps>(j)...);
            }, componentArraysTuple);
        }
    }
}

template<typename... Comps, typename Func, typename>
std::queue<Mutation> Query::IterateMutable(Func func) {
    std::queue<Mutation> mutations;

    auto activeComponentArrayVectors = std::make_tuple(
        std::ref(componentArrayVectors.at(ComponentTypeId::Get<Comps>()))...
    );

    for (size_t i = 0; i < archetypes.size(); ++i) {
        Archetype& archetype = archetypes[i];

        auto componentArraysTuple = std::apply([i](auto&... vecs) {
            return std::make_tuple(std::ref(vecs[i].get())...);
        }, activeComponentArrayVectors);

        for (ComponentIndex j = 0; j < archetype.GetSize(); ++j) {
            std::apply([&](auto&... arrays) {
                EntityId entityId = archetype.GetEntityId(j);

                auto mutatorsTuple = std::make_tuple(DeferredMutator<Comps>(mutations, entityId, arrays.template Get<Comps>(j))...);

                std::apply([&](auto&... mutators) {
                    if constexpr (std::is_invocable_v<Func, EntityId, const DeferredMutator<Comps>&...>) func(entityId, mutators...);
                    else if constexpr (std::is_invocable_v<Func, const DeferredMutator<Comps>&...>) func(mutators...);
                }, mutatorsTuple);
            }, componentArraysTuple);
        }
    }

    return mutations;
}
