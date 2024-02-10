file(GLOB_RECURSE PROJECT_FILES src/**.h src/**.cpp LIST_DIRECTORIES TRUE)

add_executable(test ${PROJECT_FILES})
source_group(TREE ${CMAKE_CURRENT_LIST_DIR} FILES ${PROJECT_FILES})

target_link_libraries(test PRIVATE glass)

set_target_properties(test PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/test)