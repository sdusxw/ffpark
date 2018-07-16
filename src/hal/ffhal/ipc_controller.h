#ifndef _BOON_IPC_CONTROLLER_H
#define _BOON_IPC_CONTROLLER_H
/*
 * bipc工作的总控制器
 */
#include "bglobal.h"
#include "common.h"

//#include "snapcamera.h"
//#include "aiocamera.h"

/*
 * IPC初始化,开始工作
 */
bool ipc_start();
/*
 * 落杆命令
 */
bool close_door(Json::Value json);
/*
 * 开闸命令
 * flag: once表示开一次，keep表示一直开
 */
bool open_door(Json::Value json);
/*
 * 手动抓拍
 * flag: 目前只支持once
 */
bool snap_shot(Json::Value json);

#endif
