#include <decs/EntityBuilder.hpp>

#include <decs/Decs.hpp>

EntityBuilder::EntityBuilder(std::shared_ptr<Decs> decs) : decs(std::move(decs)) { }

EntityId EntityBuilder::Build() {
    return decs->createEntity(constructors);
}
