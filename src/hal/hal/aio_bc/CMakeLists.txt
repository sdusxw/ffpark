
include_directories(
    ${SOLUTION_INCLUDE_DIRS}
)

add_library(aio_bc
	aio_bc.h
	aio_bc.cpp
)

add_executable(test_open_door
    test_open_door.cpp)

target_link_libraries(aio_bc common)

target_link_libraries(test_open_door common)

