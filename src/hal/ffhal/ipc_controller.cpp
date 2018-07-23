/***************************ipc_config.cpp***************************************
 功能：bipc工作的总控制器
 创建时间：2017-02-07
 创建人：孙希伟
 单位：山东博昂信息科技有限公司
 修改时间：
 ***************************************************************************/
#include "ipc_controller.h"

/*
 * IPC初始化,开始工作
 */
bool ipc_start()
{
	bool result = false;
	if (!g_machine.snap_or_aio) //中维抓拍相机
	{
		log_output("[ipc_controller]中维抓拍相机开始工作");
		//result = snap_ipc_start();
	}
	if (g_machine.snap_or_aio)	//智能相机
	{
		log_output("[ipc_controller]智能相机开始工作");
		result = aio_ipc_start();
	}
	return result;
}
/*
 * 落杆命令
 */
bool close_door(Json::Value json)
{
	bool result = false;
	std::string channel_id = json["channel_id"].asString();
	std::string in_out = json["in_out"].asString();
	if (!g_machine.snap_or_aio) //中维抓拍相机
	{
		//result = snap_close_door(channel_id, in_out);
		if (result)
		{
			log_output("[ipc_controller]" + in_out + "落杆成功");
		}
		else
		{
			log_output("[ipc_controller]" + in_out + "落杆失败");
		}
	}
	if (g_machine.snap_or_aio)	//中维智能相机
	{
		log_output("[ipc_controller]中维智能相机收到落杆命令");
	}
	return result;
}
/*
 * 开闸命令
 * flag: once表示开一次，keep表示一直开
 */
bool open_door(Json::Value json)
{
	bool result = false;
	std::string channel_id = json["channel_id"].asString();
	std::string in_out = json["in_out"].asString();
	std::string flag = json["flag"].asString();
	if (!g_machine.snap_or_aio) //中维抓拍相机
	{
		//result = snap_open_door(channel_id, in_out, flag);
		if (result)
		{
			log_output("[ipc_controller]" + in_out + "开闸放行成功");
		}
		else
		{
			log_output("[ipc_controller]" + in_out + "开闸放行失败");
		}
	}
	if (g_machine.snap_or_aio)	//智能相机
	{
		log_output("[ipc_controller]智能相机收到开闸命令");
		result = aio_open_door(channel_id, in_out, flag);
		if (result)
		{
			log_output("[ipc_controller]" + in_out + "开闸放行成功");
		}
		else
		{
			log_output("[ipc_controller]" + in_out + "开闸放行失败");
		}
	}
	return result;
}
/*
 * 手动抓拍
 * flag: 目前只支持once
 */
bool snap_shot(Json::Value json)
{
	bool result = false;
	std::string channel_id = json["channel_id"].asString();
	std::string in_out = json["in_out"].asString();
	std::string flag = json["flag"].asString();
	if (!g_machine.snap_or_aio) //中维抓拍相机
	{
		//result = snap_snap_shot(channel_id, in_out, flag);
		if (result)
		{
			log_output("[ipc_controller]" + in_out + "手动抓拍成功");
		}
		else
		{
			log_output("[ipc_controller]" + in_out + "手动抓拍失败");
		}
	}
	if (g_machine.snap_or_aio) //中维智能相机
	{
		//result = aio_snap_shot(channel_id, in_out, flag);
		if (result)
		{
			log_output("[ipc_controller]" + in_out + "手动抓拍成功");
		}
		else
		{
			log_output("[ipc_controller]" + in_out + "手动抓拍失败");
		}
	}
	return result;
}
/*
 * LCD显示消息
 */
bool lcd_show(Json::Value json)
{
    bool result = false;
    std::string led_ip = json["led_ip"].asString();
    std::string row1 = json["row1"].asString();
    std::string row2 = json["row2"].asString();
    std::string row3 = json["row3"].asString();
    std::string row4 = json["row4"].asString();
    result = aio_lcd_show(led_ip, row1, row2, row3, row4);
    return result;
}
/*
 * LCD语音消息
 */
bool lcd_voice(Json::Value json)
{
    bool result = false;
    std::string content = json["content"].asString();
    std::string led_ip = json["led_ip"].asString();
    result = aio_lcd_voice(content, led_ip);
    return result;
}
