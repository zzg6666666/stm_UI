#include "ui.h"
#include "string.h"
#include "driver_oled.h"
#include "stm32f1xx_hal.h"
#include "portable.h"
#include "cmsis_os.h"
#include "curve.h"
#include "selector.h"
#include "oled_font.h"

extern UART_HandleTypeDef huart1;

ItemNodeTypedef ItemNodes[] = {
    {.ItemType = 0, .itemNodeNext = &ItemNodes[1], .vram = NULL, .name = "Node11"},
    {.ItemType = 0, .itemNodeNext = &ItemNodes[2], .vram = NULL, .name = "Node22"},
    {.ItemType = 0, .itemNodeNext = &ItemNodes[3], .vram = NULL, .name = "Node333"},
    {.ItemType = 0, .itemNodeNext = &ItemNodes[4], .vram = NULL, .name = "Node32231113"},
    {.ItemType = 0, .itemNodeNext = &ItemNodes[5], .vram = NULL, .name = "Node333"},
    {.ItemType = 0, .itemNodeNext = NULL, .vram = NULL, .name = "Node444222244"},
};

PageNodeTypedef pageNodeHead = {.name = "pageNodeHead", .itemNodeHead = &ItemNodes[0], .selectedItem = &ItemNodes[0]};

PageNodeTypedef *currentNodePage = &pageNodeHead;

void page_node_init(PageNodeTypedef *NodePage)
{
    ItemNodeTypedef *ItemNode = NodePage->itemNodeHead;

    uint8_t ItemNodeLocation_y = ITEM_GAP_HEIGHT;
    uint8_t ItemNodeLocation_x = ITEM_GAP_WIDTH;
    uint8_t item_width = 0;

    // 为页面下的所有元素初始化
    while (ItemNode != NULL)
    {
        if (ItemNode->ItemType == 0)
        {
            item_width = item_node_text_init(ItemNode, ItemNodeLocation_x, ItemNodeLocation_y);
            // 获取当前元素的高度，并将设置到下个元素的y坐标上
            ItemNodeLocation_y += get_write_block_height(ItemNode->vram) + ITEM_GAP_HEIGHT;

            // 更新页面宽度
            if (NodePage->page_width < ItemNodeLocation_x + item_width)
            {
                // 将页面宽度设置为当前元素宽度
                NodePage->page_width = ItemNodeLocation_x + item_width;
            }
            else
            {
                // do nothing
            }
            // 更新页面高度设置
            NodePage->page_height += ItemNodeLocation_y;
            // 更新页面item数量
            NodePage->ItemSize++;
        }

        ItemNode = ItemNode->itemNodeNext;
    }

    // 为页面下的所有元素设置nextVram
    ItemNode = NodePage->itemNodeHead;
    while (ItemNode != NULL)
    {
        if (ItemNode->itemNodeNext != NULL)
        {
            // 将vram 组成链表
            ItemNode->vram->nextVram = ItemNode->itemNodeNext->vram;
        }
        ItemNode = ItemNode->itemNodeNext;
    }
}

// 给text 节点分配内存 与 初始化
uint8_t item_node_text_init(ItemNodeTypedef *NodeItem, uint8_t text_loction_x, uint8_t text_loction_y)
{
    uint8_t text_vram_len = 0;

    // 分配 vram 内存
    NodeItem->vram = (VRAM_typedef *)pvPortMalloc(sizeof(VRAM_typedef));

    if (NodeItem->vram != NULL)
    {
        text_vram_len = get_char_vram_length(NodeItem->name);
        // 假如使用16 * 16的字体
        NodeItem->vram->height = 16;
        NodeItem->vram->width = 8 * text_vram_len;
        NodeItem->vram->priority = 3;
        NodeItem->vram->x = text_loction_x;
        NodeItem->vram->y = text_loction_y;
        // 分配 vram data 内存 一个字占 字节
        NodeItem->vram->data = (uint8_t *)pvPortMalloc(16 * text_vram_len);
    }

    // 显存数据初始化
    if (NodeItem->vram->data != NULL)
    {
        set_oled_font_vram(NodeItem->name, NodeItem->vram->data, text_vram_len);
    }

    // 分配曲线动画内存
    NodeItem->curve = (curve_state_typedef *)pvPortMalloc(sizeof(curve_state_typedef));

    // 设置元素的曲线结构体
    if (NodeItem->curve != NULL)
    {
        NodeItem->curve->endLocation_X = text_loction_x;
        NodeItem->curve->endLocation_y = text_loction_y;
        NodeItem->curve->wantedTimes = CURVE_MOVING_TIME;
        NodeItem->curve->startLocation_X = text_loction_x;
        NodeItem->curve->startLocation_y = VRAM_HIGH;
        NodeItem->curve->setLocation_y = 0;
        NodeItem->curve->setLocation_X = 0;
        NodeItem->curve->passedTimes = 0;
    }

    return NodeItem->vram->width;
}

// 页面元素释放内存
void item_node_text_deinit(ItemNodeTypedef *ItemNode)
{
    // 释放页面元素内存
    if (ItemNode != NULL)
    {
        // 释放曲线内存
        if (ItemNode->curve != NULL)
        {
            vPortFree(ItemNode->curve);
        }
        // 释放vram内存
        if (ItemNode->vram != NULL)
        {
            // 释放vram data内存
            if (ItemNode->vram->data != NULL)
            {
                vPortFree(ItemNode->vram->data);
            }
            vPortFree(ItemNode->vram);
        }
    }
}

void show_UI(void)
{
    // 页面元素初始化
    page_node_init(&pageNodeHead);
    selector_init(currentNodePage);
    while (1)
    {
        // 执行动画
        curve_animotion();
        write_to_vram(pageNodeHead.itemNodeHead->vram);
        // 焦点动画
        selector_state(currentNodePage);
        send_vram_data_to_oled();
        osDelay(20);
    }
}

// 移动当前页面的元素
void curve_animotion(void)
{
    ItemNodeTypedef *ItemNode = currentNodePage->itemNodeHead;
    while (ItemNode != NULL)
    {
        // 有曲线结构体
        if (ItemNode->curve != NULL)
        {
            if (ItemNode->ItemType == ITEM_TYPE_TEXT)
            {
                // 进行曲线动画
                animation_entry(ItemNode->curve, NON_LINEAR_ANIMATION);
                // 设置vram 位置
                ItemNode->vram->x = ItemNode->curve->setLocation_X;
                ItemNode->vram->y = ItemNode->curve->setLocation_y;
            }
        }
        // HAL_UART_Transmit(&huart1, &ItemNode->curve->setLocation_y, 1, 100);
        ItemNode = ItemNode->itemNodeNext;
    }
}
