#include "camera.h"
#include "ui.h"
#include "selector.h"
#include "string.h"

camera_typedef main_camera;

void camera_init()
{
    main_camera.camera_x = 0;
    main_camera.camera_y = 0;
    main_camera.camera_width = VRAM_WIDTH;
    main_camera.camera_height = VRAM_HIGH;
    memset(&main_camera.camera_curve, 0, sizeof(curve_state_typedef));
}

static void camera_update_common(uint16_t *camera_pos, uint16_t focus_pos, uint8_t focus_size, uint16_t camera_size)
{
    // 如果焦点大小小于相机大小
    if (focus_size < camera_size)
    {
        // 焦点结束超过相机结束范围
        if (focus_pos + focus_size > camera_size + *camera_pos)
        {
            *camera_pos = focus_pos + focus_size - camera_size;
        }
        // 焦点开始超过相机开始范围
        else if (focus_pos < *camera_pos)
        {
            *camera_pos = focus_pos;
        }
    }
    // 如果焦点大小大于等于相机大小
    else
    {
        *camera_pos = focus_pos;
    }
}

// 更新摄像机位置
void camera_update_x(uint16_t *endLocation_x, uint16_t focus_x, uint8_t focus_width)
{
    // camera的目标位置x
    camera_update_common(endLocation_x, focus_x, focus_width, main_camera.camera_width);
}

void camera_update_y(uint16_t *endLocation_y, uint16_t focus_y, uint8_t focus_height)
{
    // camera的目标位置y
    camera_update_common(endLocation_y, focus_y, focus_height, main_camera.camera_height);
}

void camera_update(uint16_t focus_x, uint8_t focus_width, uint16_t focus_y, uint8_t focus_height)
{

    uint16_t endLocation_y = main_camera.camera_curve.endLocation_y;
    uint16_t endLocation_X = main_camera.camera_curve.endLocation_X;

    camera_update_y(&endLocation_y, focus_y, focus_height);
    camera_update_x(&endLocation_X, focus_x, focus_width);

    // curve_animotion();

    // 设置的位置和当前结束位置不一致时，设置动画
    if ((main_camera.camera_curve.endLocation_X != endLocation_X) || (main_camera.camera_curve.endLocation_y != endLocation_y))
    {
        main_camera.camera_curve.endLocation_X = endLocation_X;
        main_camera.camera_curve.endLocation_y = endLocation_y;
        main_camera.camera_curve.startLocation_X = main_camera.camera_x;
        main_camera.camera_curve.startLocation_y = main_camera.camera_y;
        main_camera.camera_curve.wantedTimes = CURVE_MOVING_TIME / 2;
        main_camera.camera_curve.passedTimes = 0;
    }

    animation_entry(&main_camera.camera_curve, NON_LINEAR_ANIMATION);
    main_camera.camera_x = main_camera.camera_curve.setLocation_X;
    main_camera.camera_y = main_camera.camera_curve.setLocation_y;
}

uint8_t camera_get_x(void)
{
    return main_camera.camera_x;
}

uint8_t camera_get_y(void)
{
    return main_camera.camera_y;
}

uint8_t camera_get_endLocation_x(void)
{
    return main_camera.camera_curve.endLocation_X;
}

uint8_t camera_get_endLocation_y(void)
{
    return main_camera.camera_curve.endLocation_y;
}