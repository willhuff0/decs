cmake_minimum_required(VERSION 3.31)
project(decs)

set(CMAKE_CXX_STANDARD 17)

add_library(decs)
add_library(decs::decs ALIAS decs)

target_sources(decs
    PRIVATE
        src/Decs.cpp
        src/ComponentTypeId.cpp
        src/ComponentArray.cpp
        src/Archetype.cpp
        src/EntityBuilder.cpp
        src/Query.cpp
        src/SystemManager.cpp
        src/DeferredExecutor.cpp
    PUBLIC
        FILE_SET HEADERS
        BASE_DIRS include
        FILES
            include/decs/Decs.hpp
            include/decs/Types.hpp
            include/decs/ComponentTypeId.hpp
            include/decs/ComponentArray.hpp
            include/decs/Archetype.hpp
            include/decs/Query.hpp
            include/decs/System.hpp
            include/decs/SystemManager.hpp
            include/decs/EntityBuilder.hpp
            include/decs/DeferredConstructor.hpp
            include/decs/DeferredMutator.hpp
            include/decs/DeferredExecutor.hpp
            include/decs/ConcurrentQueue.hpp
)

target_include_directories(decs
    PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
           $<INSTALL_INTERFACE:include>
)
