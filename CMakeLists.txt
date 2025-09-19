cmake_minimum_required(VERSION 3.11)
project(App CXX)

set(CMAKE_CXX_STANDARD 20)

find_package(spdlog REQUIRED)

file(GLOB SRC_FILES
    ${CMAKE_SOURCE_DIR}/src/*.cpp
    ${CMAKE_SOURCE_DIR}/include/*/*.cpp
)

file(GLOB LIBRARY_DIRS RELATIVE ${CMAKE_SOURCE_DIR}/include ${CMAKE_SOURCE_DIR}/include/*)

set(INCLUDE_DIRS "")
foreach(dir ${LIBRARY_DIRS})
    if(IS_DIRECTORY "${CMAKE_SOURCE_DIR}/include/${dir}")
        list(APPEND INCLUDE_DIRS "${CMAKE_SOURCE_DIR}/include/${dir}")
    endif()
endforeach()

add_executable(app ${SRC_FILES})

target_include_directories(app PRIVATE ${INCLUDE_DIRS})

target_link_libraries(app PRIVATE
    spdlog::spdlog
    pthread
)
