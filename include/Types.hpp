#pragma once

#include <bitset>
#include <limits>
#include <cstdint>

#include <ComponentTypeId.hpp>

using EntityId = uint32_t;
#define MAX_ENTITIES std::numeric_limits<EntityId>::max();

#define MAX_COMPONENTS 32
using Signature = std::bitset<MAX_COMPONENTS>;

using ComponentIndex = EntityId;

template<typename... Comps>
Signature SignatureFromComps() {
    Signature signature;
    (signature.set(ComponentTypeId::Get<Comps>().GetValue()), ...);
    return signature;
}