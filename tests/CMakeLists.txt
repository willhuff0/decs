add_executable(DECS_TESTS)

target_sources(DECS_TESTS
    PRIVATE
        main.cpp
        moodycamel/MoodycamelQueue.hpp
)

target_link_libraries(DECS_TESTS
    PRIVATE
        decs::decs
)