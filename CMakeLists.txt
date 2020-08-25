cmake_minimum_required(VERSION 3.5)

project(RootExtraction)

SET (CMAKE_LIBRARY_OUTPUT_DIRECTORY
        ${PROJECT_BINARY_DIR}/bin
        CACHE PATH
        "Single Directory for all"
    )

SET (CMAKE_RUNTIME_OUTPUT_DIRECTORY
        ${PROJECT_BINARY_DIR}/bin
        CACHE PATH
        "Single Directory for all"
    )

SET (CMAKE_ARCHIVE_OUTPUT_DIRECTORY
        ${PROJECT_BINARY_DIR}/lib
        CACHE PATH
        "Single Directory for all"
    )
    
find_package(ROOT CONFIG REQUIRED)
list(APPEND CMAKE_PREFIX_PATH $ENV{ROOTSYS})

include(${ROOT_USE_FILE})

add_subdirectory("./General")