#ifndef _BSP_HALT_H
#define _BSP_HALT_H

//HALT 定义 APP 0 - 0x7FFF
#define HALT_USER                               0           //用户自定义的HALT范围在0~0xFFF,请在Plugin/user_halt.h进行定义
#define HALT_FUNC                               0x1000
#define HALT_BSP                                0x4000
#define HALT_GUI                                0x5000

//HALT_FUNC
#define HALT_FUNC_HEAP                          (HALT_FUNC | 1)                  //0x1001
#define HALT_FUNC_SORT                          (HALT_FUNC | 2)                  //0x1002
#define HALT_FUNC_SWITCH                        (HALT_FUNC | 0x10)               //0x1010
#define HALT_FUNC_SWITCH_MODE                   (HALT_FUNC_SWITCH | 1)           //0x1011
#define HALT_FUNC_SWITCH_MENU_PTR               (HALT_FUNC_SWITCH | 2)           //0x1012
#define HALT_FUNC_SWITCH_MENU_MODE              (HALT_FUNC_SWITCH | 3)           //0x1013
#define HALT_FUNC_SWITCH_LR_PTR                 (HALT_FUNC_SWITCH | 4)           //0x1014
#define HALT_FUNC_SWITCH_LR_MODE                (HALT_FUNC_SWITCH | 5)           //0x1015
#define HALT_FUNC_SWITCH_ZOOM_PTR               (HALT_FUNC_SWITCH | 6)           //0x1016
#define HALT_FUNC_SWITCH_ZOOM_MODE              (HALT_FUNC_SWITCH | 7)           //0x1017
#define HALT_FUNC_SWITCH_ZOOM_PARAM             (HALT_FUNC_SWITCH | 8)           //0x1018
#define HALT_FUNC_SWITCH_UD_PTR                 (HALT_FUNC_SWITCH | 9)           //0x1019
#define HALT_FUNC_SWITCH_UD_MODE                (HALT_FUNC_SWITCH | 0x0A)        //0x101A

#define HALT_MSGBOX                             (HALT_FUNC | 0x80)               //0x1080
#define HALT_MSGBOX_MODE                        (HALT_MSGBOX | 1)                //0x1081

#define HALT_FUNC_MENU                          (HALT_FUNC | 0x100)              //0x1100
#define HALT_FUNC_MENU_SUBSTA                   (HALT_FUNC_MENU | 1)             //0x1101

//HALT_BSP
#define HALT_I18N_INIT                          (HALT_BSP | 1)                   //0x4001
#define HALT_BSP_SYS_CTLBITS                    (HALT_BSP | 2)                   //0x4002
#define HALT_MALLOC                             (HALT_BSP | 3)                   //0x4003
#define HALT_BSP_PORT                           (HALT_BSP | 0x10)                //0x4010
#define HALT_BSP_PORT_INIT                      (HALT_BSP_PORT | 1)              //0x4011
#define HALT_BSP_PORT_IRQ                       (HALT_BSP_PORT | 2)              //0x4012

//HALT_GUI
#define HALT_GUI_COMPO                          (HALT_GUI | 0x100)               //0x5100
#define HALT_GUI_COMPO_UNKNOW_TYPE              (HALT_GUI_COMPO | 1)             //0x5101
#define HALT_GUI_COMPO_PTR                      (HALT_GUI_COMPO | 2)             //0x5102
#define HALT_GUI_COMPO_GET_NEXT                 (HALT_GUI_COMPO | 3)             //0x5103
#define HALT_GUI_COMPO_BUF_EXCEED               (HALT_GUI_COMPO | 5)             //0x5105
#define HALT_GUI_COMPO_POOL_CREATE              (HALT_GUI_COMPO | 6)             //0x5106
#define HALT_GUI_COMPO_POOL_CLEAR               (HALT_GUI_COMPO | 7)             //0x5107
#define HALT_GUI_COMPO_POOL_SETID               (HALT_GUI_COMPO | 8)             //0x5108
#define HALT_GUI_COMPO_POOL_BONDDATA            (HALT_GUI_COMPO | 9)             //0x5109
#define HALT_GUI_COMPO_POOL_SETTOP              (HALT_GUI_COMPO | 10)            //0x510A

#define HALT_GUI_COMPO_COMM                     (HALT_GUI_COMPO | 0x20)          //0x5120
#define HALT_GUI_COMPO_CREATE                   (HALT_GUI_COMPO_COMM | 1)        //0x5121
#define HALT_GUI_COMPO_ROLL                     (HALT_GUI_COMPO_COMM | 2)        //0x5122

#define HALT_GUI_COMPO_FORM                     (HALT_GUI_COMPO | 0x40)          //0x5140
#define HALT_GUI_COMPO_FORM_CREATE              (HALT_GUI_COMPO_FORM | 1)        //0x5141
#define HALT_GUI_COMPO_FORM_PTR                 (HALT_GUI_COMPO_FORM | 2)        //0x5142

#define HALT_GUI_COMPO_ICONLIST                 (HALT_GUI_COMPO | 0x80)          //0x5180
#define HALT_GUI_COMPO_ICONLIST_TYPE            (HALT_GUI_COMPO_ICONLIST | 1)    //0x5181
#define HALT_GUI_COMPO_ICONLIST_STYLE           (HALT_GUI_COMPO_ICONLIST | 2)    //0x5182
#define HALT_GUI_COMPO_ICONLIST_ADD             (HALT_GUI_COMPO_ICONLIST | 3)    //0x5183
#define HALT_GUI_COMPO_ICONLIST_ICONSIZE        (HALT_GUI_COMPO_ICONLIST | 4)    //0x5184
#define HALT_GUI_COMPO_ICONLIST_SET_ICONSIZE    (HALT_GUI_COMPO_ICONLIST | 5)    //0x5185
#define HALT_GUI_COMPO_ICONLIST_SET_FOCUS       (HALT_GUI_COMPO_ICONLIST | 6)    //0x5186
#define HALT_GUI_COMPO_ICONLIST_TIME_TYPE       (HALT_GUI_COMPO_ICONLIST | 7)    //0x5187
#define HALT_GUI_COMPO_ICONLIST_GETLINE         (HALT_GUI_COMPO_ICONLIST | 8)    //0x5188
#define HALT_GUI_COMPO_ICONLIST_TIME_IDX        (HALT_GUI_COMPO_ICONLIST | 9)    //0x5189

#define HALT_GUI_COMPO_LISTBOX                  (HALT_GUI_COMPO | 0x90)          //0x5190
#define HALT_GUI_COMPO_LISTBOX_TYPE             (HALT_GUI_COMPO_LISTBOX | 1)     //0x5191
#define HALT_GUI_COMPO_LISTBOX_STYLE            (HALT_GUI_COMPO_LISTBOX | 2)     //0x5192
#define HALT_GUI_COMPO_LISTBOX_SET_FOCUS        (HALT_GUI_COMPO_LISTBOX | 3)     //0x5193
#define HALT_GUI_COMPO_LISTBOX_CREATE           (HALT_GUI_COMPO_LISTBOX | 4)     //0x5194
#define HALT_GUI_COMPO_LISTBOX_SET              (HALT_GUI_COMPO_LISTBOX | 5)     //0x5195
#define HALT_GUI_COMPO_LISTBOX_SET_BGIMG        (HALT_GUI_COMPO_LISTBOX | 6)     //0x5196
#define HALT_GUI_COMPO_LISTBOX_SET_STA_ICON     (HALT_GUI_COMPO_LISTBOX | 7)     //0x5197
#define HALT_GUI_COMPO_LISTBOX_GET_Y            (HALT_GUI_COMPO_LISTBOX | 8)     //0x5198
#define HALT_GUI_COMPO_LISTBOX_UPDATE           (HALT_GUI_COMPO_LISTBOX | 9)     //0x5199
#define HALT_GUI_COMPO_LISTBOX_SELECT           (HALT_GUI_COMPO_LISTBOX | 0xA)   //0x519A
#define HALT_GUI_COMPO_LISTBOX_GET_SELECT       (HALT_GUI_COMPO_LISTBOX | 0xB)   //0x519B
#define HALT_GUI_COMPO_LISTBOX_ITEM_MODE        (HALT_GUI_COMPO_LISTBOX | 0xC)   //0x519C
#define HALT_GUI_COMPO_LISTBOX_SET_BITHOOK      (HALT_GUI_COMPO_LISTBOX | 0xD)   //0x519D
#define HALT_GUI_COMPO_LISTBOX_SET_BTN_ICON     (HALT_GUI_COMPO_LISTBOX | 0xE)   //0x519E
#define HALT_GUI_COMPO_LISTBOX_MOVE_CMD         (HALT_GUI_COMPO_LISTBOX | 0xF)   //0x519F
#define HALT_GUI_COMPO_LISTBOX_MOVE_STA         (HALT_GUI_COMPO_LISTBOX | 0x10)  //0x51A0

#define HALT_GUI_COMPO_STACKLIST                (HALT_GUI_COMPO | 0xA0)          //0x51A0
#define HALT_GUI_COMPO_STACKLIST_COUNT          (HALT_GUI_COMPO_STACKLIST | 1)   //0x51A1

#define HALT_GUI_COMPO_ROTARY                   (HALT_GUI_COMPO | 0xB0)          //0x51B0
#define HALT_GUI_COMPO_ROTARY_TYPE              (HALT_GUI_COMPO_ROTARY | 1)      //0x51B1
#define HALT_GUI_COMPO_ROTARY_CREATE            (HALT_GUI_COMPO_ROTARY | 2)      //0x51B2
#define HALT_GUI_COMPO_ROTARY_MOVE_CMD          (HALT_GUI_COMPO_ROTARY | 3)      //0x51B3
#define HALT_GUI_COMPO_ROTARY_MOVE_STA          (HALT_GUI_COMPO_ROTARY | 4)      //0x51B4

#define HALT_GUI_COMPO_ARC                      (HALT_GUI_COMPO | 0xC0)          //0x51C0
#define HALT_GUI_COMPO_ARC_FAIL                 (HALT_GUI_COMPO_ARC | 0)         //0x51C0
#define HALT_GUI_COMPO_ARC_NULL                 (HALT_GUI_COMPO_ARC | 1)         //0x51C1
#define HALT_GUI_COMPO_ARC_OUT_OF_RANGE         (HALT_GUI_COMPO_ARC | 2)         //0x51C2
#define HALT_GUI_COMPO_ARC_TYPE                 (HALT_GUI_COMPO_ARC | 3)         //0x51C3

#define HALT_GUI_COMPO_CHARTBOX                 (HALT_GUI_COMPO | 0xD0)          //0x51D0
#define HALT_GUI_COMPO_CHARTBOX_PIXEL           (HALT_GUI_COMPO_CHARTBOX | 0)    //0x51D0
#define HALT_GUI_COMPO_CHARTBOX_NOSIZE          (HALT_GUI_COMPO_CHARTBOX | 1)    //0x51D1
#define HALT_GUI_COMPO_CHARTBOX_REALNUM         (HALT_GUI_COMPO_CHARTBOX | 2)    //0x51D2

#define HALT_GUI_COMPO_FOOTBALL                 (HALT_GUI_COMPO | 0xE0)          //0x51E0
#define HALT_GUI_COMPO_FOOTBALL_TYPE            (HALT_GUI_COMPO_FOOTBALL | 1)    //0x51E1
#define HALT_GUI_COMPO_FOOTBALL_CREATE          (HALT_GUI_COMPO_FOOTBALL | 2)    //0x51E2
#define HALT_GUI_COMPO_FOOTBALL_MOVE_CMD        (HALT_GUI_COMPO_FOOTBALL | 3)    //0x51E3
#define HALT_GUI_COMPO_FOOTBALL_MOVE_STA        (HALT_GUI_COMPO_FOOTBALL | 4)    //0x51E4

#define HALT_GUI_COMPO_BUTTERFLY                (HALT_GUI_COMPO | 0xF0)          //0x51F0
#define HALT_GUI_COMPO_BUTTERFLY_TYPE           (HALT_GUI_COMPO_BUTTERFLY | 1)   //0x51F1
#define HALT_GUI_COMPO_BUTTERFLY_CREATE         (HALT_GUI_COMPO_BUTTERFLY | 2)   //0x51F2
#define HALT_GUI_COMPO_BUTTERFLY_MOVE_CMD       (HALT_GUI_COMPO_BUTTERFLY | 3)   //0x51F3
#define HALT_GUI_COMPO_BUTTERFLY_MOVE_STA       (HALT_GUI_COMPO_BUTTERFLY | 4)   //0x51F4

#define HALT_GUI_COMPO_CUBE                     (HALT_GUI_COMPO | 0xF0)          //0x51F0
#define HALT_GUI_COMPO_CUBE_TYPE                (HALT_GUI_COMPO_CUBE | 0x0A)     //0x51FA
#define HALT_GUI_COMPO_CUBE_CREATE              (HALT_GUI_COMPO_CUBE | 0x0B)     //0x51FB
#define HALT_GUI_COMPO_CUBE_MOVE_CMD            (HALT_GUI_COMPO_CUBE | 0x0C)     //0x51FC
#define HALT_GUI_COMPO_CUBE_MOVE_STA            (HALT_GUI_COMPO_CUBE | 0x0D)     //0x51FD

#define HALT_GUI_COMPO2                         (HALT_GUI | 0x200)               //0x5200
#define HALT_GUI_COMPO_NUMBER                   (HALT_GUI_COMPO2 | 0x10)         //0x5210
#define HALT_GUI_COMPO_NUMBER_CNT               (HALT_GUI_COMPO_NUMBER | 1)      //0x5211
#define HALT_GUI_COMPO_NUMBER_VALUE             (HALT_GUI_COMPO_NUMBER | 2)      //0x5212

#define HALT_GUI_DIALPLATE                      (HALT_GUI | 0x300)               //0x5300
#define HALT_GUI_DIALPLATE_HEAD                 (HALT_GUI_DIALPLATE | 1)         //0x5301
#define HALT_GUI_DIALPLATE_TYPE                 (HALT_GUI_DIALPLATE | 2)         //0x5302

void halt(u32 halt_no);

#endif
