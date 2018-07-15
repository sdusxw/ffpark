
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

target_link_libraries(common pthread jpeg jsoncpp)

target_link_libraries(test_http pthread common)
