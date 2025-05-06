#include "include.h"
#include "func.h"

#if TRACE_EN
#define TRACE(...)              printf(__VA_ARGS__)
#else
#define TRACE(...)
#endif


typedef struct f_heart_about_t_
{
   page_tp_move_t *ptm;
   compo_textbox_t *txt_page;
} f_heart_about_t;


#if GUI_SCREEN_SIZE_240X284RGB_I335001_SUPPORT
compo_form_t *func_heart_about_form_create(void)
{
   compo_form_t *frm = compo_form_create(true);

   compo_textbox_t *textbox = compo_textbox_create(frm, strlen(i18n[STR_RESTING_HEART]));
   compo_textbox_set_location(textbox, GUI_SCREEN_CENTER_X, 20, 230, 30);
   compo_textbox_set(textbox,i18n[STR_RESTING_HEART]);

   textbox = compo_textbox_create(frm, strlen(i18n[STR_HEART_ABOUT]));
   compo_textbox_set_align_center(textbox,false);
   compo_textbox_set_location(textbox,10, 43, 220, 280);
   compo_textbox_set_multiline(textbox, true);
   widget_text_set_ellipsis(textbox->txt, false);      //避免既有滚动又有省略号的情况
   compo_textbox_set(textbox,i18n[STR_HEART_ABOUT]);
   compo_textbox_set_location(textbox,10, 43, 220, widget_text_get_area(textbox->txt).hei);
   compo_textbox_set(textbox,i18n[STR_HEART_ABOUT]);

   if(func_cb.sta == FUNC_HEAR_ABOUT)
   {
      f_heart_about_t *f_heart_about = (f_heart_about_t *)func_cb.f_cb;
      f_heart_about->txt_page = textbox;
   }


   return frm;
}
#elif GUI_SCREEN_SIZE_360X360RGB_I338001_SUPPORT
compo_form_t *func_heart_about_form_create(void)
{
   compo_form_t *frm = compo_form_create(true);

   // //设置标题栏
   compo_form_set_mode(frm, COMPO_FORM_MODE_SHOW_TITLE | COMPO_FORM_MODE_SHOW_TIME);
   compo_form_set_title(frm, i18n[STR_HEART_RATE]);
   // compo_textbox_t *textbox = compo_textbox_create(frm, strlen(i18n[STR_RESTING_HEART]));
   // compo_textbox_set_location(textbox, GUI_SCREEN_CENTER_X, 20, 230, 30);
   // compo_textbox_set(textbox,i18n[STR_RESTING_HEART]);

   compo_textbox_t *textbox = compo_textbox_create(frm, strlen(i18n[STR_HEART_ABOUT]));
   compo_textbox_set_align_center(textbox,false);
   compo_textbox_set_location(textbox,58, 43, 244, 280);
   compo_textbox_set_multiline(textbox, true);
   widget_text_set_ellipsis(textbox->txt, false);      //避免既有滚动又有省略号的情况
   compo_textbox_set(textbox,i18n[STR_HEART_ABOUT]);
   compo_textbox_set_location(textbox,58, 43, 244, widget_text_get_area(textbox->txt).hei);
   compo_textbox_set(textbox,i18n[STR_HEART_ABOUT]);

   if(func_cb.sta == FUNC_HEAR_ABOUT)
   {
      f_heart_about_t *f_heart_about = (f_heart_about_t *)func_cb.f_cb;
      f_heart_about->txt_page = textbox;
   }


   return frm;
}
#else
compo_form_t *func_heart_about_form_create(void)
{
   compo_form_t *frm = compo_form_create(true);

   return frm;
}
#endif


//时钟表盘右滑菜单功能消息处理
static void func_heart_about_message(size_msg_t msg)
{
   f_heart_about_t *f_heart_about= (f_heart_about_t*)func_cb.f_cb;

   switch (msg)
   {
      case MSG_CTP_TOUCH:
         if(f_heart_about->ptm)
         {
            compo_page_move_touch_handler(f_heart_about->ptm);
         }
         break;
      case MSG_CTP_SHORT_RIGHT:
         func_switch_to(FUNC_HEARTRATE,FUNC_SWITCH_LR_ZOOM_RIGHT);
         break;
      case MSG_CTP_SHORT_LEFT:
         break;

       default:
           func_message(msg);
         break;
   }
}
//下滑事件流程处理
static void func_heart_about_process(void)
{
   f_heart_about_t *f_heart_about = (f_heart_about_t *)func_cb.f_cb;
   if(f_heart_about->ptm)
   {
      compo_page_move_process(f_heart_about->ptm);
   }
   func_process();
}

//创建界面
static void func_heart_about_enter(void)
{
   func_cb.f_cb = func_zalloc(sizeof(f_heart_about_t));
   func_cb.frm_main = func_heart_about_form_create();
   f_heart_about_t *f_heart_about = (f_heart_about_t *)func_cb.f_cb;
#if GUI_SCREEN_SIZE_240X284RGB_I335001_SUPPORT
   f_heart_about->ptm = (page_tp_move_t *)func_zalloc(sizeof(page_tp_move_t));
   page_move_info_t info =
   {
      .title_used = false,
      .page_size = widget_text_get_area(f_heart_about->txt_page->txt).hei+50,
      .page_count = 1,
      .quick_jump_perc =10,
   };
   compo_page_move_init(f_heart_about->ptm, func_cb.frm_main->page_body, &info);
#elif GUI_SCREEN_SIZE_360X360RGB_I338001_SUPPORT
   f_heart_about->ptm = (page_tp_move_t *)func_zalloc(sizeof(page_tp_move_t));
   page_move_info_t info =
   {
      .title_used = false,
      .page_size = widget_text_get_area(f_heart_about->txt_page->txt).hei+90,
      .page_count = 1,
      .quick_jump_perc =10,
   };
   compo_page_move_init(f_heart_about->ptm, func_cb.frm_main->page_body, &info);
#endif
}
//退出任务
static void func_heart_about_exit(void)
{
   f_heart_about_t *f_heart_about = (f_heart_about_t *)func_cb.f_cb;
   if (f_heart_about->ptm)
   {
      func_free(f_heart_about->ptm);
   }
   func_cb.last = FUNC_HEAR_ABOUT;
}

//心率说明功能
void func_heart_about(void)
{
   printf("%s\n", __func__);
   func_heart_about_enter();
   while (func_cb.sta == FUNC_HEAR_ABOUT)
   {
       func_heart_about_process();
       func_heart_about_message(msg_dequeue());
   }
   func_heart_about_exit();
}


