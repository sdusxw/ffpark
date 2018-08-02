#include "aio_bc.h"
#include "net_tcp.h"
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

NetTcpServer tcp_server_5232;   //蓝卡设备识别结果接收器

std::string plate_channel_a;
long ts_channel_a;

bool control_bc_lcd(std::string json_msg, std::string led_ip);
std::string compose_out_temp_paid();

/*
 * 智能机初始化
 * 1. 初始化相机
 */
bool aio_ipc_start()
{
    char str_msg[1024];
    std::string log_msg;
    plate_channel_a = "";
    ts_channel_a = get_unix_ts();
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
    //打开TCP server服务端口 蓝卡一体机车牌接收端口5232
    if (tcp_server_5232.open_bind_listen(5232))
    {
        sprintf(str_msg, "[BuleCard]TCP服务器端口%d打开成功", 5232);
        log_msg = str_msg;
        msg_print(log_msg);
        log_output(log_msg);
    }
    pthread_t tid_bc_plate;
    pthread_attr_t attr_bc_plate;
    pthread_attr_init(&attr_bc_plate);
    pthread_create(&tid_bc_plate, &attr_bc_plate, bc_plate,
                   NULL);
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
    std::string message;
    
    bool b_say_goodbye = false;
    
    unsigned char open_in[] = {0x14, 0x6b, 0x9d, 0x98, 0x40, 0x49, 0x50, 0x52, 0x54, 0x01, 0x00, 0x01, 0x02, 0x25};
    unsigned char open_out[] ={0x14, 0x6b, 0x9d, 0x98, 0x76, 0x49, 0x50, 0x52, 0x54, 0x01, 0x00, 0x01, 0x02, 0x25};
    
	if ((in_out == "入口") || (g_machine.one_way))
	{
        camera_ip = "192.168.1.101";//g_machine.channel_a.main_camera.device_ip_id;
        message = std::string((const char *)open_in, 14);
	}
	else    //出口
	{
		camera_ip = "192.168.1.103";//g_machine.channel_b.main_camera.device_ip_id;
        message = std::string((const char *)open_out, 14);
        b_say_goodbye = true;
	}
	if (tcp_client.connect_server(camera_ip, 5231))
	{
		sprintf(str_msg, "[AioCamera]连接相机%s的5231端口成功", camera_ip.c_str());
		log_msg = str_msg;
		msg_print(log_msg);
		log_output(log_msg);
		b_connect = true;
	}
	else
	{
		sprintf(str_msg, "[AioCamera]连接相机%s的5231端口失败", camera_ip.c_str());
		log_msg = str_msg;
		msg_print(log_msg);
		log_output(log_msg);
		b_connect = false;
	}
	if (b_connect)
	{
		
		std::string recv_msg;
		int n = tcp_client.send_data(message, recv_msg);
		tcp_client.dis_connect();
		if (recv_msg.length() > 0)	//有返回的消息
		{
            if (recv_msg.length() == 14 && (int)(recv_msg[12]) == 1) {
                sprintf(str_msg, "[AioCamera]开闸成功");
                log_msg = str_msg;
                msg_print(log_msg);
                log_output(log_msg);
            } else
            {
                sprintf(str_msg, "[AioCamera]开闸失败");
                log_msg = str_msg;
                msg_print(log_msg);
                log_output(log_msg);
            }
		}
	}
    if (b_say_goodbye) {
        std::string json_msg = compose_out_temp_paid();
        control_bc_lcd(json_msg, camera_ip);
    }
	return true;
}

bool control_bc_lcd(std::string json_msg, std::string led_ip)
{
    if(led_ip.length()==0)
        return false;
    bool b_connect = false;
    char str_msg[1024];
    std::string log_msg;
    NetTcpClient tcp_client;
    unsigned char msg_head[] = {0x96, 0x6b, 0x9d, 0x98, 0x76, 0x49, 0x50, 0x52, 0x54, 0x01};
    std::string message = std::string ((const char *)msg_head, 10);
    
    std::string content_gbk;
    
    utf8togb2312(json_msg, content_gbk);
    
    int l=content_gbk.length();
    
    unsigned short ls=(unsigned short)l;
    
    unsigned char l0=(unsigned char)((ls&0xff00)>>8);
    
    unsigned char l1=(unsigned char)(ls&0xff);
    
    message += l0;
    
    message += l1;
    
    message += content_gbk;
    
    unsigned char cs = (unsigned char)0x30;
    
    message += cs;
    
    std::cout << "message length:\t" << message.length() << std::endl;
    
    std::cout << message << std::endl;
    
    for(int i=0;i<message.length();i++)
    {
        printf("%02x ", (unsigned char)message[i]);
    }
    std::cout << std::endl;
    
    
    if (tcp_client.connect_server(led_ip, 5831))
    {
        printf("[AioCamera]连接一体机%s的5831端口成功", led_ip.c_str());
        b_connect = true;
    }
    else
    {
        sprintf(str_msg, "[AioCamera]连接一体机%s的5831端口失败", led_ip.c_str());
        b_connect = false;
    }
    if (b_connect)
    {
        
        std::string recv_msg;
        std::cout << message << std::endl;
        int n = tcp_client.send_data(message, recv_msg);
        tcp_client.dis_connect();
        std::cout << "recv message length:\t" << recv_msg.length() <<"\t"<< recv_msg << std::endl;
        if (recv_msg.length() > 0)    //有返回的消息
        {
            for(int i=0;i<recv_msg.length();i++)
            {
                printf("%02x ", (unsigned char)recv_msg[i]);
            }
            return true;
        }
        std::cout << std::endl;
        return false;
    }
    else
    {
        return false;
    }
}

std::string compose_out_temp(std::string plate, std::string duration, std::string charge, std::string in_time)
{
    Json::Value json_lcd_show;
    Json::Value json_key, json_keys;
    json_lcd_show["Page"] = "out_temp_car.html";
    json_key["Name"] = Json::Value("BCA_IMG_QRCODE");
    json_key["Type"] = 0;
    json_key["Data"] = "http://ipark.sdboon.com/slyy/out.php?park_id=0531000008&box_ip=192.168.1.201";
    json_keys.append(json_key);
    
    json_key["Name"] = "BCA_TEXT_PLATE";
    json_key["Type"] = 0;
    json_key["Data"] = plate;
    json_keys.append(json_key);
    
    json_key["Name"] = "BCA_TEXT_CHARGE";
    json_key["Type"] = 0;
    json_key["Data"] = charge;
    json_keys.append(json_key);
    
    json_key["Name"] = "BCA_TEXT_CAR_IN_TIME";
    json_key["Type"] = 0;
    json_key["Data"] = in_time;
    json_keys.append(json_key);
    
    json_key["Name"] = "BCA_TEXT_CAR_DURATION";
    json_key["Type"] = 0;
    json_key["Data"] = duration;
    json_keys.append(json_key);
    
    json_lcd_show["Key"] = json_keys;
    json_lcd_show["Voice"] = "请微信扫码 谢谢配合";
    return json_lcd_show.toStyledString();
}

std::string compose_out_temp_paid()
{
    Json::Value json_lcd_show;
    json_lcd_show["Page"] = "out_temp_payed.html";
    json_lcd_show["Voice"] = "祝您出入平安";
    return json_lcd_show.toStyledString();
}

std::string compose_in_temp(std::string plate)
{
    Json::Value json_lcd_show;
    Json::Value json_key, json_keys;
    json_lcd_show["Page"] = "in_temp_car.html";
    json_lcd_show["Voice"] = "请您谨慎驾驶 减速慢行 车头向外 停车入位";
    
    json_key["Name"] = "BCA_TEXT_PLATE";
    json_key["Type"] = 0;
    json_key["Data"] = plate;
    json_keys.append(json_key);
    
    json_key["Name"] = "BCA_TEXT_CAR_TYPE";
    json_key["Type"] = 0;
    json_key["Data"] = "临时车辆";
    json_keys.append(json_key);
    
    json_lcd_show["Key"] = json_keys;
    return json_lcd_show.toStyledString();
}

/*
 * LCD显示消息处理
 {
 "cmd" : "sendled",
 "led_ip" : "192.168.1.103",
 "row1" : "收费2元",
 "row2" : "鲁A536SZ",
 "row3" : "临时车",
 "row4" : "停车29分钟",
 "row_num" : "4"
 }
 */
bool aio_lcd_show(std::string led_ip, std::string row1, std::string row2, std::string row3, std::string row4)
{
    if (led_ip == "192.168.1.103") //出口
    {
        
            if(row1[6] != '0')
            {
                std::string charge = row1.substr(6);
                std::string plate = row2;
                std::string duration = row4.substr(6);
                std::string in_time = row3;
                std::string json_msg = compose_out_temp(plate, duration, charge, in_time);
                return control_bc_lcd(json_msg, led_ip);
            }
        
    }
    else if(led_ip == "192.168.1.101") //入口
    {
        // 1. 判断临时车
        if(row3 == "临时车未授权车辆")
        {
            std::string plate = row2;
            std::string json_msg = compose_in_temp(plate);
            return control_bc_lcd(json_msg, led_ip);
        }
    }
    return false;
}
/*
 * LCD语音消息处理
 */
bool aio_lcd_voice(std::string content, std::string led_ip)
{
    Json::Value json_voice;
    json_voice["Voice"] = Json::Value(content);
    return control_bc_lcd(json_voice.toStyledString(), led_ip);
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
    long ts = get_unix_ts();
    long interval = ts - ts_channel_a;
    if((interval<5)&&(aio_channel_a_main_vehicle.plate == plate_channel_a))
    {
        log_output(car_come_msg);
        log_output("时间间隔小于5秒而且车牌号一致，放弃推送A通道车辆信息");
        return;
    }
	if (udp_sender.send(car_come_msg))
	{
		log_output(car_come_msg);
		log_output("推送A通道车辆信息成功");
        plate_channel_a = aio_channel_a_main_vehicle.plate;
        ts_channel_a = get_unix_ts();
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
	//端口分配：A入口主机8080，AA入口辅机XXXX，B出口主机8090，BA出口辅机XXXX
	if (camera_label == "A")
		listen_port = 8080;
	else if (camera_label == "AA")
		listen_port = 2351;
	else if (camera_label == "B")
		listen_port = 8090;
	else if (camera_label == "BA")
		listen_port = 2353;
	else
		return false;
	//打开TCP server服务端口
	if (http_server.open_bind_listen(listen_port))
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
		p_camera->http_server.get_message(msg);
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
			if(vehicle_info.plate == "_无_")
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
//蓝卡一体机车牌识别结果接收线程
void * bc_plate(void * para)
{
    while(true)
    {
        std::string msg;
        int length=0;
        if(tcp_server_5232.get_message(msg))
        {
            length = msg.length();
            if(length > 100)   //仲裁结果推送
            {
                unsigned char devid = (unsigned char)(msg[4]);
                if(0x40 == devid)   //入口
                {
                    std::string result_msg = msg.substr(12);
                    std::vector<std::string> result = string_split(msg, ",");
                    if(result.size()<5)
                        continue;
                    std::string plate_gb2312 = result[5];
                    std::string plate_utf8;
                    if(gbk2utf8(plate_gb2312, plate_utf8))
                    {
                        std::cout << "PLATE-BLUECARD:\t"  << plate_utf8 << std::endl;
                        aio_channel_a_main_vehicle.plate = plate_utf8;
                        aio_channel_a_main_vehicle.pcolor = "blue";
                        aio_send_car_come_a();
                    }
                    
                }
            }
        }
        
        usleep(1000);
    }
}
