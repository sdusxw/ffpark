
include_directories(
    ${SOLUTION_INCLUDE_DIRS}
)

add_library(aio_bc
	aio_bc.h
	aio_bc.cpp
)

add_executable(test_open_door
    test_open_door.cpp)

add_executable(test_lcd_voice
    test_lcd_voice.cpp)

add_executable(test_lcd_show
    test_lcd_show.cpp)

target_link_libraries(aio_bc common)

target_link_libraries(test_open_door common)

target_link_libraries(test_lcd_voice common)

target_link_libraries(test_lcd_show common jsoncpp)

