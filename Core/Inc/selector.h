#ifndef _SELECTOR_H_
#define _SELECTOR_H_

#include "vram.h"
#include "curve.h"
#include "ui.h"

// #define DIRECTIONS_UP (1u)
// #define DIRECTIONS_DOWN (2u)
// #define DIRECTOPNS_ENTER (3u)
// #define DIRECTOPNS_BACK (4u)

// 焦点结构体
typedef struct selectorTypedef
{
    // 当前的焦点坐标
    uint8_t current_x;
    uint8_t current_y;
    // 目标焦点位置
    uint8_t end_x;
    uint8_t end_y;
    // 选择器的显存
    VRAM_typedef *selectorVram;
    // 选择器的面积运动曲线
    curve_state_typedef *selectorAreaAanimotion;
    // 选择器的位置运动曲线
    curve_state_typedef *selectorPositionBanimotion;
    // 当前选择项
    ItemNodeTypedef *selectedItem;
    // 目标选择项目
    ItemNodeTypedef *targetItem;

} selector_Typedef;

typedef enum
{
    //状态未知
    SELECTOR_STATE_UNKNOW= 0,
    // 上电后的首次动画
    SELECTOR_STATE_INIT,
    // 空闲状态
    SELECTOR_STATE_IDLE,
    // 运动中
    SELECTOR_STATE_MOVING,
} selector_state_typedef;

void selector_state(PageNodeTypedef *currentNodePage);
void selector_init(PageNodeTypedef *currentNodePage);
void selector_init_animation(PageNodeTypedef *currentNodePage);
uint8_t update_selector_state(PageNodeTypedef *currentNodePage);
void selector_animation(PageNodeTypedef *currentNodePage);
void set_selector_location(VRAM_typedef *vram);
uint16_t get_selector_y();
uint16_t get_selector_x();
uint8_t get_selector_height();
uint8_t get_selector_width();
#endif