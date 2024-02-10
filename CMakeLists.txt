cmake_minimum_required(VERSION 3.10)
project(glass)

set(GLASS_CONTEXT_VERSION_MAJOR 4)
set(GLASS_CONTEXT_VERSION_MINOR 3)

file(GLOB_RECURSE 
    PROJECT_FILES 
        src/**.cpp 
        src/**.h 
        include/**.h 
        include/**.inl 
    LIST_DIRECTORIES ON)

add_library(glass STATIC ${PROJECT_FILES})

target_include_directories(glass PUBLIC include PRIVATE src)
source_group(TREE ${CMAKE_CURRENT_LIST_DIR} FILES ${PROJECT_FILES})

add_subdirectory(vendor/glad)
target_compile_definitions(glass 
    PUBLIC 
        "GLASS_CONTEXT_VERSION_MAJOR=${GLASS_CONTEXT_VERSION_MAJOR}"
        "GLASS_CONTEXT_VERSION_MINOR=${GLASS_CONTEXT_VERSION_MINOR}")