cmake_minimum_required(VERSION 3.5)

project(RootExtraction)
enable_testing()

# SET (CMAKE_LIBRARY_OUTPUT_DIRECTORY
#         ${PROJECT_BINARY_DIR}/bin
#         CACHE PATH
#         "Single Directory for all"
#     )

# SET (CMAKE_RUNTIME_OUTPUT_DIRECTORY
#         ${PROJECT_BINARY_DIR}/bin
#         CACHE PATH
#         "Single Directory for all"
#     )

# SET (CMAKE_ARCHIVE_OUTPUT_DIRECTORY
#         ${PROJECT_BINARY_DIR}/lib
#         CACHE PATH
#         "Single Directory for all"
#     )
    

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(ROOT CONFIG REQUIRED)
list(APPEND CMAKE_PREFIX_PATH $ENV{ROOTSYS})
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wnon-virtual-dtor -Wold-style-cast -Wcast-align -Wunused -Woverloaded-virtual -Wpedantic -Wconversion -Wsign-conversion -Wnull-dereference -Wdouble-promotion -Wformat=2 -fsanitize=undefined -g")

include(${ROOT_USE_FILE})

add_subdirectory("./General")
