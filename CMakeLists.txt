cmake_minimum_required(VERSION 3.15)
project(StatsLib)

# Create the header-only library
add_library(StatsLib INTERFACE)

target_include_directories(StatsLib INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

find_package(Armadillo REQUIRED)
find_package(Eigen3 REQUIRED)
find_package(Boost REQUIRED)

add_executable(main examples/main.cpp)
target_link_libraries(main PRIVATE 
    StatsLib 
    ${ARMADILLO_LIBRARIES} 
    Eigen3::Eigen
    Boost::boost
    )
    
target_include_directories(main PRIVATE ${ARMADILLO_INCLUDE_DIRS})