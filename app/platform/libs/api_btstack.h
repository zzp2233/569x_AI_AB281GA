/*
 *  api_btstack.h
 *
 *  Created by zoro on 2017-8-24.
 */
#ifndef _API_BTSTACK_H
#define _API_BTSTACK_H

//工作模式
#define MODE_NORMAL                     0       //正常连接模式，关闭DUT测试
#define MODE_NORMAL_DUT                 1       //正常连接模式，使能DUT测试
#define MODE_FCC_TEST                   2       //FCC测试模式，通过串口通信
#define MODE_BQB_RF_BREDR               3       //BR/EDR DUT测试模式，通过蓝牙通信
#define MODE_BQB_RF_BLE                 4       //BLE DUT测试模式，通过串口通信
#define MODE_IODM_TEST                  5       //IODM测试模式，通过串口通信

//包类型
#define FCC_TYPE_MODE_UNDEFINE          0
#define FCC_TYPE_MODE_BR                1
#define FCC_TYPE_MODE_EDR               2
#define FCC_TYPE_MODE_BLE1M             4
#define FCC_TYPE_MODE_BLE2M             8

//蓝牙功能
#define PROF_A2DP                       0x0007  //蓝牙音乐功能
#define PROF_HID                        0x0018  //蓝牙键盘功能
#define PROF_HFP                        0x0060  //蓝牙通话功能
#define PROF_SPP                        0x0080  //蓝牙串口功能
#define PROF_PBAP                       0x0100  //蓝牙电话本功能
#define PROF_HSP                        0x0200
#define PROF_MAP                        0x0400  //蓝牙短信功能
#define PROF_GATT                       0x0800  //GATT over BREDR功能

//蓝牙特性
#define HFP_BAT_REPORT                  0x01    //是否支持手机电量显示
#define HFP_3WAY_CALL                   0x02    //是否支持三通电话
#define HFP_INBAND_RING_TONE            0x04    //是否支持手机来电铃声
#define HFP_CALL_PRIVATE                0x08    //是否打开强制私密接听
#define HFP_SIRI_CMD                    0x10    //是否打开siri控制命令
#define HFP_EC_AND_NR                   0x20    //是否打开手机端回音和降噪
#define HFP_RING_NUMBER_EN              0x40    //是否支持来电报号
#define A2DP_AVRCP_VOL_CTRL             0x01    //是否支持手机音量控制同步
#define A2DP_AVRCP_PLAY_STATUS_IOS      0x02    //是否支持IOS手机播放状态同步，可加快播放暂停响应速度，蓝牙后台建议打开；注意：打开后微信小视频会无声
#define A2DP_AVRCP_PLAY_STATUS          0x04    //是否支持手机播放状态同步，可加快播放暂停响应速度，蓝牙后台建议打开；
#define A2DP_RESTORE_PLAYING            0x08    //是否支持掉线回连后恢复播放
#define A2DP_AVDTP_DELAY_REPORT         0x10    //是否支持AVDTP delay report功能
#define A2DP_AVDTP_DYN_LATENCY          0x20    //是否支持动态延迟控制功能
#define A2DP_AVDTP_EXCEPT_REST_PLAY     0x40    //是否支持异常复位后恢复连接和播放状态功能
//a2dp特性扩展1
#define A2DP_AVRCP_RECORD_DEVICE_VOL    0x01    //分别记录不同连接设备的音量
#define A2DP_RESET_DEVICE_VOL           0x02    //不支持音量同步手机，连接恢复音量
#define A2DP_PLAY_SEQN_OPT              0x04    //是否支持不连续seqn pkt播放
//hfp特性扩展
#define HFP_RECORD_DEVICE_VOL           0x01    //分别记录不同连接设备的音量

//SPP rfcomm通道号
#define SPP_RFCOMM_SERVER_CHANNEL0      1
#define SPP_RFCOMM_SERVER_CHANNEL1      5
#define SPP_RFCOMM_SERVER_CHANNEL2      6

//蓝牙编解码
#define CODEC_SBC                       0x01
#define CODEC_AAC                       0x02
#define CODEC_MSBC                      0x04
#define CODEC_PLC                       0x08
#define CODEC_LHDCV5                    0x10
#define CODEC_LHDCV4                    0x20
#define CODEC_LDAC                      0x40

//TWS feature
#define TWS_FEAT_TSCO                   0x0001  //双耳通话
#define TWS_FEAT_TSCO_RING              0x0002  //同步铃声
#define TWS_FEAT_CODEC_MSBC             0x0004  //MSBC高清通话
#define TWS_FEAT_CODEC_AAC              0x0008  //AAC高清音频
#define TWS_FEAT_MS_SWITCH              0x0010  //主从切换
#define TWS_FEAT_NAME_MENU              0x0020  //蓝牙改名称菜单
#define TWS_FEAT_MS_BONDING             0x0040  //主从绑定
#define TWS_FEAT_PAIR_NOR_LINKINFO      0x0400  //TWS配对时判断是否存在手机信息，有信息的为主耳
#define TWS_FEAT_VERSION_MASK           0x0380  //版本号, bit7~9
#define TWS_FEAT_CODEC_HIRES            0x1000  //LHDC&LADC

//一拖二特性
#define BT_2ACL_SCAN_ALWAYS             0x0001  //连接第一个设备继续打开可被发现
#define BT_2ACL_RECONN_ODEV             0x0002  //只回连最后一次连接的手机

//TWS pair
#define TWS_PAIR_OP_MASK                0x0f
#define TWS_PAIR_OP_AUTO                0x00
#define TWS_PAIR_OP_API                 0x01
#define TWS_PAIR_MS_BONDING_MASK        0x20
#define TWS_PAIR_MS_BONDING             0x20
#define TWS_PAIR_MODE_MASK              0xc0
#define TWS_PAIR_MODE_BTID              0x40
#define TWS_PAIR_MODE_BTNAME            0x00

#define BT_TWS_SYNC_TO                  50      //BT同步等待，对齐到一个50ms，250ms超时

//删除配对信息TAG
#define BT_INFO_TAG_NOR                 "NULL"   //删除手机
#define BT_INFO_TAG_UNPAIR              "UPAR"   //删除TWS，并断开
#define BT_INFO_TAG_QTEST               "QTET"   //快测删除TWS配对信息
#define BT_INFO_TAG_IODM                "IODM"   //IODM删除TWS配对信息
#define BT_INFO_TAG_CHARGE_BOX          "CBOX"   //充电仓删除TWS配对信息
#define BT_INFO_TAG_USER                "USER"   //用户删除TWS配对信息

//蓝牙状态
enum
{
    BT_STA_OFF,                                 //蓝牙模块已关闭
    BT_STA_INITING,                             //初始化中
    BT_STA_IDLE,                                //蓝牙模块打开，未连接

    BT_STA_SCANNING,                            //扫描中
    BT_STA_DISCONNECTING,                       //断开中
    BT_STA_CONNECTING,                          //连接中

    BT_STA_CONNECTED,                           //已连接
    BT_STA_PLAYING,                             //播放
    BT_STA_INCOMING,                            //来电响铃
    BT_STA_OUTGOING,                            //正在呼出
    BT_STA_INCALL,                              //通话中
    BT_STA_OTA,                                 //OTA升级中
};

//通话状态
enum
{
    BT_CALL_IDLE,                               //
    BT_CALL_INCOMING,                           //来电响铃
    BT_CALL_OUTGOING,                           //正在呼出
    BT_CALL_ACTIVE,                             //通话中
    BT_CALL_3WAY_CALL,                          //三通电话或两部手机通话
};

//蓝牙通知
enum
{
    BT_NOTICE_INIT_FINISH,                      //蓝牙初始化完成
    BT_NOTICE_CONNECT_START,                    //开始回连手机, param[0]=status, param[1]=reason, param[7:2]=bd_addr
    BT_NOTICE_CONNECT_FAIL,                     //回连手机失败, param[0]=reason, param[1]=reason, param[7:2]=bd_addr
    BT_NOTICE_DISCONNECT,                       //蓝牙断开,     param[0]=feat,index, param[1]=reason, param[7:2]=bd_addr
    BT_NOTICE_LOSTCONNECT,                      //蓝牙连接丢失, param[0]=status, param[1]=reason, param[7:2]=bd_addr
    BT_NOTICE_CONNECTED,                        //蓝牙连接成功, param[0]=feat,index, param[1]=reason, param[7:2]=bd_addr
    BT_NOTICE_SCO_CONNECTED,                    //SCO建立连接
    BT_NOTICE_SCO_DISCONNECT,                   //SCO断开连接
    BT_NOTICE_SCO_CON_FAIL,                     //SCO连接失败
    BT_NOTICE_SCO_SETUP,                        //SCO音频建立
    BT_NOTICE_SCO_KILL,                         //SCO音频退出
    BT_NOTICE_INCOMING,                         //来电
    BT_NOTICE_RING,                             //来电响铃
    BT_NOTICE_OUTGOING,                         //去电
    BT_NOTICE_CALL_NUMBER,                      //来电/去电号码
    BT_NOTICE_CALL,                             //建立通话
    BT_NOTICE_SET_SPK_GAIN,                     //设置通话音量
    BT_NOTICE_CALL_CHANGE_DEV,                  //1拖2时改变了通话设备
    BT_NOTICE_MUSIC_PLAY,                       //蓝牙音乐开始播放
    BT_NOTICE_MUSIC_STOP,                       //蓝牙音乐停止播放
    BT_NOTICE_MUSIC_CHANGE_VOL,                 //手机端改变蓝牙音乐音量, param[0]=down/up, param[1]=index, param[7:2]=bd_addr
    BT_NOTICE_MUSIC_SET_VOL,                    //手机端设置蓝牙音乐音量, param[0]=a2dp_vol, param[1]=index, param[7:2]=bd_addr
    BT_NOTICE_MUSIC_CHANGE_DEV,                 //1拖2时改变了播放设备, 例如从A手机切换到B手机, param[0]=a2dp_vol, param[1]=index, param[7:2]=bd_addr
    BT_NOTICE_HID_CONN_EVT,                     //HID服务连接事件
    BT_NOTICE_RECON_FINISH,                     //回连完成, param[0]=status, param[1]=reason, param[7:2]=bd_addr
    BT_NOTICE_ABORT_STATUS,                     //中止状态, param[0]=status, param[1]=reason, param[7:2]=bd_addr
    BT_NOTICE_NORCONNECT_FAIL,                  //手机发起连接到一半失败, param[0]=status, param[1]=reason, param[7:2]=bd_addr
    BT_NOTICE_LOW_LATENCY_STA,                  //低延时状态切换

    BT_NOTICE_TWS_SEARCH_FAIL = 0x40,           //搜索TWS失败, param[0]=reason(0=timeout, 0x0B=conn_exists, 0x0C=cmd_disallow)
    BT_NOTICE_TWS_CONNECT_START,                //开始回连TWS, param[0]=status, param[1]=reason, param[7:2]=bd_addr
    BT_NOTICE_TWS_CONNECT_FAIL,                 //TWS回连失败, param[0]=status, param[1]=reason, param[7:2]=bd_addr
    BT_NOTICE_TWS_DISCONNECT,                   //TWS牙断开,   param[0]=feat,index, param[1]=reason, param[7:2]=bd_addr
    BT_NOTICE_TWS_LOSTCONNECT,                  //TWS连接丢失, param[0]=status, param[1]=reason, param[7:2]=bd_addr
    BT_NOTICE_TWS_CONNECTED,                    //TWS连接成功, param[0]=feat,index, param[1]=reason, param[7:2]=bd_addr
    BT_NOTICE_TWS_INIT_VOL,                     //TWS设置副耳初始音乐音量
    BT_NOTICE_TWS_HID_SHUTTER,                  //远端TWS拍照键
    BT_NOTICE_TWS_USER_KEY,                     //TWS自定义按键
    BT_NOTICE_TWS_SET_OPERATION,
    BT_NOTICE_TWS_STATUS_CHANGE,
    BT_NOTICE_TWS_ROLE_CHANGE,                  //主从角色变换
    BT_NOTICE_TWS_RES5,
    BT_NOTICE_TWS_RES6,
    BT_NOTICE_TWS_RES7,
    BT_NOTICE_TWS_WARNING,

    BT_NOTICE_PBAP_CONNECTED,
    BT_NOTICE_PBAP_GET_PHONEBOOK_SIZE_COMPLETE, //获取电话薄数量完成
    BT_NOTICE_PBAP_PULL_PHONEBOOK_COMPLETE,     //获取电话薄完成
    BT_NOTICE_PBAP_DISCONNECT,

    BT_NOTICE_TWS_CON_FINISH,
    BT_NOTICE_TWS_CON_RETRY_END,

    BT_NOTICE_WIRELESS_CONNECTED = 0xc0,        //无线mic连接成功, param[0]=index, param[1]=reason, param[7:2]=bd_addr
    BT_NOTICE_WIRELESS_DISCONNECT,              //无线mic断开连接, param[0]=index, param[1]=reason, param[7:2]=bd_addr
    BT_NOTICE_WIRELESS_CONNECT_FAIL,            //无线mic连接失败, param[0]=index, param[1]=reason, param[7:2]=bd_addr

    BT_NOTICE_MAP_CONNECT_SUCCESS,
    BT_NOTICE_MAP_CONNECT_FAIL,
    BT_NOTICE_MAP_DISCONNECTED,
    BT_NOTICE_MAP_GET_FINISH,
    BT_NOTICE_MAP_GET_FAIL,
};

//param[0]=feat,index
enum
{
    FEAT_TWS_FLAG       = 0x80,
    FEAT_TWS_ROLE       = 0x40,
    FEAT_TWS_MUTE_FLAG  = 0x20,
    FEAT_TWS_FIRST_ROLE = 0x10,
    FEAT_INCOME_CON     = 0x08,
    FEAT_FIRST_CON      = 0x04,
    FEAT_INDEX_MASK     = 0x03,
};

//控制消息
enum
{
    BT_CTL_VOL_CHANGE               = 0,        //音量调整，之后通过回调函数设置音量
    BT_CTL_PLAY_PAUSE,                          //切换播放、暂停
    BT_CTL_VOL_UP,                              //音乐加音量，之后通过回调函数调节音量
    BT_CTL_VOL_DOWN,                            //音乐减音量，之后通过回调函数调节音量
    BT_CTL_2ACL_PALY_SWITCH,                    //一拖二播放切换
    BT_CTL_MSC_RES1,
    BT_CTL_MSC_RES2,
    BT_CTL_MSC_RES3,

    BT_CTL_CALL_REDIAL,                         //回拨电话（最后一次通话）
    BT_CTL_CALL_REDIAL_NUMBER,                  //回拨电话（从hfp_get_outgoing_number获取号码）
    BT_CTL_CALL_ANSWER_INCOM,                   //接听来电（三通时挂起当前通话）
    BT_CTL_CALL_ANSWER_INCOM_REJ_OTHER,         //接听来电（三通时挂断当前通话）
    BT_CTL_CALL_TERMINATE,                      //挂断通话或来电
    BT_CTL_CALL_SWAP,                           //切换三通电话
    BT_CTL_CALL_PRIVATE_SWITCH,                 //切换私密通话
    BT_CTL_HFP_REPORT_BAT,                      //报告电池电量
    BT_CTL_HFP_MIC_GAIN,                        //设置通话麦克风音量
    BT_CTL_HFP_AT_CMD,                          //发送AT命令（从hfp_get_at_cmd获取命令）
    BT_CTL_HFP_SIRI_SW,                         //唤出/关闭siri
    BT_CTL_HFP_CUSTOM_AT_CMD,
    BT_CTL_CALL_ANSWER_INCOM_HOLD_OTHER,
    BT_CTL_CALL_GET_REMOTE_PHONE_NUM,

    BT_CTL_TWS_SWITCH,                          //主从切换
    BT_CTL_NOR_CONNECT,
    BT_CTL_NOR_DISCONNECT,
    BT_CTL_HID_CONNECT,
    BT_CTL_HID_DISCONNECT,
    BT_CTL_CONN_RES1,
    BT_CTL_CONN_RES2,

    BT_CTL_BLE_ADV_DISABLE,                     //关闭BLE 广播
    BT_CTL_BLE_ADV_ENABLE,                      //打开BLE 广播
    BT_CTL_BLE_RES1,
    BT_CTL_BLE_RES2,

    BT_CTL_FOT_RESP,
    BT_CTL_FCC_TEST,
    BT_CTL_LOW_LATENCY_EN,
    BT_CTL_LOW_LATENCY_DIS,
    BT_CTL_EAR_STA_CHANGE,
    BT_CTL_NR_STA_CHANGE,
    BT_CTL_AAP_USER_DATA,
    BT_CTL_ID3_GET_ELE_ATT,                     //ID3获取歌曲信息（需打开ID3功能）
    BT_CTL_GET_PLAY_STATUS_INFO,                //获取歌曲播放状态信息
    BT_CTL_DUEROS_AI_START,
    BT_CTL_DUEROS_AI_STOP,
    BT_CTL_MAX,

    BT_CTL_A2DP_VOLUME_UP           = 0xff0041,     //音量加
    BT_CTL_A2DP_VOLUME_DOWN         = 0xff0042,     //音量减
    BT_CTL_A2DP_MUTE                = 0xff0043,     //MUTE
    BT_CTL_A2DP_PLAY                = 0xff0044,     //播放
    BT_CTL_A2DP_STOP                = 0xff0045,     //停止
    BT_CTL_A2DP_PAUSE               = 0xff0046,     //暂停
    BT_CTL_A2DP_RECORD              = 0xff0047,
    BT_CTL_A2DP_REWIND              = 0xff0048,     //快退
    BT_CTL_A2DP_FAST_FORWARD        = 0xff0049,     //快进
    BT_CTL_A2DP_EJECT               = 0xff004a,
    BT_CTL_A2DP_FORWARD             = 0xff004b,     //下一曲
    BT_CTL_A2DP_BACKWARD            = 0xff004c,     //上一曲
    BT_CTL_A2DP_REWIND_END          = 0xff00c8,     //结束快退
    BT_CTL_A2DP_FAST_FORWARD_END    = 0xff00c9,     //结束快进

    BT_CTL_NO                       = 0xffffff,
};

enum bt_msg_comm_t
{
    COMM_BT_CTL0,                               //无传参的消息
    COMM_BT_START_WORK,                         //蓝牙开始工作
    COMM_BT_SET_SCAN,                           //设置可被发现/可被连接
    COMM_BT_CONNECT,                            //连接蓝牙
    COMM_BT_DISCONNECT,                         //断开蓝牙
    COMM_BT_ABORT_RECONNECT,                    //中止回连
    COMM_BT_ATT_MSG,
    COMM_BT_SET_SCAN_INTV,                      //设置PAGE SCAN PARAM
    COMM_BT_ABORT_PAGE_PSCAN,                   //中止PAGE和PAGESCAN
    COMM_BT_ADD_VOL_SET_QUEUE,                  //添加音量到设置队列中
};

enum bt_msg_tws_t
{
    TWS_SET_SCAN,
    TWS_SEARCH_SLAVE,
    TWS_USER_KEY,                               //用户自定义消息（主副互发）
    TWS_SYNC_INFO,                              //同步状态消息（主副同步）
    TWS_SET_STATUS,                             //tws_send_set_status
    TWS_RES_ADD,
    TWS_RES_DONE,
    TWS_ALARM_REQ,                              //同步ALARM请求（主发给副）
    TWS_DISCONNECT,
    TWS_CONNECT,
    TWS_ABORT_SEARCH_SLAVE,
};

enum bt_msg_hid_t
{
    HID_KEYBOARD,
    HID_CONSUMER,
    HID_TOUCH_SCREEN,
    HID_MOUSE,
};

enum bt_msg_pbap_t
{
    BT_PBAP_CTRL,
    BT_PBAP_SELECT_PHONEBOOK,
    BT_PBAP_GET_PHONEBOOK_SIZE,
    BT_PBAP_PULL_PHONEBOOK,
};

enum bt_msg_wireless_t
{
    WIRELESS_ADV_SET_ENABLE,
    WIRELESS_ADV_SET_PARAM,
    WIRELESS_ADV_SET_DATA,
    WIRELESS_SET_SCAN_RSP_DATA,
    WIRELESS_SCAN_SET_ENABLE,
    WIRELESS_SCAN_SET_PARAM,
    WIRELESS_CONNECT_REQ,
    WIRELESS_DISCONNECT_REQ,
    WIRELESS_CON_PARAM_UPDATE,
    WIRELESS_CON_SEND_CMD,
    WIRELESS_SETUP,
    WIRELESS_OFF,
};

//蓝牙消息
enum bt_msg_t
{
    BT_MSG_CTRL                 = 0,            //蓝牙控制消息
    BT_MSG_COMM,
    BT_MSG_RES1,
    BT_MSG_TWS,                                 //TWS消息
    BT_MSG_ADV0,
    BT_MSG_A2DP,                                //A2DP消息
    BT_MSG_HFP,                                 //HFP消息
    BT_MSG_HSP,                                 //HSP消息
    BT_MSG_HID,                                 //HID消息，KEYPAD按键/CONSUMER按键/触摸屏
    BT_MSG_PBAP,                                //PBAP消息
    BT_MSG_MAP,                                 //MAP消息
    BT_MSG_PRIV_CON,
    BT_MSG_WIRELESS,
    BT_MSG_MAX,
    BT_MSG_RES                  = 0xf0,         //0xf0~0xff保留给传参较多的api
};

enum sync_info_enum
{
    SYNC_INFO_SETTING,                  //VOL/LANG/EQ/BAT/...
    SYNC_INFO_LEDCNT,                   //同步 led cnt
    SYNC_INFO_EQPARAM,
    SYNC_INFO_FOT_DATA,
    SYNC_INFO_RING_STA,
    SYNC_INFO_CUSTOM_DATA,
};

//tws_status
enum
{
    TWS_STA_FLAG        = 0x80,
    TWS_STA_ROLE        = 0x40,
    TWS_STA_NOR_FLAG    = 0x01,
};

//LE Addr Type, public or random(static or non resolvable or resolvable)
enum
{
    GAP_RANDOM_ADDRESS_TYPE_OFF = 0,    //Public
    GAP_RANDOM_ADDRESS_TYPE_STATIC,
    GAP_RANDOM_ADDRESS_NON_RESOLVABLE,
    GAP_RANDOM_ADDRESS_RESOLVABLE,
};

//LE状态
enum
{
    LE_STA_STANDBY,
    LE_STA_ADVERTISING,                         //正在广播
    LE_STA_CONNECTION,                          //已连接
};

//LE GATT 服务相关
/**
    BLE GATTS ERR return
*/
#define BLE_GATTS_SUCCESS                                       0x00
#define BLE_GATTS_SRVC_TYPE_ERR                                 0x01
#define BLE_GATTS_SRVC_RULES_ERR                                0x02
#define BLE_GATTS_SRVC_PROPS_ERR                                0x03
#define BLE_GATTS_SRVC_ATT_FULL_ERR                             0x04
#define BLE_GATTS_SRVC_PROFILE_FULL_ERR                         0x05


/**
    att property flag
*/
#define ATT_BROADCAST                                           0x01
#define ATT_READ                                                0x02
#define ATT_WRITE_WITHOUT_RESPONSE                              0x04
#define ATT_WRITE                                               0x08
#define ATT_NOTIFY                                              0x10
#define ATT_INDICATE                                            0x20
#define ATT_AUTHENTICATED_SIGNED_WRITE                          0x40
#define ATT_EXTENDED_PROPERTIES                                 0x80
#define ATT_DYNAMIC                                             0x100
#define ATT_UUID128                                             0x200
#define ATT_AUTHENTICATION_REQUIRED                             0x400
#define ATT_AUTHORIZATION_REQUIRED                              0x800


/**
    define group GATT Server Service Types
*/
typedef enum
{
    BLE_GATTS_SRVC_TYPE_PRIMARY  =   0x00,                //Primary Service
    BLE_GATTS_SRVC_TYPE_SEVONDARY=   0x01,                //Secondary Service
    BLE_GATTS_SRVC_TYPE_INCLUDE  =   0x02,                //include Type
} ble_gatts_service_type;

/**
    define group GATT Server UUID Types
*/
typedef enum
{
    BLE_GATTS_UUID_TYPE_16BIT  =   0x00,                //UUID 16BIT
    BLE_GATTS_UUID_TYPE_128BIT =   0x01,                //UUID 128BIT
} ble_gatts_uuid_type;

/**
    define GATT handler callback
    con_handle: connect handle
    handle    : attribute handle
    flag      : call back flag
*/
#define ATT_CB_FALG_OFFSET         (0xffff)
#define ATT_CB_FALG_DIR_WRITE      (0x10000)
typedef int (*ble_gatt_callback_func)(uint16_t con_handle, uint16_t handle, uint32_t flag, uint8_t *ptr, uint16_t len);

/**
    define GATT service base
*/
typedef struct gatts_service_base
{
    uint16_t handle;
} gatts_service_base_st;


/**
    define GATT service uuid
*/
typedef struct gatts_uuid_base
{
    uint16_t props;
    uint8_t type;
    const uint8_t *uuid;
} gatts_uuid_base_st;
/**
    define GATT service call back info
*/
typedef struct
{
    uint16_t  client_config; // att config  1:notify enable  2:indicate enable
    uint16_t value_len;
    ble_gatt_callback_func att_callback_func;
    uint8_t *value;         //read return data when att_callback_func is NULL
} ble_gatt_characteristic_cb_info_t;

#define GATT_CLIENT_CONFIG_NOTIFY       1
#define GATT_CLIENT_CONFIG_INDICATE     2

/**
    define group GATT Server profile struct
*/
typedef struct gatts_profile_list
{
    void *item;
    uint16_t  profile_start_handle;
    uint16_t  profile_end_handle;
    const uint8_t   *profile_date;
    uint16_t profile_date_size;
} ble_gatts_profile_list_st;

typedef struct
{
    uint8_t callsetup;
    uint8_t callheld;
    uint8_t call;
    uint8_t ccwa;
} hfp_indicators_status_t;

extern uint8_t cfg_bt_work_mode;
extern uint8_t cfg_bt_max_acl_link;
extern bool cfg_bt_dual_mode;
extern bool cfg_bt_tws_mode;
extern uint8_t cfg_bt_scan_ctrl_mode;
extern bool cfg_bt_simple_pair_mode;
extern uint16_t cfg_bt_support_profile;
extern uint16_t cfg_bt_support_codec;
extern uint8_t cfg_bt_hid_type;
extern const uint8_t cfg_bt_spp_rfcomm_server_ch0;
extern const uint8_t cfg_bt_spp_rfcomm_server_ch1;
extern const uint8_t cfg_bt_spp_rfcomm_server_ch2;

extern uint8_t cfg_bt_connect_times;
extern uint8_t cfg_bt_pwrup_connect_times;
extern uint16_t cfg_bt_sup_to_connect_times;

extern uint8_t cfg_bt_rf_def_txpwr;
extern uint8_t cfg_bt_page_txpwr;
extern uint8_t cfg_bt_inq_txpwr;
extern uint8_t cfg_ble_page_txpwr;
extern uint8_t cfg_ble_page_rssi_thr;

extern uint8_t cfg_bt_a2dp_feature;
extern uint8_t cfg_bt_a2dp_feature1;
extern uint8_t cfg_bt_hfp_feature;
extern uint8_t cfg_bt_hfp_feature1;
extern uint8_t cfg_bt_tws_pair_mode;
extern uint16_t cfg_bt_tws_feat;
extern uint8_t cfg_bt_tws_not_auto_connect;
extern uint8_t cfg_bt_hci_disc_only_spp;

extern const uint16_t link_info_page_size;

#define bt_is_scan_ctrl()                       cfg_bt_scan_ctrl_mode
#define bt_a2dp_is_vol_ctrl()                   (cfg_bt_a2dp_feature & A2DP_AVRCP_VOL_CTRL)

#define BT_OPT_AFH_UNKNOW_DIS               BIT(0)
#define BT_OPT_SCO_ACK_EN                   BIT(1)
extern const uint8_t cfg_bb_bt_opt;

//control
void bb_run_loop(void);
void bt_fcc_init(void);
void bt_init(void);                             //初始化蓝牙变量
int bt_setup(void);                             //打开蓝牙模块
void bt_off(void);                              //关闭蓝牙模块
void bt_wakeup(void);                           //唤醒蓝牙模块
void bt_start_work(uint8_t opcode, uint8_t scan_en); //蓝牙开始工作，opcode: 0=回连, 1=不回连
void bt_send_msg_do(uint msg);                  //蓝牙控制消息，参数详见bt_msg_t
void bt_thread_check_trigger(void);
void bt_audio_bypass(void);                     //蓝牙A2DP/SCO通路关闭，必须与bt_audio_enable成对使用
void bt_audio_enable(void);                     //蓝牙A2DP/SCO通路使能，bypass n次后需要enable n次才能出声
void bt_get_stack_local_name(char* name);
void bt_set_stack_local_name(const char* name);
void bt_set_sco_far_delay(void *buf, uint size, uint delay);

void bt_set_scan(uint8_t scan_en);              //打开/关闭可被发现和可被连接, bit0=可被发现, bit1=可被连接
uint8_t bt_get_scan(void);                      //获取设置的可被发现可被连接状态（已连接时设置完不会立即生效，需要等断开连接）
uint8_t bt_get_curr_scan(void);                 //获取当前可被发现可被连接状态
void bt_connect(void);                          //蓝牙设备回连, 回连次数在cfg_bt_connect_times配置
void bt_connect_address(void);                  //蓝牙设备回连地址, 回连地址在bt_get_connect_addr函数设置
void bt_disconnect_address(void);               //蓝牙设备断开地址, 断开地址在bt_get_disconnect_addr函数设置
void bt_disconnect(uint reason);                //蓝牙设备断开, reason: 0=单独断开（入仓）; 1=断开并同步关机（按键/自动关机）;用户单独调用断开，并不关机reason=0xff
void bt_hid_connect(void);                      //蓝牙HID服务回连
void bt_hid_disconnect(void);                   //蓝牙HID服务断开
int bt_hid_is_connected(void);
bool bt_hid_is_ready_to_discon(void);

//status
uint bt_get_disp_status(void);                  //获取蓝牙的当前显示状态, V060
uint bt_get_status(void);                       //获取蓝牙的当前状态
uint8_t bt_get_scan(void);                      //判断当前可被连接可被扫描状态
uint8_t bt_get_curr_scan(void);                 //获取实时可被连接可被扫描状态
uint bt_get_call_indicate(void);                //获取通话的当前状态
uint bt_get_siri_status(void);                  //获取SIRI当前状态, 0=SIRI已退出, 1=SIRI已唤出
uint8_t bt_call_get_hfp_index(void);            //获取当前通话的INDEX
bool bt_is_calling(void);                       //判断是否正在通话
bool bt_is_playing(void);                       //判断是否正在播放
bool bt_is_testmode(void);                      //判断当前蓝牙是否处于测试模式
bool bt_is_sleep(void);                         //判断蓝牙是否进入休眠状态
bool bt_is_allow_sleep(void);                   //判断蓝牙是否允许进入休眠状态
bool bt_is_connected(void);                     //判断蓝牙是否已连接（TWS副耳被连接，或主耳与手机已连接）
bool bt_is_ios_device(void);                    //判断当前播放的是否ios设备
bool bt_is_support_vol_ctrl(void);              //判断当前播放的是否支持音量同步
uint32_t bt_sleep_proc(void);
void bt_enter_sleep(void);
void bt_exit_sleep(void);
void bt_updata_local_name(char *bt_name);
bool a2dp_is_bypass(void);
bool bt_is_low_latency(void);                   //判断蓝牙是否在低延时状态
bool bt_is_silence(void);
bool bt_decode_is_aac(void);                    //判断蓝牙解码是否是aac
bool bt_decode_is_lhdc(void);                   //判断蓝牙解码是否是LHDC
bool bt_decode_is_ldac(void);                   //判断蓝牙解码是否是LDAC
uint8_t bt_get_connected_num(void);             //一拖二获取当前连接了几台设备
uint8_t bt_get_cur_a2dp_media_index(void);      //一拖二获取当前播放设备的index
u8 bt_call_get_ring_index(void);                //一拖二获取当前响铃设备的index
u8 bt_get_connect_status(u8 *bd_addr);          //获取当前地址设备的连接状态，已连接返回BT_STA_CONNECTED，连接中BT_STA_CONNECTING，未连接BT_STA_DISCONNECTING
bool sbc_is_bypass(void);

//sco status
enum
{
    SCO_STATUS_IDLE             = 0,
    SCO_STATUS_W4_SETUP,
    SCO_STATUS_SETUP,
    SCO_STATUS_W4_KILL,
};
uint8_t bt_sco_get_status(void);
bool sco_is_connected(void);
bool bt_sco_is_msbc(void);                      //判断当前通话是否是宽带通话

//info
bool bt_get_link_btname(uint8_t index, char *name, uint8_t max_size);   //index: 0=link0, 1=link1, 0xff=auto(default link0)
bool bt_get_link_btaddr(uint8_t index, u8 *addr);                       //index: 0=link0, 1=link1, 0xff=auto(default link0)
void bt_get_local_bd_addr(u8 *addr);
bool bt_nor_get_link_info(uint8_t *bd_addr);    //获取手机配对信息，bd_addr=NULL时仅查询是否存在回连信息
bool bt_nor_get_link_info_addr(uint8_t *bd_addr, uint8_t order);    //获取第n个手机配对信息，bd_addr=NULL时仅查询是否存在回连信息
void bt_nor_delete_link_info(void);             //删除手机配对信息
int bt_nor_get_link_name_for_addr(uint8_t *bd_addr, char *name, uint8_t max_size); //获取配对信息中的蓝牙名
uint bt_tws_get_link_info(uint8_t *bd_addr);    //获取tws配对信息，返回值（0=未连接过TWS，0x80=作为master连过副耳，0x81=作为slave连接过主耳），被连地址放在bd_addr
void bt_tws_put_link_info_addr(uint8_t *bd_addr, uint8_t *link_key);
void bt_tws_put_link_info_feature(uint8_t *bd_addr, uint8_t feature);
void bt_tws_delete_link_info_with_tag(const char *tag, uint32_t ra_addr);      //带参数删除tws配对信息，参数会保存在flash，方便追溯
#define bt_tws_delete_link_info()               bt_tws_delete_link_info_with_tag("USER", (uint32_t)__builtin_return_address(0))     //删除tws配对信息


//tws api
uint8_t bt_tws_get_status(void);                //获取tws连接状态
uint8_t bt_tws_get_scan(void);                  //tws是否已打开可被搜索与连接
void bt_tws_set_scan(uint8_t scan_en);          //打开/关闭tws可被搜索与连接, bit0=可被发现, bit1=可被连接
void bt_tws_disconnect(void);                   //断开tws设备
void bt_tws_connect(void);                      //回连tws设备，连接成功返回BT_NOTICE_TWS_CONNECTED，连接失败返回BT_NOTICE_TWS_CONNECT_FAIL
void bt_tws_search_slave(uint timeout);         //搜索tws设备，搜索失败返回BT_NOTICE_TWS_SEARCH_FAIL，搜索成功并建立连接返回BT_NOTICE_TWS_CONNECTED
void bt_tws_abort_search_slave(void);
void bt_tws_unpair_device(void);                //删除tws配对信息并断开
uint8_t bt_tws_user_key(uint keycode);          //发送tws自定义按键, 16bit有效; 返回值：0=成功，1=失败，2=缓冲区满
void bt_tws_switch(void);                       //主从切换，例如通话时切主副MIC
bool bt_tws_need_switch(bool calling);          //查看是否需要切换
void bt_nor_connect(void);                      //回连手机
void bt_nor_disconnect(void);                   //断开手机
void bt_nor_unpair_device(void);                //删除手机配对信息并断开
u8 bt_tws_get_force_role(void);
bool bt_tws_get_first_role(void);
bool bt_tws_get_link_rssi(int8_t *buffer, bool left_channel);
void bt_tws_report_dgb(void);

/**
* @Desc: Switch the current device MIC as active MIC.
* @return: 0-Switch MIC Success, 1-Switch MIC Fail
*/
u8 bt_tws_switch_mic(void);

/**
* @Desc: Hold the current device MIC as active MIC, if not, switch to active MIC first
* @return: 0-HOLD MIC Success, 1-HOLD MIC Fail
*/
u8 bt_tws_hold_mic(void);
/**
* @Desc: Release the permission of the current device MIC as active MIC,
*       the MIC of other device can become active MIC.
*/
void bt_tws_release_mic(void);

/**
* @Desc: Get the device of the active MIC
* @return: 0-local device,1-remote device
*/
u8 bt_tws_get_dev_of_active_mic(void);


#define bt_is_tws_mode()                        cfg_bt_tws_mode
#define bt_tws_is_connected()                   (bool)(bt_tws_get_status() & TWS_STA_FLAG)
#define bt_tws_is_slave()                       (bool)(bt_tws_get_status() & TWS_STA_ROLE)
#define bt_nor_is_connected()                   (bool)(bt_tws_get_status() & TWS_STA_NOR_FLAG)

#define bt_tws_is_ms_switch()                   ((cfg_bt_tws_feat & TWS_FEAT_MS_SWITCH) != 0)

#define bt_tws_is_tsco()                        ((cfg_bt_tws_feat & TWS_FEAT_TSCO) != 0)
#define bt_tws_is_name_menu()                   ((cfg_bt_tws_feat & TWS_FEAT_NAME_MENU) != 0)

#define bt_send_msg(ogf, ocf)                   bt_send_msg_do((ogf<<24) | (ocf))
#define bt_ctrl_msg(msg)                        bt_send_msg(BT_MSG_CTRL, msg)
#define bt_comm_msg(msg, param)                 bt_send_msg(BT_MSG_COMM, (msg<<16) | (u16)(param))
#define bt_tws_msg(msg, param)                  bt_send_msg(BT_MSG_TWS, (msg<<16) | (u16)(param))
#define bt_hid_msg(msg, param)                  bt_send_msg(BT_MSG_HID, (msg<<16) | (u16)(param))
#define bt_pbap_msg(msg, param)                 bt_send_msg(BT_MSG_PBAP, (msg<<16) | (u16)param)
#define bt_wireless_msg(msg, param)             bt_send_msg(BT_MSG_WIRELESS, (msg<<16) | (u16)param)

//蓝牙连接
#define bt_scan_enable()                        bt_set_scan(0x03)                       //打开扫描
#define bt_scan_disable()                       bt_set_scan(0x00)                       //关闭扫描
#define bt_set_scan_param(ps_intv,is_intv)      bt_comm_msg(COMM_BT_SET_SCAN_INTV, (ps_intv << 8) | is_intv)  //设置scan 参数，0x100*(intv+1)
#define bt_abort_reconnect()                    bt_comm_msg(COMM_BT_ABORT_RECONNECT, 0xffff)        //终止回接
#define bt_abort_reconnect_silence(feat)        bt_comm_msg(COMM_BT_ABORT_RECONNECT, (u16)feat)     //终止回接，没有消息回调。feat:0=手机, BT_FEAT_TWS=TWS


//蓝牙音乐
#define bt_music_play()                         bt_ctrl_msg(BT_CTL_A2DP_PLAY)               //播放
#define bt_music_pause()                        bt_ctrl_msg(BT_CTL_A2DP_PAUSE)              //暂停
#define bt_music_play_pause()                   bt_ctrl_msg(BT_CTL_PLAY_PAUSE)              //切换播放/暂停
#define bt_music_stop()                         bt_ctrl_msg(BT_CTL_A2DP_STOP)               //停止
#define bt_music_prev()                         bt_ctrl_msg(BT_CTL_A2DP_BACKWARD)           //上一曲
#define bt_music_next()                         bt_ctrl_msg(BT_CTL_A2DP_FORWARD)            //下一曲
#define bt_music_rewind()                       bt_ctrl_msg(BT_CTL_A2DP_REWIND)             //开始快退
#define bt_music_rewind_end()                   bt_ctrl_msg(BT_CTL_A2DP_REWIND_END)         //结束快退
#define bt_music_fast_forward()                 bt_ctrl_msg(BT_CTL_A2DP_FAST_FORWARD)       //开始快进
#define bt_music_fast_forward_end()             bt_ctrl_msg(BT_CTL_A2DP_FAST_FORWARD_END)   //结束快进
#define bt_music_vol_change()                   bt_ctrl_msg(BT_CTL_VOL_CHANGE)              //调节音乐音量，之后通过回调函数a2dp_vol_set_cb设置音量
#define bt_music_vol_up()                       bt_ctrl_msg(BT_CTL_VOL_UP)                  //音乐加音量，之后通过回调函数a2dp_vol_adj_cb调节音量
#define bt_music_vol_down()                     bt_ctrl_msg(BT_CTL_VOL_DOWN)                //音乐减音量，之后通过回调函数a2dp_vol_adj_cb调节音量
#define bt_music_play_switch()                  bt_ctrl_msg(BT_CTL_2ACL_PALY_SWITCH)        //一拖二切换播放手机
#define bt_music_id3_ele_att_req()              bt_ctrl_msg(BT_CTL_ID3_GET_ELE_ATT)         //ID3获取歌曲信息
#define bt_music_play_status_info_req()         bt_ctrl_msg(BT_CTL_GET_PLAY_STATUS_INFO)    //获取歌曲播放状态信息
#define bt_low_latency_enable()                 bt_ctrl_msg(BT_CTL_LOW_LATENCY_EN)          //蓝牙使能低延时
#define bt_low_latency_disable()                bt_ctrl_msg(BT_CTL_LOW_LATENCY_DIS)         //蓝牙关闭低延时
#define bt_fcc_test_start()                     bt_ctrl_msg(BT_CTL_FCC_TEST)                //FCC test模式

//蓝牙通话
#define bt_call_redial_last_number()            bt_ctrl_msg(BT_CTL_CALL_REDIAL)         //电话回拨（最后一次通话）
#define bt_call_answer_incoming()               bt_ctrl_msg(BT_CTL_CALL_ANSWER_INCOM)   //接听电话，三通时挂起当前通话
#define bt_call_answer_incom_rej_other()        bt_ctrl_msg(BT_CTL_CALL_ANSWER_INCOM_REJ_OTHER)     //接听电话，三通时挂断当前通话，1拖2时挂断当前的手机通话
#define bt_call_answer_incom_hold_other()       bt_ctrl_msg(BT_CTL_CALL_ANSWER_INCOM_HOLD_OTHER)    //接听电话，三通时挂起当前通话，1拖2时挂起当前的手机通话
#define bt_call_terminate()                     bt_ctrl_msg(BT_CTL_CALL_TERMINATE)      //挂断电话
#define bt_call_swap()                          bt_ctrl_msg(BT_CTL_CALL_SWAP)           //切换三通电话
#define bt_call_private_switch()                bt_ctrl_msg(BT_CTL_CALL_PRIVATE_SWITCH) //切换私密通话
#define bt_call_redial_number()                 bt_ctrl_msg(BT_CTL_CALL_REDIAL_NUMBER)
#define bt_call_get_remote_phone_number()       bt_ctrl_msg(BT_CTL_CALL_GET_REMOTE_PHONE_NUM) //通话中发起获取手机号码功能
#define bt_hfp_siri_switch()                    bt_ctrl_msg(BT_CTL_HFP_SIRI_SW)         //开关SIRI, android需要在语音助手中打开“蓝牙耳机按键启动”, ios需要打开siri功能
#define bt_hfp_report_bat()                     bt_ctrl_msg(BT_CTL_HFP_REPORT_BAT)
#define bt_hfp_send_at_cmd()                    bt_ctrl_msg(BT_CTL_HFP_AT_CMD)
#define bt_hfp_send_custom_at_cmd()             bt_ctrl_msg(BT_CTL_HFP_CUSTOM_AT_CMD)

#define bt_ctl_nr_sta_change()
void bt_update_ear_sta(void);                   //更新耳机入耳状态
void bt_set_ear_sta(bool ear_sta);              //设置耳机入耳状态

#if BT_TWS_EN
#define bt_tws_vol_change()
#define bt_tws_sync_setting()                   bt_tws_msg(TWS_SYNC_INFO, (SYNC_INFO_SETTING<<8))
#define bt_tws_sync_led()                       bt_tws_msg(TWS_SYNC_INFO, (SYNC_INFO_LEDCNT<<8))
#define bt_tws_sync_eq_param()                  bt_tws_msg(TWS_SYNC_INFO, (SYNC_INFO_EQPARAM<<8))
#define bt_tws_sync_fot_data()                  bt_tws_msg(TWS_SYNC_INFO, (SYNC_INFO_FOT_DATA<<8))
#define bt_tws_sync_ring_sta()                  bt_tws_msg(TWS_SYNC_INFO, (SYNC_INFO_RING_STA<<8))
#define bt_tws_sync_custom_data()               bt_tws_msg(TWS_SYNC_INFO, (SYNC_INFO_CUSTOM_DATA<<8))

#define bt_fot_tws_resp()                       bt_ctrl_msg(BT_CTL_FOT_RESP)
#else
#define bt_tws_vol_change()
#define bt_tws_sync_setting()                   //bt_tws_msg(TWS_SYNC_INFO, (SYNC_INFO_SETTING<<8))
#define bt_tws_sync_led()                       //bt_tws_msg(TWS_SYNC_INFO, (SYNC_INFO_LEDCNT<<8))
#define bt_tws_sync_eq_param()                  //bt_tws_msg(TWS_SYNC_INFO, (SYNC_INFO_EQPARAM<<8))
#define bt_tws_sync_fot_data()                  //bt_tws_msg(TWS_SYNC_INFO, (SYNC_INFO_FOT_DATA<<8))
#define bt_tws_sync_ring_sta()                  //bt_tws_msg(TWS_SYNC_INFO, (SYNC_INFO_RING_STA<<8))
#define bt_tws_sync_custom_data()               //bt_tws_msg(TWS_SYNC_INFO, (SYNC_INFO_CUSTOM_DATA<<8))

#define bt_fot_tws_resp()                       //bt_ctrl_msg(BT_CTL_FOT_RESP)
#endif

//tws alarm
enum
{
    ALARM_RES,
    ALARM_TONE,
    ALARM_VOL,
    ALARM_USER,
};

#define bt_tws_req_res(param)

#define bt_tws_res_add(param)                   bt_tws_msg(TWS_RES_ADD, param)
#define bt_tws_res_done(param)                  bt_tws_msg(TWS_RES_DONE, param)
#define bt_tws_req_alarm(param)                 bt_tws_msg(TWS_ALARM_REQ, param)

#define bt_tws_req_alarm_res(res_idx)           bt_tws_req_alarm((ALARM_RES<<12)  | (0x0fff & (res_idx)))
#define bt_tws_req_alarm_tone(tone_idx)         bt_tws_req_alarm((ALARM_TONE<<12) | (0x0fff & (tone_idx)))
#define bt_tws_req_alarm_vol(vol, feat)         bt_tws_req_alarm((ALARM_VOL<<12)  | (0x00ff & (vol)) | ((0x000f & (feat))<<8))
#define bt_tws_req_alarm_user(value)            bt_tws_req_alarm((ALARM_USER<<12) | (0x0fff & (value)))

//ring_buf
typedef struct rbuf_tbl_tag
{
    uint8_t *buf;
    uint32_t size;
} rbuf_tbl_t;

typedef struct rbuf_page_tag
{
    uint8_t *buf;               //分页的buffer地址
    uint32_t begin;             //分页在buf中的起始位置
    uint32_t end;               //分页在buf中的结束位置
} rbuf_page_t;

typedef struct
{
    uint32_t rpos;              //读地址位置
    uint32_t wpos;              //写地址位置
    volatile uint32_t count;    //写了多少byte

    uint32_t total;             //buf总大小
    uint16_t trig;              //写到多少byte开始可读
    uint8_t page_nb;            //buf分页总数
    rbuf_page_t page[1];        //分页描述，按实际page分配
} ring_buf_t;

typedef void (*rbuf_callback_t)(uint8_t *, uint8_t *, uint32_t);

void ring_buf_init(ring_buf_t *rbuf, const rbuf_tbl_t *rbuf_tbl, uint8_t page_nb, uint16_t trig);
bool ring_buf_put(ring_buf_t *rbuf, uint8_t *ptr, uint32_t len);
bool ring_buf_get_old(ring_buf_t *rbuf, rbuf_callback_t callback, uint32_t len);
bool ring_buf_get(ring_buf_t *rbuf, uint8_t *ptr, uint32_t len);
bool ring_buf_peek(ring_buf_t *rbuf, uint8_t *ptr, uint32_t len, uint32_t offset);

//pkt
typedef void (*kick_func_t)(void);

struct txbuf_tag
{
    uint8_t *ptr;
    uint16_t len;
    uint16_t handle;
} __attribute__ ((packed)) ;

struct txpkt_tag
{
    struct txbuf_tag txbuf;
    ring_buf_t *pool;
    kick_func_t send_kick;
    uint16_t mtu;
};

void txpkt_init(struct txpkt_tag *txpkt, uint8_t *txbuf, void *pool, uint16_t mtu, kick_func_t kick_func);

//init
void hfp_hf_init(void);
void a2dp_init(void);
void aap_init(void);
uint8_t sdp_add_service(void *item);
uint bt_get_hfp_feature(void);

//a2dp
bool a2dp_is_playing(void);
uint8_t a2dp_vol_reverse(uint vol);                 //将系统音量转换为a2dp_vol
uint8_t a2dp_vol_conver(uint8_t a2dp_vol);          //将a2dp_vol转换为系统音量级数

//spp
enum
{
    SPP_SERVICE_CH0,     //channel0，是默认SPP UUID 服务通路
    SPP_SERVICE_CH1,     //channel1，自定义UUID,默认用GFPS
    SPP_SERVICE_CH2,     //channel2, 自定义UUID，未使用
};
void spp_init(void);
extern struct txpkt_tag spp_tx;
void spp_send_kick(void);
int bt_spp_tx(uint8_t ch, uint8_t *packet, uint16_t len);
void spp_support_mul_server(uint8_t support);
bool spp_is_connect(void);
bool spp_is_connected_with_channel(uint8_t ch);  //判断某一个SPP通路是否连接，目前只有 0,1,2 共3个通路
void spp_disconnect(void);

//voice assistant
enum
{
    VA_STA_IDLE = 0,
    VA_STA_CONNECT,
    VA_STA_STOP,
    VA_STA_START,
    VA_STA_END,
};

#define bt_menu_va_stop()               bt_comm_msg(COMM_BT_ATT_MSG, BIT(1))
void bt_menu_va_operate(u8 enble);
u8 bt_menu_va_is_connected(void);

//hid
void hid_device_init(void);
bool bt_hid_send(void *buf, uint len, bool auto_release);                                           //自定义HID数组
bool bt_hid_send_key(uint type, uint keycode);                                                      //标准HID按键
#define bt_hid_key(keycode)                     bt_hid_send_key(HID_KEYBOARD, keycode)              //标准HID键, 如Enter
#define bt_hid_consumer(keycode)                bt_hid_send_key(HID_CONSUMER, keycode)              //自定义HID键, 如VOL+ VOL-
#define bt_hid_touch_screen(keycode)            bt_hid_send_key(HID_TOUCH_SCREEN, keycode)          //触屏
bool bt_hid_touch_screen_set_key(void *ts);

//pbap
#define bt_pbap_connect()                       bt_pbap_msg(BT_PBAP_CTRL, 1)
#define bt_pbap_disconnect()                    bt_pbap_msg(BT_PBAP_CTRL, 0)
#define bt_pbap_select_phonebook(book, sim)     bt_pbap_msg(BT_PBAP_SELECT_PHONEBOOK, (sim<<8) | (u8)book)
// sim - 1:选择SIM卡，0:本机
// book- 0:pb, 1:fav, 2-ich, 3:och, 4-mch, 5-cch, 6-spd
// 若不配置，则选择默认值为本机pb
#define bt_pbap_get_phonebook_size()            bt_pbap_msg(BT_PBAP_GET_PHONEBOOK_SIZE, 0)
#define bt_pbap_pull_phonebook_whole()          bt_pbap_msg(BT_PBAP_PULL_PHONEBOOK, 0)
#define bt_pbap_pull_phonebook_single(idx)      bt_pbap_msg(BT_PBAP_PULL_PHONEBOOK, idx)
// 按编号获取联系人信息
// idx不为零，如果为零则直接获取整个电话本信息
void pbap_client_init(void);

//MAP
void bt_map_start(void);
void bt_map_abort(void);
void map_client_init(void);

//GOEP
void goep_client_init(void);

//hsp
void hsp_hs_init(void);
void hsp_hs_init_var(void);
void bt_hsp_call_switch(void);                  //挂断/接听
void bt_hsp_sco_conn(void);                     //建立HSP SCO连接
void bt_hsp_sco_disconn(void);                  //断开HSP SCO连接


//latt
void latt_send_kick(void);
int latt_tx_notify(uint16_t att_handle, const uint8_t * value, uint16_t value_len);


/*****************************************************************************
 * WIRRELESS连接相关
 *****************************************************************************/
//feature
#define FEAT_D2A                BIT(7)      //Device to ADAPTER
#define FEAT_A2D                BIT(6)      //ADAPTER to Device
#define FEAT_BONDING            BIT(5)      //组队绑定，需要清除配对才能重新组队
#define FEAT_VERS               (0x7<<8)    //bit[10:8]
#define FEAT_RATE               (0x7<<11)   //bit[13:11]，传输速率
#define FEAT_HOP_V1             BIT(14)     //bit[14]，新版本跳频计算

#define WL_ADV_INTERVAL         0x0018
#define WL_ADV_SLEEP_INTERVAL   (0x0018*8)

extern uint8_t cfg_wireless_role;
extern uint16_t cfg_wireless_feat;

#define wireless_role_is_adapter()              cfg_wireless_role
#define wireless_mic_is_bonding()               (bool)(cfg_wireless_feat & FEAT_BONDING)

#define wireless_adv_set_enable(pscan, iscan)   bt_wireless_msg(WIRELESS_ADV_SET_ENABLE, (pscan<<1)|(iscan))     //广播使能，pscan=可被连接，iscan=可被发现
#define wireless_adv_set_interval(interval)     bt_wireless_msg(WIRELESS_ADV_SET_PARAM,interval)                 //设置ADV 广播间隔
#define wireless_scan_set_enable(en)            bt_wireless_msg(WIRELESS_SCAN_SET_ENABLE, en)                    //扫描使能
#define wireless_connect_req(ms)                bt_wireless_msg(WIRELESS_CONNECT_REQ, ms)                        //发起连接，ms=连接超时（单位毫秒，0xffff时一直连接不超时）
#define wireless_disconnect_req(idx)            bt_wireless_msg(WIRELESS_DISCONNECT_REQ, idx)                    //发起断开，idx=连接通道
#define wireless_setup()                        bt_wireless_msg(WIRELESS_SETUP, 0)                               //无线麦状态设为setup
#define wireless_off()                          bt_wireless_msg(WIRELESS_OFF, 0)                                 //无线麦状态设为off

bool wireless_pscan_adv_is_close(void);
void wireless_con_change_high_prio(uint8_t index);
void wireless_con_audio_stop(uint8_t index);
void wireless_create_con_for_addr(uint8_t *addr, uint16_t timeout);
void wireless_con_channel_assess(uint8_t chidx, bool rx_ok);
bool wireless_con_user_cmd_tx_req(uint8_t index);        //请求发送用户私有命令，请求后库通过ble_con_user_cmd_get_tx_cb获取，返回值1代表成功，0代表失败
#define bt_get_link_info_nb()                   0
uint8_t wireless_con_get_status(void);
bool wireless_con_tws_is_slave(void);
void wireless_con_tws_int_set(uint8_t tws_int);
//V3
uint16_t wl_single_link_duration_get(void);


/*****************************************************************************
 * BLE连接相关（通道1，与BLE无连接广播相互独立）
 *****************************************************************************/
struct att_hdl_t
{
    u16 hdl;    //att handle
    u8 cfg;     //client config enable
    u8 res;
};

extern struct txpkt_tag notify_tx;


//BLE
#define ble_adv_dis()                           bt_ctrl_msg(BT_CTL_BLE_ADV_DISABLE)
#define ble_adv_en()                            bt_ctrl_msg(BT_CTL_BLE_ADV_ENABLE)
void ble_set_adv(u8 chanel, u8 type);
void ble_set_adv_interval(u16 interval);
bool ble_set_adv_data(const u8 *adv_buf, u32 size);
bool ble_set_scan_rsp_data(const u8 *scan_rsp_buf, u32 size);
void ble_update_conn_param(u16 interval, u16 latency, u16 timeout);
u8 ble_get_status(void);
void ble_disconnect(void);
bool ble_is_connect(void);

void ble_init_att_for_handle(u16 handle, uint8_t *buf, uint16_t len);
void ble_send_kick(void);
u16 ble_get_gatt_mtu(void);
int ble_tx_notify(u16 handle, u8* buf, u8 len);

uint8_t ble_set_delta_gain(void);
//init gatt
//profile_table: profile cache buf
//cb_info_table_p: call back info cache
void ble_gatts_init(uint8_t *profile_table, uint16_t profile_table_size,
                    ble_gatt_characteristic_cb_info_t **cb_info_table_p,
                    uint16_t gatt_max_att);
int ble_gatts_service_add(ble_gatts_service_type service_type, const uint8_t *service_uuid, ble_gatts_uuid_type uuid_type, uint16_t *service_handle);
int ble_gatts_characteristic_add( const uint8_t *att_uuid, ble_gatts_uuid_type uuid_type, uint16_t props,
                                  uint16_t *att_handle, ble_gatt_characteristic_cb_info_t *cb_info);
bool ble_gatt_init_att_info(uint16_t att_handle, ble_gatt_characteristic_cb_info_t *att_cb_info);
int ble_gatts_inlcude_service_add(ble_gatts_uuid_type uuid_type, const uint8_t *service_uuid, uint16_t start_handle, uint16_t end_handle, uint16_t *att_handle);
int ble_gatts_add_profile_date(ble_gatts_profile_list_st *gatts_profile);
int ble_gatts_attribute_add(const uint8_t *att_data, uint16_t *att_handle);
bool ble_gatts_profile_mg_alloc_att_num_check(uint8_t att_num);

/*****************************************************************************
 * BLE无连接广播相关（通道0，与BLE连接相互独立）
 *****************************************************************************/
#define BLE_ADV0_EN_BIT             0x01
#define BLE_ADV0_MS_VAR_BIT         0x02
#define BLE_ADV0_ADDR_PUBIC_BIT          0x04

extern const uint8_t cfg_ble_adv0_en;

void ble_adv0_set_intv(u16 intv);                       //设置广播间隔，单位625us
void ble_adv0_set_ctrl(uint opcode);                    //0=关闭广播, 1=打开广播, 2=更新广播数据（打开时直接广播，关闭时仅更新buffer）
uint8_t ble_adv0_get_adv_en(void);

void ble_adv0_update_adv(void);
void ble_adv0_idx_update(void);

/*****************************************************************************
 * BLE私有连接相关（与BLE连接相互独立）
 *****************************************************************************/
uint8_t ble_priv_adv_get_adv_en(void);
int ble_priv_tx_notify(u16 att_handle, u8* buf, u8 len);
void ble_priv_adv_en(u16 opcode);
bool ble_priv_is_connect(void);
void ble_priv_con_discon(void);
#endif //_API_BTSTACK_H
