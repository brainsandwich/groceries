cmake_minimum_required(VERSION 3.0)
project(groceries VERSION 0.1.0 LANGUAGES CXX)
cmake_policy(SET CMP0048 NEW)

set(EXTERNALS_DIR ${PROJECT_SOURCE_DIR}/external)

# GROCERIES SOURCES
file(GLOB_RECURSE GROCERIES_SOURCES "${PROJECT_SOURCE_DIR}/src/**.cpp")
file(GLOB_RECURSE GROCERIES_INCLUDES "${PROJECT_SOURCE_DIR}/include/**.hpp")
set(GROCERIES_INCLUDEDIR ${PROJECT_SOURCE_DIR}/include)

# GROCERIES LIB
add_library(groceries ${GROCERIES_SOURCES} ${GROCERIES_INCLUDES})
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set_target_properties(groceries PROPERTIES FOLDER GROCERIES/CORE)
target_include_directories(groceries PUBLIC ${GROCERIES_INCLUDEDIR})

# FETCH DEPENDENCIES
message(STATUS "cmd: python deps_fetcher.py")
execute_process(COMMAND "python" "./deps_fetcher.py"
	WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
	RESULT_VARIABLE fetch_result
)
message(STATUS "command returned ${fetch_result}")

# LOFX
add_subdirectory(${EXTERNALS_DIR}/lofx)
target_link_libraries(groceries lofx)

# TESTS
option(GROCERIES_BUILD_TESTS "Activate to build GROCERIES tests" OFF)

if (${GROCERIES_BUILD_TESTS})
	add_subdirectory(${PROJECT_SOURCE_DIR}/tests)
endif()