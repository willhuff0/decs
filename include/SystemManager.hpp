#pragma once

#include <Types.hpp>
#include <System.hpp>

#include <unordered_map>
#include <memory>

using SystemKey = size_t;

class SystemManager {
    friend class Decs;

private:
    template<typename... Comps, typename Func>
    SystemKey RegisterSystem(Func func);
    void UnregisterSystem(SystemKey key);

    void OnArchetypeAdded(Signature signature, Archetype& archetype);
    void OnArchetypeRemoved(Signature signature, Archetype& archetype);

private:
    template<typename Func>
    void forEachSubsignature(Signature signature, Func&& func);

    std::atomic<SystemKey> nextKey;

    std::unordered_map<SystemKey, std::pair<Signature, std::unique_ptr<ISystem>>> systems;
    std::unordered_map<Signature, std::vector<SystemKey>> signatureToSystems;
};

template<typename... Comps, typename Func>
SystemKey SystemManager::RegisterSystem(Func func) {
    static_assert(std::is_invocable_v<Func, Comps...>);

    Signature signature = SignatureFromComps<Comps...>();
    SystemKey key = nextKey++;

    systems.emplace(key, std::make_pair(signature, std::make_unique<System<Comps...>>(Query({},{}), func)));

    auto [iter, emplaced] = signatureToSystems.emplace(signature, std::vector<std::unique_ptr<ISystem>>());
    iter->second.push_back(key);

    return key;
}

template<typename Func>
void SystemManager::forEachSubsignature(Signature signature, Func&& func) {
    static_assert(std::is_invocable_v<Func, Signature>);

#if MAX_COMPONENTS <= 32
    unsigned long mask = signature.to_ulong();
#elif MAX_COMPONENTS <= 64
    unsigned long long mask = signature.to_ullong();
#endif
    for (decltype(mask) submask = mask;; submask = (submask - 1) & mask) {
        func(Signature(submask));
        if (submask == 0) break;
    }
}
