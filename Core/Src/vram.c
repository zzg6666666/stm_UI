#include "vram.h"
#include "string.h"
#include "driver_oled.h"
#include "stm32f1xx_hal.h"
#include "stdlib.h"
#include "cmsis_os.h"

/*
oled format:
______________________>x 128
|  o o o o o o o o o o
|  o o o o o o o o o o
|  o o o o o o o o o o
|  o o o o o o o o o o
v
y 64
*/

uint8_t main_vram_data[VRAM_WIDTH][VRAM_HIGH / 8];
void write_to_vram(VRAM_typedef const *vram_block)
{
    uint8_t vram_data[VRAM_WIDTH][VRAM_HIGH / 8] = {0};
    const VRAM_typedef *vram_blockHead = vram_block;

    // 遍历链表
    while (vram_blockHead != NULL)
    {
        // 计算写入数据的宽度长度
        uint8_t write_width = get_write_block_width(vram_blockHead);
        uint8_t write_height = get_write_block_height(vram_blockHead);

        // 清除vram block
        // clear_vram_block(vram_blockHead, write_width, write_height, vram_data);
        // 写入vram block
        write_vram_block(vram_blockHead, write_width, write_height, vram_data);

        vram_blockHead = vram_blockHead->nextVram;
    }
    memcpy(main_vram_data, vram_data, VRAM_WIDTH * VRAM_HIGH / 8);
}
void write_to_vram_with_camera(VRAM_typedef const *vram_block, uint16_t camera_x, uint16_t camera_y)
{
    uint8_t vram_data[VRAM_WIDTH][VRAM_HIGH / 8] = {0};
    const VRAM_typedef *vram_blockHead = vram_block;

    // 遍历链表
    while (vram_blockHead != NULL)
    {
        // 清除vram block
        // clear_vram_block(vram_blockHead, write_width, write_height, vram_data);
        // 写入vram block
        write_vram_block_with_camera(vram_blockHead, camera_x, camera_y, vram_data);
        vram_blockHead = vram_blockHead->nextVram;
    }
    memcpy(main_vram_data, vram_data, VRAM_WIDTH * VRAM_HIGH / 8);
}

// 往显存中写数据但是不擦除整个显存块，只擦出覆盖部分
void write_to_vram_with_cover(VRAM_typedef const *vram_block)
{
    const VRAM_typedef *vram_blockHead = vram_block;

    // 遍历链表
    while (vram_blockHead != NULL)
    {
        // 计算写入数据的宽度长度
        uint8_t write_width = get_write_block_width(vram_blockHead);
        uint8_t write_height = get_write_block_height(vram_blockHead);

        // 清除vram block
        clear_vram_block(vram_blockHead, write_width, write_height, main_vram_data);
        // 写入vram block
        write_vram_block(vram_blockHead, write_width, write_height, main_vram_data);
        vram_blockHead = vram_blockHead->nextVram;
    }
}

/**
 * 获取写入VRAM块的宽度
 *
 * 此函数计算在不超出VRAM总宽度的情况下，可以写入的VRAM块的最大宽度
 * 它通过比较VRAM块的右边界和VRAM总宽度来确定实际可写入的宽度
 *
 * @param vram_block 指向VRAM块的常量指针，该块包含x坐标和宽度
 * @return 返回可以在VRAM中写入的块的实际宽度
 */
uint8_t get_write_block_width(VRAM_typedef const *vram_block)
{
    uint8_t write_width = 0;
    if (vram_block != NULL)
    {
        if ((vram_block->x < VRAM_WIDTH) && (vram_block->width != 0))
        {
            // 如果VRAM块的右边界超出VRAM总宽度，则返回VRAM剩余的宽度；否则返回VRAM块的原始宽度
            write_width = (vram_block->x + vram_block->width > VRAM_WIDTH) ? (VRAM_WIDTH - vram_block->x) : (vram_block->width);
        }
    }
    return write_width;
}

/**
 * 获取写入块的最大高度
 *
 * 此函数旨在计算VRAM块在Y轴方向上可写入的最大高度它考虑了VRAM的总高度限制，
 * 以及VRAM块的当前位置和高度，以确定实际可写入的高度
 *
 * @param vram_block 指向VRAM块的指针如果为NULL，表示没有有效的VRAM块
 * @return 返回写入块的最大高度如果没有有效的VRAM块或没有可用的高度，则返回0
 */
uint8_t get_write_block_height(VRAM_typedef const *vram_block)
{
    // 初始化写入高度为0，表示默认情况下没有可用的高度
    uint8_t write_height = 0;

    // 检查传入的VRAM块指针是否有效
    if (vram_block != NULL)
    {
        // 确保VRAM块的起始Y坐标在VRAM的有效范围内，并且VRAM块具有非零的高度
        if ((vram_block->y < VRAM_HIGH) && (vram_block->height != 0))
        {
            // 计算写入高度如果VRAM块的顶部加上高度超出VRAM的顶部，则限制写入高度为VRAM的剩余高度
            write_height = (vram_block->y + vram_block->height > VRAM_HIGH) ? (VRAM_HIGH - vram_block->y) : (vram_block->height);
        }
    }

    // 返回计算得到的写入高度
    return write_height;
}

void clear_vram_block(VRAM_typedef const *vram_block, const uint8_t clear_width, const uint8_t clear_height, uint8_t vram_data[VRAM_WIDTH][VRAM_HIGH / 8])
{

    for (uint8_t i = 0; i < clear_width; i++)
    {
        memset_bit(&vram_data[vram_block->x + i][vram_block->y / 8], (vram_block->y % 8), 0U, clear_height);
    }
}

void write_vram_block(VRAM_typedef const *vram_block, const uint8_t write_width, const uint8_t write_height, uint8_t vram_data[VRAM_WIDTH][VRAM_HIGH / 8])
{
    if (vram_block != NULL)
    {
        // 以X轴的像素点为起点
        for (uint8_t i = 0; i < write_width; i++)
        {
            memcpy_bit(&vram_data[i + vram_block->x][vram_block->y / 8], &vram_block->data[i * vram_block->height / 8], (vram_block->y % 8), 0, write_height);
        }
    }
}

void clear_vram_block_with_camera(VRAM_typedef const *vram_block, const uint8_t camera_x, const uint8_t camera_y, uint8_t vram_data[VRAM_WIDTH][VRAM_HIGH / 8])
{
    if (vram_block != NULL)
    {
        // 清除的目标坐标x
        uint8_t write_position_x = 0;
        // 清除的目标坐标y
        uint8_t write_position_y = 0;

        // 清除宽度
        uint8_t write_length_x = 0;
        // 清除长度
        uint8_t write_length_y = 0;

        // 获得 vram block 和 摄像机的交集
        get_intersection(vram_block->x, vram_block->width, camera_x, VRAM_WIDTH, &write_position_x, &write_length_x);
        get_intersection(vram_block->y, vram_block->height, camera_y, VRAM_HIGH, &write_position_y, &write_length_y);

        if ((write_length_x > 0) && (write_length_y > 0))
        {
            for (uint8_t i = 0; i < write_length_x; i++)
            {
                memset_bit(&vram_data[i + write_position_x - camera_x][(write_position_y - camera_y) / 8], (write_position_y - camera_y) % 8, 0U, write_length_y);
            }
        }
    }
}

void write_vram_block_with_camera(VRAM_typedef const *vram_block, const uint8_t camera_x, const uint8_t camera_y, uint8_t vram_data[VRAM_WIDTH][VRAM_HIGH / 8])
{
    if (vram_block != NULL)
    {
        // 写入的目标坐标x
        uint8_t write_position_x = 0;
        // 写入的目标坐标y
        uint8_t write_position_y = 0;

        // 写入宽度
        uint8_t write_length_x = 0;
        // 写入长度
        uint8_t write_length_y = 0;

        // vram_block 中 被复制像素点的起始位置
        uint16_t vram_block_index = 0;

        // 获得 vram block 和 摄像机的交集
        get_intersection(vram_block->x, vram_block->width, camera_x, VRAM_WIDTH, &write_position_x, &write_length_x);
        get_intersection(vram_block->y, vram_block->height, camera_y, VRAM_HIGH, &write_position_y, &write_length_y);

        if ((write_length_x > 0) && (write_length_y > 0))
        {
            for (uint8_t i = 0; i < write_length_x; i++)
            {
                vram_block_index = (i + write_position_x - vram_block->x) * vram_block->height // x轴的相对位置
                                   + write_position_y - vram_block->y;                         // y轴的相对位置
                // i + write_position_x - camera_x 映射到carmera的坐标  write_position_y - camera_y 映射到carmera的坐标
                memcpy_bit(&vram_data[i + write_position_x - camera_x][(write_position_y - camera_y) / 8], &vram_block->data[vram_block_index / 8], (write_position_y - camera_y) % 8, vram_block_index % 8, write_length_y);
            }
        }
    }
}
void get_intersection(uint8_t object1_start, uint8_t object1_length, uint8_t object2_start, uint8_t object2_length, uint8_t *intersection_start, uint8_t *intersection_length)
{

    if ((intersection_start != NULL) && (intersection_length != NULL))
    {
        uint8_t start = object1_start > object2_start ? object1_start : object2_start;
        uint8_t end = object1_start + object1_length < object2_start + object2_length
                          ? object1_start + object1_length
                          : object2_start + object2_length;
        *intersection_start = start;
        // 无交集
        if (start > end)
        {
            *intersection_length = 0;
        }
        else
        {
            *intersection_length = end - start;
        }
    }
}

// led 的节约内存的格式 大概申请 1K 的内存 128 * 8 = 1024
void send_vram_data_to_oled()
{
    uint8_t oled_data[VRAM_WIDTH] = {0};
    // 将vram数据转成 oled 需要的 格式
    for (uint8_t k = 0; k < 8; k++)
    {
        memset(oled_data, 0, VRAM_WIDTH);
        for (uint8_t i = 0; i < VRAM_WIDTH; i++)
        {
            oled_data[i] = main_vram_data[i][k];
        }
        OLED_SetPosition(k, 0);
        MY_OLED_WriteNBytes(oled_data, VRAM_WIDTH);
    }
}

// 获取vram数据
void get_vram_data(VRAM_typedef *vram_block)
{

    uint8_t get_width = get_write_block_width(vram_block);
    uint8_t get_height = get_write_block_height(vram_block);
    memset(vram_block->data, 0, vram_block->width * vram_block->height / 8);
    if (vram_block != NULL)
    {
        // 需要获取的显存的x轴范围
        for (uint8_t i = 0; i < get_width; i++)
        {
            memcpy_bit(&vram_block->data[i * vram_block->height / 8], &main_vram_data[i + vram_block->x][vram_block->y / 8], ((i * vram_block->height) % 8), vram_block->y % 8, get_height);
        }
    }
}

void get_vram_data_with_camera(VRAM_typedef *vram_block, uint16_t camera_x, uint16_t camera_y)
{
    if (vram_block != NULL)
    {
        // 读取的目标坐标x
        uint8_t get_position_x = 0;
        // 读取的目标坐标y
        uint8_t get_position_y = 0;

        // 读取宽度
        uint8_t get_length_x = 0;
        // 读取长度
        uint8_t get_length_y = 0;

        // vram_block 中 被复制像素点的起始位置
        uint16_t vram_block_index = 0;

        // 获得 vram block 和 摄像机的交集
        get_intersection(vram_block->x, vram_block->width, camera_x, VRAM_WIDTH, &get_position_x, &get_length_x);
        get_intersection(vram_block->y, vram_block->height, camera_y, VRAM_HIGH, &get_position_y, &get_length_y);

        memset(vram_block->data, 0, vram_block->width * vram_block->height / 8);

        if ((get_length_x > 0) && (get_length_y > 0))
        {
            for (uint8_t i = 0; i < get_length_x; i++)
            {
                vram_block_index = (i + get_position_x - vram_block->x) * vram_block->height // x轴的相对位置
                                   + get_position_y - vram_block->y;                         // y轴的相对位置
                // i + write_position_x - camera_x 映射到carmera的坐标  write_position_y - camera_y 映射到carmera的坐标
                memcpy_bit(&vram_block->data[vram_block_index / 8], &main_vram_data[i + get_position_x - camera_x][(get_position_y - camera_y) / 8],
                           vram_block_index % 8, (get_position_y - camera_y) % 8, get_length_y);
            }
        }
    }
}

void send_vram_data_to_oled_test()
{
    uint8_t vramtest[VRAM_WIDTH][VRAM_HIGH / 8] = {0};
    while (1)
    {
        // 宽度是8个像素点
        for (uint8_t i = 0; i < VRAM_WIDTH / 8; i++)
        {
            // 高度也是8个像素点
            for (uint8_t j = 0; j < VRAM_HIGH / 8; j++)
            {
                memset(vramtest, 0, VRAM_WIDTH * VRAM_HIGH / 8);

                for (uint8_t k = 0; k < 8; k++)
                {
                    vramtest[i * 8 + k][j] = 0xff;
                }
                memcpy(main_vram_data, vramtest, VRAM_WIDTH * VRAM_HIGH / 8);
                send_vram_data_to_oled();
                osDelay(20);
                // HAL_Delay(20);
            }
        }
    }
}

void write_to_vram_test(void)
{

    uint8_t test01_data[16 * 4] = {0x00, 0x01, 0x80, 0x00, 0x60, 0x00, 0xf8, 0xff,
                                   0x07, 0x00, 0x40, 0x10, 0x20, 0x0c, 0x18, 0x03,
                                   0x0f, 0x40, 0x08, 0x80, 0xc8, 0x7f, 0x08, 0x00,
                                   0x08, 0x01, 0x28, 0x06, 0x18, 0x18, 0x00, 0x00,
                                   0x10, 0x40, 0x10, 0x22, 0xf0, 0x15, 0x1f, 0x08,
                                   0x10, 0x16, 0xf0, 0x61, 0x00, 0x00, 0x80, 0x00,
                                   0x82, 0x40, 0x82, 0x80, 0xe2, 0x7f, 0x92, 0x00,
                                   0x8a, 0x00, 0x86, 0x00, 0x80, 0x00, 0x00, 0x00};

    VRAM_typedef test01 = {.x = 0, .y = 0, .width = 32, .height = 16, .data = test01_data, .nextVram = NULL};

    while (1)
    {
        test01.x = rand() % (VRAM_WIDTH - test01.width);
        test01.y = rand() % (VRAM_HIGH - test01.height);

        write_to_vram(&test01);
        osDelay(1000);
    }
}

void memcpy_bit(unsigned char *dest, unsigned char *src, unsigned char dest_bit_offset, unsigned char src_bit_offset, unsigned int bit_count)
{

    // 复制的第几个字节
    unsigned int src_byte;
    // 复制的第几个bit
    unsigned int src_bit;
    // 目标的第几个字节
    unsigned int dest_byte;
    // 目标的第几个bit
    unsigned int dest_bit;
    // 需要复制的bit数据
    unsigned char bit;

    for (unsigned int i = 0; i < bit_count; i++)
    {
        src_byte = (src_bit_offset + i) / 8;
        src_bit = (src_bit_offset + i) % 8;

        dest_byte = (dest_bit_offset + i) / 8;
        dest_bit = (dest_bit_offset + i) % 8;

        bit = (src[src_byte] >> src_bit) & 0x01;
        // 将需要复制的bit 清 0
        dest[dest_byte] &= ~(bit << dest_bit);
        // 复制需要复制的bit
        dest[dest_byte] |= (bit << dest_bit);
    }
}

void memset_bit(unsigned char *dest, unsigned char dest_bit_offset, unsigned char value, unsigned int bit_count)
{
    // 目标的第几个字节
    unsigned int dest_byte;
    // 目标的第几个bit
    unsigned int dest_bit;

    for (unsigned int i = 0; i < bit_count; i++)
    {
        dest_byte = (dest_bit_offset + i) / 8;
        dest_bit = (dest_bit_offset + i) % 8;
        // bit set 为1
        if (value & 0x1U)
        {
            dest[dest_byte] |= (0x1U << dest_bit);
        }
        else
        {
            dest[dest_byte] &= ~(0x1U << dest_bit);
        }
    }
}