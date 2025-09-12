#include <decs/EntityBuilder.hpp>

EntityBuilder::~EntityBuilder() {
    for (auto [componentTypeId, constructor] : constructors) {
        delete constructor;
    }
}

EntityId EntityBuilder::Build() {
    // TODO: push to DeferredExecutor
    [=]() {
      // TODO: construct entity
    }();
}
