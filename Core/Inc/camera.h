#ifndef _CAMERA_H_
#define _CAMERA_H_

#include "stdint.h"
#include "curve.h"

typedef struct
{
    // 相机的位置坐标x
    uint16_t camera_x;
    // 相机的位置坐标y
    uint16_t camera_y;
    // 相机宽度
    uint8_t camera_width;
    // 相机高度
    uint8_t camera_height;
    // 相机的位置运动曲线
    curve_state_typedef camera_curve;
} camera_typedef;

void camera_init();

// 更新相机位置， 更具 selector的位置 和 iteam 的 length
void camera_update_x(uint16_t *endLocation_x, uint16_t focus_x, uint8_t focus_width);
void camera_update_y(uint16_t *endLocation_y, uint16_t focus_y, uint8_t focus_height);
void camera_update(uint16_t focus_x, uint8_t focus_width, uint16_t focus_y, uint8_t focus_height);
uint8_t camera_get_y(void);
uint8_t camera_get_x(void);
uint8_t camera_get_endLocation_x(void);
uint8_t camera_get_endLocation_y(void);


#endif