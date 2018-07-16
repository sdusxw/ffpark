
include_directories(
    ${SOLUTION_INCLUDE_DIRS}
)

add_executable(ffhal
    main.cpp
    ipc_config.h
    ipc_config.cpp
    ipc_controller.h
    ipc_controller.cpp
)

#add_executable(trigger
#	trigger.cpp
#)

#add_executable(test-server
#	test-server.cpp
#)

target_link_libraries(ffhal jsoncpp common aiocamera)

#target_link_libraries(trigger jsoncpp common )

#target_link_libraries(test-server jsoncpp common )
