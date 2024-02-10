set(GLAD_SOURCE_DIR "${CMAKE_CURRENT_LIST_DIR}/${GLASS_CONTEXT_VERSION_MAJOR}.${GLASS_CONTEXT_VERSION_MINOR}/")
message(STATUS "Selected Glad version ${GLASS_CONTEXT_VERSION_MAJOR}.${GLASS_CONTEXT_VERSION_MINOR}")

file(GLOB_RECURSE 
        GLAD_FILES 
            "${GLAD_SOURCE_DIR}/**.h" 
            "${GLAD_SOURCE_DIR}/**.c" 
        LIST_DIRECTORIES ON)

add_library(glad STATIC ${GLAD_FILES})
target_include_directories(glad PUBLIC ${GLAD_SOURCE_DIR}/include)