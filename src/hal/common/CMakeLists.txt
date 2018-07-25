
add_library(common
	common.h
	common.cpp
	net_udp.h
	net_udp.cpp
	net_tcp.h
	net_tcp.cpp
	base64.h
	base64.cpp
	bglobal.h
	bglobal.cpp
)

add_executable(test_http
    test_http_push.cpp
)

add_executable(test_json_base64
    test_json_base64.cpp
)

add_executable(test_bc_plate
    test_bc_plate.cpp
)

target_link_libraries(common pthread jpeg jsoncpp iconv)

target_link_libraries(test_http pthread common)

target_link_libraries(test_json_base64 pthread jsoncpp common)

target_link_libraries(test_bc_plate pthread common)
