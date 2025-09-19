cmake_minimum_required(VERSION 3.11)
project(App CXX)

set(CMAKE_CXX_STANDARD 20)

# Set default build type if not specified
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build." FORCE)
endif()

# Find Boost with log component
find_package(Boost REQUIRED COMPONENTS log)

# Find fmt
find_package(fmt REQUIRED)

# Automatically collect source files (with CONFIGURE_DEPENDS for auto-detection of new files)
file(GLOB_RECURSE SRC_FILES CONFIGURE_DEPENDS ${CMAKE_SOURCE_DIR}/src/*.cpp ${CMAKE_SOURCE_DIR}/src/*/*.cpp)

# Collect all directories in the 'include' folder to be searched for headers
file(GLOB INCLUDE_DIRS LIST_DIRECTORIES true ${CMAKE_SOURCE_DIR}/include/*)
list(APPEND INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/include)

# Create the executable
add_executable(app ${SRC_FILES})

# Add the include directories to the target (this will add all subdirectories in 'include')
target_include_directories(app PRIVATE ${INCLUDE_DIRS})

# Link the libraries
target_link_libraries(app PRIVATE
    Boost::log
    fmt::fmt
    pthread
)

# Custom target to run the application
add_custom_target(run
    COMMAND app
    DEPENDS app
    COMMENT "Running the application"
)

# Custom target to clean the build directory
add_custom_target(clean-build
    COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_BINARY_DIR}/*
    COMMENT "Cleaning the build directory"
)

# Install target
install(TARGETS app DESTINATION bin)
