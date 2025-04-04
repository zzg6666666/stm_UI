#ifndef UI_H
#define UI_H

#include "vram.h"
#include <stddef.h>
#include "FreeRTOS.h"
#include "curve.h"

// item 之间的间隙
#define ITEM_GAP_HEIGHT 0

// item 之间的间隙
#define ITEM_GAP_WIDTH 4

typedef enum ItemType
{
    ITEM_TYPE_TEXT = 1,
} ItemType;

typedef enum SettingType
{
    SETTING_TYPE_SWITCH = 1,
    SETTING_TYPE_NUMS = 2
} SettingType;

// 子项节点结构体
typedef struct ItemNodeTypedef
{
    char name[20];
    struct ItemNodeTypedef *itemNodeNext;
    ItemType ItemType;          // 子节点类型
    SettingType SettingType;    // 设置项类型
    VRAM_typedef *vram;         // item的vram
    curve_state_typedef *curve; // 子节点的曲线结构体
    void (*init)(void);         // 初始化函数
    uint8_t (*Hook)(void);      // hook 函数 用于更新setting vram的状态
    void (*CallBack)(uint8_t);  // call back 函数 点击按键后执行的函数
    VRAM_typedef *settingVram;  // hook 函数返回状态的状态的vram
} ItemNodeTypedef;

// 页面节点结构体
typedef struct PageNodeTypedef
{
    char name[20];
    struct pageNodeTypedef *pageNodeNext;
    struct pageNodeTypedef *pageNodeLast;
    struct ItemNodeTypedef *itemNodeHead;
    ItemNodeTypedef *selectedItem; // 当前选中项
    uint8_t ItemSize;              // 当前页面的item项数
    uint16_t page_width;           // 当前页面的宽度
    uint16_t page_height;          // 当前页面的长度

} PageNodeTypedef;

uint8_t item_node_text_init(ItemNodeTypedef *NodeItem, uint16_t text_loction_x, uint16_t text_loction_y);
void page_node_init(PageNodeTypedef *NodePage);
void curve_animotion(void);
void show_UI(void);
void write_setting_item_vram(const ItemNodeTypedef *itemNodeHead);
void turn_num_to_char(uint8_t num, char *char_num);

#endif
