#include "common.h"
#include "bglobal.h"
#include "ipc_config.h"
#include "ipc_controller.h"

#include "net_tcp.h"

#include <pthread.h>
#include <jsoncpp/json/json.h>

void * bcenter_message_loop(void* para);
void * get_ipc_config(void* para);

int main()
{
	//初始化全局变量
	g_bipc_config_done = false;
	open_in_queue_a = false;
	open_in_queue_b = false;
	msg_print("Bipc程序启动...");
	log_init();
	log_output("Bipc程序启动...");
	msg_print("检查是否Bipc已经启动...");
	log_output("检查是否Bipc已经启动...");
	if (is_have_instance())
	{
		msg_print("Bipc已经启动, 本次启动失败，程序自动退出");
		log_output("Bipc已经启动, 本次启动失败，程序自动退出");
		exit(1);
	}
	else
	{
		msg_print("Bipc进程检查通过");
		log_output("Bipc进程检查通过");
	}
	//获取本机IP地址
	g_ip_addr = get_local_ip();
	msg_print("本机IP地址："+g_ip_addr);
	log_output("本机IP地址："+g_ip_addr);
	msg_print("初始化与Bcenter的网络连接...");
	log_output("初始化与Bcenter的网络连接...");
	char lo_ip[] = "127.0.0.1";
	if (udp_sender.open(lo_ip, 5001))
		log_output("Bipc与Bcenter发送端口打开成功");
	else
	{
		log_output("Bipc与Bcenter发送端口打开失败");
		exit(1);
	}

	if (udp_receiver.listen(5002))
		log_output("Bcenter消息监听端口打开成功");
	else
	{
		log_output("Bcenter消息监听端口打开失败");
		exit(1);
	}
	log_output("Bipc与Bcenter的网络连接初始化成功");

	pthread_t tid_bcenter_msg_loop;
	pthread_attr_t attr_bcenter_msg_loop;
	pthread_attr_init(&attr_bcenter_msg_loop);
	pthread_create(&tid_bcenter_msg_loop, &attr_bcenter_msg_loop,
			bcenter_message_loop, NULL);

	pthread_t tid_get_ipc_config;
	pthread_attr_t attr_get_ipc_config;
	pthread_attr_init(&attr_get_ipc_config);
	pthread_create(&tid_get_ipc_config, &attr_get_ipc_config, get_ipc_config,
	NULL);

	pthread_join(tid_bcenter_msg_loop, NULL);
}

void * bcenter_message_loop(void* para)
{
	std::string bcenter_message = "";
	do
	{
		//获取bcenter发送的消息
		bcenter_message = udp_receiver.get_mesage();
		log_output("[bcenter->bipc]" + bcenter_message);
		//对bcenter发送的消息进行解析处理
		Json::Reader reader;
		Json::Value json_object;

		if (!reader.parse(bcenter_message, json_object))
		{
			//JSON格式错误导致解析失败
			log_output("[json]解析失败");
			continue;
		}
		//根据cmd来进入相应处理分支
		std::string string_cmd = json_object["cmd"].asString();
		if (string_cmd == "ipc_config")	//硬件配置信息
		{
			log_output("[json]收到硬件配置信息");
			if (get_ipc_config(json_object))
			{
				log_output("[json]硬件配置信息解析成功");
				g_bipc_config_done = true;
				log_output("[work]开始工作");
				ipc_start();
			}
		}
		if (string_cmd == "close_door")	//落杆
		{
			log_output("[json]收到落杆指令");
			close_door(json_object);
		}
		if (string_cmd == "open_door")	//开闸放行
		{
			log_output("[json]收到开闸放行指令");
			open_door(json_object);
		}
		if (string_cmd == "snap_shot")	//手动抓拍
		{
			log_output("[json]收到手动抓拍指令");
			snap_shot(json_object);
		}
		if (string_cmd == "exit")	//退出命令
		{
			log_output("[json]收到退出命令");
			exit(0);
		}
	} while (true);
	return NULL;
}
//间隔1s循环发送get_ipc_config命令，直到收到合法的硬件配置
void * get_ipc_config(void* para)
{
	do
	{
		udp_sender.send("{\"cmd\":\"get_ipc_config\"}");
		log_output(
				"[bipc->bcenter][get_ipc_config]{\"cmd\":\"get_ipc_config\"}");
		usleep(1000000);
	} while (!g_bipc_config_done);
	return NULL;
}
