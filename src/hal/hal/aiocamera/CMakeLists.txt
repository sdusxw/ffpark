
include_directories(
    ${SOLUTION_INCLUDE_DIRS}
)

add_library(aiocamera
	aiocamera.h
	aiocamera.cpp
)

target_link_libraries(aiocamera common blpr)

