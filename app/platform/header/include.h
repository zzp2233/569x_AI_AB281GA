#ifndef _INCLUDE_H
#define _INCLUDE_H

#define A_CONTROL           1
#include "global.h"
#include "xcfg.h"
#include "config.h"
#include "io_def.h"

//API头文件
#include "api.h"

//BSP头文件
#include "bsp.h"

//GUI
#include "gui.h"

//Functions头文件
#include "func.h"

//多国语言
#include "lang.h"

//扩展
#include "plugin.h"

//资源文件
#include "res.h"
#include "ui.h"
#include "ui_external.h"

///客户module include文件
//#include "ute_module_filesystem.h"
//#include "ute_module_gui_common.h"
//#include "ute_module_log.h"
//#include "ute_module_mem.h"
//#include "ute_module_message.h"
//#include "ute_module_platform.h
//#include "ute_module_profile_ble.h"
//#include "ute_module_protocol.h"
#include "ute_module_systemtime.h"

#ifndef sscanf
extern int sscanf(char *s, const char *format, ...);
#endif // sscanf

#endif // _INCLUDE_H
