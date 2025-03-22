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

#define ITEM_TYPE_TEXT 0

// 子项节点结构体
typedef struct ItemNodeTypedef
{
    char name[20];
    struct ItemNodeTypedef *itemNodeNext;
    uint8_t ItemType;           // 子节点类型
    VRAM_typedef *vram;         // item的vram
    curve_state_typedef *curve; // 子节点的曲线结构体
    void (*init)(void);         // 初始化函数
    void (*Hook)(void);         // hook 函数
    void (*CallBack)(void);     // call back 函数
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
    uint16_t page_widght;           // 当前页面的宽度
    uint16_t page_height;          // 当前页面的长度

} PageNodeTypedef;

uint8_t item_node_text_init(ItemNodeTypedef *NodeItem, uint8_t text_loction_x, uint8_t text_loction_y);
void page_node_init(PageNodeTypedef *NodePage);
void curve_animotion(void);
void show_UI(void);

#endif
