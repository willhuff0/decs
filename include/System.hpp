#pragma once

#include <Types.hpp>
#include <DeferredMutator.hpp>
#include <ComponentArray.hpp>
#include <Query.hpp>

#include <type_traits>
#include <utility>

class ISystem {
public:
    virtual ~ISystem() = default;
};

template<typename... Comps>
class System : public ISystem {
    friend class Decs;

    using Func = std::function<void(Comps...)>;

public:
    explicit System(Query query, Func func) :
        func(func), query(std::move(query)) {
        cacheArchetypesComponentArrays();
    }

private:
    template<typename F = Func, typename = std::enable_if_t<
            std::is_invocable_v<F, Archetype&, ComponentIndex, const Comps&...> ||
            std::is_invocable_v<F, Archetype&, const Comps&...> ||
            std::is_invocable_v<F, ComponentIndex, const Comps&...> ||
            std::is_invocable_v<F, const Comps&...>>>
    void Iterate();

    template<typename F = Func, typename = std::enable_if_t<
            std::is_invocable_v<F, EntityId, const DeferredMutator<Comps>&...> ||
            std::is_invocable_v<F, const DeferredMutator<Comps>&...>>>
    std::queue<Mutation> IterateMutable();

private:
    Func func;

    Query query;

    template<typename T>
    struct TransformType { using Type = ComponentArray; };
    std::vector<std::pair<std::reference_wrapper<Archetype>, std::tuple<std::reference_wrapper<typename TransformType<Comps>::Type>...>>> archetypesComponentArrays;

    void cacheArchetypesComponentArrays();
};

template<typename... Comps>
template<typename F, typename>
void System<Comps...>::Iterate() {
    for (const auto& [archetype, componentArrays] : archetypesComponentArrays) {
        for (ComponentIndex j = 0; j < archetype.get().GetSize(); ++j) {
            std::apply([&](auto&... arrays) {
                if constexpr (std::is_invocable_v<Func, Archetype&, ComponentIndex, const Comps&...>) func(archetype, j, arrays.get().template Get<Comps>(j)...);
                else if constexpr (std::is_invocable_v<Func, Archetype&, const Comps&...>) func(archetype, arrays.get().template Get<Comps>(j)...);
                else if constexpr (std::is_invocable_v<Func, ComponentIndex, const Comps&...>) func(j, arrays.get().template Get<Comps>(j)...);
                else if constexpr (std::is_invocable_v<Func, const Comps&...>) func(arrays.get().template Get<Comps>(j)...);
            }, componentArrays);
        }
    }

}

template<typename... Comps>
template<typename F, typename>
std::queue<Mutation> System<Comps...>::IterateMutable() {
    std::queue<Mutation> mutations;

    for (const auto& [archetype, componentArrays] : archetypesComponentArrays) {
        for (ComponentIndex j = 0; j < archetype.get().GetSize(); ++j) {
            std::apply([&](auto&... arrays) {
                EntityId entityId = archetype.get().GetEntityId(j);

                auto mutatorsTuple = std::make_tuple(DeferredMutator<Comps>(mutations, entityId, arrays.template Get<Comps>(j))...);

                std::apply([&](auto&... mutators) {
                    if constexpr (std::is_invocable_v<Func, EntityId, const DeferredMutator<Comps>&...>) func(entityId, mutators...);
                    else if constexpr (std::is_invocable_v<Func, const DeferredMutator<Comps>&...>) func(mutators...);
                }, mutatorsTuple);
            }, componentArrays);
        }
    }

    return mutations;
}

template<typename... Comps>
void System<Comps...>::cacheArchetypesComponentArrays() {
    auto componentArrayVectors = std::make_tuple(std::ref(query.GetComponentArrayVectors().at(ComponentTypeId::Get<Comps>()))...);

    archetypesComponentArrays.clear();
    archetypesComponentArrays.reserve(query.GetArchetypes().size());
    for (size_t i = 0; i < query.GetArchetypes().size(); ++i) {
        archetypesComponentArrays.push_back(std::make_pair(query.GetArchetypes()[i], std::apply([i](auto& ... vecs) {
            return std::make_tuple(std::ref(vecs[i].get())...);
        }, componentArrayVectors)));
    }
}