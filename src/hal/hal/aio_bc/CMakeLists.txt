
include_directories(
    ${SOLUTION_INCLUDE_DIRS}
)

add_library(aio_bc
	aio_bc.h
	aio_bc.cpp
)

target_link_libraries(aio_bc common)

