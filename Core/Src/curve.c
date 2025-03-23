#include "curve.h"
// 线形动画曲线
#include <stdint.h>
#include "math.h"

// 三次缓出函数
static float cubicEaseOut(float t)
{
    return (1.0f - powf(1.0f - t, 3.0f));
}

void update_location_linear(uint16_t *setLocation, uint16_t startLocation, uint16_t endLocation, uint8_t passedTimes, uint8_t wantedTimes)
{

    float delta = 0;

    // 超时
    if (passedTimes > wantedTimes)
    {
        *setLocation = endLocation;
    }

    // 当前位置大于结束位置
    if (startLocation > endLocation)
    {
        delta = (float)(startLocation - endLocation) / wantedTimes;
        *setLocation = (uint16_t)roundf(startLocation - delta * passedTimes);
    }
    // 当前位置小于结束位置
    else
    {
        delta = (float)(endLocation - startLocation) / wantedTimes;
        *setLocation = (uint16_t)roundf(startLocation + delta * passedTimes);
    }
}

void update_location_non_linear(uint16_t *setLocation, uint16_t startLocation, uint16_t endLocation, uint8_t passedTimes, uint8_t wantedTimes)
{
    // 超时
    if (passedTimes > wantedTimes)
    {
        *setLocation = endLocation;
    }

    float t = (float)passedTimes / wantedTimes; // 归一化时间
    float easeOutValue = cubicEaseOut(t);       // 计算缓动值

    if (startLocation > endLocation)
    {
        float delta = (float)(startLocation - endLocation);
        *setLocation = (uint16_t)roundf(startLocation - delta * easeOutValue);
    }
    else
    {
        float delta = (float)(endLocation - startLocation);
        *setLocation = (uint16_t)roundf(startLocation + delta * easeOutValue);
    }
}

// 线形运动
void linear_animation(curve_state_typedef *curveState)
{
    update_location_linear(&curveState->setLocation_X, curveState->startLocation_X, curveState->endLocation_X, curveState->passedTimes, curveState->wantedTimes);
    update_location_linear(&curveState->setLocation_y, curveState->startLocation_y, curveState->endLocation_y, curveState->passedTimes, curveState->wantedTimes);
}

// 非线形动画
void nonLinear_animation(curve_state_typedef *curveState)
{
    update_location_non_linear(&curveState->setLocation_X, curveState->startLocation_X, curveState->endLocation_X, curveState->passedTimes, curveState->wantedTimes);
    update_location_non_linear(&curveState->setLocation_y, curveState->startLocation_y, curveState->endLocation_y, curveState->passedTimes, curveState->wantedTimes);
}

// 动画入口
void animation_entry(curve_state_typedef *curveState, curve_typedef curveType)
{

    // 传入的时间非法
    if (curveState->wantedTimes == 0)
    {
        curveState->setLocation_X = curveState->endLocation_X;
        curveState->setLocation_y = curveState->endLocation_y;
        return;
    }

    switch (curveType)
    {
    case NON_LINEAR_ANIMATION:
        nonLinear_animation(curveState);
        break;
    case LINEAR_ANIMATION:
    default:
        linear_animation(curveState);
        break;
    }

    // 更新时间
    if (curveState->passedTimes <= curveState->wantedTimes)
    {
        curveState->passedTimes++;
    }
}