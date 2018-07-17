/***************************ipc_config.cpp***************************************
 功能：解析get_ipc_config的返回JSON信息
 创建时间：2017-02-06
 创建人：孙希伟
 单位：山东博昂信息科技有限公司
 修改时间：
 ***************************************************************************/
#include "ipc_config.h"

#include <vector>

//解析get_ipc_config命令返回的硬件配置信息
//解析结果直接写入全局变量g_machine
bool get_ipc_config(Json::Value json_ipc_config)
{
	g_machine.channel_a_enable = false;
	g_machine.channel_b_enable = false;
	g_machine.a_relay_enable = false;
	g_machine.b_relay_enable = false;
	g_machine.second_relay = false;
	g_machine.one_way = false;
	Json::Value json_channel = json_ipc_config["channel"];
	std::string log_msg = "";
	char char_msg[1024] = "";
	int n_channel = json_channel.size();
	sprintf(char_msg, "[ipc_config]通道个数%d", n_channel);
	log_msg = char_msg;
	log_output(log_msg);
	//如果没有通道配置，则返回失败
	if (json_channel.size() == 0)
	{
		log_output("[ipc_config]没有收到通道配置信息，请检查通道配置");
		return false;
	}
	Channel channels[2];
	//目前最多支持2个通道，一进一出
	if (json_channel.size() > 2)
	{
		log_output("[ipc_config]目前最多支持2个通道，请检查通道配置");
		return false;
	}
	//解析channel到对象
	for (int i = 0; i < n_channel; i++)
	{
		channels[i].channel_id = json_channel[i]["channel_id"].asString();
		channels[i].in_out = json_channel[i]["in_out"].asString();
		std::string str_one_way = json_channel[i]["one_way"].asString();
		if (str_one_way == "是")
		{
			g_machine.one_way = true;
			log_output("[ipc_config]同口进出");
		}
		//解析ipc到devices对象
		std::vector<Device> devices;
		Json::Value json_ipc = json_channel[i]["ipc"];
		int n_ipc = json_channel[i]["ipc"].size();
		sprintf(char_msg, "[ipc_config]第%d通道解析出%d个设备", i, n_ipc);
		log_msg = char_msg;
		log_output(log_msg);
		for (int j = 0; j < n_ipc; j++)
		{
			Device device;
			device.device_ip_id = json_ipc[j]["device_ip_id"].asString();
			device.device_password = json_ipc[j]["device_password"].asString();
			device.device_type = json_ipc[j]["device_type"].asString();
			device.device_username = json_ipc[j]["device_username"].asString();
			devices.push_back(device);
		}
		channels[i].devices = devices;
	}
	//把入口通道分配给g_machine的channel_a, 出口通道分配给channel_b
	for (int i = 0; i < n_channel; i++)
	{
		if (channels[i].in_out == "入口")
		{
			g_machine.channel_a = channels[i];
			g_machine.channel_a_enable = true;
			log_output("[ipc_config]入口通道使能");
		}
		if (channels[i].in_out == "出口")
		{
			g_machine.channel_b = channels[i];
			g_machine.channel_b_enable = true;
			log_output("[ipc_config]出口通道使能");
		}
	}
	//遍历设备列表，确定相机类型
	bool b_determine = false;
	for (int i = 0; i < n_channel && !b_determine; i++)
	{
		for (int j = 0; j < channels[i].devices.size(); j++)
		{
			if (channels[i].devices[j].device_type == "中维抓拍相机")
			{
				g_machine.snap_or_aio = false;			//(false中维抓拍相机)
				b_determine = true;
				log_output("[ipc_config]确定为中维抓拍相机");
				break;
			}
			if (channels[i].devices[j].device_type == "中维智能相机")
			{
				g_machine.snap_or_aio = true;			//(true中维智能相机)
				b_determine = true;
				log_output("[ipc_config]确定为中维智能相机");
				break;
			}
            if (channels[i].devices[j].device_type == "臻识智能相机")
            {
                g_machine.snap_or_aio = true;            //(true臻识智能相机)
                b_determine = true;
                log_output("[ipc_config]确定为臻识智能相机");
                break;
            }
		}
	}
	//确定A通道是否有辅机
	if (g_machine.channel_a_enable)
	{
		std::vector<Device> vec_dev;
		for (int i = 0; i < g_machine.channel_a.devices.size(); i++)
		{
			if (g_machine.channel_a.devices[i].device_type == "中维抓拍相机"
					|| g_machine.channel_a.devices[i].device_type == "中维智能相机"
                    || g_machine.channel_a.devices[i].device_type == "臻识智能相机")
			{
				sprintf(char_msg, "[ipc_config]A通道发现相机，IP：%s",
						g_machine.channel_a.devices[i].device_ip_id.c_str());
				log_msg = char_msg;
				log_output(log_msg);
				vec_dev.push_back(g_machine.channel_a.devices[i]);
			}
		}
		if (1 == vec_dev.size())
		{
			g_machine.channel_a.main_camera = vec_dev[0];
			log_output("[ipc_config]A通道没有配置辅机");
			g_machine.a_aux_camera = false;
		}
		if (2 == vec_dev.size())
		{
			g_machine.channel_a.main_camera = vec_dev[0];
			g_machine.channel_a.aux_camera = vec_dev[1];
			log_output("[ipc_config]A通道有辅机");
			g_machine.a_aux_camera = true;
		}
	}
	//确定B通道是否有辅机
	if (g_machine.channel_b_enable)
	{
		std::vector<Device> vec_dev;
		for (int i = 0; i < g_machine.channel_b.devices.size(); i++)
		{
			if (g_machine.channel_b.devices[i].device_type == "中维抓拍相机"
					|| g_machine.channel_b.devices[i].device_type == "中维智能相机"
                    || g_machine.channel_b.devices[i].device_type == "臻识智能相机")
			{
				sprintf(char_msg, "[ipc_config]B通道发现相机，IP：%s",
						g_machine.channel_b.devices[i].device_ip_id.c_str());
				log_msg = char_msg;
				log_output(log_msg);
				vec_dev.push_back(g_machine.channel_b.devices[i]);
			}
		}
		if (1 == vec_dev.size())
		{
			g_machine.channel_b.main_camera = vec_dev[0];
			log_output("[ipc_config]B通道没有配置辅机");
			g_machine.b_aux_camera = false;
		}
		if (2 == vec_dev.size())
		{
			g_machine.channel_b.main_camera = vec_dev[0];
			g_machine.channel_b.aux_camera = vec_dev[1];
			log_output("[ipc_config]B通道有辅机");
			g_machine.b_aux_camera = true;
		}
	}
	return true;
}
