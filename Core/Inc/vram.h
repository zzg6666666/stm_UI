#ifndef VRAM_H
#define VRAM_H

#include "stdint.h"

extern uint32_t xTaskGetTickCount(void);

typedef struct VRAM_typedef
{
    uint16_t x;       // x坐标
    uint16_t y;       // y坐标
    uint8_t priority; // 显示优先级
    uint16_t width;   // 宽度
    uint16_t height;  // 高度
    uint8_t *data;    // 数据
    struct VRAM_typedef *nextVram;
} VRAM_typedef;

#define VRAM_HIGH 64
#define VRAM_WIDTH 128

void write_to_vram(VRAM_typedef const *vram_block);
void write_to_vram_with_cover(VRAM_typedef const *vram_block);
void send_vram_data_to_oled();
void send_vram_data_to_oled_test();
void write_to_vram_test(void);
void get_vram_data(VRAM_typedef *vram_block);
uint8_t get_write_block_width(VRAM_typedef const *vram_block);
uint8_t get_write_block_height(VRAM_typedef const *vram_block);
void clear_vram_block(VRAM_typedef const *vram_block, const uint8_t clear_width, const uint8_t clear_height, uint8_t vram_data[VRAM_WIDTH][VRAM_HIGH / 8]);
void write_vram_block(VRAM_typedef const *vram_block, const uint8_t clear_width, const uint8_t clear_height, uint8_t vram_data[VRAM_WIDTH][VRAM_HIGH / 8]);
void memcpy_bit(unsigned char *dest, unsigned char *src, unsigned char dest_bit_offset, unsigned char src_bit_offset, unsigned int bit_count);
void memset_bit(unsigned char *dest, unsigned char dest_bit_offset, unsigned char value, unsigned int bit_count);
void get_intersection(uint8_t object1_start, uint8_t object1_length, uint8_t object2_start, uint8_t object2_length, uint8_t *intersection_start, uint8_t *intersection_length);
void write_vram_block_with_camera(VRAM_typedef const *vram_block, const uint8_t camera_x, const uint8_t camera_y, uint8_t vram_data[VRAM_WIDTH][VRAM_HIGH / 8]);
void write_to_vram_with_camera(VRAM_typedef const *vram_block, uint16_t camera_x, uint16_t camera_y);
void clear_vram_block_with_camera(VRAM_typedef const *vram_block, const uint8_t camera_x, const uint8_t camera_y, uint8_t vram_data[VRAM_WIDTH][VRAM_HIGH / 8]);
void get_vram_data_with_camera(VRAM_typedef *vram_block, uint16_t camera_x, uint16_t camera_y);
#endif