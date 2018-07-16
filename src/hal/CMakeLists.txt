project(Bipc)

cmake_minimum_required(VERSION 2.8)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(CMAKE_USE_RELATIVE_PATHS ON)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fPIC -O2 -g")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC -O2 -g")

if(APPLE)
	add_definitions(-DHAVE_XLOCALE_H)
endif(APPLE)

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/../bin)
set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/../lib)

set(bin_dir ${PROJECT_BINARY_DIR})
set(src_dir ${PROJECT_SOURCE_DIR})

set(SOLUTION_INCLUDE_DIRS
    ${src_dir}/ffhal
    ${src_dir}/hal/aio_bc
    ${src_dir}/common
)

#-------------------------------------------------------------------------------
#add subdirectories

add_subdirectory(ffhal)
add_subdirectory(hal/aio_bc)
add_subdirectory(common)

