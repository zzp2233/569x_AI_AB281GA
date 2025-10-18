#ifndef _API_MUSIC_H
#define _API_MUSIC_H

enum
{
    MUSIC_STOP = 0,
    MUSIC_PAUSE,
    MUSIC_PLAYING,
};

typedef enum
{
    NONE_TYPE = 0,
    WAV_TYPE,
    MP3_TYPE,
    WMA_TYPE,
    APE_TYPE,
    FLAC_TYPE,
    SBCIO_TYPE,
    SBC_TYPE,
    AAC_TYPE,
} enum_msc_type_t;

enum
{
    //decode msg
    MUSIC_MSG_STOP = 0,
    MUSIC_MSG_PAUSE,
    MUSIC_MSG_PLAY,
    MUSIC_MSG_FRAME,
    MUSIC_MSG_BT_PLAY,

    //encode msg
    ENC_MSG_MP3 = 64,
    ENC_MSG_AEC,
    ENC_MSG_ALC,
    ENC_MSG_SCO,
    ENC_MSG_SBC,
    ENC_MSG_WAV,
    ENC_MSG_ADPCM,
};

enum
{
    QSKIP_FORWARD,
    QSKIP_BACKWARD,
};

enum FADE_DIR_T
{
    FADE_DIR_SUB    = -1,
    FADE_DIR_KEEP   = 0,
    FADE_DIR_ADD    = 1,
};

typedef struct
{
    u16 fade_step;
    volatile s8 fade_dir;
    u16 gain;
    volatile u16 target_gain;
} soft_vol_t;
extern soft_vol_t soft_vol_cb;
extern unsigned char avio_buf[556];

void music_control(u8 msg);
u8 get_music_dec_sta(void);
int music_decode_init(void);
int wav_decode_init(void);
int mp3_decode_init(void);
int wma_decode_init(void);
int ape_decode_init(void);
int flac_decode_init(void);
int sbcio_decode_init(void);
void sbc_decode_init(void);
u16 music_get_total_time(void);
u16 music_get_cur_time(void);
void music_set_cur_time(u32 cur_time);
u32 get_music_bitrate(void);
void music_stream_var_init(void);
int music_is_encrypt(u16 key);                 //返回0表示为加密音乐
void music_stream_decrypt(void *buf, unsigned int len);
void music_qskip(bool direct, u8 second);      //快进快退控制 direct = 1为快退，0为快进
void music_qskip_end(void);
void music_set_jump(void *brkpt);
void music_get_breakpiont(void *brkpt);

void soft_vol_init(soft_vol_t* p);              //软件数字音量初始化

/**
 * @brief 软件数字音量初始化
 * @param[in] p: 控制块
 * @param[in] vol: 设置的数字音量(0~0x7fff)
 * @param[in] vol_direct_set: 音量是否直接设置, 不过淡入淡出
*/
void soft_vol_set_vol_param(soft_vol_t* p, u16 vol, u8 vol_direct_set);

/**
 * @brief 软件数字音量增益处理
 * @param[in] p: 控制块
 * @param[in] inputl: 左声道数据
 * @param[in] inputr: 右声道数据
*/
void soft_vol_process_stereo_one_sample(soft_vol_t* p, s16* inputl, s16* inputr);

#endif // _API_MUSIC_H
