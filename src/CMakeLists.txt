set(PROJECT_NAME ${CMAKE_PROJECT_NAME})

set(CMAKE_CXX_FLAGS "-g -std=c++17")
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

include_directories(${PROJECT_SOURCE_DIR}/data)
add_executable(${PROJECT_NAME}_run test.cpp)
