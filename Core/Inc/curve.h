#ifndef CURVE_H
#define CURVE_H

#include "stdint.h"

#define CURVE_MOVING_TIME 20

// 动画管理结构
typedef struct
{
    // 动画过渡的时间
    uint8_t wantedTimes;
    // 已经度过的时间
    uint8_t passedTimes;
    // 开始的x轴地址
    uint16_t startLocation_X;
    // 开始的y轴地址
    uint16_t startLocation_y;
    // 结束的x轴
    uint16_t endLocation_X;
    // 结束的y轴
    uint16_t endLocation_y;
    // 当前设置的位置x
    uint16_t setLocation_X;
    // 当前设置的位置y
    uint16_t setLocation_y;
} curve_state_typedef;

typedef enum
{
    LINEAR_ANIMATION = 0,
    NON_LINEAR_ANIMATION
}curve_typedef;


// static float cubicEaseOut(float t);
void linear_animation(curve_state_typedef *curveState);
void animation_entry(curve_state_typedef *curveState, curve_typedef curveType);
void update_location_linear(uint16_t *setLocation, uint16_t startLocation, uint16_t endLocation, uint8_t passedTimes, uint8_t wantedTimes);
void update_location_non_linear(uint16_t *setLocation, uint16_t startLocation, uint16_t endLocation, uint8_t passedTimes, uint8_t wantedTimes);
void nonLinear_animation(curve_state_typedef *curveState);
void linear_animation(curve_state_typedef *curveState);

#endif