#include "include.h"
#include "func.h"
#include "ute_all_sports_int_algorithms.h"
#include "ute_module_sport.h"
#if TRACE_EN
#define TRACE(...)              printf(__VA_ARGS__)
#else
#define TRACE(...)
#endif

#define MENU_LIST_CNT                       ((int)(sizeof(tbl_sport_list) / sizeof(tbl_sport_list[0])))

enum
{
    COMPO_ID_LISTBOX = 1,
};

typedef struct f_sport_list_t_
{
    compo_listbox_t *listbox;

} f_sport_list_t;


#define UTE_SPORT_TYPE(type)        [type-1]
static const compo_listbox_item_t tbl_sport_list[] =
{
    UTE_SPORT_TYPE(SPORT_TYPE_RUNNING)      = {STR_SPORT_RUN,           UI_BUF_SPORT_ICON_RUN_BIN,              .func_sta = FUNC_SPORT_SWITCH},
    UTE_SPORT_TYPE(SPORT_TYPE_RIDE_BIKE)    = {STR_SPORT_RIDE_BIKE,     UI_BUF_SPORT_ICON_RIDE_BIKE_BIN,        .func_sta = FUNC_SPORT_SWITCH},
    UTE_SPORT_TYPE(SPORT_TYPE_JUMP_ROPE)    = {STR_SPORT_JUMP_ROPE,     UI_BUF_SPORT_ICON_JUMP_ROPE_BIN,        .func_sta = FUNC_SPORT_SWITCH},
    UTE_SPORT_TYPE(SPORT_TYPE_SWIMMING)     = {STR_SPORT_SWIMMING,      UI_BUF_SPORT_ICON_SWIMMING_BIN,         .func_sta = FUNC_SPORT_SWITCH},
    UTE_SPORT_TYPE(SPORT_TYPE_BADMINTON)    = {STR_SPORT_BADMINTON,     UI_BUF_SPORT_ICON_BADMINTON_BIN,        .func_sta = FUNC_SPORT_SWITCH},
    UTE_SPORT_TYPE(SPORT_TYPE_TABLE_TENNIS) = {STR_SPORT_TABLE_TENNIS,  UI_BUF_SPORT_ICON_TABLE_TENNIS_BIN,     .func_sta = FUNC_SPORT_SWITCH},
    UTE_SPORT_TYPE(SPORT_TYPE_TENNIS)       = {STR_SPORT_TENNIS,        UI_BUF_SPORT_ICON_TENNIS_BIN,           .func_sta = FUNC_SPORT_SWITCH},
    UTE_SPORT_TYPE(SPORT_TYPE_CLIMBING)     = {STR_SPORT_CLIMBING,      UI_BUF_SPORT_ICON_CLIMBING_BIN,         .func_sta = FUNC_SPORT_SWITCH},
    UTE_SPORT_TYPE(SPORT_TYPE_WALKING)      = {STR_SPORT_WALKING,       UI_BUF_SPORT_ICON_WALKING_BIN,          .func_sta = FUNC_SPORT_SWITCH},
    UTE_SPORT_TYPE(SPORT_TYPE_BASKETBALL)   = {STR_SPORT_BASKETBALL,    UI_BUF_SPORT_ICON_BASKETBALL_BIN,       .func_sta = FUNC_SPORT_SWITCH},
    UTE_SPORT_TYPE(SPORT_TYPE_FOOTBALL)     = {STR_SPORT_FOOTBALL,      UI_BUF_SPORT_ICON_FOOTBALL_BIN,         .func_sta = FUNC_SPORT_SWITCH},
    UTE_SPORT_TYPE(SPORT_TYPE_BASEBALL)     = {STR_SPORT_BASEBALL,      UI_BUF_SPORT_ICON_BASEBALL_BIN,         .func_sta = FUNC_SPORT_SWITCH},
    UTE_SPORT_TYPE(SPORT_TYPE_VOLLEYBALL)   = {STR_SPORT_VOLLEYBALL,    UI_BUF_SPORT_ICON_VOLLEYBALL_BIN,       .func_sta = FUNC_SPORT_SWITCH},
    UTE_SPORT_TYPE(SPORT_TYPE_CRICKET)      = {STR_SPORT_CRICKET,       UI_BUF_SPORT_BEFORE_EXERCISE_WALK_BIN,  .func_sta = FUNC_SPORT_SWITCH},
    UTE_SPORT_TYPE(SPORT_TYPE_RUGBY)        = {STR_SPORT_RUGBY,         UI_BUF_SPORT_ICON_RUGBY_BIN,            .func_sta = FUNC_SPORT_SWITCH},
    UTE_SPORT_TYPE(SPORT_TYPE_HOCKEY)       = {STR_SPORT_HOCKEY,        UI_BUF_SPORT_ICON_HOCKEY_BIN,           .func_sta = FUNC_SPORT_SWITCH},
    UTE_SPORT_TYPE(SPORT_TYPE_DANCE)        = {STR_SPORT_DANCE,         UI_BUF_SPORT_ICON_DANCE_BIN,            .func_sta = FUNC_SPORT_SWITCH},
    UTE_SPORT_TYPE(SPORT_TYPE_SPINNING)     = {STR_SPORT_SPINNING,      UI_BUF_SPORT_ICON_SPINNING_BIN,         .func_sta = FUNC_SPORT_SWITCH},
    UTE_SPORT_TYPE(SPORT_TYPE_YOGA)         = {STR_SPORT_YOGA,          UI_BUF_SPORT_ICON_YOGA_BIN,             .func_sta = FUNC_SPORT_SWITCH},
    UTE_SPORT_TYPE(SPORT_TYPE_SIT_UP)       = {STR_SPORT_SIT_UP,        UI_BUF_SPORT_ICON_SIT_UP_BIN,           .func_sta = FUNC_SPORT_SWITCH},
    UTE_SPORT_TYPE(SPORT_TYPE_TREADMILL)    = {STR_SPORT_TREADMILL,     UI_BUF_SPORT_ICON_TREADMILL_BIN,        .func_sta = FUNC_SPORT_SWITCH},
    UTE_SPORT_TYPE(SPORT_TYPE_GYMNASTICS)   = {STR_SPORT_GYMNASTICS,    UI_BUF_SPORT_ICON_GYMNASTICS_BIN,       .func_sta = FUNC_SPORT_SWITCH},
    UTE_SPORT_TYPE(SPORT_TYPE_BOATING)      = {STR_SPORT_BOATING,       UI_BUF_SPORT_ICON_BOATING_BIN,          .func_sta = FUNC_SPORT_SWITCH},
    UTE_SPORT_TYPE(SPORT_TYPE_JUMPING_JACK) = {STR_SPORT_JUMPING_JACK,  UI_BUF_SPORT_ICON_JUMPING_JACK_BIN,     .func_sta = FUNC_SPORT_SWITCH},
    UTE_SPORT_TYPE(SPORT_TYPE_FREE_TRAINING)= {STR_SPORT_FREE_TRAINING, UI_BUF_SPORT_ICON_FREE_TRAINING_BIN,    .func_sta = FUNC_SPORT_SWITCH},

#if UTE_MODULE_SPORT_HUNDRED_SUPPORT
    UTE_SPORT_TYPE(SPORT_TYPE_INDOOR_WALK)          = {STR_SPORT_RUN,           UI_BUF_SPORT_ICON_RUN_BIN,              .func_sta = FUNC_SPORT_SWITCH},
    UTE_SPORT_TYPE(SPORT_TYPE_INDOOR_RUN)           = {STR_SPORT_RUN,           UI_BUF_SPORT_ICON_RUN_BIN,              .func_sta = FUNC_SPORT_SWITCH},
    UTE_SPORT_TYPE(SPORT_TYPE_STRENGTH_TRAINING)    = {STR_SPORT_RUN,           UI_BUF_SPORT_ICON_RUN_BIN,              .func_sta = FUNC_SPORT_SWITCH},
    UTE_SPORT_TYPE(SPORT_TYPE_STEP_TRAINING)        = {STR_SPORT_RUN,           UI_BUF_SPORT_ICON_RUN_BIN,              .func_sta = FUNC_SPORT_SWITCH},
    UTE_SPORT_TYPE(SPORT_TYPE_HORSE_RIDING)         = {STR_SPORT_RUN,           UI_BUF_SPORT_ICON_RUN_BIN,              .func_sta = FUNC_SPORT_SWITCH},
    UTE_SPORT_TYPE(SPORT_TYPE_ELLIPTICAL_TRAINER)   = {STR_SPORT_RUN,           UI_BUF_SPORT_ICON_RUN_BIN,              .func_sta = FUNC_SPORT_SWITCH},
    UTE_SPORT_TYPE(SPORT_TYPE_TAI_CHI)              = {STR_SPORT_RUN,           UI_BUF_SPORT_ICON_RUN_BIN,              .func_sta = FUNC_SPORT_SWITCH},
    UTE_SPORT_TYPE(SPORT_TYPE_SHUTTLECOCK)          = {STR_SPORT_RUN,           UI_BUF_SPORT_ICON_RUN_BIN,              .func_sta = FUNC_SPORT_SWITCH},
    UTE_SPORT_TYPE(SPORT_TYPE_BOXING)               = {STR_SPORT_RUN,           UI_BUF_SPORT_ICON_RUN_BIN,              .func_sta = FUNC_SPORT_SWITCH},
    UTE_SPORT_TYPE(SPORT_TYPE_OUTDOOR_WALK)         = {STR_SPORT_RUN,           UI_BUF_SPORT_ICON_RUN_BIN,              .func_sta = FUNC_SPORT_SWITCH},
    UTE_SPORT_TYPE(SPORT_TYPE_TRAIL_RUNNING)        = {STR_SPORT_RUN,           UI_BUF_SPORT_ICON_RUN_BIN,              .func_sta = FUNC_SPORT_SWITCH},
    UTE_SPORT_TYPE(SPORT_TYPE_SKIING)               = {STR_SPORT_RUN,           UI_BUF_SPORT_ICON_RUN_BIN,              .func_sta = FUNC_SPORT_SWITCH},
    UTE_SPORT_TYPE(SPORT_TYPE_ICE_HOCKEY)           = {STR_SPORT_RUN,           UI_BUF_SPORT_ICON_RUN_BIN,              .func_sta = FUNC_SPORT_SWITCH},
    UTE_SPORT_TYPE(SPORT_TYPE_TAEKWONDO)            = {STR_SPORT_RUN,           UI_BUF_SPORT_ICON_RUN_BIN,              .func_sta = FUNC_SPORT_SWITCH},
    UTE_SPORT_TYPE(SPORT_TYPE_VO2MAX_TEST)          = {STR_SPORT_RUN,           UI_BUF_SPORT_ICON_RUN_BIN,              .func_sta = FUNC_SPORT_SWITCH},
    UTE_SPORT_TYPE(SPORT_TYPE_ROWING_MACHING)       = {STR_SPORT_RUN,           UI_BUF_SPORT_ICON_RUN_BIN,              .func_sta = FUNC_SPORT_SWITCH},
    UTE_SPORT_TYPE(SPORT_TYPE_AIR_WALKER)           = {STR_SPORT_RUN,           UI_BUF_SPORT_ICON_RUN_BIN,              .func_sta = FUNC_SPORT_SWITCH},
    UTE_SPORT_TYPE(SPORT_TYPE_HIKING)               = {STR_SPORT_RUN,           UI_BUF_SPORT_ICON_RUN_BIN,              .func_sta = FUNC_SPORT_SWITCH},
    UTE_SPORT_TYPE(SPORT_TYPE_ATHLETICS)            = {STR_SPORT_RUN,           UI_BUF_SPORT_ICON_RUN_BIN,              .func_sta = FUNC_SPORT_SWITCH},
    UTE_SPORT_TYPE(SPORT_TYPE_WAIST_TRAINING)       = {STR_SPORT_RUN,           UI_BUF_SPORT_ICON_RUN_BIN,              .func_sta = FUNC_SPORT_SWITCH},
    UTE_SPORT_TYPE(SPORT_TYPE_KARATE)               = {STR_SPORT_RUN,           UI_BUF_SPORT_ICON_RUN_BIN,              .func_sta = FUNC_SPORT_SWITCH},
    UTE_SPORT_TYPE(SPORT_TYPE_COOLDOWN)             = {STR_SPORT_RUN,           UI_BUF_SPORT_ICON_RUN_BIN,              .func_sta = FUNC_SPORT_SWITCH},
    UTE_SPORT_TYPE(SPORT_TYPE_CROSS_TRAINING)       = {STR_SPORT_RUN,           UI_BUF_SPORT_ICON_RUN_BIN,              .func_sta = FUNC_SPORT_SWITCH},
    UTE_SPORT_TYPE(SPORT_TYPE_PILATES)              = {STR_SPORT_RUN,           UI_BUF_SPORT_ICON_RUN_BIN,              .func_sta = FUNC_SPORT_SWITCH},
    UTE_SPORT_TYPE(SPORT_TYPE_CROSS_FIT)            = {STR_SPORT_RUN,           UI_BUF_SPORT_ICON_RUN_BIN,              .func_sta = FUNC_SPORT_SWITCH},
    UTE_SPORT_TYPE(SPORT_TYPE_FUNCTIONAL_TRAINING)  = {STR_SPORT_RUN,           UI_BUF_SPORT_ICON_RUN_BIN,              .func_sta = FUNC_SPORT_SWITCH},
    UTE_SPORT_TYPE(SPORT_TYPE_PHYSICAL_TRAINING)    = {STR_SPORT_RUN,           UI_BUF_SPORT_ICON_RUN_BIN,              .func_sta = FUNC_SPORT_SWITCH},
    UTE_SPORT_TYPE(SPORT_TYPE_ARCHERY)              = {STR_SPORT_RUN,           UI_BUF_SPORT_ICON_RUN_BIN,              .func_sta = FUNC_SPORT_SWITCH},
    UTE_SPORT_TYPE(SPORT_TYPE_FLEXIBILITY)          = {STR_SPORT_RUN,           UI_BUF_SPORT_ICON_RUN_BIN,              .func_sta = FUNC_SPORT_SWITCH},
    UTE_SPORT_TYPE(SPORT_TYPE_MIXED_CARDIO)         = {STR_SPORT_RUN,           UI_BUF_SPORT_ICON_RUN_BIN,              .func_sta = FUNC_SPORT_SWITCH},
    UTE_SPORT_TYPE(SPORT_TYPE_LATIN_DANCE)          = {STR_SPORT_RUN,           UI_BUF_SPORT_ICON_RUN_BIN,              .func_sta = FUNC_SPORT_SWITCH},
    UTE_SPORT_TYPE(SPORT_TYPE_STREET_DANCE)         = {STR_SPORT_RUN,           UI_BUF_SPORT_ICON_RUN_BIN,              .func_sta = FUNC_SPORT_SWITCH},
    UTE_SPORT_TYPE(SPORT_TYPE_KICKBOXING)           = {STR_SPORT_RUN,           UI_BUF_SPORT_ICON_RUN_BIN,              .func_sta = FUNC_SPORT_SWITCH},
    UTE_SPORT_TYPE(SPORT_TYPE_BARRE)                = {STR_SPORT_RUN,           UI_BUF_SPORT_ICON_RUN_BIN,              .func_sta = FUNC_SPORT_SWITCH},
    UTE_SPORT_TYPE(SPORT_TYPE_AUSTRALIAN_FOOTBALL)  = {STR_SPORT_RUN,           UI_BUF_SPORT_ICON_RUN_BIN,              .func_sta = FUNC_SPORT_SWITCH},
    UTE_SPORT_TYPE(SPORT_TYPE_MARTIAL_ARTS)         = {STR_SPORT_RUN,           UI_BUF_SPORT_ICON_RUN_BIN,              .func_sta = FUNC_SPORT_SWITCH},
    UTE_SPORT_TYPE(SPORT_TYPE_STAIRS)               = {STR_SPORT_RUN,           UI_BUF_SPORT_ICON_RUN_BIN,              .func_sta = FUNC_SPORT_SWITCH},
    UTE_SPORT_TYPE(SPORT_TYPE_HANDBALL)             = {STR_SPORT_RUN,           UI_BUF_SPORT_ICON_RUN_BIN,              .func_sta = FUNC_SPORT_SWITCH},
    UTE_SPORT_TYPE(SPORT_TYPE_BOWLING)              = {STR_SPORT_RUN,           UI_BUF_SPORT_ICON_RUN_BIN,              .func_sta = FUNC_SPORT_SWITCH},
    UTE_SPORT_TYPE(SPORT_TYPE_RACQUETBALL)          = {STR_SPORT_RUN,           UI_BUF_SPORT_ICON_RUN_BIN,              .func_sta = FUNC_SPORT_SWITCH},
    UTE_SPORT_TYPE(SPORT_TYPE_CURLING)              = {STR_SPORT_RUN,           UI_BUF_SPORT_ICON_RUN_BIN,              .func_sta = FUNC_SPORT_SWITCH},
    UTE_SPORT_TYPE(SPORT_TYPE_HUNTING)              = {STR_SPORT_RUN,           UI_BUF_SPORT_ICON_RUN_BIN,              .func_sta = FUNC_SPORT_SWITCH},
    UTE_SPORT_TYPE(SPORT_TYPE_SNOWBOARDING)         = {STR_SPORT_RUN,           UI_BUF_SPORT_ICON_RUN_BIN,              .func_sta = FUNC_SPORT_SWITCH},
    UTE_SPORT_TYPE(SPORT_TYPE_PLAY)                 = {STR_SPORT_RUN,           UI_BUF_SPORT_ICON_RUN_BIN,              .func_sta = FUNC_SPORT_SWITCH},
    UTE_SPORT_TYPE(SPORT_TYPE_AMERICAN_FOOTBALL)    = {STR_SPORT_RUN,           UI_BUF_SPORT_ICON_RUN_BIN,              .func_sta = FUNC_SPORT_SWITCH},
    UTE_SPORT_TYPE(SPORT_TYPE_HAND_CYCLING)         = {STR_SPORT_RUN,           UI_BUF_SPORT_ICON_RUN_BIN,              .func_sta = FUNC_SPORT_SWITCH},
    UTE_SPORT_TYPE(SPORT_TYPE_FISHING)              = {STR_SPORT_RUN,           UI_BUF_SPORT_ICON_RUN_BIN,              .func_sta = FUNC_SPORT_SWITCH},
    UTE_SPORT_TYPE(SPORT_TYPE_DISC_SPORTS)          = {STR_SPORT_RUN,           UI_BUF_SPORT_ICON_RUN_BIN,              .func_sta = FUNC_SPORT_SWITCH},
    UTE_SPORT_TYPE(SPORT_TYPE_RUGGER)               = {STR_SPORT_RUN,           UI_BUF_SPORT_ICON_RUN_BIN,              .func_sta = FUNC_SPORT_SWITCH},
    UTE_SPORT_TYPE(SPORT_TYPE_GOLF)                 = {STR_SPORT_RUN,           UI_BUF_SPORT_ICON_RUN_BIN,              .func_sta = FUNC_SPORT_SWITCH},
    UTE_SPORT_TYPE(SPORT_TYPE_FOLK_DANCE)           = {STR_SPORT_RUN,           UI_BUF_SPORT_ICON_RUN_BIN,              .func_sta = FUNC_SPORT_SWITCH},
    UTE_SPORT_TYPE(SPORT_TYPE_DOWNHILL_SKIING)      = {STR_SPORT_RUN,           UI_BUF_SPORT_ICON_RUN_BIN,              .func_sta = FUNC_SPORT_SWITCH},
    UTE_SPORT_TYPE(SPORT_TYPE_SNOW_SPORTS)          = {STR_SPORT_RUN,           UI_BUF_SPORT_ICON_RUN_BIN,              .func_sta = FUNC_SPORT_SWITCH},
    UTE_SPORT_TYPE(SPORT_TYPE_MIND_BODY)            = {STR_SPORT_RUN,           UI_BUF_SPORT_ICON_RUN_BIN,              .func_sta = FUNC_SPORT_SWITCH},
    UTE_SPORT_TYPE(SPORT_TYPE_CORE_TRAINING)        = {STR_SPORT_RUN,           UI_BUF_SPORT_ICON_RUN_BIN,              .func_sta = FUNC_SPORT_SWITCH},
    UTE_SPORT_TYPE(SPORT_TYPE_SKATING)              = {STR_SPORT_RUN,           UI_BUF_SPORT_ICON_RUN_BIN,              .func_sta = FUNC_SPORT_SWITCH},
    UTE_SPORT_TYPE(SPORT_TYPE_FITNESS_GAMING)       = {STR_SPORT_RUN,           UI_BUF_SPORT_ICON_RUN_BIN,              .func_sta = FUNC_SPORT_SWITCH},
    UTE_SPORT_TYPE(SPORT_TYPE_AEROBICS)             = {STR_SPORT_RUN,           UI_BUF_SPORT_ICON_RUN_BIN,              .func_sta = FUNC_SPORT_SWITCH},
    UTE_SPORT_TYPE(SPORT_TYPE_GROUP_TRAINING)       = {STR_SPORT_RUN,           UI_BUF_SPORT_ICON_RUN_BIN,              .func_sta = FUNC_SPORT_SWITCH},
    UTE_SPORT_TYPE(SPORT_TYPE_KENDO)                = {STR_SPORT_RUN,           UI_BUF_SPORT_ICON_RUN_BIN,              .func_sta = FUNC_SPORT_SWITCH},
    UTE_SPORT_TYPE(SPORT_TYPE_LACROSSE)             = {STR_SPORT_RUN,           UI_BUF_SPORT_ICON_RUN_BIN,              .func_sta = FUNC_SPORT_SWITCH},
    UTE_SPORT_TYPE(SPORT_TYPE_ROLLING)              = {STR_SPORT_RUN,           UI_BUF_SPORT_ICON_RUN_BIN,              .func_sta = FUNC_SPORT_SWITCH},
    UTE_SPORT_TYPE(SPORT_TYPE_WRESTLING)            = {STR_SPORT_RUN,           UI_BUF_SPORT_ICON_RUN_BIN,              .func_sta = FUNC_SPORT_SWITCH},
    UTE_SPORT_TYPE(SPORT_TYPE_FENCING)              = {STR_SPORT_RUN,           UI_BUF_SPORT_ICON_RUN_BIN,              .func_sta = FUNC_SPORT_SWITCH},
    UTE_SPORT_TYPE(SPORT_TYPE_SOFTBALL)             = {STR_SPORT_RUN,           UI_BUF_SPORT_ICON_RUN_BIN,              .func_sta = FUNC_SPORT_SWITCH},
    UTE_SPORT_TYPE(SPORT_TYPE_SINGLE_BAR)           = {STR_SPORT_RUN,           UI_BUF_SPORT_ICON_RUN_BIN,              .func_sta = FUNC_SPORT_SWITCH},
    UTE_SPORT_TYPE(SPORT_TYPE_PARALLEL_BARS)        = {STR_SPORT_RUN,           UI_BUF_SPORT_ICON_RUN_BIN,              .func_sta = FUNC_SPORT_SWITCH},
    UTE_SPORT_TYPE(SPORT_TYPE_ROLLER_SKATING)       = {STR_SPORT_RUN,           UI_BUF_SPORT_ICON_RUN_BIN,              .func_sta = FUNC_SPORT_SWITCH},
    UTE_SPORT_TYPE(SPORT_TYPE_HULA_HOOP)            = {STR_SPORT_RUN,           UI_BUF_SPORT_ICON_RUN_BIN,              .func_sta = FUNC_SPORT_SWITCH},
    UTE_SPORT_TYPE(SPORT_TYPE_DARTS)                = {STR_SPORT_RUN,           UI_BUF_SPORT_ICON_RUN_BIN,              .func_sta = FUNC_SPORT_SWITCH},
    UTE_SPORT_TYPE(SPORT_TYPE_PICKLEBALL)           = {STR_SPORT_RUN,           UI_BUF_SPORT_ICON_RUN_BIN,              .func_sta = FUNC_SPORT_SWITCH},
    UTE_SPORT_TYPE(SPORT_TYPE_HIIT)                 = {STR_SPORT_RUN,           UI_BUF_SPORT_ICON_RUN_BIN,              .func_sta = FUNC_SPORT_SWITCH},
    UTE_SPORT_TYPE(SPORT_TYPE_SHOOTING)             = {STR_SPORT_RUN,           UI_BUF_SPORT_ICON_RUN_BIN,              .func_sta = FUNC_SPORT_SWITCH},
    UTE_SPORT_TYPE(SPORT_TYPE_JUDO)                 = {STR_SPORT_RUN,           UI_BUF_SPORT_ICON_RUN_BIN,              .func_sta = FUNC_SPORT_SWITCH},
    UTE_SPORT_TYPE(SPORT_TYPE_TRAMPOLINE)           = {STR_SPORT_RUN,           UI_BUF_SPORT_ICON_RUN_BIN,              .func_sta = FUNC_SPORT_SWITCH},
    UTE_SPORT_TYPE(SPORT_TYPE_SKATEBOARDING)        = {STR_SPORT_RUN,           UI_BUF_SPORT_ICON_RUN_BIN,              .func_sta = FUNC_SPORT_SWITCH},
    UTE_SPORT_TYPE(SPORT_TYPE_HOVERBOARD)           = {STR_SPORT_RUN,           UI_BUF_SPORT_ICON_RUN_BIN,              .func_sta = FUNC_SPORT_SWITCH},
    UTE_SPORT_TYPE(SPORT_TYPE_BLADING)              = {STR_SPORT_RUN,           UI_BUF_SPORT_ICON_RUN_BIN,              .func_sta = FUNC_SPORT_SWITCH},
    UTE_SPORT_TYPE(SPORT_TYPE_PARKOUR)              = {STR_SPORT_RUN,           UI_BUF_SPORT_ICON_RUN_BIN,              .func_sta = FUNC_SPORT_SWITCH},
    UTE_SPORT_TYPE(SPORT_TYPE_DIVING)               = {STR_SPORT_RUN,           UI_BUF_SPORT_ICON_RUN_BIN,              .func_sta = FUNC_SPORT_SWITCH},
    UTE_SPORT_TYPE(SPORT_TYPE_SURFING)              = {STR_SPORT_RUN,           UI_BUF_SPORT_ICON_RUN_BIN,              .func_sta = FUNC_SPORT_SWITCH},
    UTE_SPORT_TYPE(SPORT_TYPE_SNORKELING)           = {STR_SPORT_RUN,           UI_BUF_SPORT_ICON_RUN_BIN,              .func_sta = FUNC_SPORT_SWITCH},
    UTE_SPORT_TYPE(SPORT_TYPE_PULL_UP)              = {STR_SPORT_RUN,           UI_BUF_SPORT_ICON_RUN_BIN,              .func_sta = FUNC_SPORT_SWITCH},
    UTE_SPORT_TYPE(SPORT_TYPE_PUSH_UP)              = {STR_SPORT_RUN,           UI_BUF_SPORT_ICON_RUN_BIN,              .func_sta = FUNC_SPORT_SWITCH},
    UTE_SPORT_TYPE(SPORT_TYPE_PLANKING)             = {STR_SPORT_RUN,           UI_BUF_SPORT_ICON_RUN_BIN,              .func_sta = FUNC_SPORT_SWITCH},
    UTE_SPORT_TYPE(SPORT_TYPE_ROCK_CLIMBING)        = {STR_SPORT_RUN,           UI_BUF_SPORT_ICON_RUN_BIN,              .func_sta = FUNC_SPORT_SWITCH},
    UTE_SPORT_TYPE(SPORT_TYPE_HIGHTJUMP)            = {STR_SPORT_RUN,           UI_BUF_SPORT_ICON_RUN_BIN,              .func_sta = FUNC_SPORT_SWITCH},
    UTE_SPORT_TYPE(SPORT_TYPE_BUNGEE_JUMPING)       = {STR_SPORT_RUN,           UI_BUF_SPORT_ICON_RUN_BIN,              .func_sta = FUNC_SPORT_SWITCH},
    UTE_SPORT_TYPE(SPORT_TYPE_LONGJUMP)             = {STR_SPORT_RUN,           UI_BUF_SPORT_ICON_RUN_BIN,              .func_sta = FUNC_SPORT_SWITCH},
    UTE_SPORT_TYPE(SPORT_TYPE_MARATHON)             = {STR_SPORT_RUN,           UI_BUF_SPORT_ICON_RUN_BIN,              .func_sta = FUNC_SPORT_SWITCH},
    UTE_SPORT_TYPE(SPORT_TYPE_OTHER)                = {STR_SPORT_RUN,           UI_BUF_SPORT_ICON_RUN_BIN,              .func_sta = FUNC_SPORT_SWITCH},
    UTE_SPORT_TYPE(SPORT_TYPE_CYWEE_SWIM_GNSS)      = {STR_SPORT_RUN,           UI_BUF_SPORT_ICON_RUN_BIN,              .func_sta = FUNC_SPORT_SWITCH},
    UTE_SPORT_TYPE(SPORT_TYPE_CYWEE_SWIM)           = {STR_SPORT_RUN,           UI_BUF_SPORT_ICON_RUN_BIN,              .func_sta = FUNC_SPORT_SWITCH},
#endif // UTE_MODULE_SPORT_HUNDRED_SUPPORT
};

u32 func_sport_get_current_idx(void)
{
    return sys_cb.sport_idx;
}

u32 func_sport_get_str(u8 sport_idx)
{
    if (sport_idx < MENU_LIST_CNT)
    {
        return tbl_sport_list[sport_idx].str_idx;
    }
    return STR_NULL;
}

u32 func_sport_get_ui(u8 sport_idx)
{
    if (sport_idx < MENU_LIST_CNT)
    {
        return tbl_sport_list[sport_idx].res_addr;
    }
    return 0;
}

//创建运动窗体，创建窗体中不要使用功能结构体 func_cb.f_cb
compo_form_t *func_sport_form_create(void)
{
    //新建窗体和背景
    compo_form_t *frm = compo_form_create(true);

    //设置标题栏
    compo_form_set_mode(frm, COMPO_FORM_MODE_SHOW_TITLE | COMPO_FORM_MODE_SHOW_TIME);
    compo_form_set_title(frm, i18n[STR_SPORTS]);

    //新建菜单列表
    compo_listbox_t *listbox = compo_listbox_create(frm, COMPO_LISTBOX_STYLE_MENU_NORMAL);
    compo_listbox_set(listbox, tbl_sport_list, MENU_LIST_CNT);
    compo_listbox_set_bgimg(listbox, UI_BUF_SPORT_BEFORE_EXERCISE_BG_BIN);
    compo_setid(listbox, COMPO_ID_LISTBOX);

    u8 sport_idx = sys_cb.sport_idx;
    compo_listbox_set_focus_byidx(listbox, sport_idx);
    compo_listbox_update(listbox);

    return frm;
}

//点进图标进入应用
static void func_sport_list_icon_click(void)
{
    int icon_idx;
    f_sport_list_t *f_sport = (f_sport_list_t *)func_cb.f_cb;
    compo_listbox_t *listbox = f_sport->listbox;
    u8 func_sta;

    icon_idx = compo_listbox_select(listbox, ctp_get_sxy());
    if (icon_idx < 0 || icon_idx >= MENU_LIST_CNT)
    {
        return;
    }

    //根据图标索引获取应用ID
    func_sta = tbl_sport_list[icon_idx].func_sta;

    //切入应用
    if (func_sta > 0 && !listbox->flag_area)
    {
        compo_form_t *frm = func_create_form(func_sta);
        func_switching(FUNC_SWITCH_ZOOM_ENTER | FUNC_SWITCH_AUTO, listbox->sel_icon);
        compo_form_destroy(frm);
        func_cb.sta = func_sta;
        sys_cb.sport_idx = listbox->focus_icon_idx;                //记住当前编号
        printf("sport_idx=%d\n", sys_cb.sport_idx);
    }

    if(listbox->flag_area)
    {
        func_cb.sta = FUNC_SPORT_CONFIG;
        listbox->flag_area = 0;
    }
}

//运动功能事件处理
static void func_sport_process(void)
{
    f_sport_list_t *f_sport = (f_sport_list_t *)func_cb.f_cb;
    compo_listbox_move(f_sport->listbox);
    func_process();
}

//运动功能消息处理
static void func_sport_message(size_msg_t msg)
{
    f_sport_list_t *f_sport = (f_sport_list_t *)func_cb.f_cb;
    compo_listbox_t *listbox = f_sport->listbox;

    if (compo_listbox_message(listbox, msg))
    {
        return;                                         //处理列表框信息
    }
    switch (msg)
    {
        case MSG_CTP_CLICK:
            func_sport_list_icon_click();                //单击图标
            break;

        case MSG_CTP_LONG:
            break;

        case MSG_CTP_SHORT_RIGHT:
            func_message(msg);
            sys_cb.sport_idx = 0;
            break;

        case KU_DELAY_BACK:
//        if (tick_check_expire(func_cb.enter_tick, TICK_IGNORE_KEY)) {
//            func_menu_sub_list_switch_to_clock();       //返回时钟表盘界面
//        }
            break;

        default:
            func_message(msg);
            break;
    }
}

//进入运动功能
static void func_sport_enter(void)
{
    func_cb.f_cb = func_zalloc(sizeof(f_sport_list_t));
    func_cb.frm_main = func_sport_form_create();

    f_sport_list_t *f_sport = (f_sport_list_t *)func_cb.f_cb;
    f_sport->listbox = compo_getobj_byid(COMPO_ID_LISTBOX);
    compo_listbox_t *listbox = f_sport->listbox;
    if (listbox->type != COMPO_TYPE_LISTBOX)
    {
        halt(HALT_GUI_COMPO_LISTBOX_TYPE);
    }
    listbox->mcb = func_zalloc(sizeof(compo_listbox_move_cb_t));        //建立移动控制块，退出时需要释放
    compo_listbox_move_init_modify(listbox, 100, compo_listbox_gety_byidx(listbox, MENU_LIST_CNT - 1));
    func_cb.enter_tick = tick_get();
}

//退出运动功能
static void func_sport_exit(void)
{
    f_sport_list_t *f_sport = (f_sport_list_t *)func_cb.f_cb;
    compo_listbox_t *listbox = f_sport->listbox;
    func_free(listbox->mcb);
    func_cb.last = FUNC_SPORT;
}

//运动功能
void func_sport(void)
{
    printf("%s\n", __func__);
    func_sport_enter();
    while (func_cb.sta == FUNC_SPORT)
    {
        func_sport_process();
        func_sport_message(msg_dequeue());
    }
    func_sport_exit();
}
