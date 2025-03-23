#include "selector.h"
#include "curve.h"
#include "vram.h"
#include "ui.h"
#include <string.h>

curve_typedef selector_vram_curve;

selector_Typedef selector;

// 选择器状态
void selector_state(PageNodeTypedef *currentNodePage)
{
    // 0为动画初始化中
    static uint8_t selectorState = SELECTOR_STATE_INIT;

    // 获取selector的状态
    selectorState = update_selector_state(currentNodePage);

    switch (selectorState)
    {

    case SELECTOR_STATE_INIT:
        selector_init_animation(currentNodePage);
        break;
    case SELECTOR_STATE_MOVING:
        selector_animation(currentNodePage);
    case SELECTOR_STATE_IDLE:
    default:
        break;
    }

    // 读取显存块中，选择器的块的数据
    get_vram_data(selector.selectorVram);

    // 选择器块进行反色
    for (uint16_t i = 0; i < selector.selectorVram->width * selector.selectorVram->height / 8; i++)
    {
        selector.selectorVram->data[i] = ~selector.selectorVram->data[i];
    }
    // 更新显存块
    write_to_vram_with_cover(selector.selectorVram);
}

// 初始化动画
void selector_init_animation(PageNodeTypedef *currentNodePage)
{
    static uint8_t temp = 0;
    temp++;
    if (temp > 10)
    {
        animation_entry(selector.selectorPositionBanimotion, NON_LINEAR_ANIMATION);
        selector.selectorVram->x = selector.selectorPositionBanimotion->setLocation_X;
        selector.selectorVram->y = selector.selectorPositionBanimotion->setLocation_y;
    }
    else
    {
        selector.selectorVram->x = VRAM_WIDTH;
        selector.selectorVram->y = VRAM_HIGH;
    }
}

// 选择器动画
void selector_animation(PageNodeTypedef *currentNodePage)
{
    // if ((currentNodePage->selectedItem != selector.selectedItem) &&
    //     (selector.selectorPositionBanimotion->wantedTimes == selector.selectorPositionBanimotion->passedTimes))
    // {
    //     // 设置显存位置
    //     selector.selectorVram->width = currentNodePage->selectedItem->vram->width + ITEM_GAP_WIDTH * 2;
    //     selector.selectorVram->height = currentNodePage->selectedItem->vram->height + ITEM_GAP_HEIGHT * 2;
    //     // 设置目标的位置
    //     selector.selectorPositionBanimotion->endLocation_X = currentNodePage->selectedItem->curve->endLocation_X - ITEM_GAP_WIDTH;
    //     selector.selectorPositionBanimotion->endLocation_y = currentNodePage->selectedItem->curve->endLocation_y - ITEM_GAP_HEIGHT;
    //     // 当前位置
    //     selector.selectorPositionBanimotion->startLocation_X = selector.selectorVram->x;
    //     selector.selectorPositionBanimotion->startLocation_y = selector.selectorVram->y;
    //     // 设置时间参数
    //     selector.selectorPositionBanimotion->wantedTimes = CURVE_MOVING_TIME;
    //     selector.selectorPositionBanimotion->passedTimes = 0;
    // }

    // 选择器位置运动
    animation_entry(selector.selectorPositionBanimotion, NON_LINEAR_ANIMATION);
    selector.selectorVram->x = selector.selectorPositionBanimotion->setLocation_X;
    selector.selectorVram->y = selector.selectorPositionBanimotion->setLocation_y;

    // 选择器显存面积运动
    animation_entry(selector.selectorAreaAanimotion, NON_LINEAR_ANIMATION);
    selector.selectorVram->width = selector.selectorAreaAanimotion->setLocation_X;
    selector.selectorVram->height = selector.selectorAreaAanimotion->setLocation_y;

    if (selector.selectorPositionBanimotion->wantedTimes == selector.selectorPositionBanimotion->passedTimes)
    {
        currentNodePage->selectedItem = selector.selectedItem;
    }
}

// 设置选择器目标location
void set_selector_location(VRAM_typedef *vram)
{
    // 设置选择器目标位置
    selector.selectorPositionBanimotion->endLocation_X = vram->x - ITEM_GAP_WIDTH;
    selector.selectorPositionBanimotion->endLocation_y = vram->y - ITEM_GAP_HEIGHT;

    // 设置选择器当前位置
    selector.selectorPositionBanimotion->startLocation_X = selector.selectorVram->x;
    selector.selectorPositionBanimotion->startLocation_y = selector.selectorVram->y;

    // 设置选择器时间参数
    selector.selectorPositionBanimotion->wantedTimes = CURVE_MOVING_TIME;
    selector.selectorPositionBanimotion->passedTimes = 0;

    // 设置选择器显存块大小
    selector.selectorAreaAanimotion->endLocation_X = vram->width + ITEM_GAP_WIDTH * 2;
    selector.selectorAreaAanimotion->endLocation_y = vram->height + ITEM_GAP_HEIGHT * 2;

    // 设置选择器显存块当前位置
    selector.selectorAreaAanimotion->startLocation_X = selector.selectorVram->width;
    selector.selectorAreaAanimotion->startLocation_y = selector.selectorVram->height;

    // 设置选择器时间参数
    selector.selectorAreaAanimotion->wantedTimes = CURVE_MOVING_TIME;
    selector.selectorAreaAanimotion->passedTimes = 0;
}

// 更新selector 的状态
uint8_t update_selector_state(PageNodeTypedef *currentNodePage)
{
    uint8_t selector_state = SELECTOR_STATE_INIT;

    if (selector.selectorPositionBanimotion->passedTimes == selector.selectorPositionBanimotion->wantedTimes)
    {
        selector_state = SELECTOR_STATE_IDLE;
    }
    else
    {
        selector_state = SELECTOR_STATE_MOVING;
    }

    // debug 

    if (selector_state == SELECTOR_STATE_IDLE)
    {
        if (selector.selectedItem == currentNodePage->itemNodeHead)
        {
            selector.selectedItem = currentNodePage->itemNodeHead->itemNodeNext->itemNodeNext->itemNodeNext;
            set_selector_location(selector.selectedItem->vram);
        }
        else
        {
            selector.selectedItem = currentNodePage->itemNodeHead;
            set_selector_location(selector.selectedItem->vram);
        }
    }

    return selector_state;
}

// 选择器初始化
void selector_init(PageNodeTypedef *currentNodePage)
{
    // 显存结构体分配
    selector.selectorVram = (VRAM_typedef *)pvPortMalloc(sizeof(VRAM_typedef));
    if (selector.selectorVram != NULL)
    {
        selector.selectorVram->data = pvPortMalloc(VRAM_HIGH * VRAM_WIDTH / 8);
    }
    // 选择器面积运动曲线分配
    selector.selectorAreaAanimotion = (curve_state_typedef *)pvPortMalloc(sizeof(curve_state_typedef));
    memset(selector.selectorAreaAanimotion, 0, sizeof(curve_state_typedef));
    // 选择器位置运动曲线分配
    selector.selectorPositionBanimotion = (curve_state_typedef *)pvPortMalloc(sizeof(curve_state_typedef));
    memset(selector.selectorPositionBanimotion, 0, sizeof(curve_state_typedef));
    // 暂时无用
    #if 0
    selector.current_x = ITEM_GAP_WIDTH;
    selector.current_y = VRAM_HIGH;
    selector.end_x = ITEM_GAP_WIDTH;
    selector.end_y = ITEM_GAP_HEIGHT;
    #endif

    set_selector_location(currentNodePage->selectedItem->vram);

    // 暂时无用
    selector.selectedItem = currentNodePage->itemNodeHead;
}

uint16_t get_selector_x()
{
    return selector.selectorPositionBanimotion->endLocation_X;
}

uint16_t get_selector_y()
{
    return selector.selectorPositionBanimotion->endLocation_y;
}

uint8_t get_selector_height()
{
    return selector.selectorAreaAanimotion->endLocation_y;
}

uint8_t get_selector_width()
{
    return selector.selectorAreaAanimotion->endLocation_X;
}


