#include "net_tcp.h"

int main()
{
	NetTcpServer ts;
	ts.open_bind_listen(2350);
	while(1)
	{
		std::string str;
		ts.get_message(str);
		printf("http response length:\t%d\n", str.length());
	}
	return 0;
}
