#pragma once

#include <bitset>
#include <limits>
#include <cstdint>

using EntityId = uint32_t;
#define MAX_ENTITIES std::numeric_limits<EntityId>::max();

#define MAX_COMPONENTS 8
using Signature = std::bitset<MAX_COMPONENTS>;
