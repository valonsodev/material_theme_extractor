
cmake_minimum_required(VERSION 3.14)
project(material_theme VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

set_target_properties(${LIBNAME} PROPERTIES CXX_VISIBILITY_PRESET hidden)
add_subdirectory(third_party)

add_executable(${PROJECT_NAME}
        material.cpp
)

target_link_libraries(${PROJECT_NAME} PRIVATE material_color)

target_include_directories(${PROJECT_NAME} PRIVATE
        ${CMAKE_SOURCE_DIR}
)
