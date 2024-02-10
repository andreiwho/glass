cmake_minimum_required(VERSION 3.10)

set_property(GLOBAL PROPERTY USE_FOLDERS TRUE)
set_property(GLOBAL PROPERTY PREDEFINED_TARGETS_FOLDER glassProj/vendor/cmake)

project(glass)

set(GLASS_CONTEXT_VERSION_MAJOR 4)
set(GLASS_CONTEXT_VERSION_MINOR 3)

option(GLASS_BUILD_TEST "Build test project" OFF)
option(GLASS_SHARED "" OFF)

file(GLOB_RECURSE 
    PROJECT_FILES 
        src/**.cpp 
        src/**.h 
        include/**.h 
        include/**.inl 
    LIST_DIRECTORIES ON)

if(GLASS_SHARED)
    add_library(glass SHARED ${PROJECT_FILES})
    target_compile_definitions(glass PUBLIC GLASS_SHARED PRIVATE GLASS_BUILD_SHARED)
else()
    add_library(glass STATIC ${PROJECT_FILES})
endif()

target_include_directories(glass PUBLIC include PRIVATE src)
source_group(TREE ${CMAKE_CURRENT_LIST_DIR} FILES ${PROJECT_FILES})

add_subdirectory(vendor/glad)
target_compile_definitions(glass 
    PUBLIC 
        "GLASS_CONTEXT_VERSION_MAJOR=${GLASS_CONTEXT_VERSION_MAJOR}"
        "GLASS_CONTEXT_VERSION_MINOR=${GLASS_CONTEXT_VERSION_MINOR}")

# GLFW
set(GLFW_BUILD_DOCS OFF CACHE BOOL "")
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "")
set(GLFW_BUILD_TESTS OFF CACHE BOOL "")
add_subdirectory(vendor/glfw)

target_link_libraries(glass PRIVATE glad glfw)
target_compile_features(glass PUBLIC cxx_std_23)

if(MSVC)
    set(GLASS_PROJECTS_VENDOR glad glfw uninstall update_mappings)
    set(GLASS_PROJECTS glass)
    foreach(PROJ ${GLASS_PROJECTS})
        set_target_properties(${PROJ} PROPERTIES FOLDER glassProj)
    endforeach()

    foreach(PROJ ${GLASS_PROJECTS_VENDOR})
        set_target_properties(${PROJ} PROPERTIES FOLDER glassProj/vendor)
    endforeach()
endif()

if(GLASS_BUILD_TEST)
    add_subdirectory(test)
    set_target_properties(test PROPERTIES FOLDER glassProj/tests)
endif()

if(CMAKE_CURRENT_LIST_DIR EQUAL CMAKE_SOURCE_DIR)
    file(COPY ".clang-format" DESTINATION ${CMAKE_BINARY_DIR})
endif()