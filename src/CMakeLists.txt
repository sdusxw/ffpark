cmake_minimum_required(VERSION 2.8)

project(ffpark)

set(CMAKE_CXX_STANDARD 11)

#cmake_policy(SET CMP0053 OLD)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -pthread")

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fPIC -g")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC -g")

#
set(CMAKE_USE_RELATIVE_PATHS ON)

#find_package(OpenCV REQUIRED)

#
set(src_dir "@PROJECT_SOURCE_DIR@")
set(bin_dir "@PROJECT_BINARY_DIR@")
set(lib_dir "@PROJECT_BINARY_DIR@")

SET(EXECUTABLE_OUTPUT_PATH ${bin_dir}/bin)
SET(LIBRARY_OUTPUT_PATH ${lib_dir}/lib)

#set(SOLUTION_INCLUDE_DIRS
#        ${src_dir}/include
#        ${src_dir}/include/3rd
#        ${src_dir}/include/3rd/caffe
#        ${src_dir}/common
#        ${src_dir}/utils
#        )

# add subdirectories
add_subdirectory(hal)
