#include "aio_bc.h"

#include "base64.h"

#include <sys/time.h>
#include <time.h>
#include <jsoncpp/json/json.h>

AioCamera aio_cam_a_main;	//A通道抓拍主机
AioCamera aio_cam_a_aux;		//A通道抓拍辅机
AioCamera aio_cam_b_main;	//B通道抓拍主机
AioCamera aio_cam_b_aux;		//B通道抓拍辅机

Vehicle aio_channel_a_main_vehicle;	//A通道主相机识别结果
Vehicle aio_channel_a_aux_vehicle;		//A通道辅相机识别结果
Vehicle aio_channel_b_main_vehicle;	//B通道主相机识别结果
Vehicle aio_channel_b_aux_vehicle;		//B通道辅相机识别结果

/*
 * 智能机初始化
 * 1. 初始化相机
 */
bool aio_ipc_start()
{
	//初始化A通道
	if (g_machine.channel_a_enable)
	{
		//初始化A通道抓拍主机
		aio_cam_a_main.camera_ip = g_machine.channel_a.main_camera.device_ip_id;
		aio_cam_a_main.username =
				g_machine.channel_a.main_camera.device_username;
		aio_cam_a_main.password =
				g_machine.channel_a.main_camera.device_password;
		aio_cam_a_main.camera_label = "A";
		aio_cam_a_main.initialize();
		//初始化A通道抓拍辅机
		if (g_machine.a_aux_camera)
		{
			aio_cam_a_aux.camera_ip =
					g_machine.channel_a.aux_camera.device_ip_id;
			aio_cam_a_aux.username =
					g_machine.channel_a.aux_camera.device_username;
			aio_cam_a_aux.password =
					g_machine.channel_a.aux_camera.device_password;
			aio_cam_a_aux.camera_label = "AA";
			aio_cam_a_aux.initialize();
		}
	}
	//初始化B通道
	if (g_machine.channel_b_enable)
	{
		//初始化B通道抓拍主机
		aio_cam_b_main.camera_ip = g_machine.channel_b.main_camera.device_ip_id;
		aio_cam_b_main.username =
				g_machine.channel_b.main_camera.device_username;
		aio_cam_b_main.password =
				g_machine.channel_b.main_camera.device_password;
		aio_cam_b_main.camera_label = "B";
		aio_cam_b_main.initialize();
		//初始化B通道抓拍辅机
		if (g_machine.b_aux_camera)
		{
			aio_cam_b_aux.camera_ip =
					g_machine.channel_b.aux_camera.device_ip_id;
			aio_cam_b_aux.username =
					g_machine.channel_b.aux_camera.device_username;
			aio_cam_b_aux.password =
					g_machine.channel_b.aux_camera.device_password;
			aio_cam_b_aux.camera_label = "BA";
			aio_cam_b_aux.initialize();
		}
	}
	return true;
}
/*
 * 落杆命令
 */
bool aio_close_door(std::string channel_id, std::string in_out)
{
	return true;
}
/*
 * 开闸命令
 * flag: once表示开一次，keep表示一直开
 */
bool aio_open_door(std::string channel_id, std::string in_out, std::string flag)
{
	bool b_connect = false;
	NetTcpClient tcp_client;
	char str_msg[1024];
	std::string log_msg;
	std::string camera_ip = "";
	if ((in_out == "入口") || (g_machine.one_way))
	{
		camera_ip = g_machine.channel_a.main_camera.device_ip_id;
	}
	else
	{
		camera_ip = g_machine.channel_b.main_camera.device_ip_id;
	}
	if (tcp_client.connect_server(camera_ip, 8127))
	{
		sprintf(str_msg, "[AioCamera]连接相机%s的8127端口成功", camera_ip.c_str());
		log_msg = str_msg;
		msg_print(log_msg);
		log_output(log_msg);
		b_connect = true;
	}
	else
	{
		sprintf(str_msg, "[AioCamera]连接相机%s的8127端口失败", camera_ip.c_str());
		log_msg = str_msg;
		msg_print(log_msg);
		log_output(log_msg);
		b_connect = false;
	}
	if (b_connect)
	{
		Json::Value json_obj;
		Json::Value json_user;
		json_user["name"] = Json::Value("admin");
		json_user["digest"] = Json::Value("3ed07a078ad9d9cec0480d2e9736e14e");
		json_obj["sentcnt"] = Json::Value(1024);
		json_obj["user"] = json_user;
		json_obj["method"] = Json::Value("ivp_lpr_manual_open_gate");
		std::string json_msg = json_obj.toStyledString();

		char str_len[10] = "";
		sprintf(str_len, "%d", json_msg.length());

		std::string http_header = "POST / HTTP/1.0\r\n";
		http_header += "Connection: keep-alive\r\n";
		http_header += "Content-Type: application/json;charset=utf-8\r\n";
		http_header += "Content-Length: ";
		http_header = http_header + str_len;
		http_header += "\r\nHost: 192.168.1.110:8127\r\n\r\n";
		std::string http_msg = http_header + json_msg;
		std::string recv_msg;
		int n = tcp_client.send_data(http_msg, recv_msg);
		tcp_client.dis_connect();
		std::cout << recv_msg << std::endl;
		if (recv_msg.length() > 0)	//有返回的消息
		{
			usleep(500000);
			json_obj["method"] = Json::Value("ivp_lpr_manual_close_gate");
			sprintf(str_len, "%d", json_msg.length());

			http_header = "POST / HTTP/1.0\r\n";
			http_header += "Connection: keep-alive\r\n";
			http_header += "Content-Type: application/json;charset=utf-8\r\n";
			http_header += "Content-Length: ";
			http_header = http_header + str_len;
			http_header += "\r\nHost: 192.168.1.110:8127\r\n\r\n";
			http_msg = http_header + json_msg;
			if (tcp_client.connect_server(camera_ip, 8127))
			{
				sprintf(str_msg, "[AioCamera]连接相机%s的8127端口成功",
						camera_ip.c_str());
				log_msg = str_msg;
				msg_print(log_msg);
				log_output(log_msg);
				b_connect = true;
			}
			else
			{
				sprintf(str_msg, "[AioCamera]连接相机%s的8127端口失败",
						camera_ip.c_str());
				log_msg = str_msg;
				msg_print(log_msg);
				log_output(log_msg);
				b_connect = false;
			}
			if (b_connect)
			{
				std::string recv_msg2 = "";
				std::cout << "http send:\t" << http_msg << std::endl;
				int n = tcp_client.send_data(http_msg, recv_msg2);
				tcp_client.dis_connect();
				std::cout << "http recv:\t" << recv_msg2 << std::endl;
			}
		}
	}
	return true;
}
/*
 * 手动抓拍
 * flag: 目前只支持once
 */
bool aio_snap_shot(std::string channel_id, std::string in_out, std::string flag)
{
	bool b_connect = false;
	NetTcpClient tcp_client;
	char str_msg[1024];
	std::string log_msg;
	std::string camera_ip = "";
	if (in_out == "入口")
	{
		camera_ip = g_machine.channel_a.main_camera.device_ip_id;
	}
	else
	{
		camera_ip = g_machine.channel_b.main_camera.device_ip_id;
	}
	if (tcp_client.connect_server(camera_ip, 8127))
	{
		sprintf(str_msg, "[AioCamera]连接相机%s的8127端口成功", camera_ip.c_str());
		log_msg = str_msg;
		msg_print(log_msg);
		log_output(log_msg);
		b_connect = true;
	}
	else
	{
		sprintf(str_msg, "[AioCamera]连接相机%s的8127端口失败", camera_ip.c_str());
		log_msg = str_msg;
		msg_print(log_msg);
		log_output(log_msg);
		b_connect = false;
	}
	if (b_connect)
	{
		Json::Value json_obj;
		Json::Value json_user;
		json_user["name"] = Json::Value("admin");
		json_user["digest"] = Json::Value("3ed07a078ad9d9cec0480d2e9736e14e");
		json_obj["sentcnt"] = Json::Value(1024);
		json_obj["user"] = json_user;
		json_obj["method"] = Json::Value("ivp_lpr_trigger");
		std::string json_msg = json_obj.toStyledString();

		char str_len[10] = "";
		sprintf(str_len, "%d", json_msg.length());

		std::string http_header = "POST / HTTP/1.0\r\n";
		http_header += "Connection: keep-alive\r\n";
		http_header += "Content-Type: application/json;charset=utf-8\r\n";
		http_header += "Content-Length: ";
		http_header = http_header + str_len;
		http_header += "\r\nHost: 192.168.1.110:8127\r\n\r\n";
		std::string http_msg = http_header + json_msg;
		std::string recv_msg;
		int n = tcp_client.send_data(http_msg, recv_msg);
		tcp_client.dis_connect();
		std::cout << recv_msg << std::endl;
		if (recv_msg.length() > 0)	//有返回的消息
		{

		}
	}
	return true;
}

/*
 * A通道发送车辆通过信息
 */
void aio_send_car_pass_a()
{

}

/*
 * B通道发送车辆通过信息
 */
void aio_send_car_pass_b()
{

}
/*
 * A通道推送车辆识别信息
 */
void aio_send_car_come_a()
{
	Json::Value json_car;
	Json::Value json_car_info;
	Json::Value json_vehicle;
	json_car["cmd"] = Json::Value("car_come");
	json_car["time"] = Json::Value(get_time_sec());
	json_car["channel_id"] = Json::Value(g_machine.channel_a.channel_id);
	json_car["in_out"] = Json::Value(g_machine.channel_a.in_out);
	json_car_info["ipc_ip"] = Json::Value(
			g_machine.channel_a.main_camera.device_ip_id);
	json_vehicle["plate"] = Json::Value(aio_channel_a_main_vehicle.plate);
	json_vehicle["pcolor"] = Json::Value(aio_channel_a_main_vehicle.pcolor);
	json_vehicle["brand"] = Json::Value(aio_channel_a_main_vehicle.brand);
	json_vehicle["type"] = Json::Value(aio_channel_a_main_vehicle.type);
	json_vehicle["color"] = Json::Value(aio_channel_a_main_vehicle.color);
	json_vehicle["path"] = Json::Value(aio_channel_a_main_vehicle.path);
	json_car_info["vehicle"] = json_vehicle;
	json_car["car_info"].append(json_car_info);
	std::string car_come_msg = json_car.toStyledString();
	if (udp_sender.send(car_come_msg))
	{
		log_output(car_come_msg);
		log_output("推送A通道车辆信息成功");
	}
	else
	{
		log_output(car_come_msg);
		log_output("推送A通道车辆信息失败");
	}
}
/*
 * B通道推送车辆识别信息
 */
void aio_send_car_come_b()
{
	Json::Value json_car;
	Json::Value json_car_info;
	Json::Value json_vehicle;
	json_car["cmd"] = Json::Value("car_come");
	json_car["time"] = Json::Value(get_time_sec());
	json_car["channel_id"] = Json::Value(g_machine.channel_b.channel_id);
	json_car["in_out"] = Json::Value(g_machine.channel_b.in_out);
	json_car_info["ipc_ip"] = Json::Value(
			g_machine.channel_b.main_camera.device_ip_id);
	json_vehicle["plate"] = Json::Value(aio_channel_b_main_vehicle.plate);
	json_vehicle["pcolor"] = Json::Value(aio_channel_b_main_vehicle.pcolor);
	json_vehicle["brand"] = Json::Value(aio_channel_b_main_vehicle.brand);
	json_vehicle["type"] = Json::Value(aio_channel_b_main_vehicle.type);
	json_vehicle["color"] = Json::Value(aio_channel_b_main_vehicle.color);
	json_vehicle["path"] = Json::Value(aio_channel_b_main_vehicle.path);
	json_car_info["vehicle"] = json_vehicle;
	json_car["car_info"].append(json_car_info);
	std::string car_come_msg = json_car.toStyledString();
	if (udp_sender.send(car_come_msg))
	{
		log_output(car_come_msg);
		log_output("推送B通道车辆信息成功");
	}
	else
	{
		log_output(car_come_msg);
		log_output("推送B通道车辆信息失败");
	}
}
/**
 * 智能相机初始化
 * 1. 配置相机
 * 2. 初始化TCP接收服务器
 *
 */
bool AioCamera::initialize()
{
	char str_msg[1024];
	std::string log_msg;
	//根据相机的camera_label来确定端口号
	//端口分配：A入口主机2350，AA入口辅机2351，B出口主机2352，BA出口辅机2353
	if (camera_label == "A")
		listen_port = 2350;
	else if (camera_label == "AA")
		listen_port = 2351;
	else if (camera_label == "B")
		listen_port = 2352;
	else if (camera_label == "BA")
		listen_port = 2353;
	else
		return false;
	//打开TCP server服务端口
	if (tcp_server.open_bind_listen(listen_port))
	{
		sprintf(str_msg, "[AioCamera]TCP服务器端口%d打开成功", listen_port);
		log_msg = str_msg;
		msg_print(log_msg);
		log_output(log_msg);
	}
	else
	{
		sprintf(str_msg, "[AioCamera]TCP服务器端口%d打开失败", listen_port);
		log_msg = str_msg;
		msg_print(log_msg);
		log_output(log_msg);
		return false;
	}

	//通过TCP 8127端口发送消息，配置接收车牌推送消息HTTP服务器
	bool b_connect = false;
	NetTcpClient tcp_client;
	if (tcp_client.connect_server(camera_ip, 8127))
	{
		sprintf(str_msg, "[AioCamera]连接相机%s的8127端口成功", camera_ip.c_str());
		log_msg = str_msg;
		msg_print(log_msg);
		log_output(log_msg);
		b_connect = true;
	}
	else
	{
		sprintf(str_msg, "[AioCamera]连接相机%s的8127端口失败", camera_ip.c_str());
		log_msg = str_msg;
		msg_print(log_msg);
		log_output(log_msg);
		b_connect = false;
	}
	if (b_connect)
	{
		Json::Value json_obj;
		Json::Value json_user;
		json_user["name"] = Json::Value("admin");
		json_user["digest"] = Json::Value("3ed07a078ad9d9cec0480d2e9736e14e");
		json_obj["sentcnt"] = Json::Value(1024);
		json_obj["user"] = json_user;
		json_obj["method"] = Json::Value("ivp_get_param");
		std::string json_msg = json_obj.toStyledString();

		char str_len[10] = "";
		sprintf(str_len, "%d", json_msg.length());

		std::string http_header = "POST / HTTP/1.0\r\n";
		http_header += "Connection: keep-alive\r\n";
		http_header += "Content-Type: application/json;charset=utf-8\r\n";
		http_header += "Content-Length: ";
		http_header = http_header + str_len;
		http_header += "\r\nHost: 192.168.1.110:8127\r\n\r\n";
		std::string http_msg = http_header + json_msg;
		std::string recv_msg;
		int n = tcp_client.send_data(http_msg, recv_msg);
		tcp_client.dis_connect();
		std::cout << recv_msg << std::endl;
		if (recv_msg.length() > 0)	//有返回的消息
		{
			Json::Reader reader;
			Json::Value json_object;

			if (!reader.parse(recv_msg, json_object))
			{
				//JSON格式错误导致解析失败
				log_output("[AioCamera]解析失败");
				return false;
			}
			Json::Value json_param = json_object["result"];
			json_param["channelid"] = Json::Value(0);	//必须带channelid，否则协议不起作用
			json_param["bIvpLprImgFull"] = Json::Value(true);	//上传全景图
			json_param["bIvpLprImgLP"] = Json::Value(false);	//不传车牌小图
			json_param["ivpLprHttpServer"]["ivpLprHttpIP"] = Json::Value(
					g_ip_addr);
			json_param["ivpLprHttpServer"]["ivpLprHttpPort"] = Json::Value(
					listen_port);
			json_param["ivpLprHttpServer"]["ivpLprHttpAddr"] = Json::Value(
					"push");
			json_obj["method"] = Json::Value("ivp_set_param");
			json_obj["param"] = json_param;
			json_msg = json_obj.toStyledString();

			sprintf(str_len, "%d", json_msg.length());	//长度必须正确
			http_header = "POST / HTTP/1.0\r\n";
			http_header += "Connection: keep-alive\r\n";
			http_header += "Content-Type: application/json;charset=utf-8\r\n";
			http_header += "Content-Length: ";
			http_header = http_header + str_len;
			http_header += "\r\nHost: 192.168.1.110:8127\r\n\r\n";
			http_msg = http_header + json_msg;

			sleep(1);

			if (tcp_client.connect_server(camera_ip, 8127))
			{
				sprintf(str_msg, "[AioCamera]连接相机%s的8127端口成功",
						camera_ip.c_str());
				log_msg = str_msg;
				msg_print(log_msg);
				log_output(log_msg);
				b_connect = true;
			}
			else
			{
				sprintf(str_msg, "[AioCamera]连接相机%s的8127端口失败",
						camera_ip.c_str());
				log_msg = str_msg;
				msg_print(log_msg);
				log_output(log_msg);
				b_connect = false;
			}
			if (b_connect)
			{
				std::string recv_msg2 = "";
				std::cout << "http send:\t" << http_msg << std::endl;
				int n = tcp_client.send_data(http_msg, recv_msg2);
				tcp_client.dis_connect();
				std::cout << "http recv:\t" << recv_msg2 << std::endl;
			}
		}
	}

	pthread_t tid_aio_camera_loop;
	pthread_attr_t attr_aio_camera_loop;
	pthread_attr_init(&attr_aio_camera_loop);
	pthread_create(&tid_aio_camera_loop, &attr_aio_camera_loop, aio_camera_loop,
			this);
}

//智能相机服务端侦听线程
void * aio_camera_loop(void* para)
{
	AioCamera *p_camera = (AioCamera*) para;
	while (true)
	{
		std::string msg;
		p_camera->tcp_server.get_message(msg);
		if (msg.length() > 0)
		{
			//首先解析车牌结果
			Json::Reader reader;
			Json::Value json_object;

			if (!reader.parse(msg, json_object))
			{
				//JSON格式错误导致解析失败
				log_output("[AioCamera]解析失败");
				continue;
			}
			//判断是否是HTTP结果推送
			Json::Value json_info = json_object["AlarmInfoPlate"];
			if (json_info.isNull())
				continue;
			Vehicle vehicle_info;
			if (json_info["result"].isNull())
				continue;
			//获取车牌号
			vehicle_info.plate =
					json_info["result"]["PlateResult"]["license"].asString();
			//判断是否无车牌
			if(vehicle_info.plate.length() == 0)
				vehicle_info.plate = "无车牌";
			//处理车牌颜色
			int colorType =
					json_info["result"]["PlateResult"]["colorType"].asInt();
			switch (colorType)
			{
			case 0:
			case 1:
				vehicle_info.pcolor = "blue";
				break;
			case 2:
				vehicle_info.pcolor = "yellow";
				break;
			case 3:
				vehicle_info.pcolor = "white";
				break;
			case 4:
				vehicle_info.pcolor = "black";
				break;
			case 5:
				vehicle_info.pcolor = "green";
				break;
			}
			//保存图片
			std::string scene_image_base64 =
					json_info["result"]["PlateResult"]["imageFile"].asString();
			std::string scene_image = base64_decode(scene_image_base64);
			FILE *fp;
			char path[1024] =
			{ 0 };
			sprintf(path, "/home/boon/carPicture/%s", get_time_date().c_str());
			int n = create_dir(path);
			if (n != 0)
			{
				printf("create_dir error\n");
				continue;
			}
			std::string current_time_ms = get_time_ms();
			std::string current_date = get_time_date();
			sprintf(path, "/home/boon/carPicture/%s/%s_%s.jpg",
					current_date.c_str(), current_time_ms.c_str(),
					vehicle_info.plate.c_str());
			fp = fopen(path, "wb");
			if (fp == NULL)
			{
				printf("open file error\n");
				continue;
			}
			if (scene_image.length() > 0 && fp)
			{
				size_t n_ret = fwrite(scene_image.c_str(), 1,
						scene_image.length(), fp);
				if (n_ret < 0)
				{
					printf("write file error\n");
					continue;
				}
			}
			fclose(fp);
			sprintf(path, "http://%s/piclog/%s/%s_%s.jpg", g_ip_addr.c_str(),
					current_date.c_str(), current_time_ms.c_str(),
					vehicle_info.plate.c_str());
			vehicle_info.path = path;
			//判断通道，推送车牌消息
			if (p_camera->camera_label == "A")
			{
				aio_channel_a_main_vehicle.plate = vehicle_info.plate;
				aio_channel_a_main_vehicle.pcolor = vehicle_info.pcolor;
				aio_channel_a_main_vehicle.path = vehicle_info.path;
				aio_send_car_come_a();
			}
			else if (p_camera->camera_label == "B")
			{
				aio_channel_b_main_vehicle.plate = vehicle_info.plate;
				aio_channel_b_main_vehicle.pcolor = vehicle_info.pcolor;
				aio_channel_b_main_vehicle.path = vehicle_info.path;
				aio_send_car_come_b();
			}
		}
	}
}
