#include "common.h"
#include "bglobal.h"
#include "ipc_config.h"
#include "ipc_controller.h"

#include "net_tcp.h"

#include <pthread.h>
#include <jsoncpp/json/json.h>

int main()
{
	NetTcpClient tcp_client;
	if(tcp_client.connect_server("192.168.1.13", 8127))
	{
		printf("ok\n");
	}
	Json::Value json_obj;
	Json::Value json_user;
	json_user["name"] = Json::Value("admin");
	json_user["digest"] = Json::Value("3ed07a078ad9d9cec0480d2e9736e14e");
	json_obj["sentcnt"] = Json::Value(1024);
	json_obj["user"] = json_user;
	json_obj["method"] = Json::Value("ivp_lpr_trigger");
	std::string json_msg = json_obj.toStyledString();

	int len_msg = json_msg.length();
	char str_len[10] = "";
	sprintf(str_len, "%d", len_msg);

	std::string http_header = "POST / HTTP/1.0\r\n";
	http_header += "Connection: keep-alive\r\n";
	http_header += "Content-Type: application/json;charset=utf-8\r\n";
	http_header += "Content-Length: ";
	http_header = http_header + str_len;
	http_header += "\r\nHost: 192.168.1.13:8127\r\n\r\n";
	std::string http_msg = http_header + json_msg;
	std::cout << http_msg << std::endl;
	std::string recv_msg;
	int n = tcp_client.send_data(http_msg, recv_msg);
	printf("sent:\t%d\n", n);
	std::cout << "recv:\t" << recv_msg << std::endl;
	return 0;
}
