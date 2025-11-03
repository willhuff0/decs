#pragma once

#include "Types.hpp"
#include "System.hpp"
#include "Archetype.hpp"

#include <unordered_map>
#include <memory>
#include <queue>
#include <cstdint>

using SystemKey = size_t;

class Decs;

class SystemManager {
    friend class Decs;

private:
    explicit SystemManager(Decs* decs);

    template<typename Func, typename... Comps>
    SystemKey RegisterSystem(Func func);
    void UnregisterSystem(SystemKey key);

    void OnArchetypeAdded(Signature signature, Archetype& archetype);
    void OnArchetypeRemoved(Signature signature, Archetype& archetype);

    void IterateAll();
    void IterateMutableAll(std::queue<Mutation>& mutations);

private:
    Decs* decs;

    template<typename Func>
    void forEachSubsignature(Signature signature, Func&& func);

    std::vector<std::reference_wrapper<Archetype>> getArchetypesWithSupersignature(Signature signature);

    std::atomic<SystemKey> nextKey;

    std::unordered_map<SystemKey, std::pair<Signature, std::unique_ptr<ISystem>>> systems;
    std::unordered_map<Signature, std::vector<SystemKey>> signatureToSystems;
};

template<typename Func, typename... Comps>
SystemKey SystemManager::RegisterSystem(Func func) {
    Signature signature = SignatureFromComps<Comps...>();
    SystemKey key = nextKey++;

    systems.emplace(key, std::make_pair(signature, std::make_unique<System<Func, Comps...>>(Query(signature, getArchetypesWithSupersignature(signature)), func)));

    auto [iter, emplaced] = signatureToSystems.emplace(signature, std::vector<SystemKey>());
    iter->second.push_back(key);

    return key;
}

template<typename Func>
void SystemManager::forEachSubsignature(Signature signature, Func&& func) {
    static_assert(std::is_invocable_v<Func, Signature>);

#if MAX_COMPONENTS <= 32
    using IntMask = uint32_t;
#elif MAX_COMPONENTS <= 64
    using IntMask = uint64_t;
#endif

    IntMask mask = signature.to_ullong();

    for (decltype(mask) submask = mask;; submask = (submask - 1) & mask) {
        func(Signature(submask));
        if (submask == 0) break;
    }
}
