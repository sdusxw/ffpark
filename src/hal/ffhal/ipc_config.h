#ifndef _BOON_IPC_CONFIG_H
#define _BOON_IPC_CONFIG_H
/*
 * 解析get_ipc_config的返回JSON信息
 */
#include "bglobal.h"
#include "common.h"

//解析get_ipc_config命令返回的硬件配置信息
//解析结果直接写入全局变量g_machine
bool get_ipc_config(Json::Value json_ipc_config);

#endif
