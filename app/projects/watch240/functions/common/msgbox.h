#ifndef _MSGBOX_H
#define _MSGBOX_H

enum {
    MSGBOX_MODE_BTN_OK,
    MSGBOX_MODE_BTN_OKCANCEL,
    MSGBOX_MODE_BTN_YESNO,
    MSGBOX_MODE_BTN_DELETE, //消息推送删除按键
    MSGBOX_MODE_BTN_NONE,
    MSGBOX_MODE_BTN_REMIND_LATER_CLOSE,   //稍后提醒,关闭
};

//msgbox返回值
enum {
    MSGBOX_RES_NONE,
    MSGBOX_RES_OK,          //确定
    MSGBOX_RES_CANCEL,      //取消
    MSGBOX_RES_ABORT,       //终止
    MSGBOX_RES_RETRY,       //重试
    MSGBOX_RES_INGNORE,     //忽略
    MSGBOX_RES_YES,         //是
    MSGBOX_RES_NO,          //否
    MSGBOX_RES_DELETE,      //删除
    MSGBOX_RES_REMIND_LATER,//稍后提醒
    MSGBOX_RES_EXIT,        //关闭弹窗

    MSGBOX_RES_TIMEOUT_EXIT,      //闹钟提醒弹窗超时退出

};

enum {
    MSGBOX_MSG_TYPE_NONE,
    MSGBOX_MSG_TYPE_WECHAT,   //微信消息
    MSGBOX_MSG_TYPE_COVER,    //弹窗消息
};

int msgbox(char *msg, char *title, int mode, char msg_type);

#endif
