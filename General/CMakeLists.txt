cmake_minimum_required(VERSION 3.5)

project(General)

include_directories(include)

find_package(ROOT 6.16 CONFIG REQUIRED)
find_package(nlohmann_json 3.2.0 REQUIRED)

file(GLOB_RECURSE SOURCES . source/*.cpp)

add_library(export_lib ${SOURCES})

target_link_libraries(export_lib PRIVATE nlohmann_json::nlohmann_json)
target_link_libraries(export_lib PUBLIC ROOT::Geom)


set(CATCH_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/test/catch)
add_library(Catch INTERFACE)
target_include_directories(Catch INTERFACE ${CATCH_INCLUDE_DIR})

file(GLOB_RECURSE TEST_SOURCES . test/*.cpp)
add_executable(my_test ${TEST_SOURCES})
add_test(NAME my_test COMMAND my_test)

target_link_libraries(my_test PRIVATE nlohmann_json::nlohmann_json)
target_link_libraries(my_test PUBLIC ROOT::Geom)
target_link_libraries(my_test PRIVATE Catch)
target_link_libraries(my_test PRIVATE export_lib)