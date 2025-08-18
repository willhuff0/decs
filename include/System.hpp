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

protected:
    virtual Query& GetQuery() = 0;
    virtual void SetQuery(Query newQuery) = 0;

    virtual void Iterate() = 0;
    virtual std::queue<Mutation> IterateMutable() = 0;
};

template<typename... Comps>
class System : public ISystem {
    using Func = std::function<void(Comps...)>;

public:
    explicit System(Query query, Func func) :
        func(func), query(std::move(query)) {
        cacheArchetypesComponentArrays();
    }

protected:
    Query& GetQuery() override;
    void SetQuery(Query query) override;

    void Iterate() override;
    std::queue<Mutation> IterateMutable() override;

private:
    Func func;

    Query query;

    template<typename T>
    struct TransformType { using Type = ComponentArray; };
    std::vector<std::pair<std::reference_wrapper<Archetype>, std::tuple<std::reference_wrapper<typename TransformType<Comps>::Type>...>>> archetypesComponentArrays;

    void cacheArchetypesComponentArrays();
};

template<typename... Comps>
Query& System<Comps...>::GetQuery() {
    return query;
}

template<typename... Comps>
void System<Comps...>::SetQuery(Query newQuery) {
    query = newQuery;
}

template<typename... Comps>
void System<Comps...>::Iterate() {
    if constexpr (std::is_invocable_v<Func, Archetype&, ComponentIndex, const Comps&...> ||
                  std::is_invocable_v<Func, Archetype&, const Comps&...> ||
                  std::is_invocable_v<Func, ComponentIndex, const Comps&...> ||
                  std::is_invocable_v<Func, const Comps&...>) {
        for (const auto& [archetype, componentArrays] : archetypesComponentArrays) {
            for (ComponentIndex j = 0; j < archetype.get().GetSize(); ++j) {
                std::apply([&](auto&... arrays) {
                    if constexpr (std::is_invocable_v<Func, Archetype&, ComponentIndex, const Comps& ...>) func(archetype, j, arrays.get().template Get<Comps>(j)...);
                    else if constexpr (std::is_invocable_v<Func, Archetype&, const Comps& ...>) func(archetype, arrays.get().template Get<Comps>(j)...);
                    else if constexpr (std::is_invocable_v<Func, ComponentIndex, const Comps& ...>) func(j, arrays.get().template Get<Comps>(j)...);
                    else if constexpr (std::is_invocable_v<Func, const Comps& ...>) func(arrays.get().template Get<Comps>(j)...);
                }, componentArrays);
            }
        }
    }
}

template<typename... Comps>
std::queue<Mutation> System<Comps...>::IterateMutable() {
    if constexpr (std::is_invocable_v<Func, EntityId, const DeferredMutator<Comps>&...> ||
                  std::is_invocable_v<Func, const DeferredMutator<Comps>&...>) {
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
    return {};
}

template<typename... Comps>
void System<Comps...>::cacheArchetypesComponentArrays() {
    auto componentArrayVectors = std::make_tuple(std::ref(query.GetComponentArrayVectors().at(ComponentTypeId::Get<Comps>()))...);

    archetypesComponentArrays.clear();
    archetypesComponentArrays.reserve(query.GetArchetypes().size());
    for (size_t i = 0; i < query.GetArchetypes().size(); ++i) {
        archetypesComponentArrays.push_back(std::make_pair(query.GetArchetypes()[i], std::apply([i](auto&... vecs) {
            return std::make_tuple(std::ref(vecs[i].get())...);
        }, componentArrayVectors)));
    }
}